#include "tests.hpp"

BOOST_AUTO_TEST_SUITE(ErrorTests)

BOOST_AUTO_TEST_CASE(make_empty_error)
{
	auto error = az::cli::Error(az::cli::Error::Code::None);
	BOOST_CHECK(error.code() == az::cli::Error::Code::None);
	BOOST_CHECK(!error.hasArgument());
	BOOST_CHECK(!error.hasValue());
	BOOST_CHECK(!error.hasHelp());
	BOOST_CHECK_EQUAL(error.what(), "no error");
	BOOST_CHECK_EQUAL(error.argument(), "");
	BOOST_CHECK_EQUAL(error.value(), "");
	BOOST_CHECK_EQUAL(error.help(), "");
}

BOOST_AUTO_TEST_CASE(make_solid_error)
{
	auto error = az::cli::Error(az::cli::Error::Code::InvalidArgument).argument("argument").value("value").help("help");
	BOOST_CHECK(error.code() == az::cli::Error::Code::InvalidArgument);
	BOOST_CHECK(error.hasArgument());
	BOOST_CHECK(error.hasValue());
	BOOST_CHECK(error.hasHelp());
	BOOST_CHECK_EQUAL(error.what(), "invalid argument");
	BOOST_CHECK_EQUAL(error.argument(), "argument");
	BOOST_CHECK_EQUAL(error.value(), "value");
	BOOST_CHECK_EQUAL(error.help(), "help");
}

BOOST_AUTO_TEST_SUITE_END()
