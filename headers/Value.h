#pragma once
#include <list>
#include <string>
#include <ostream>

namespace az::cli
{

class Value
{
public:
	enum class Type : uint8_t {
		None,
		Bool,
		Integer,
		Real,
		String,
		Array
	};
	using String = std::string;
	using Array = std::list<Value>;

	Value();
	Value(Type);
	Value(const Value&);
	Value(Value&&);
	Value(bool);
	Value(int);
	Value(int64_t);
	Value(double);
	Value(const char*);
	Value(const wchar_t*);
	Value(float real_value) : Value(double(real_value)) {}
	Value(char character) : Value(std::string(1, character)) {}
	Value(wchar_t character) : Value(std::wstring(1, character)) {}
	Value(const std::string& string_value) : Value(string_value.c_str()) {}
	Value(const std::wstring& string_value) : Value(string_value.c_str()) {}
	Value(const std::initializer_list<Value>& list);
	~Value();
	void reset(Type type = Type::None);
	Value& append(const Value& value);
	Value& operator=(const Value& other);
	Value& convert(Type new_type);
	Value& convert(const Value& other);
	Value as(Type type) const;

	bool operator==(const Value& other) const;
	bool operator<(const Value& other) const;
	bool operator>(const Value& other) const;

	operator int() const;
	operator bool() const;
	operator int64_t() const;
	operator uint64_t() const;
	operator double() const;
	operator std::string() const;
	operator std::wstring() const;
	String asString() const;
	std::wstring asWideString() const;
	const char* c_str() const;
	const char* c_str();

	bool isNone() const;
	bool isBool() const;
	bool isInteger() const;
	bool isReal() const;
	bool isString() const;
	bool isArray() const;
	bool isNumber() const;
	Type getType() const;
	const char* getTypeName() const noexcept;
	static const char* getTypeName(Type type) noexcept;
	static const char* getTypeString(Type type) noexcept;
	static std::list<std::string> getBoolStrings();

	bool empty() const;
	size_t size() const;
	bool contains(const Value&) const;
	const Value& operator[](uint32_t index) const;

	Array::const_iterator begin() const;
	Array::const_iterator end() const;
	Array::iterator begin();
	Array::iterator end();

	/*
	 * For example:
	 *  toContainer(std::back_inserter(std::vector<int>&))
	 */
	template<class OutputIt>
	void toContainer(OutputIt output) const {
		for (const auto& value : *this) {
			*output++ = value;
		}
	}

private:
	Type type = Type::None;
	union {
		bool bool_;
		int64_t int_;
		double real_;
		String* string_;
		Array* array_;
	} any;
};

}

// namespace std here is only needed by old boost test library we use
// should be deleted when it is up to date
namespace std
{

ostream& operator<<(ostream& stream, const az::cli::Value::Type& type);
ostream& operator<<(ostream& stream, const az::cli::Value& value);

}
