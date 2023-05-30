// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/Spin.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace lizard {

/**
 * Collection of useful properties of a given index space
 */
class IndexSpaceData {
public:
	IndexSpaceData(std::string name, unsigned int size, Spin defaultSpin, std::vector< char > labels,
				   char labelExtension = '\'');

	/**
	 * @returns The name of the index space
	 */
	[[nodiscard]] auto getName() const -> const std::string &;
	/**
	 * @returns The size (amount of indices inside the space) of the index space
	 */
	[[nodiscard]] auto getSize() const -> unsigned int;
	/**
	 * @returns The spin that indices in this space have by default
	 */
	[[nodiscard]] auto getDefaultSpin() const -> Spin;
	/**
	 * @returns A list of labels to be used when printing indices from the index space
	 */
	[[nodiscard]] auto getLabels() const -> const std::vector< char > &;
	/**
	 * @returns A character that shall be used to extend labels of indices in the index space if
	 * the provided labels are too few
	 */
	[[nodiscard]] auto getLabelExtension() const -> char;

private:
	std::string m_name;
	unsigned int m_size;
	Spin m_defaultSpin;
	std::vector< char > m_labels;
	char m_labelExtension;
};

} // namespace lizard
