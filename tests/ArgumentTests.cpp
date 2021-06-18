#include "tests.hpp"

BOOST_AUTO_TEST_SUITE(ArgumentTests)

BOOST_AUTO_TEST_CASE(make_without_value)
{
	az::cli::Argument arg(1, {"-n", "--name", "name"}, "Description");
	BOOST_CHECK_EQUAL(arg.id(), 1);
	BOOST_CHECK_EQUAL(arg.getLongestKey(), "--name");
	BOOST_CHECK_EQUAL(arg.getKeysString(), "-n, --name, name");
	BOOST_CHECK_EQUAL(arg.getDescription(), "Description");
	BOOST_CHECK(!arg.needValue());
	BOOST_CHECK(!arg.isRequired());
}

BOOST_AUTO_TEST_CASE(make_with_optional_value)
{
	auto arg = az::cli::Argument().with_value();
	BOOST_CHECK(arg.needValue());
	BOOST_CHECK(!arg.isRequired());
}

BOOST_AUTO_TEST_CASE(make_with_required_value)
{
	auto arg = az::cli::Argument().required().with_value();
	BOOST_CHECK(arg.needValue());
	BOOST_CHECK(arg.isRequired());
}

BOOST_AUTO_TEST_CASE(make_with_default_value)
{
	auto arg = az::cli::Argument().with_value().by_default("foo");
	BOOST_CHECK(arg.hasDefaultValue());
	BOOST_CHECK_EQUAL(arg.getDefaultValue().asString(), "foo");
}

BOOST_AUTO_TEST_SUITE_END()
