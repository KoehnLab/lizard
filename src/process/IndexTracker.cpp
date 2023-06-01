// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "IndexTracker.hpp"

#include "lizard/symbolic/IndexType.hpp"

namespace lizard {

IndexTracker::IndexTracker(nonstd::span< const Index > indices) {
	track(indices);
}

void IndexTracker::track(nonstd::span< const Index > indices) {
	m_creators.clear();
	m_annihilators.clear();
	m_externals.clear();

	for (std::size_t i = 0; i < indices.size(); ++i) {
		switch (indices[i].getType()) {
			case IndexType::Creator:
				m_creators.push_back(i);
				break;
			case IndexType::Annihilator:
				m_annihilators.push_back(i);
				break;
			case IndexType::External:
				m_externals.push_back(i);
				break;
		}
	}
}

auto IndexTracker::creators() const -> const std::vector< std::size_t > & {
	return m_creators;
}


auto IndexTracker::annihilators() const -> const std::vector< std::size_t > & {
	return m_annihilators;
}
auto IndexTracker::externals() const -> const std::vector< std::size_t > & {
	return m_externals;
}

} // namespace lizard
