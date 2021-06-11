#include "Printer.h"
#include <sstream>
#include <stdlib.h>

namespace az::cli
{

Printer::Printer(std::ostream& stream, const Options& options)
	: stream(stream), options(options)
{
}

Printer& Printer::withIndentation(int indent)
{
	options.indentation = indent;
	return *this;
}

Printer& Printer::withWidth(int width)
{
	options.width = width;
	return *this;
}

Printer& Printer::withDivider(char divider)
{
	options.divider = divider;
	return *this;
}

Printer& Printer::recursively(bool whether)
{
	options.recursively = whether;
	return *this;
}

Printer& Printer::showHidden(bool whether)
{
	options.show_hidden = whether;
	return *this;
}

int Printer::print(const Argument& argument, const Usage& usage, int margin) const
{
	if (!argument.isHidden() || options.show_hidden) {
		print(argument, margin);
		for (const auto& sub_argument : usage(argument)) {
			if (options.recursively) {
				print(sub_argument, usage, margin + options.indentation);
			}
			else if (!sub_argument.isHidden() || options.show_hidden) {
				print(sub_argument, margin + options.indentation);
			}
		}
	}
	return argument.id();
}

void Printer::print(const Argument& argument, int margin) const
{
	auto offset = [](int spaces){ return std::string(spaces, ' '); };

	const auto& instruction = argument.getInstruction();
	stream << offset(margin) << instruction;

	std::stringstream description(argument.getDescription());
	if (!description.eof()) {
		std::string description_line;
		size_t instruction_length = std::mbstowcs(nullptr, instruction.c_str(), 0);
		int description_indent = options.width - instruction_length;
		while (description_indent <= 0 || std::getline(description, description_line)) {
			if (!description_line.empty()) {
				stream << offset(description_indent) << options.divider << " " << description_line;
			}
			description_indent = margin + options.width;
			stream << std::endl;
		}
	}
}

}
