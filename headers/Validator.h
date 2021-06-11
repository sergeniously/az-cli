#pragma once
#include <map>
#include <ostream>
#include "Value.h"

namespace az::cli
{

enum class Rule {
	TYPE, UNIT, MIN, MAX, NONEMPTY, PATTERN, GLOSSARY,
	NO_DUPLICATES_OF, NO_REPEATS_OF, CANNOT_START_WITH, CANNOT_END_WITH,
	TRIM, PREFIX, SUFFIX, LOWER_CASE, UPPER_CASE
};

class Validator
{
public:
	// Overrides its rules by @Validator's
	Validator& operator+=(const Validator&);

	// Sets minimal value for a number or minimal length for a string
	// Provokes Error::Code::TooSmall and Error::Code::TooShort
	Validator& min(int64_t min);
	// Sets maximal value for a number or maximal length for a string
	// Provokes Error::Code::TooLarge and Error::Code::TooLong
	Validator& max(int64_t max);
	// Requires the value to be non-empty
	Validator& nonempty();
	// Sets a unit for the value (better use with type setters)
	Validator& unit(const std::string& unit);
	// Makes the value a real number and sets its @unit; e.g: real("kg")
	Validator& real(const std::string& unit = {});
	// Makes the value a boolean and sets its @unit; e.g: boolean("answer")
	// Also, adds possible variants aka one_of({"yes", "no", "true", "false"...})
	//   if there was no set of possible variants yet
	Validator& boolean(const std::string& unit = {});
	// Makes the value an integer number and sets its @unit; e.g: integer("days")
	Validator& integer(const std::string& unit = {});
	// Makes the value a string and sets its @unit; e.g: string("path")
	Validator& string(const std::string& unit = {});
	// Sets allowed variants for the value; e.g: one_of({true, 123, 3.14, "str"})
	// Other values won't be accepted; provokes Error::Code::InvalidValue
	Validator& one_of(const std::list<Value>& variants);
	// Sets allowed variants for the value and their substitutes after the value will be parsed
	// E.g. Validator().glossary({{"pi",3.14},{"IP","internet protocol"}})
	Validator& glossary(const std::list<std::pair<std::string, Value>>& glossary);
	// Sets a regular expression pattern for the value
	// Provokes Error::Code::InvalidValue if the value doesn't match the pattern
	Validator& pattern(const std::string& pattern);
	// Sets the chars which cannot be presented more than one time in entire string
	// E.g: Validator().no_duplicates_of("@"); provokes Error::Code::DuplicateChar
	Validator& no_duplicates_of(const std::string& chars);
	// Sets the chars which cannot be presented in sequence
	// E.g: Validator().no_repeats_of("."); provokes Error::Code::RepetitiveChar
	Validator& no_repeats_of(const std::string& chars);
	// Sets the chars the string value cannot start with
	// E.g: Validator().cannot_start_with("$"); provokes Error::Code::InvalidStart
	Validator& cannot_start_with(const std::string& chars);
	// Sets the chars the string value cannot end with
	// E.g: Validator().cannot_end_with("!"); provokes Error::Code::InvalidEnd
	Validator& cannot_end_with(const std::string& chars);
	// Calls cannot_start_with(chars).cannot_end_with(chars)
	Validator& cannot_start_end_with(const std::string& chars);
	// Trims @chars from the start and the end of the string
	Validator& trim(const std::string& chars = " \t\r\n");
	// Inserts @prefix into the front of the string if it doesn't start with @prefix
	Validator& prefix(const std::string& prefix);
	// Appends @suffix in the string if it doesn't end with @suffix
	Validator& suffix(const std::string& suffix);
	// Makes a string lower cased
	Validator& lower_case();
	// Makes a string upper cased
	Validator& upper_case();

	// Prints the validation details
	virtual void print(std::ostream& stream, const Value& /*default value*/) const;
	// Modifies the @raw string according to rules
	virtual std::string amend(const char* raw) const;
	// Checks the @source string according to rules
	virtual bool check(const std::string& source) const;
	// Converts the @source string into the @Value according to rules
	virtual Value apply(const std::string& source) const;
	// Checks if the @string can be converted and equal to the @Value
	static bool match(const std::string&, const Value&);
protected:
	// Checks if there is the @rule
	bool has(Rule rule) const;
	// Gets the @rule's value
	Value get(Rule rule) const;
private:
	std::map<Rule, Value> rules;
};

Validator evaluate();

}
