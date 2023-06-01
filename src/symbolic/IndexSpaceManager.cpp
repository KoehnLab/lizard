// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/IndexSpaceManager.hpp"
#include "lizard/core/Utils.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/InvalidIndexSpaceException.hpp"

#include <algorithm>

namespace lizard {

class IndexSpaceManager::FindBySpace {
public:
	FindBySpace(IndexSpace space) : m_space(std::move(space)) {}

	auto operator()(const IndexSpaceManager::Pair &pair) const -> bool {
		return IndexSpace::FindByID{ pair.space }(m_space);
	}

private:
	IndexSpace m_space;
};

class IndexSpaceManager::FindByName {
public:
	FindByName(std::string_view name) : m_name(std::move(name)) {}

	auto operator()(const IndexSpaceManager::Pair &pair) const -> bool { return pair.data.getName() == m_name; }

private:
	std::string_view m_name;
};


void IndexSpaceManager::registerSpace(IndexSpace space, IndexSpaceData data) {
	if (std::find_if(m_spaces.begin(), m_spaces.end(), FindBySpace{ space }) != m_spaces.end()) {
		throw InvalidIndexSpaceException("Duplicate registration of index space " + toString(space));
	}
	if (std::find_if(m_spaces.begin(), m_spaces.end(), FindByName{ data.getName() }) != m_spaces.end()) {
		throw InvalidIndexSpaceException("Duplicate registration of index space " + toString(data.getName()));
	}

	// Ensure that the space object has the correct default spin
	space.setSpin(data.getDefaultSpin());

	m_spaces.push_back(Pair{ std::move(space), std::move(data) });
}

auto IndexSpaceManager::getData(const IndexSpace &space) const -> const IndexSpaceData & {
	auto iter = std::find_if(m_spaces.begin(), m_spaces.end(), FindBySpace{ space });

	if (iter == m_spaces.end()) {
		throw InvalidIndexSpaceException("Can't obtain data for unknown index space " + toString(space));
	}

	return iter->data;
}

auto IndexSpaceManager::createFromName(std::string_view name) const -> IndexSpace {
	auto iter = std::find_if(m_spaces.begin(), m_spaces.end(), FindByName{ name });

	if (iter == m_spaces.end()) {
		throw InvalidIndexSpaceException("No index space known with name '" + toString(name) + "'");
	}

	return iter->space;
}

auto IndexSpaceManager::createFromLabel(char label) const -> IndexSpace {
	for (const Pair &current : m_spaces) {
		auto iter = std::find(current.data.getLabels().begin(), current.data.getLabels().end(), label);

		if (iter != current.data.getLabels().end()) {
			return current.space;
		}
	}

	throw InvalidIndexSpaceException("No index space known for label '" + toString(label) + "'");
}

} // namespace lizard
