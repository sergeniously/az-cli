#pragma once
#include <string>
#include <exception>

namespace az::cli
{

struct Error : public std::exception
{
	enum class Code {
		None,
		RequireArgument,
		InvalidArgument,
		InvalidValue,
		InvalidRule,
		InvalidStart,
		InvalidEnd,
		EmptyValue,
		DuplicateChar,
		DuplicateValue,
		RepetitiveChar,
		WrongType,
		NeedValue,
		NeedHelp,
		Multiple,
		TooFew,
		TooMany,
		TooShort,
		TooLong,
		TooSmall,
		TooLarge
	};
	struct Context {
		Code code = Code::None;
		std::string argument;
		std::string value;
		std::string help;
	};
	Error(Code code);
	Error& argument(const std::string& argument);
	Error& value(const std::string& value);
	Error& help(const std::string& help);

	virtual const char* what() const noexcept;
	const char* argument() const;
	const char* value() const;
	const char* help() const;
	Code code() const;

	bool hasArgument() const;
	bool hasValue() const;
	bool hasHelp() const;

	Context context;
};

}
