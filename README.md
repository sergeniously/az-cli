# Introducing

**az-cli** is another skillful interpreter of command line arguments and options which allows to design complex command line interfaces for interaction with user via the console. It is called "interpreter" because it supports parsing, running, printing and inputing arguments in all-in-one way.

# Design goals

- One usage function for parsing and printing arguments which allows to avoid creating and storing unnecessary groups of arguments which will never be used during parsing.
- Ability to bind callbacks with arguments and perform them automatically.
- Ability to interactively input argument values in case they are not specified in command line.
- Powerful validation of argument values which can be customized.

# Quick guide

First of all include the main header of interpreter:
```c++
#include <az-cli/Interpreter.h>
```
Then declare the enum of arguments identificators where the first one (**APP**) will stand for a root application argument:
```c++
enum Arg {
    APP = 0,
    HELP,
    ACTION,
    OPTION,
    VERBOSE
}
```
Also, define some function to perfome some action:
```c++
int action(const az::cli::Argument& arg, az::cli::Context& ctx)
{
    std::cout << "Hello, World!" << std::endl;
    if (ctx.has(Arg::VERBOSE)) {
        std::cout << "Option value: " << ctx[Arg::OPTION].asString() << std::endl;
    }
}
```
After that define the usage function which returns lists of arguments according to hierarchy:
```c++
std::list<az::cli::Arg> usage(const az::cli::Arg& arg)
{
	switch (arg.id()) {
		case Arg::APP:
			return {
				az::cli::Arg(Arg::HELP, {"-h", "--help", "help"}, "Print this usage").with_action([&]{
                    return az::cli::Interpreter::print(arg, usage); 
                }),
				az::cli::Arg(Arg::ACTION, {"action", "--action", "-a"}, "Perform action").with_action(action),
				az::cli::Arg(Arg::VERBOSE, {"-v", "--verbose"}, "Provide detailed output").with_no_value(),
			};
		case Arg::ACTION:
			return {
				az::cli::Arg(Arg::OPTION, {"-o", "--option"}, "Option of action")
					.with_value(az::cli::Validator().string("VALUE").min(1).max(16)).by_default("value")
			};
		default:
			return {};
	}
}
```
Finally, inside the **main** function define a root argument with a description of the application and then do interpretation based on the usage function:
```c++
int main(int argc, const char** argv)
{
	az::cli::Arg app(Arg::APP, {"az-cli"}, 
		"Another command line interpreter C++ library\n"
		"(C) Sergeniously, 2021. All rights reserved\n");

	try {
		return az::cli::Interpreter(argv, argc).run(app, usage);
	}
	catch (const az::cli::Error& error) {
		if (error.code() == az::cli::Error::Code::NeedHelp) {
			std::cout << error.help() << std::endl;
		} else {
            std::cout << "Error: " << error.what() << std::endl;
		}
	}
	return -1;
}
```
Now, after building, it is ready to be used in following ways:
```
az-cli --help
az-cli action ?
az-cli action --option=value
az-cli action -o value --verbose
```
