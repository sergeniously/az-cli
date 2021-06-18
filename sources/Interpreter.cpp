#include "Interpreter.h"
#include <sstream>
#include <string.h>
#include "Printer.h"

namespace az::cli
{

Interpreter::Interpreter(const char** argv, int argc, const Options& options)
	: cursor(argv, argc), options(options)
{
}

Interpreter& Interpreter::doEverything(bool whether)
{
	options.do_everything = whether;
	return *this;
}

Interpreter& Interpreter::ignoreUnknown(bool whether)
{
	options.ignore_unknown = whether;
	return *this;
}

Interpreter& Interpreter::withNeedHelpString(const char* string)
{
	options.need_help_string = string;
	return *this;
}

Interpreter& Interpreter::interactively(const Argument::Interactor& interactor)
{
	return interactively(interactor.input, interactor.blame);
}

Interpreter& Interpreter::interactively(const Argument::Interactor::Input& input, const Argument::Interactor::Blame& blame)
{
	interactor.input = input ? input : this->input;
	interactor.blame = blame ? blame : this->blame;
	return *this;
}

int Interpreter::print(const Argument& argument, const Usage& usage, std::ostream& stream)
{
	return Printer(stream).recursively().print(argument, usage);
}

bool Interpreter::parse(const Argument& argument, const Usage& usage, Context& context)
{
	if (!argument.parse(cursor, context)) {
		return false;
	}

	// memorize discovered arguments for ability of recursive calls
	// to recognize unknown ones and to skip them if needs to
	argumentation.push_back(usage(argument));

	while (!cursor.eol()) {
		bool parsed = false;
		for (const auto& sub_argument : argumentation.back()) {
			parsed = parse(sub_argument, usage, context);
			if (parsed) {
				break;
			}
		}
		if (!parsed) {
			if (options.need_help_string == cursor.arg()) {
				std::stringstream help; print(argument, usage, help);
				throw Error(Error::Code::NeedHelp).help(help.str());
			}
			if (!argumentation.back().empty() && !recognize(cursor.arg())) {
				if (!options.ignore_unknown) {
					throw Error(Error::Code::InvalidArgument).argument(cursor.arg());
				}
				++cursor; // skip unknown argument
				continue; // and try to parse again
			}
			break;
		}
	}
	// provide @context with input or default values of sub arguments
	for (const auto& sub_argument : argumentation.back()) {
		sub_argument.provideValue(interactor, context);
	}
	// current (last) argument group is not needed anymore for previous recursive call
	// so it should be dropped; thus the @argumentation is gonna be clean in the end
	argumentation.pop_back();

	if (argument.hasAction()) {
		actions.push_back(argument);
	}
	return true;
}

bool Interpreter::recognize(const char* arg) const
{
	// search @arg among discovered argument groups except the last one in the reverse order, because:
	//   the last one is supposed to be already checked by the time the function is called;
	//   the reverse order predicts that @arg is more likely presented in the recent groups.
	for (auto from = ++argumentation.rbegin(), to = argumentation.rend(); from != to; from++) {
		for (const auto& argument : *from) {
			if (argument.match(arg)) {
				return true;
			}
		}
	}
	return false;
}

int Interpreter::run(const Argument& app, const Usage& usage, Context& context)
{
	parse(app, usage, context);

	for (const auto& action : actions) {
		int result = action.perform(context);
		if (!options.do_everything) {
			return result;
		}
	}
	return app.id();
}

int Interpreter::run(const Argument& app, const Usage& usage)
{
	Context context;
	return run(app, usage, context);
}

std::string Interpreter::input(const Argument& argument)
{
	std::string line;
	std::cout << argument.getDescription() << argument.getValidation() << ": ";
	std::getline(std::cin, line);
	if (std::cin.eof() || std::cin.bad()) {
		line.clear();
	}
	return line;
}

void Interpreter::blame(const Error& error)
{
	printf("Error: %s. Try again!\n", error.what());
}

}
