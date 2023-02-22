// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/parser/GeCCoExportParser.hpp"
#include "lizard/parser/ParseException.hpp"
#include "lizard/parser/TensorSymmetryParser.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <string_view>

namespace lizard::test::parser {

template< typename Parser, bool shouldError > void parseFilesIn(std::string_view inputFilePath) {
	Parser parser;

	for (const std::filesystem::directory_entry &currentEntry :
		 std::filesystem::recursive_directory_iterator(inputFilePath)) {
		if (!currentEntry.is_regular_file() && !currentEntry.is_symlink()) {
			continue;
		}

		try {
			parser.parse(currentEntry.path());

			if (shouldError) {
				FAIL() << "Parsing of invalid file " << currentEntry.path() << " succeeded without errors";
			}
		} catch (::lizard::parser::ParseException &e) {
			if (!shouldError) {
				FAIL() << "Parsing of valid file produced error\n" << e.what();
			}
		}
	}
}

TEST(Parser, gecco_export_parse_valid) {
	parseFilesIn<::lizard::parser::GeCCoExportParser, false >(TEST_FILE_DIR "/parser_input/GeCCoExport/valid");
}

TEST(Parser, gecco_export_parse_invalid) {
	parseFilesIn<::lizard::parser::GeCCoExportParser, true >(TEST_FILE_DIR "/parser_input/GeCCoExport/invalid");
}

TEST(Parser, tensor_symmetry_parse_valid) {
	parseFilesIn<::lizard::parser::TensorSymmetryParser, false >(TEST_FILE_DIR "/parser_input/TensorSymmetry/valid");
}

TEST(Parser, tensor_symmetry_parse_invalid) {
	parseFilesIn<::lizard::parser::TensorSymmetryParser, true >(TEST_FILE_DIR "/parser_input/TensorSymmetry/invalid");
}

} // namespace lizard::test::parser
