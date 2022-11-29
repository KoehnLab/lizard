// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_PARSER_GECCOEXPORTPARSER_HPP_
#define LIZARD_PARSER_GECCOEXPORTPARSER_HPP_

#include <filesystem>
#include <iosfwd>
#include <string_view>

namespace lizard::parser {

class GeCCoExportParser {
public:
	void parse(const std::filesystem::path &filePath);
	void parse(std::istream &inputStream, std::string_view fileName = "");
};

} // namespace lizard::parser

#endif // LIZARD_PARSER_GECCOEXPORTPARSER_HPP_
