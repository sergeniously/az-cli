#include "tests.hpp"

BOOST_AUTO_TEST_SUITE(ValueTests)

BOOST_AUTO_TEST_CASE(testNone)
{
	az::cli::Value value;
	BOOST_CHECK_EQUAL(value.getType(), az::cli::Value::Type::None);
	BOOST_CHECK(value.isNone());
	BOOST_CHECK(value.empty());
	BOOST_CHECK_EQUAL(value.size(), 0);
	BOOST_CHECK(!value.contains(123));
}

BOOST_AUTO_TEST_CASE(testBoolTrue)
{
	az::cli::Value value(true);
	BOOST_CHECK_EQUAL(value.getType(), az::cli::Value::Type::Bool);
	BOOST_CHECK(value.isBool());
	BOOST_CHECK(value);
	BOOST_CHECK(!value.empty());
	CUSTOM_REQUIRE_THROW_CLI_ERROR(value.size(), az::cli::Error::Code::WrongType);
}

BOOST_AUTO_TEST_CASE(testBoolFalse)
{
	az::cli::Value value(false);
	BOOST_CHECK_EQUAL(value.getType(), az::cli::Value::Type::Bool);
	BOOST_CHECK(value.isBool());
	BOOST_CHECK(!value);
	BOOST_CHECK(!value.empty());
	CUSTOM_REQUIRE_THROW_CLI_ERROR(value.size(), az::cli::Error::Code::WrongType);
}

BOOST_AUTO_TEST_CASE(testInteger)
{
	az::cli::Value value(123);
	BOOST_CHECK_EQUAL(value.getType(), az::cli::Value::Type::Integer);
	BOOST_CHECK(value.isInteger());
	BOOST_CHECK(value.isNumber());
	BOOST_CHECK(!value.empty());
}

BOOST_AUTO_TEST_CASE(testReal)
{
	az::cli::Value value(3.14);
	BOOST_CHECK_EQUAL(value.getType(), az::cli::Value::Type::Real);
	BOOST_CHECK(value.isReal());
	BOOST_CHECK(value.isNumber());
	BOOST_CHECK(!value.empty());
}

BOOST_AUTO_TEST_CASE(testString)
{
	az::cli::Value value("text");
	BOOST_CHECK_EQUAL(value.getType(), az::cli::Value::Type::String);
	BOOST_CHECK(value.isString());
	BOOST_CHECK(!value.empty());
	BOOST_CHECK_EQUAL(value.size(), 4);
	BOOST_CHECK(value.contains("ex"));
	BOOST_CHECK(!value.contains("fx"));
}

BOOST_AUTO_TEST_CASE(testArray)
{
	az::cli::Value value({true, 123, 3.14, "text"});
	BOOST_CHECK_EQUAL(value.getType(), az::cli::Value::Type::Array);
	BOOST_CHECK(value.isArray());
	BOOST_CHECK(!value.empty());
	BOOST_CHECK_EQUAL(value.size(), 4);
	BOOST_CHECK(value[0].isBool());
	BOOST_CHECK(value[1].isInteger());
	BOOST_CHECK(value[2].isReal());
	BOOST_CHECK(value[3].isString());
	CUSTOM_REQUIRE_THROW_CLI_ERROR(value[4], az::cli::Error::Code::TooFew);
	BOOST_CHECK(value.contains(true));
	BOOST_CHECK(value.contains(123));
	BOOST_CHECK(value.contains(3.14));
	BOOST_CHECK(value.contains("text"));
	BOOST_CHECK(!value.contains("no"));
}

BOOST_AUTO_TEST_CASE(makeStringFromChar)
{
	az::cli::Value value(char('c'));
	BOOST_CHECK(value.isString());
	BOOST_CHECK_EQUAL(value.size(), 1);
	BOOST_CHECK_EQUAL(value.asString(), "c");
}

BOOST_AUTO_TEST_CASE(makeEmptyArray)
{
	az::cli::Value value(az::cli::Value::Type::Array);
	BOOST_CHECK(value.isArray());
	BOOST_CHECK(value.empty());
	BOOST_CHECK_EQUAL(value.size(), 0);
}

BOOST_AUTO_TEST_CASE(appendToNone)
{
	az::cli::Value value;
	value.append(123);
	BOOST_CHECK(value.isArray());
	BOOST_CHECK_EQUAL(value.size(), 1);
	BOOST_CHECK(value[0].isInteger());
}

BOOST_AUTO_TEST_CASE(appendToSimple)
{
	az::cli::Value value(123);
	value.append("str");
	BOOST_CHECK(value.isArray());
	BOOST_CHECK_EQUAL(value.size(), 1);
	BOOST_CHECK(value[0].isString());
}

BOOST_AUTO_TEST_CASE(appendToArray)
{
	az::cli::Value value({1,2});
	value.append(3.14);
	BOOST_CHECK(value.isArray());
	BOOST_CHECK_EQUAL(value.size(), 3);
	BOOST_CHECK(value[2].isReal());
}

BOOST_AUTO_TEST_CASE(convertToBool)
{
	BOOST_CHECK_EQUAL(bool(az::cli::Value(0)), false);
	BOOST_CHECK_EQUAL(bool(az::cli::Value(1)), true);
	BOOST_CHECK_EQUAL(bool(az::cli::Value(0.0)), false);
	BOOST_CHECK_EQUAL(bool(az::cli::Value(0.1)), true);
	BOOST_CHECK_EQUAL(bool(az::cli::Value("false")), false);
	BOOST_CHECK_EQUAL(bool(az::cli::Value("true")), true);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(bool(az::cli::Value("idk")), az::cli::Error::Code::InvalidValue);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(bool(az::cli::Value({})), az::cli::Error::Code::WrongType);
}

BOOST_AUTO_TEST_CASE(convertToInteger)
{
	BOOST_CHECK_EQUAL(int(az::cli::Value(true)), 1);
	BOOST_CHECK_EQUAL(int(az::cli::Value(false)), 0);
	BOOST_CHECK_EQUAL(int(az::cli::Value(0.1)), 0);
	BOOST_CHECK_EQUAL(int(az::cli::Value(3.14)), 3);
	BOOST_CHECK_EQUAL(int(az::cli::Value("123")), 123);
	BOOST_CHECK_EQUAL(int(az::cli::Value("-123")), -123);
	BOOST_CHECK_EQUAL(int(az::cli::Value("3.14")), 3);
	BOOST_CHECK_EQUAL(int(az::cli::Value("123abc")), 123);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(int(az::cli::Value("")), az::cli::Error::Code::InvalidValue);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(int(az::cli::Value("abc")), az::cli::Error::Code::InvalidValue);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(int(az::cli::Value({})), az::cli::Error::Code::WrongType);
}

BOOST_AUTO_TEST_CASE(convertToReal)
{
	BOOST_CHECK_EQUAL(double(az::cli::Value(true)), 1.0);
	BOOST_CHECK_EQUAL(double(az::cli::Value(false)), 0.0);
	BOOST_CHECK_EQUAL(double(az::cli::Value(123)), 123.0);
	BOOST_CHECK_EQUAL(double(az::cli::Value(-123)), -123.0);
	BOOST_CHECK_EQUAL(double(az::cli::Value("3.14")), 3.14);
	BOOST_CHECK_EQUAL(double(az::cli::Value("-1.23")), -1.23);
	BOOST_CHECK_EQUAL(double(az::cli::Value("123abc")), 123);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(double(az::cli::Value("")), az::cli::Error::Code::InvalidValue);
	CUSTOM_REQUIRE_THROW_CLI_ERROR(double(az::cli::Value("abc")), az::cli::Error::Code::InvalidValue);
}

BOOST_AUTO_TEST_CASE(convertToString)
{
	BOOST_CHECK_EQUAL(az::cli::Value(true).asString(), "true");
	BOOST_CHECK_EQUAL(az::cli::Value(false).asString(), "false");
	BOOST_CHECK_EQUAL(az::cli::Value(123).asString(), "123");
	BOOST_CHECK_EQUAL(az::cli::Value(-123).asString(), "-123");
	BOOST_CHECK_EQUAL(az::cli::Value(3.14).asString(), "3.14");
	BOOST_CHECK_EQUAL(az::cli::Value(-100.0).asString(), "-100");
	BOOST_CHECK_EQUAL(az::cli::Value(0.00).asString(), "0");
	CUSTOM_REQUIRE_THROW_CLI_ERROR(az::cli::Value({true, 2, 3.14}).asString(), az::cli::Error::Code::WrongType);
}

BOOST_AUTO_TEST_CASE(convertToArray)
{
	az::cli::Value value(123);
	value.convert(az::cli::Value::Type::Array);
	BOOST_CHECK(value.isArray());
	BOOST_CHECK_EQUAL(value.size(), 1);
	BOOST_CHECK_EQUAL(value[0].getType(), az::cli::Value::Type::Integer);
	BOOST_CHECK(value.contains(123));
}

BOOST_AUTO_TEST_SUITE_END()
