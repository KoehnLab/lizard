// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexSpaceData.hpp"

#include <vector>

namespace lizard {

/**
 * The IndexSpaceManager is responsible for storing meta data (IndexSpaceData) for existing index spaces
 * and be able to convert between spaces and data.
 */
class IndexSpaceManager {
public:
	IndexSpaceManager() = default;

	/**
	 * Registers a new IndexSpace object with this manager
	 *
	 * @param space The space to register
	 * @param data The IndexSpaceData that shall be associated with the registered space
	 *
	 * @throws InvalidIndexSpaceException if the provided IndexSpace is already registered
	 * (index spaces that differ only by their Spin are considered to be equal for this purpose)
	 */
	void registerSpace(IndexSpace space, IndexSpaceData data);

	/**
	 * @param space The IndexSpace to obtain the data of
	 * @returns The corresponding IndexSpaceData for the provided space
	 *
	 * @throws InvalidIndexSpaceException if no such index space has been registered with this manager
	 */
	[[nodiscard]] auto getData(const IndexSpace &space) const -> const IndexSpaceData &;

	/**
	 * @param name The name of the space to create
	 * @returns A new instance of the index space with the given name
	 *
	 * @throws InvalidIndexSpaceException if there is no index space with the given name that was
	 * registered with this manager
	 */
	[[nodiscard]] auto createFromName(std::string_view name) const -> IndexSpace;

private:
	struct Pair {
		IndexSpace space;
		IndexSpaceData data;
	};
	class FindBySpace;
	class FindByName;
	friend class IndexSpaceManager::FindBySpace;
	friend class IndexSpaceManager::FindByName;

	std::vector< Pair > m_spaces;
};

} // namespace lizard
