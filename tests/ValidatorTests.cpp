#include "tests.hpp"

BOOST_AUTO_TEST_SUITE(ValidatorTests)

BOOST_AUTO_TEST_CASE(check_min_number)
{
	auto validator = az::cli::Validator().integer().min(3);
	BOOST_REQUIRE_NO_THROW(validator.check("5"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("2"), az::cli::Error::Code::TooSmall);
}

BOOST_AUTO_TEST_CASE(check_max_number)
{
	auto validator = az::cli::Validator().integer().max(3);
	BOOST_REQUIRE_NO_THROW(validator.check("2"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("5"), az::cli::Error::Code::TooLarge);
}

BOOST_AUTO_TEST_CASE(check_min_string)
{
	auto validator = az::cli::Validator().string().min(4);
	BOOST_REQUIRE_NO_THROW(validator.check("good"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("bad"), az::cli::Error::Code::TooShort);
}

BOOST_AUTO_TEST_CASE(check_max_string)
{
	auto validator = az::cli::Validator().string().max(2);
	BOOST_REQUIRE_NO_THROW(validator.check("ok"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("bad"), az::cli::Error::Code::TooLong);
}

BOOST_AUTO_TEST_CASE(check_one_of)
{
	auto validator = az::cli::Validator().one_of({true, 123, 3.14, "str"});
	BOOST_REQUIRE_NO_THROW(validator.check("true"));
	BOOST_REQUIRE_NO_THROW(validator.check("123"));
	BOOST_REQUIRE_NO_THROW(validator.check("3.14"));
	BOOST_REQUIRE_NO_THROW(validator.check("str"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("bad"), az::cli::Error::Code::InvalidValue);
}

BOOST_AUTO_TEST_CASE(check_pattern)
{
	auto validator = az::cli::Validator().pattern("\\w+");
	BOOST_REQUIRE_NO_THROW(validator.check("123abc"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("what?"), az::cli::Error::Code::InvalidValue);
}

BOOST_AUTO_TEST_CASE(check_duplication)
{
	auto validator = az::cli::Validator().no_duplicates_of("@|");
	BOOST_REQUIRE_NO_THROW(validator.check("user@host"));
	BOOST_REQUIRE_NO_THROW(validator.check("one|two"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("user@do@men"), az::cli::Error::Code::DuplicateChar);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("one|two|three"), az::cli::Error::Code::DuplicateChar);
}

BOOST_AUTO_TEST_CASE(check_repetition)
{
	auto validator = az::cli::Validator().no_repeats_of(".,");
	BOOST_REQUIRE_NO_THROW(validator.check("1.2.3"));
	BOOST_REQUIRE_NO_THROW(validator.check("1,2,3"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("1.2..3"), az::cli::Error::Code::RepetitiveChar);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("1,2,,3"), az::cli::Error::Code::RepetitiveChar);
}

BOOST_AUTO_TEST_CASE(cannot_start_with)
{
	auto validator = az::cli::Validator().cannot_start_with("!?");
	BOOST_REQUIRE_NO_THROW(validator.check("Hello!"));
	BOOST_REQUIRE_NO_THROW(validator.check("What?"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("!Hello"), az::cli::Error::Code::InvalidStart);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("?What"), az::cli::Error::Code::InvalidStart);
}

BOOST_AUTO_TEST_CASE(cannot_end_with)
{
	auto validator = az::cli::Validator().cannot_end_with(",:");
	BOOST_REQUIRE_NO_THROW(validator.check("Hi, man!"));
	BOOST_REQUIRE_NO_THROW(validator.check("0 : 0"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("Hi,"), az::cli::Error::Code::InvalidEnd);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("No:"), az::cli::Error::Code::InvalidEnd);
}

BOOST_AUTO_TEST_CASE(cannot_start_end_with)
{
	auto validator = az::cli::Validator().cannot_start_end_with("+=");
	BOOST_REQUIRE_NO_THROW(validator.check("1+1=2"));
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("1+1="), az::cli::Error::Code::InvalidEnd);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(validator.check("+1+1=2"), az::cli::Error::Code::InvalidStart);
}

BOOST_AUTO_TEST_CASE(trim_by_default)
{
	auto validator = az::cli::Validator().trim();
	BOOST_CHECK_EQUAL(validator.amend(" \t\r\nok \t\r\n"), "ok");
}

BOOST_AUTO_TEST_CASE(trim_by_custom)
{
	auto validator = az::cli::Validator().trim(":[]");
	BOOST_CHECK_EQUAL(validator.amend("[123]"), "123");
	BOOST_CHECK_EQUAL(validator.amend(":one:"), "one");
}

BOOST_AUTO_TEST_CASE(amend_lower_case)
{
	auto validator = az::cli::Validator().lower_case();
	BOOST_CHECK_EQUAL(validator.amend("Yes"), "yes");
	BOOST_CHECK_EQUAL(validator.amend("YES"), "yes");
}

BOOST_AUTO_TEST_CASE(amend_upper_case)
{
	auto validator = az::cli::Validator().upper_case();
	BOOST_CHECK_EQUAL(validator.amend("yes"), "YES");
	BOOST_CHECK_EQUAL(validator.amend("Yes"), "YES");
}

BOOST_AUTO_TEST_CASE(amend_prefix)
{
	auto validator = az::cli::Validator().prefix("http://");
	BOOST_CHECK_EQUAL(validator.amend("www"), "http://www");
	BOOST_CHECK_EQUAL(validator.amend("http://www"), "http://www");
}

BOOST_AUTO_TEST_CASE(amend_suffix)
{
	auto validator = az::cli::Validator().suffix(".ext");
	BOOST_CHECK_EQUAL(validator.amend("file"), "file.ext");
	BOOST_CHECK_EQUAL(validator.amend("file.ext"), "file.ext");
}

BOOST_AUTO_TEST_CASE(apply_bool)
{
	auto validator = az::cli::Validator().boolean();
	auto value = validator.apply("true");
	BOOST_CHECK(value.isBool());
	BOOST_CHECK_EQUAL(bool(value), true);
}

BOOST_AUTO_TEST_CASE(apply_integer)
{
	auto validator = az::cli::Validator().integer();
	auto value = validator.apply("123");
	BOOST_CHECK(value.isInteger());
	BOOST_CHECK_EQUAL(int(value), 123);
}

BOOST_AUTO_TEST_CASE(apply_real)
{
	auto validator = az::cli::Validator().real();
	auto value = validator.apply("3.14");
	BOOST_CHECK(value.isReal());
	BOOST_CHECK_EQUAL(double(value), 3.14);
}

BOOST_AUTO_TEST_CASE(apply_glossary)
{
	auto validator = az::cli::Validator().glossary({{"yes", true}, {"one", 1}, {"pi", 3.14}, {"btw", "by the way"}});
	BOOST_CHECK(validator.apply("yes").isBool());
	BOOST_CHECK_EQUAL(bool(validator.apply("yes")), true);
	BOOST_CHECK(validator.apply("one").isInteger());
	BOOST_CHECK_EQUAL(int(validator.apply("one")), 1);
	BOOST_CHECK(validator.apply("pi").isReal());
	BOOST_CHECK_EQUAL(double(validator.apply("pi")), 3.14);
	BOOST_CHECK(validator.apply("btw").isString());
	BOOST_CHECK_EQUAL(std::string(validator.apply("btw")), "by the way");
	BOOST_CHECK_EQUAL(validator.apply("bad").asString(), "bad");
}

BOOST_AUTO_TEST_SUITE_END()
