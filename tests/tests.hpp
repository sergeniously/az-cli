#pragma once
#include <boost/test/unit_test.hpp>
#include "Error.h"
#include "Value.h"
#include "Validator.h"
#include "Argument.h"
#include "Interpreter.h"
#include "Printer.h"

#define CUSTOM_REQUIRE_THROW_CLI_ERROR(statement, error_code) \
	BOOST_REQUIRE_EXCEPTION(statement, az::cli::Error, [](const az::cli::Error& error){ return error.code() == error_code; });

namespace test
{

// define arguments ids to describe application usage
enum Arg {
	APP = 0,
	HELP,
	CALL,
	FLAG,
	BOOL,
	INTEGER,
	REAL,
	STRING,
	ARRAY,
	PAIR,
	PAIR_FIRST,
	PAIR_SECOND,
	NAMELESS,
	HIDDEN
};

// Callback for active argument
extern az::cli::Argument::Action action;

// This function describes all arguments of the application according to their hierarchy
std::list<az::cli::Arg> usage(const az::cli::Arg& arg);

}
