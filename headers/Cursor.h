#pragma once

namespace az::cli
{

// Class for access to program args
class Cursor {
	const char** arg_vector = nullptr;
	uint32_t arg_count = 0;
	uint32_t arg_index = 0;
public:
	Cursor(const char** argv, uint32_t argc)
		: arg_vector(argv), arg_count(argc) {}
	Cursor& operator++() {
		arg_index++;
		return *this;
	}
	int pos() const {
		return arg_index;
	}
	// end of line
	bool eol() const {
		return arg_index >= arg_count;
	}
	operator bool() const {
		return !eol();
	}
	const char* arg() const {
		return eol() ? nullptr : arg_vector[arg_index];
	}
};

}
