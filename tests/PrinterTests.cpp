#include "tests.hpp"

struct PrinterFixture
{
	az::cli::Argument app;
	az::cli::Printer printer;
	std::stringstream stream;

	PrinterFixture()
		: app(test::Arg::APP, {"app"}, "Application argument"),
		printer(stream)
	{}
};

BOOST_AUTO_TEST_SUITE(PrinterTests)

BOOST_FIXTURE_TEST_CASE(do_not_print_hidden, PrinterFixture)
{
	printer.recursively().print(app, test::usage);
	auto output = stream.str();
	BOOST_CHECK(output.find("Hidden argument") == output.npos);
	BOOST_CHECK(output.find("Nameless argument") == output.npos);
}

BOOST_FIXTURE_TEST_CASE(print_hidden, PrinterFixture)
{
	printer.showHidden().recursively().print(app, test::usage);
	auto output = stream.str();
	BOOST_CHECK(output.find("Hidden argument") != output.npos);
}

BOOST_FIXTURE_TEST_CASE(print_recursively, PrinterFixture)
{
	printer.recursively().print(app, test::usage);
	auto output = stream.str();
	BOOST_CHECK(output.find("--flag") != output.npos);
	BOOST_CHECK(output.find("--help") != output.npos);
	BOOST_CHECK(output.find("--call") != output.npos);
	BOOST_CHECK(output.find("--bool") != output.npos);
	BOOST_CHECK(output.find("--real") != output.npos);
	BOOST_CHECK(output.find("--pair") != output.npos);
	BOOST_CHECK(output.find("-1st") != output.npos);
	BOOST_CHECK(output.find("-2nd") != output.npos);
}

BOOST_FIXTURE_TEST_CASE(print_non_recursively, PrinterFixture)
{
	printer.print(app, test::usage);
	auto output = stream.str();
	BOOST_CHECK(output.find("--flag") != output.npos);
	BOOST_CHECK(output.find("--help") != output.npos);
	BOOST_CHECK(output.find("--call") != output.npos);
	BOOST_CHECK(output.find("--bool") == output.npos);
	BOOST_CHECK(output.find("--real") == output.npos);
	BOOST_CHECK(output.find("--pair") == output.npos);
	BOOST_CHECK(output.find("-1st") == output.npos);
	BOOST_CHECK(output.find("-2nd") == output.npos);
}

BOOST_AUTO_TEST_SUITE_END()
