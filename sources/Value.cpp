#include "Value.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <locale>
#include <codecvt>
#include <string.h>
#include <wchar.h>
#include "Error.h"
#include "Convert.h"
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

namespace az::cli
{

struct ErrorWrongType : public Error
{
	ErrorWrongType(Value::Type wrong_type, const std::list<Value::Type>& right_types = {})
		: Error(Error::Code::WrongType)
	{
		std::list<std::string> right_strings;
		std::transform(right_types.begin(), right_types.end(), std::back_inserter(right_strings),
			[](Value::Type type){ return Value::getTypeName(type); });

		value(Value::getTypeName(wrong_type));
		help(std::accumulate(std::next(right_strings.begin()), right_strings.end(), right_strings.front(),
			[](std::string all, const std::string& one) { return std::move(all) + ", " + one; }));
	}
};

// TODO: implement setBoolVariants user API
const std::unordered_map<std::string,bool> boolean_strings_map {
	{"true", true}, {"false", false},
	{"yes", true}, {"no", false},
	{"y", true}, {"n", false},
	{"on", true}, {"off", false}
};

Value::Value()
{
	memset(&any, 0, sizeof(any));
}

Value::Value(Type type)
{
	reset(type);
}

Value::Value(const Value& other)
{
	*this = other;
}

Value::Value(Value&& other)
{
	type = other.type;
	any = other.any;

	other.type = Type::None;
	memset(&other.any, 0, sizeof(other.any));
}

Value::Value(bool bool_value)
{
	reset(Type::Bool);
	any.bool_ = bool_value;
}

Value::Value(int int_value)
	: Value(int64_t(int_value))
{
}

Value::Value(int64_t int_value)
{
	reset(Type::Integer);
	any.int_ = int_value;
}

Value::Value(double real_value)
{
	reset(Type::Real);
	any.real_ = real_value;
}

Value::Value(const char* byte_string)
{
	reset(Type::String);
	any.string_->assign(byte_string);
}

Value::Value(const wchar_t* wide_string)
	: Value(Convert<Utf8>().to_bytes(wide_string))
{
}

Value::Value(const std::initializer_list<Value>& list)
{
	reset(Type::Array);
	any.array_->assign(list.begin(), list.end());
}

Value::~Value()
{
	reset();
}

void Value::reset(Type type)
{
	if (this->type != type) {
		switch (this->type) {
			case Type::String:
				delete any.string_; break;
			case Type::Array:
				delete any.array_; break;
			default:
				break;
		}
		memset(&any, 0, sizeof(any));
		switch (type) {
			case Type::String:
				any.string_ = new String; break;
			case Type::Array:
				any.array_ = new Array; break;
			default:
				break;
		}
		this->type = type;
	}
	else {
		switch (type) {
			case Type::String:
				any.string_->clear(); break;
			case Type::Array:
				any.array_->clear(); break;
			default:
				memset(&any, 0, sizeof(any));
		}
	}
}

Value& Value::operator=(const Value& other)
{
	reset(other.type);
	switch (other.type) {
		case Type::String:
			*any.string_ = *other.any.string_; break;
		case Type::Array:
			*any.array_ = *other.any.array_; break;
		default:
			memcpy(&any, &other.any, sizeof(any));
	}
	return *this;
}

bool Value::operator==(const Value& other) const
{
	if (type != other.type) {
		return false;
	}
	switch (type) {
		case Type::Bool:
			return any.bool_ == other.any.bool_;
		case Type::Integer:
			return any.int_ == other.any.int_;
		case Type::Real:
			return any.real_ == other.any.real_;
		case Type::String:
			return *any.string_ == *other.any.string_;
		case Type::Array:
			return *any.array_ == *other.any.array_;
		default:
			return true;
	}
}

bool Value::operator<(const Value& other) const
{
	if (type != other.type) {
		return false;
	}
	switch (type) {
		case Type::Bool:
			return any.bool_ < other.any.bool_;
		case Type::Integer:
			return any.int_ < other.any.int_;
		case Type::Real:
			return any.real_ < other.any.real_;
		case Type::String:
			return *any.string_ < *other.any.string_;
		default:
			throw ErrorWrongType(type, {other.type});
	}
}

bool Value::operator>(const Value& other) const
{
	return other < *this;
}

Value::operator int() const
{
	return int(int64_t(*this));
}

Value::operator bool() const
{
	switch (type) {
		case Type::None:
			return false;
		case Type::Bool:
		case Type::Integer:
		case Type::Real:
			return any.int_ != 0;
		case Type::String: {
			auto item = boolean_strings_map.find(*any.string_);
			if (item != boolean_strings_map.end()) {
				return item->second;
			}
			auto bool_strings = getBoolStrings();
			throw Error(Error::Code::InvalidValue).value(*any.string_)
				.help(std::accumulate(std::next(bool_strings.begin()), bool_strings.end(), bool_strings.front(),
					[](std::string all, const std::string& one) { return std::move(all) + ", " + one; }));
		}
		default:
			throw ErrorWrongType(type, {Type::Bool});
	}
}

std::list<std::string> Value::getBoolStrings()
{
	std::list<std::string> strings;
	for (const auto& kv : boolean_strings_map) {
		strings.push_front(kv.first);
	}
	return strings;
}

Value::operator int64_t() const
{
	switch (type) {
		case Type::None:
			return 0;
		case Type::Bool:
			return any.bool_;
		case Type::Integer:
			return any.int_;
		case Type::Real:
			return int64_t(any.real_);
		case Type::String:
			try {
				return std::stoll(*any.string_);
			} catch (const std::exception&) {
				throw Error(Error::Code::InvalidValue).value(*any.string_);
			}
			break;
		default:
			throw ErrorWrongType(type, {Type::Integer});
	}
}

Value::operator uint64_t() const
{
	return int64_t(*this);
}

Value::operator double() const
{
	switch (type) {
		case Type::None:
			return 0;
		case Type::Bool:
			return any.bool_;
		case Type::Integer:
			return double(any.int_);
		case Type::Real:
			return any.real_;
		case Type::String:
			try {
				return std::stod(*any.string_);
			} catch (const std::exception&) {
				throw Error(Error::Code::InvalidValue).value(*any.string_);
			}
			break;
		default:
			throw ErrorWrongType(type, {Type::Real});
	}
}

Value::String Value::asString() const
{
	switch (type) {
		case Type::None:
			return "";
		case Type::Bool:
			return any.bool_ ? "true" : "false";
		case Type::Integer:
			return std::to_string(any.int_);
		case Type::Real: {
			auto str = std::to_string(any.real_);
			// remove trailing zeros from the end
			while (!str.empty() && (str.back() == '0' || str.back() == '.')) {
				bool done = str.back() == '.';
				str.pop_back();
				if (done) {
					break;
				}
			}
			return str;
		}
		case Type::String:
			return *any.string_;
		default:
			throw ErrorWrongType(type, {Type::String});
	}
}

Value::operator std::string() const
{
	return asString();
}

Value::operator std::wstring() const
{
	return asWideString();
}

std::wstring Value::asWideString() const
{
	auto byte_string = asString();
	try {
		return Convert<Utf8>().from_bytes(byte_string);
	}
	catch (const std::exception&) {
#if defined(_WIN32) || defined(_WIN64)
		std::vector<wchar_t> wide_string(byte_string.size() + 1);
		auto wide_length = MultiByteToWideChar(CP_ACP, 0, byte_string.c_str(), byte_string.length(),
			wide_string.data(), wide_string.size());
		if (wide_length > 0) {
			return wide_string.data();
		}
#endif
		throw;
	}
}

const char* Value::c_str() const
{
	if (!isString()) {
		throw ErrorWrongType(type, {Type::String});
	}
	return any.string_->c_str();
}

const char* Value::c_str()
{
	convert(Type::String);
	return any.string_->c_str();
}

Value& Value::convert(Type new_type)
{
	if (type != new_type) {
		switch (new_type) {
			case Type::Bool:
				*this = bool(*this); break;
			case Type::Integer:
				*this = int64_t(*this); break;
			case Type::Real:
				*this = double(*this); break;
			case Type::String:
				*this = asString(); break;
			case Type::Array:
				this->append(Value(*this)); break;
			default:
				reset();
		}
	}
	return *this;
}

Value& Value::convert(const Value& other)
{
	return convert(other.getType());
}

Value Value::as(Type type) const
{
	return Value(*this).convert(type);
}

Value& Value::append(const Value& value)
{
	if (!isArray()) {
		reset(Type::Array);
	}
	any.array_->push_back(value);
	return any.array_->back();
}

bool Value::isNone() const
{
	return type == Type::None;
}

bool Value::isBool() const
{
	return type == Type::Bool;
}

bool Value::isInteger() const
{
	return type == Type::Integer;
}

bool Value::isReal() const
{
	return type == Type::Real;
}

bool Value::isArray() const
{
	return type == Type::Array;
}

bool Value::isString() const
{
	return type == Type::String;
}

bool Value::isNumber() const
{
	return type == Type::Integer || type == Type::Real;
}

Value::Type Value::getType() const
{
	return type;
}

const char* Value::getTypeName(Type type) noexcept
{
	switch (type) {
		case Type::Integer:
		case Type::Real:
			return "number";
		default:
			return getTypeString(type);
	}
}

const char* Value::getTypeString(Type type) noexcept
{
	switch (type) {
		case Type::None:
			return "none";
		case Type::Bool:
			return "boolean";
		case Type::Integer:
			return "integer";
		case Type::Real:
			return "real";
		case Type::String:
			return "string";
		case Type::Array:
			return "array";
		default:
			return "unknown";
	}
}

const char* Value::getTypeName() const noexcept
{
	return getTypeName(type);
}

bool Value::empty() const
{
	switch (type) {
		case Type::None:
			return true;
		case Type::String:
			return any.string_->empty();
		case Type::Array:
			return any.array_->empty();
		default:
			return false;
	}
}

size_t Value::size() const
{
	switch (type) {
		case Type::None:
			return 0;
		case Type::String:
			return any.string_->size();
		case Type::Array:
			return any.array_->size();
		default:
			throw ErrorWrongType(type, {Type::String, Type::Array});
	}
}

bool Value::contains(const Value& value) const
{
	switch (type) {
		case Type::None:
			return false;
		case Type::String:
			return any.string_->find(value.asString()) != std::string::npos;
		case Type::Array:
			return std::find(begin(), end(), value) != end();
		default:
			throw ErrorWrongType(type, {Type::String, Type::Array});
	}
}

const Value& Value::operator[](uint32_t index) const
{
	if (!isArray()) {
		throw ErrorWrongType(type, {Type::Array});
	}
	if (index >= any.array_->size()) {
		throw Error(Error::Code::TooFew).value(std::to_string(index)).help(std::to_string(any.array_->size()));
	}
	auto value = any.array_->begin();
	std::advance(value, index);
	return *value;
}

Value::Array::iterator Value::begin()
{
	if (!isArray()) {
		throw ErrorWrongType(type, {Type::Array});
	}
	return any.array_->begin();
}

Value::Array::iterator Value::end()
{
	if (!isArray()) {
		throw ErrorWrongType(type, {Type::Array});
	}
	return any.array_->end();
}

Value::Array::const_iterator Value::begin() const
{
	if (!isArray()) {
		throw ErrorWrongType(type, {Type::Array});
	}
	return any.array_->begin();
}

Value::Array::const_iterator Value::end() const
{
	if (!isArray()) {
		throw ErrorWrongType(type, {Type::Array});
	}
	return any.array_->end();
}

}

namespace std
{

ostream& operator<<(ostream& stream, const az::cli::Value::Type& type)
{
	stream << az::cli::Value::getTypeString(type);
	return stream;
}

ostream& operator<<(ostream& stream, const az::cli::Value& value)
{
	stream << value.asString();
	return stream;
}

}
