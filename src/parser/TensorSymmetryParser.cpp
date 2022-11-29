// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/parser/TensorSymmetryParser.hpp"
#include "lizard/parser/ParseException.hpp"
#include "lizard/parser/autogen/TensorSymmetryLexer.h"
#include "lizard/parser/autogen/TensorSymmetryParser.h"

#include "ErrorReporter.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <Exceptions.h>
#include <tree/ParseTree.h>


namespace lizard::parser {

void TensorSymmetryParser::parse(const std::filesystem::path &filePath) {
	std::ifstream stream(filePath);
	parse(stream, filePath.string());
}

void TensorSymmetryParser::parse(std::istream &inputStream, std::string_view fileName) {
	try {
		ErrorReporter reporter(fileName);

		antlr4::ANTLRInputStream antlrInStream(inputStream);

		autogen::TensorSymmetryLexer lexer(&antlrInStream);
		lexer.removeErrorListeners();
		lexer.addErrorListener(&reporter);

		antlr4::CommonTokenStream tokens(&lexer);

		tokens.fill();

		autogen::TensorSymmetryParser parser(&tokens);

		parser.removeErrorListeners();
		parser.addErrorListener(&reporter);
		antlr4::tree::ParseTree *tree = parser.body();

		std::cout << tree->toStringTree(&parser, true) << std::endl;
	} catch (const antlr4::RuntimeException &) {
		std::throw_with_nested(ParseException("Parsing tensor symmetry file failed."));
	}
}

} // namespace lizard::parser
