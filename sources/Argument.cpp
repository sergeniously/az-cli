#include "Argument.h"
#include <iomanip>
#include <sstream>
#include <string.h>
#include "Error.h"
#include <numeric>

namespace az::cli
{

Argument::Argument(int id, const std::list<std::string>& keys, const std::string& description)
{
	attributes[Attribute::ID] = id;
	for (const auto& key : keys) {
		if (!key.empty()) {
			attributes[Attribute::KEYS].append(key);
		}
	}
	attributes[Attribute::DESCRIPTION] = description;

	if (keys.empty()) {
		hidden();
	}
}

Argument::Argument(int id, const std::string& description)
	: Argument(id, {}, description)
{
}

Argument& Argument::with_value(const Value& value)
{
	validator.reset();
	attributes[Attribute::CERTAIN_VALUE] = value;
	return *this;
}

Argument& Argument::with_no_value()
{
	return with_value(Value::Type::None);
}

Argument& Argument::by_default(const Value& value)
{
	attributes[Attribute::DEFAULT_VALUE] = value;
	return *this;
}

Argument& Argument::with_action(const Action::Easy& act)
{
	this->action = Action(act);
	return *this;
}

Argument& Argument::with_action(const Action::Full& act)
{
	this->action = Action(act);
	return *this;
}

Argument& Argument::required(bool whether)
{
	if (whether) {
		attributes[Attribute::REQUIRED];
	}
	return *this;
}

Argument& Argument::multiple(bool whether)
{
	if (whether) {
		attributes[Attribute::MULTIPLE];
	}
	return *this;
}

Argument& Argument::disabled(bool whether)
{
	if (whether) {
		// TODO: store a disabling reason string that'll be thrown in a cli::Error::help()
		attributes[Attribute::DISABLED];
	}
	return *this;
}

Argument& Argument::unique(bool whether)
{
	if (whether) {
		attributes[Attribute::UNIQUE];
	}
	return *this;
}

Argument& Argument::hidden(bool whether)
{
	if (whether) {
		attributes[Attribute::HIDDEN];
	}
	return *this;
}

bool Argument::needValue() const
{
	return validator != nullptr;
}

bool Argument::isRequired() const
{
	return attributes.contains(Attribute::REQUIRED);
}

bool Argument::isMultiple() const
{
	return attributes.contains(Attribute::MULTIPLE);
}

bool Argument::isEnabled() const
{
	return !attributes.contains(Attribute::DISABLED);
}

bool Argument::isUnique() const
{
	return attributes.contains(Attribute::UNIQUE);
}

bool Argument::isHidden() const
{
	return attributes.contains(Attribute::HIDDEN) ||
		attributes.contains(Attribute::DISABLED);
}

bool Argument::hasAction() const
{
	return action;
}

bool Argument::isValuable() const
{
	return validator || hasDefaultValue() || hasCertainValue();
}

bool Argument::hasDefaultValue() const
{
	return attributes.contains(Attribute::DEFAULT_VALUE);
}

bool Argument::hasCertainValue() const
{
	return attributes.contains(Attribute::CERTAIN_VALUE);
}

Value Argument::getDefaultValue() const
{
	if (hasDefaultValue()) {
		return attributes.at(Attribute::DEFAULT_VALUE);
	}
	return {};
}

Value Argument::getValidatedDefaultValue() const
{
	auto default_value = getDefaultValue();
	if (default_value.isString()) {
		validate(default_value.c_str(), default_value);
	}
	return default_value;
}

Value Argument::getCertainValue() const
{
	if (hasCertainValue()) {
		return attributes.at(Attribute::CERTAIN_VALUE);
	}
	return {};
}

std::string Argument::getDescription() const
{
	if (attributes.contains(Attribute::DESCRIPTION)) {
		return attributes.at(Attribute::DESCRIPTION);
	}
	return {};
}

int Argument::id() const
{
	return attributes.at(Attribute::ID);
}

std::list<std::string> Argument::getKeys() const
{
	std::list<std::string> keys;
	if (attributes.contains(Attribute::KEYS)) {
		attributes.at(Attribute::KEYS).toContainer(std::back_inserter(keys));
	}
	return keys;
}

std::string Argument::getKeysString() const
{
	auto keys = getKeys();
	if (!keys.empty()) {
		return std::accumulate(std::next(keys.begin()), keys.end(), keys.front(),
			[](std::string sum, const std::string& name) { return std::move(sum) + ", " + name; });
	}
	return {};
}

std::string Argument::getLongestKey() const
{
	std::string longest_key;
	if (attributes.contains(Attribute::KEYS)) {
		for (const auto& key : attributes.at(Attribute::KEYS)) {
			if (key.size() > longest_key.size()) {
				longest_key = key.asString();
			}
		}
	}
	return longest_key;
}

void Argument::getValidation(std::ostream& stream) const
{
	if (validator) {
		validator->print(stream, getDefaultValue());
	}
}

std::string Argument::getValidation() const
{
	std::stringstream stream;
	getValidation(stream);
	return stream.str();
}

void Argument::getInstruction(std::ostream& stream) const
{
	bool is_optional = isValuable() && !isRequired();

	if (is_optional) {
		stream << "[";
	}
	stream << getKeysString();

	getValidation(stream);

	if (is_optional) {
		stream << "]";
	}
	if (isMultiple()) {
		stream << "...";
	}
}

std::string Argument::getInstruction() const
{
	std::stringstream stream;
	getInstruction(stream);
	return stream.str();
}

bool Argument::validate(const char* arg, Value& value) const
{
	try {
		if (!arg) {
			throw Error(Error::Code::InvalidValue);
		}
		if (validator) {
			auto source = validator->amend(arg);
			if (!validator->check(source)) {
				throw Error(Error::Code::InvalidValue).value(source);
			}
			value = validator->apply(source);
		}
	}
	catch (Error& error) {
		// complete the error with the argument name
		error.argument(getLongestKey());
		throw;
	}
	catch (const std::exception& error) {
		throw Error(Error::Code::InvalidValue).
			argument(getLongestKey()).value(arg).help(error.what());
	}
	return true;
}

bool Argument::input(const Interactor& interactor, Value& value) const
{
	while (interactor && !isHidden()) {
		// TODO: input multiply if argument is multiple
		auto line = interactor.input(*this);
		if (line.empty()) {
			break;
		}
		try {
			if (validate(line.c_str(), value)) {
				return true;
			}
		}
		catch (const Error& error) {
			interactor.blame(error);
		}
	}
	return false;
}

const char* Argument::match(const char* arg) const
{
	if (attributes.count(Attribute::KEYS)) {
		// TODO: assuming a key may have an assignment character
		// firstly compare a whole key with an argument string
		// then check if an assignment is at key-size position
		auto assignment_pos = strchr(arg, '=');
		std::string key = assignment_pos ? std::string(arg, assignment_pos - arg) : std::string(arg);
		if (attributes.at(Attribute::KEYS).contains(key)) {
			return assignment_pos ? ++assignment_pos : arg;
		}
	}
	return nullptr;
}

bool Argument::parse(Cursor& cursor, Context& context) const
{
	// don't match the first argument because it's a name of the app (can be with a path)
	// TODO: maybe should save the path of app to the @context
	auto arg = cursor.pos() > 0 ? match(cursor.arg()) : cursor.arg();
	if (!arg) {
		return false;
	}
	if (!isEnabled()) {
		// TODO: throw Error(Error::Code::DisabledArgument).argument(arg).help(attributes.at(Attribute::DISABLED))
		return false;
	}
	Value value;
	if (needValue()) {
		if (arg != cursor.arg()) {
			validate(arg, value);
		} else if (++cursor) {
			validate(cursor.arg(), value);
		} else {
			throw Error(Error::Code::NeedValue).argument(arg).help(getValidation());
		}
	} else if (hasCertainValue()) {
		value = getCertainValue();
	}
	++cursor; // forward the cursor to the next argument

	if (isValuable()) {
		store(value, context);
	}
	return true;
}

void Argument::provideValue(const Interactor& interactor, Context& context) const
{
	if (!isValuable() || context.has(id())) {
		return;
	}
	Value value;
	if (needValue() && input(interactor, value)) {
		store(value, context);
		return;
	}
	if (hasDefaultValue()) {
		store(getValidatedDefaultValue(), context);
		return;
	}
	if (isRequired()) {
		throw Error(Error::Code::RequireArgument).argument(getKeysString());
	}
}

void Argument::store(const Value& value, Context& context) const
{
	if (isMultiple()) {
		if (isUnique() && context[id()].contains(value)) {
			throw Error(Error::Code::DuplicateValue)
				.argument(getLongestKey()).value(value.asString());
		}
		context[id()].append(value);
	}
	else if (context.has(id())) {
		throw Error(Error::Code::Multiple).argument(getLongestKey());
	}
	else {
		context[id()] = value;
	}
}

int Argument::perform(const Context& context) const
{
	return action(*this, context);
}

}
