#include "Error.h"
#include <map>

namespace az::cli
{

const std::map<Error::Code,const char*> error_codes_to_strings = {
	{Error::Code::None, "no error"},
	{Error::Code::RequireArgument, "require argument"},
	{Error::Code::InvalidArgument, "invalid argument"},
	{Error::Code::InvalidValue, "invalid value"},
	{Error::Code::InvalidRule, "invalid rule"},
	{Error::Code::InvalidStart, "invalid start"},
	{Error::Code::InvalidEnd, "invalid end"},
	{Error::Code::EmptyValue, "empty value"},
	{Error::Code::DuplicateChar, "duplicate char"},
	{Error::Code::DuplicateValue, "duplicate value"},
	{Error::Code::RepetitiveChar, "repetitive char"},
	{Error::Code::WrongType, "wrong type"},
	{Error::Code::NeedValue, "need value"},
	{Error::Code::NeedHelp, "need help"},
	{Error::Code::Multiple, "multiple"},
	{Error::Code::TooFew, "too few"},
	{Error::Code::TooMany, "too many"},
	{Error::Code::TooShort, "too short"},
	{Error::Code::TooLong, "too long"},
	{Error::Code::TooSmall, "too small"},
	{Error::Code::TooLarge, "too large"},
};

Error::Error(Code code)
{
	context.code = code;
}

const char* Error::what() const noexcept
{
	auto problem = error_codes_to_strings.find(context.code);
	return problem != error_codes_to_strings.end() ? problem->second : "unknown";
}

Error& Error::argument(const std::string& argument)
{
	context.argument = argument;
	return *this;
}

Error& Error::value(const std::string& value)
{
	context.value = value;
	return *this;
}

Error& Error::help(const std::string& help)
{
	context.help = help;
	return *this;
}

const char* Error::argument() const
{
	return context.argument.c_str();
}

const char* Error::value() const
{
	return context.value.c_str();
}

const char* Error::help() const
{
	return context.help.c_str();
}

Error::Code Error::code() const
{
	return context.code;
}

bool Error::hasArgument() const
{
	return !context.argument.empty();
}

bool Error::hasValue() const
{
	return !context.value.empty();
}

bool Error::hasHelp() const
{
	return !context.help.empty();
}

}
