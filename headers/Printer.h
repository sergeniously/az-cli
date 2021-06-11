#pragma once
#include <set>
#include "Argument.h"

namespace az::cli
{

class Printer
{
public:
	struct Options {
		Options() {}
		bool recursively = false; // print all hierarchy (argument & its children)
		bool show_hidden = false; // print even hidden arguments
		int indentation = 3; // hierarchy indentation
		int width = 48; // column width
		char divider = '#'; // column divider
	};
	Printer(std::ostream& stream = std::cout, const Options& options = {});
	Printer& withIndentation(int);
	Printer& withWidth(int);
	Printer& withDivider(char);
	Printer& showHidden(bool whether = true);
	Printer& recursively(bool whether = true);

	// Print the @Argument's hierarchy and return its id
	int print(const Argument&, const Usage&, int margin = 0) const;

	// Print the @Argument into the @stream indented by @margin spaces
	void print(const Argument&, int margin = 0) const;

private:
	std::ostream& stream;
	Options options;
};

}
