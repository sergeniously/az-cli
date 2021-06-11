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
	Interpreter(const char** argv, int argc, const Options& options = {});

	// Sets options.do_everything = true
	Interpreter& doEverything(bool = true);

	// Sets options.ignore_unknown = true
	Interpreter& ignoreUnknown(bool = true);

	// Sets the string that invokes Error::Code::NeedHelp with printed usage (default: "?")
	// Should be called before performing Interpreter::run
	Interpreter& withNeedHelpString(const char*);

	// Parse arguments starting from @Argument and using @Usage to get arguments descriptions
	// Fill @Context with parsed arguments values and perform parsed arguments with actions
	int run(const Argument&, const Usage&);
	int run(const Argument&, const Usage&, Context&);

	// Print arguments starting from @Argument and using @Usage to get arguments descriptions
	// It actually uses Printer class with default options
	static int print(const Argument&, const Usage&, std::ostream& stream = std::cout);

private:
	// Perform parsing of all arguments in command line
	bool parse(const Argument&, const Usage&, Context&);
	// Acknowledge the existence of @arg among discovered arguments
	bool recognize(const char* arg) const;

private:
	// arguments vector cursor
	Cursor cursor;
	// parsing options
	Options options;
	// parsed arguments with action
	std::list<Argument> actions;
	// discovered argument groups
	std::list<std::list<Argument>> argumentation;
};

}
