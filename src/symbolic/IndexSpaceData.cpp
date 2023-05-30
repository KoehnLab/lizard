// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/IndexSpaceData.hpp"

namespace lizard {

IndexSpaceData::IndexSpaceData(std::string name, unsigned int size, Spin defaultSpin, std::vector< char > labels,
							   char labelExtension)
	: m_name(std::move(name)), m_size(size), m_defaultSpin(defaultSpin), m_labels(std::move(labels)),
	  m_labelExtension(labelExtension) {
}

auto IndexSpaceData::getName() const -> const std::string & {
	return m_name;
}

auto IndexSpaceData::getSize() const -> unsigned int {
	return m_size;
}

auto IndexSpaceData::getDefaultSpin() const -> Spin {
	return m_defaultSpin;
}

auto IndexSpaceData::getLabels() const -> const std::vector< char > & {
	return m_labels;
}

auto IndexSpaceData::getLabelExtension() const -> char {
	return m_labelExtension;
}

} // namespace lizard
