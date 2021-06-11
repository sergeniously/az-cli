#include "Demo.hpp"

int show(const az::cli::Arg& argument, const az::cli::Context& context)
{
	for (const auto& kv : context.get()) {
		printf("Arg(%d) = ", kv.first);
		if (!kv.second.isArray()) {
			printf("%s(%s)", kv.second.getTypeName(), kv.second.asString().c_str());
		} else {
			printf("%s[ ", kv.second.getTypeName());
			for (const auto& value : kv.second) {
				printf("%s(%s) ", value.getTypeName(), value.asString().c_str());
			}
			printf("]");
		}
		printf("\n");
	}
	return argument.id();
}

int main(int argc, const char** argv)
{
	demo::callback = show;

	// Describe the main application argument to start interpretation from
	az::cli::Arg application(demo::Arg::APP, {"az-cli-demo"}, 
		"The demo application for the az-cli library\n"
		"© Sergeniously, 2021. All rights reserved\n");

	try {
		// parse arguments and invoke callbacks
		return az::cli::Interpreter(argv, argc).run(application, demo::usage);
	}
	catch (const az::cli::Error& error) {
		// if something went wrong, see what was mistaken
		if (error.code() == az::cli::Error::Code::NeedHelp) {
			puts(error.help());
		} else {
			printf("Error: %s", error.what());
			if (error.hasArgument()) {
				printf(": %s", error.argument());
			}
			if (error.hasValue()) {
				printf(" '%s'", error.value());
			}
			if (error.hasHelp()) {
				printf(" {%s}", error.help());
			}
			printf("\n");
		}
	}
	return -1;
}
