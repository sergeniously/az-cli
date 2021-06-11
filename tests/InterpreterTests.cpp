#define BOOST_TEST_MAIN
#include "tests.hpp"

struct AppFixture
{
	az::cli::Argument app;
	az::cli::Context context;

	AppFixture()
		: app(test::Arg::APP, {"app"}, "Application argument")
	{
		test::interactor.input = [](const az::cli::Argument&){ return ""; };
		test::action.full_act = [this](const az::cli::Argument& arg, const az::cli::Context& ctx) {
			context = ctx;
			return arg.id();
		};
	}
	int interpret(const char** argv, int argc) const {
		return az::cli::Interpreter(argv, argc).run(app, test::usage);
	}
};

BOOST_AUTO_TEST_SUITE(InterpreterTests)

BOOST_FIXTURE_TEST_CASE(require_argument, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "call"
	};
	BOOST_REQUIRE_EXCEPTION(interpret(argv.data(), argv.size()), az::cli::Error, [](const az::cli::Error& error) {
		return error.code() == az::cli::Error::Code::RequireArgument && error.context.argument == "-i, --int";
	});
}

BOOST_FIXTURE_TEST_CASE(provide_default_values, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "call", "--int", "2"
	};

	BOOST_REQUIRE_NO_THROW(interpret(argv.data(), argv.size()));

	BOOST_CHECK(context.has(test::Arg::BOOL));
	BOOST_CHECK(context.has(test::Arg::INTEGER));
	BOOST_CHECK(context.has(test::Arg::REAL));
	BOOST_CHECK(context.has(test::Arg::STRING));
	BOOST_CHECK(context.has(test::Arg::PAIR));
	BOOST_CHECK(context.has(test::Arg::NAMELESS));
	BOOST_CHECK(!context.has(test::Arg::PAIR_FIRST));
	BOOST_CHECK(!context.has(test::Arg::PAIR_SECOND));
	BOOST_CHECK(!context.has(test::Arg::ARRAY));
	BOOST_CHECK(!context.has(test::Arg::HIDDEN));

	BOOST_CHECK_EQUAL(bool(context[test::Arg::BOOL]), true);
	BOOST_CHECK_EQUAL(int(context[test::Arg::INTEGER]), 2);
	BOOST_CHECK_EQUAL(double(context[test::Arg::REAL]), 1.23);
	BOOST_CHECK_EQUAL(std::string(context[test::Arg::STRING]), "str");
	BOOST_CHECK_EQUAL(std::string(context[test::Arg::PAIR]), "pair");
	BOOST_CHECK_EQUAL(context[test::Arg::NAMELESS].asString(), "Nameless argument");
}

BOOST_FIXTURE_TEST_CASE(accept_long_names, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "--flag", "--call",
			"--bool", "n",
			"--int", "2",
			"--real", "3.14",
			"--string", "cli",
			"--pair", "term",
			"--first", "key",
			"--second", "value",
			"--array", "one",
			"--array", "two",
			"--hidden", "<tag>text</tag>"
	};

	BOOST_REQUIRE_NO_THROW(interpret(argv.data(), argv.size()));
	BOOST_CHECK(context.has(test::Arg::FLAG));
	BOOST_CHECK(context.has(test::Arg::BOOL));
	BOOST_CHECK(context.has(test::Arg::INTEGER));
	BOOST_CHECK(context.has(test::Arg::REAL));
	BOOST_CHECK(context.has(test::Arg::STRING));
	BOOST_CHECK(context.has(test::Arg::PAIR));
	BOOST_CHECK(context.has(test::Arg::PAIR_FIRST));
	BOOST_CHECK(context.has(test::Arg::PAIR_SECOND));
	BOOST_CHECK(context.has(test::Arg::ARRAY));
	BOOST_CHECK(context[test::Arg::ARRAY].isArray());
	BOOST_CHECK(context.has(test::Arg::NAMELESS));
	BOOST_CHECK(context.has(test::Arg::HIDDEN));

	BOOST_CHECK_EQUAL(bool(context[test::Arg::BOOL]), false);
	BOOST_CHECK_EQUAL(int(context[test::Arg::INTEGER]), 2);
	BOOST_CHECK_EQUAL(double(context[test::Arg::REAL]), 3.14);
	BOOST_CHECK_EQUAL(context[test::Arg::STRING].asString(), "cli");
	BOOST_CHECK_EQUAL(context[test::Arg::PAIR].asString(), "term");
	BOOST_CHECK_EQUAL(context[test::Arg::PAIR_FIRST].asString(), "key");
	BOOST_CHECK_EQUAL(context[test::Arg::PAIR_SECOND].asString(), "value");
	BOOST_CHECK_EQUAL(context[test::Arg::ARRAY].size(), 2);
	BOOST_CHECK_EQUAL(context[test::Arg::ARRAY][0].asString(), "one");
	BOOST_CHECK_EQUAL(context[test::Arg::ARRAY][1].asString(), "two");
	BOOST_CHECK_EQUAL(context[test::Arg::HIDDEN].asString(), "<tag>text</tag>");
}

BOOST_FIXTURE_TEST_CASE(accept_short_names, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "-f", "call",
			"-b", "y",
			"-i", "42",
			"-r", "6.28",
			"-s", "CLI",
			"-a", "first",
			"-p", "term",
			"-1st", "key",
			"-2nd", "value",
			"-h", "  text "
	};

	BOOST_REQUIRE_NO_THROW(interpret(argv.data(), argv.size()));
	BOOST_CHECK(context.has(test::Arg::FLAG));
	BOOST_CHECK(context.has(test::Arg::BOOL));
	BOOST_CHECK(context.has(test::Arg::INTEGER));
	BOOST_CHECK(context.has(test::Arg::REAL));
	BOOST_CHECK(context.has(test::Arg::STRING));
	BOOST_CHECK(context.has(test::Arg::PAIR));
	BOOST_CHECK(context.has(test::Arg::PAIR_FIRST));
	BOOST_CHECK(context.has(test::Arg::PAIR_SECOND));
	BOOST_CHECK(context.has(test::Arg::ARRAY));
	BOOST_CHECK(context[test::Arg::ARRAY].isArray());
	BOOST_CHECK(context.has(test::Arg::NAMELESS));
	BOOST_CHECK(context.has(test::Arg::HIDDEN));

	BOOST_CHECK_EQUAL(bool(context[test::Arg::BOOL]), true);
	BOOST_CHECK_EQUAL(int(context[test::Arg::INTEGER]), 42);
	BOOST_CHECK_EQUAL(double(context[test::Arg::REAL]), 6.28);
	BOOST_CHECK_EQUAL(std::string(context[test::Arg::STRING]), "CLI");
	BOOST_CHECK_EQUAL(context[test::Arg::PAIR].asString(), "term");
	BOOST_CHECK_EQUAL(context[test::Arg::PAIR_FIRST].asString(), "key");
	BOOST_CHECK_EQUAL(context[test::Arg::PAIR_SECOND].asString(), "value");
	BOOST_CHECK_EQUAL(context[test::Arg::ARRAY].size(), 1);
	BOOST_CHECK_EQUAL(context[test::Arg::ARRAY][0].asString(), "first");
	BOOST_CHECK_EQUAL(context[test::Arg::HIDDEN].asString(), "<tag>text</tag>");
}

BOOST_FIXTURE_TEST_CASE(accept_assignments, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "call",
			"--bool=n",
			"--int=32",
			"--real=2.78",
			"--pair=term",
				"-1st=key",
				"-2nd=value",
			"--string=Cli",
			"--array=1st",
			"--array=2nd",
			"--array=3rd",
			"--hidden=data"
	};

    BOOST_REQUIRE_NO_THROW(interpret(argv.data(), argv.size()));

	BOOST_CHECK(!context.has(test::Arg::FLAG));
	BOOST_CHECK(context.has(test::Arg::BOOL));
	BOOST_CHECK(context.has(test::Arg::INTEGER));
	BOOST_CHECK(context.has(test::Arg::REAL));
	BOOST_CHECK(context.has(test::Arg::STRING));
	BOOST_CHECK(context.has(test::Arg::PAIR));
	BOOST_CHECK(context.has(test::Arg::PAIR_FIRST));
	BOOST_CHECK(context.has(test::Arg::PAIR_SECOND));
	BOOST_CHECK(context.has(test::Arg::ARRAY));
	BOOST_CHECK(context[test::Arg::ARRAY].isArray());
	BOOST_CHECK(context.has(test::Arg::NAMELESS));
	BOOST_CHECK(context.has(test::Arg::HIDDEN));

	BOOST_CHECK_EQUAL(bool(context[test::Arg::BOOL]), false);
	BOOST_CHECK_EQUAL(int(context[test::Arg::INTEGER]), 32);
	BOOST_CHECK_EQUAL(double(context[test::Arg::REAL]), 2.78);
	BOOST_CHECK_EQUAL(std::string(context[test::Arg::STRING]), "Cli");
	BOOST_CHECK_EQUAL(context[test::Arg::PAIR].asString(), "term");
	BOOST_CHECK_EQUAL(context[test::Arg::PAIR_FIRST].asString(), "key");
	BOOST_CHECK_EQUAL(context[test::Arg::PAIR_SECOND].asString(), "value");
	BOOST_CHECK_EQUAL(context[test::Arg::ARRAY].size(), 3);
	BOOST_CHECK_EQUAL(context[test::Arg::ARRAY][0].asString(), "1st");
	BOOST_CHECK_EQUAL(context[test::Arg::ARRAY][1].asString(), "2nd");
	BOOST_CHECK_EQUAL(context[test::Arg::ARRAY][2].asString(), "3rd");
	BOOST_CHECK_EQUAL(context[test::Arg::HIDDEN].asString(), "<tag>data</tag>");
}

BOOST_FIXTURE_TEST_CASE(interact, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "call", "-i", "1"
	};
	test::interactor.input = [](const az::cli::Argument&){ return "interactive"; };

	BOOST_REQUIRE_NO_THROW(interpret(argv.data(), argv.size()));
	BOOST_CHECK(context.has(test::Arg::STRING));
	BOOST_CHECK_EQUAL(std::string(context[test::Arg::STRING]), "interactive");
}

BOOST_FIXTURE_TEST_CASE(do_not_ignore_unknown, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "call", "--int=1", "--idk", "--string", "bsv"
	};
	BOOST_REQUIRE_EXCEPTION(interpret(argv.data(), argv.size()), az::cli::Error, [](const az::cli::Error& error) {
		return error.code() == az::cli::Error::Code::InvalidArgument && error.context.argument == "--idk";
	});
}

BOOST_FIXTURE_TEST_CASE(ignore_unknown, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "any", "call", "--int=77", "--idk", "wtf", "--string", "bsv", "-unknown"
	};

	BOOST_REQUIRE_NO_THROW(az::cli::Interpreter(argv.data(), argv.size()).ignoreUnknown().run(app, test::usage));

	BOOST_CHECK(context.has(test::Arg::INTEGER));
	BOOST_CHECK(context.has(test::Arg::STRING));

	BOOST_CHECK_EQUAL(int(context[test::Arg::INTEGER]), 77);
	BOOST_CHECK_EQUAL(context[test::Arg::STRING].asString(), "bsv");
}

BOOST_FIXTURE_TEST_CASE(need_value, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "call", "--int"
	};
	BOOST_REQUIRE_EXCEPTION(interpret(argv.data(), argv.size()), az::cli::Error, [](const az::cli::Error& error) {
		return error.code() == az::cli::Error::Code::NeedValue && error.context.argument == "--int";
	});
}

BOOST_FIXTURE_TEST_CASE(need_help, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "call", "?"
	};
	CUSTOM_REQUIRE_THROW_CLI_ERROR(interpret(argv.data(), argv.size()), az::cli::Error::Code::NeedHelp);
}

BOOST_FIXTURE_TEST_CASE(customize_need_help_string, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "call", "wtf"
	};
	az::cli::Interpreter interpreter(argv.data(), argv.size());
	CUSTOM_REQUIRE_THROW_CLI_ERROR(interpreter.withNeedHelpString("wtf").run(app, test::usage), az::cli::Error::Code::NeedHelp);
}

BOOST_FIXTURE_TEST_CASE(expect_unique_values, AppFixture)
{
	std::vector<const char*> argv = {
		"app", "call", "--int=1", "--array", "one", "--array", "two", "--array", "one"
	};
	BOOST_REQUIRE_EXCEPTION(interpret(argv.data(), argv.size()), az::cli::Error, [](const az::cli::Error& error) {
		return error.code() == az::cli::Error::Code::DuplicateValue &&
			error.context.argument == "--array" && error.context.value == "one";
	});
}

BOOST_AUTO_TEST_SUITE_END()
