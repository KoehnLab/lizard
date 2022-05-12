// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "ErrorReporter.hpp"
#include "lizard/parser/ParseException.hpp"

namespace lizard::parser {

ErrorReporter::ErrorReporter(std::string_view fileName) : m_fileName(fileName) {
}

void ErrorReporter::syntaxError(antlr4::Recognizer *, antlr4::Token *, std::size_t line, std::size_t column,
								const std::string &msg, std::exception_ptr) {
	std::string errorMessage;
	if (!m_fileName.empty()) {
		errorMessage = m_fileName + ":";
	} else {
		errorMessage = "line ";
	}

	errorMessage += std::to_string(line) + ":" + std::to_string(column) + ": " + msg;

	throw ParseException(errorMessage);
}

} // namespace lizard::parser
