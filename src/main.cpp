#include "lizard/core/Exception.hpp"
#include "lizard/parser/GeCCoExportParser.hpp"

#include <exception>

int main() {
	try {
		lizard::parser::GeCCoExportParser::parse(
			"/home/robert/Documents/University/Documents/Masterarbeit/Git/program/samples/CC2/CC2_EN.EXPORT");

		return 0;
	} catch (const std::exception &e) {
		lizard::core::print_exception_message(e);
	}
}
