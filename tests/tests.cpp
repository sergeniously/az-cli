#include "tests.hpp"

namespace test
{

az::cli::Argument::Action action;
az::cli::Argument::Interactor interactor;

std::list<az::cli::Arg> usage(const az::cli::Arg& arg)
{
	switch (arg.id()) {
		case Arg::APP:
			// list arguments of the root of the application
			return {
				az::cli::Arg(Arg::FLAG, {"-f", "--flag"}, "Argument without value").with_no_value(),
				az::cli::Arg(Arg::HELP, {"-h", "--help", "help"}, "Print this usage").with_action([&]{ 
					return az::cli::Interpreter::print(arg, usage); 
				}),
				az::cli::Arg(Arg::CALL, {"--call", "call"}, "Active argument").with_action(action)
			};
		case Arg::CALL:
			// list sub arguments of the --call argument
			return {
				az::cli::Arg(Arg::BOOL, {"-b", "--bool"}, "Bool argument")
					.required().with_value(az::cli::evaluate().boolean().lower_case().glossary({{"y", true}, {"n", false}})).by_default("y"),
				az::cli::Arg(Arg::INTEGER, {"-i", "--int"}, "Integer argument")
					.required().with_value(az::cli::evaluate().integer().min(1).max(100)),
				az::cli::Arg(Arg::REAL, {"-r", "--real"}, "Real argument")
					.with_value(az::cli::evaluate().real().min(-30).max(30)).by_default(1.23),
				az::cli::Arg(Arg::STRING, {"-s", "--string"}, "String argument")
					.interactive(interactor).with_value(az::cli::evaluate().string().min(1).max(16)).by_default("str"),
				az::cli::Arg(Arg::ARRAY, {"-a", "--array"}, "Array argument")
					.multiple().unique().with_value(az::cli::evaluate().nonempty()),
				az::cli::Arg(Arg::PAIR, {"-p", "--pair"}, "Paired argument")
					.with_value(az::cli::evaluate().string("name")).by_default("pair"),
				az::cli::Arg(Arg::HIDDEN, {"-h", "--hidden"}, "Hidden argument")
					.hidden().with_value(az::cli::evaluate().prefix("<tag>").suffix("</tag>").trim()),
				az::cli::Arg(Arg::NAMELESS).by_default("Nameless argument")
			};
		case Arg::PAIR:
			// list sub arguments of the --pair argument
			return {
				az::cli::Arg(Arg::PAIR_FIRST, {"-1st", "--first"}, "First argument").with_value().by_default("first"),
				az::cli::Arg(Arg::PAIR_SECOND, {"-2nd", "--second"}, "Second argument").with_value().by_default("second")
			};
		default:
			// if the argument has no sub arguments, list nothing
			return {};
	}
}

}