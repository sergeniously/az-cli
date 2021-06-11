#pragma once
#include <string>
#include <locale>
#include <cwchar>
#include <cstddef>
#include <memory>
#include <functional>

namespace az::cli {

// This is a simplified copy of boost::detail::utf8_codecvt_facet
// for using instead of std::codecvt_utf8 which was deprecated in c++17
struct Utf8 : public std::codecvt<wchar_t, char, std::mbstate_t>
{
public:
	explicit Utf8(std::size_t no_locale_manage = 0)
		: std::codecvt<wchar_t, char, std::mbstate_t>(no_locale_manage)
	{
	}
protected:
	std::codecvt_base::result do_in(std::mbstate_t& state, const char* from, const char* from_end,
		const char*& from_next, wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const override;

	std::codecvt_base::result do_out(std::mbstate_t& state, const wchar_t* from, const wchar_t* from_end,
		const wchar_t*& from_next, char* to, char* to_end, char*& to_next) const override;

	// How many chars can be processed to get <= max_limit wide chars?
	int do_length(std::mbstate_t&, const char* first, const char* last, std::size_t max_limit) const override;

	bool do_always_noconv() const throw () override {
		return false;
	}

	// UTF-8 isn't really stateful since we rewind on partial conversions
	std::codecvt_base::result do_unshift(std::mbstate_t&, char* from, char* /*to*/, char*& next) const override {
		next = from;
		return ok;
	}

	int do_encoding() const throw () override {
		const int variable_byte_external_encoding = 0;
		return variable_byte_external_encoding;
	}

	// Largest possible value do_length(state,from,from_end,1) could return.
	int do_max_length() const throw ()  override {
		return 6; // largest UTF-8 encoding of a UCS-4 character
	}

	bool invalid_continuing_octet(unsigned char octet_1) const {
		return (octet_1 < 0x80 || 0xbf < octet_1);
	}

	bool invalid_leading_octet(unsigned char octet_1) const {
		return (0x7f < octet_1 && octet_1 < 0xc0) || (octet_1 > 0xfd);
	}

	// continuing octets = octets except for the leading octet
	static unsigned int get_cont_octet_count(unsigned char lead_octet) {
		return get_octet_count(lead_octet) - 1;
	}

	static unsigned int get_octet_count(unsigned char lead_octet);

	// How many "continuing octets" will be needed for this word == total octets - 1.
	int get_cont_octet_out_count(wchar_t word) const;
};

// This is a simplified & improved copy of std::wstring_convert
// which was implemented in c++11 but deprecated in c++17
template<class Facet, class Elem = wchar_t>
class Convert
{
public:
	using byte_string = std::basic_string<char>;
	using wide_string = std::basic_string<Elem>;
	using facet_state = typename Facet::state_type;

	Convert()
		: facet(new Facet()) {}

	~Convert() = default;
	Convert(const Convert&) = delete;
	Convert& operator=(const Convert&) = delete;

	wide_string from_bytes(char byte) {
		char bytes[2] = { byte };
		return from_bytes(bytes, bytes + 1);
	}

	wide_string from_bytes(const char* str) {
		return from_bytes(str, str + std::char_traits<char>::length(str));
	}

	wide_string from_bytes(const byte_string& str) {
		return from_bytes(str.data(), str.data() + str.size());
	}

	wide_string from_bytes(const char* first, const char* last) {
		return perform<wchar_t>(first, last,
			std::bind(&Facet::in, facet.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
	}

	byte_string to_bytes(Elem wide_char) {
		Elem wide_chars[2] = { wide_char };
		return to_bytes(wide_chars, wide_chars + 1);
	}

	byte_string to_bytes(const Elem* str) {
		return to_bytes(str, str + wide_string::traits_type::length(str));
	}

	byte_string to_bytes(const wide_string& str) {
		return to_bytes(str.data(), str.data() + str.size());
	}

	byte_string to_bytes(const Elem* first, const Elem* last) {
		return perform<char>(first, last,
			std::bind(&Facet::out, facet.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
	}

	size_t length(const char* first, const char* last) {
		size_t wide_length = 0;
		while (first != last) {
			// count the byte chars that would be consumed to make one wide char
			auto byte_length = facet->length(state, first, last, 1);
			first += byte_length;
			wide_length++;
		}
		return wide_length;
	}

	size_t length(const byte_string& str) {
		return length(str.data(), str.data() + str.size());
	}

private:
	template<class ToChar, class FromChar, class Converter>
	std::basic_string<ToChar> perform(const FromChar* first, const FromChar* last, Converter converter)
	{
		std::basic_string<ToChar> conversion;
		// The interface of cvt is not really iterator-like, and it's
		// not possible the tell the required output size without the conversion.
		// All we can is convert data by pieces.
		while (first != last) {
			// std::basic_string does not provide non-const pointers to the data,
			// so converting directly into string is not possible.
			ToChar buffer[32];
			ToChar* to_next = buffer;
			ToChar* to_end = buffer + 32;

			auto result = converter(state, first, last, first, buffer, to_end, to_next);
			// 'partial' is not an error, it just means not all source
			// characters were converted. However, we need to check that at
			// least one new target character was produced. If not, it means
			// the source data is incomplete, and since we don't have extra
			// data to add to source, it's error.
			if (result == std::codecvt_base::error || to_next == buffer) {
				throw std::logic_error("character conversion failed");
			}
			conversion.append(buffer, to_next);
		}
		return conversion;
	}

private:
	std::unique_ptr<Facet> facet;
	facet_state state = facet_state();
};

}
