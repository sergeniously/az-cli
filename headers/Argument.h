#pragma once
#include <memory>
#include <iostream>
#include <functional>
#include "Validator.h"
#include "Context.h"
#include "Cursor.h"
#include "Error.h"

namespace az::cli
{

// TODO: support name patterns (-DFLAG, -P/path/to/file)
// TODO: support multi value arguments (--arg val1 val2)
// TODO: add an ability to store values to standard types
class Argument
{
public:
	// Callbacks handler for arguments with an action
	struct Action {
		using Easy = std::function<int()>;
		using Full = std::function<int(const Argument&, const Context&)>;

		Easy easy_act;
		Full full_act;

		Action() = default;
		Action(const Action&) = default;
		Action(const Easy& act)
			: easy_act(act) {}
		Action(const Full& act)
			: full_act(act) {}
		operator bool() const {
			return easy_act || full_act;
		}
		int operator()(const Argument& argument, const Context& context) const {
			if (full_act) {
				return full_act(argument, context);
			}
			if (easy_act) {
				return easy_act();
			}
			return argument.id();
		}
	};

	// Callbacks handler to perform user input & errors handling
	struct Interactor {
		using Input = std::function<std::string(const Argument&)>;
		using Blame = std::function<void(const Error&)>;

		Input input;
		Blame blame;

		Interactor(const Input& input = {}, const Blame& blame = {})
			: input(input), blame(blame) {}
		operator bool() const {
			return input && blame;
		}
	};

	Argument() = default;
	Argument(const Argument&) = default;
	// Describe the argument with @id, @keys and @description
	// Passing empty @keys makes the argument hidden and allows to provide @Context with a certain value
	//   without necessity of being passed via command line, e.g: Argument(ID).by_default("token")
	Argument(int id, const std::list<std::string>& keys, const std::string& description = {});
	Argument(int id, const std::string& description = {});

	// Describe the value of the argument taking @rules with preferred validation rules
	// Can be provided with overrided validator like with_value<EmailValidator>() -
	//   in that case @rules describes additional validation rules
	// Makes the argument valuable (stored to the @Context)
	template<class V = Validator> Argument& with_value(const Validator& rules = {}) {
		validator.reset(new V());
		(*validator) += rules;
		return *this;
	}
	// Determines the certain argument's @Value that'll be stored to the @Context
	//   if the argument IS passed via command line
	Argument& with_value(const Value&);
	// Does with_value(Value::Type::None)
	Argument& with_no_value();

	// Determines the default argument's @Value that'll be stored to the @Context
	//   if the argument IS NOT passed via command line
	Argument& by_default(const Value&);

	// Allow the argument to be passed several times
	Argument& multiple(bool whether = true);

	// Makes the argument required to be passed
	Argument& required(bool whether = true);

	// Makes the argument unavailable to be printed & interpreted
	// Useful when there is a need to control an argument's availability by a flag
	Argument& disabled(bool whether = true);

	// Requires multiple arguments to contain only unique values
	Argument& unique(bool whether = true);

	// Makes the argument non printable but able to be passed
	Argument& hidden(bool whether = true);

	// Set callbacks to perform action
	Argument& with_action(const Action::Easy&);
	Argument& with_action(const Action::Full&);

	// Set callbacks to input values by @input and handle errors by @blame
	Argument& interactive(const Interactor&);
	Argument& interactive(const Interactor::Input& input = {}, const Interactor::Blame& blame = {});

	// Get the argument id
	int id() const;

	// Get the longest key of the argument
	std::string getLongestKey() const;
	// Get a string of all keys separated by comma
	std::string getKeysString() const;
	// Get a list of the argument keys
	std::list<std::string> getKeys() const;

	// Get description of the argument value
	std::string getValidation() const;
	void getValidation(std::ostream&) const;

	// Get instruction of the argument usage
	std::string getInstruction() const;
	void getInstruction(std::ostream&) const;

	// Get the description of the argument
	std::string getDescription() const;

	// Check if the argument is required
	bool isRequired() const;
	// Check if the argument is multiple
	bool isMultiple() const;
	// Check if the argument is not disabled
	bool isEnabled() const;
	// Check if the argument is unique
	bool isUnique() const;
	// Check if the argument is hidden
	bool isHidden() const;
	// Check if the argument is interactive (able to get value via callback)
	bool isInteractive() const;
	// Check if the argument has a value and should be stored to the @Context
	bool isValuable() const;

	// Check if the argument has an action
	bool hasAction() const;
	// Check if the argument expects a value
	bool needValue() const;
	// Check if the argument has a default value
	bool hasDefaultValue() const;
	// Check if the argument has a certain value
	bool hasCertainValue() const;
	// Get the argument's default value
	Value getDefaultValue() const;
	Value getValidatedDefaultValue() const;
	// Get the argument's certain value
	Value getCertainValue() const;
	// Provide the @Context with input or default value
	// Throws Error::Code::RequireArgument if the argument is required but not presented in @Context
	void provideValue(Context&) const;

	// Parse the argument using @Cursor for access to [argv, argc]
	//  and fill @Context with parsed values
	bool parse(Cursor&, Context&) const;

	// Matches @arg with the argument keys and returns:
	//  - @arg if 100% matched
	//  - value pos for expressions like @key=value
	//  - nullptr if no match at all
	const char* match(const char* arg) const;

	// Perform an action of the argument
	int perform(const Context&) const;

	// Input the argument value via Interactor
	bool input(Value&) const;

private:
	bool validate(const char* arg, Value&) const;
	void store(const Value&, Context&) const;

private:
	enum class Attribute {
		ID = 0, KEYS, DESCRIPTION, DEFAULT_VALUE, CERTAIN_VALUE,
		REQUIRED, MULTIPLE, DISABLED, UNIQUE, HIDDEN
	};
	std::map<Attribute,Value> attributes;
	std::shared_ptr<Validator> validator;
	Interactor interactor;
	Action action;
};

using Arg = Argument;

// Function for describing application arguments hierarchy
// Allow to avoid creating and storing unnecessary arguments groups, that'll never be used during parsing
// TODO: add @Context for ability to describe arguments depending on the context
using Usage = std::function<std::list<Argument>(const Argument&)>;

std::string input(const Argument&);

}
