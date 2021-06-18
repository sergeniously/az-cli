#pragma once
#include <set>
#include "Argument.h"

namespace az::cli
{

// Command line interpreter
class Interpreter
{
public:
	struct Options {
		Options() {}
		bool do_everything = false; // perform every parsed argument with action
		bool ignore_unknown = false; // ignore unknown arguments
		std::string need_help_string = "?"; // string that invokes NeedHelp error
	};

	// Construct and provide the interpreter by arguments vector and parsing options
	Interpreter(const char** argv, int argc, const Options& = {});

	// Sets options.do_everything = true
	Interpreter& doEverything(bool = true);

	// Sets options.ignore_unknown = true
	Interpreter& ignoreUnknown(bool = true);

	// Sets the string that invokes Error::Code::NeedHelp with printed usage (default: "?")
	// Should be called before performing Interpreter::run
	Interpreter& withNeedHelpString(const char*);

	// Sets callbacks to input values by @input and handle errors by @blame
	Interpreter& interactively(const Argument::Interactor&);
	Interpreter& interactively(const Argument::Interactor::Input& = {}, const Argument::Interactor::Blame& = {});

	// Parse arguments starting from @Argument and using @Usage to get arguments descriptions
	// Fill @Context with parsed arguments values and perform parsed arguments with actions
	int run(const Argument&, const Usage&);
	int run(const Argument&, const Usage&, Context&);

	// Print arguments starting from @Argument and using @Usage to get arguments descriptions
	// It actually uses Printer class with default options
	static int print(const Argument&, const Usage&, std::ostream& = std::cout);

private:
	// Perform parsing of all arguments in command line
	bool parse(const Argument&, const Usage&, Context&);
	// Acknowledge the existence of @arg among discovered arguments
	bool recognize(const char* arg) const;

	static std::string input(const Argument&);
	static void blame(const Error&);

private:
	// arguments vector cursor
	Cursor cursor;
	// parsing options
	Options options;
	// interactor for interactive mode
	Argument::Interactor interactor;
	// parsed arguments with action
	std::list<Argument> actions;
	// discovered argument groups
	std::list<std::list<Argument>> argumentation;
};

}
