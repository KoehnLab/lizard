// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexType.hpp"

#include <cstdint>
#include <functional>
#include <iosfwd>


namespace lizard {

/**
 * Symbolic representation of a single index
 */
class Index {
public:
	/**
	 * The numeric type used to represent index IDs
	 */
	using Id = std::uint8_t;

	/**
	 * Helper class that can act as a unary predicate to find the provided Index by its "name",
	 * which consists of its ID and the ID of the IndexSpace it belongs to.
	 * Thus, the name explicitly does NOT depend on the index's type or the respective space's spin.
	 */
	class FindByName {
	public:
		FindByName(const Index &idx) : m_idx(idx) {}

		/**
		 * @returns Whether the provided Index matches the search criteria
		 */
		[[nodiscard]] auto operator()(const Index &other) -> bool {
			return m_idx.getID() == other.getID() && IndexSpace::FindByID{ m_idx.getSpace() }(other.getSpace());
		}

	private:
		const Index &m_idx;
	};


	Index() = default;
	Index(Id id, IndexSpace space, IndexType type);

	/**
	 * @returns The Index's ID
	 */
	[[nodiscard]] auto getID() const -> Id;
	/**
	 * Sets the ID of this Index
	 */
	void setID(Id id);

	/**
	 * @returns The IndexSpace this Index is belonging to
	 */
	[[nodiscard]] auto getSpace() const -> const IndexSpace &;
	/**
	 * Sets this index's IndexSpace
	 */
	void setSpace(IndexSpace space);

	/**
	 * @returns The IndexType of this index
	 */
	[[nodiscard]] auto getType() const -> IndexType;
	/**
	 * Sets the IndexType of this index
	 */
	void setType(IndexType type);

private:
	Id m_id            = 0;
	IndexSpace m_space = {};
	IndexType m_type   = IndexType::External;
};

[[nodiscard]] auto operator==(const Index &lhs, const Index &rhs) -> bool;
[[nodiscard]] auto operator!=(const Index &lhs, const Index &rhs) -> bool;
[[nodiscard]] auto operator<(const Index &lhs, const Index &rhs) -> bool;
[[nodiscard]] auto operator<=(const Index &lhs, const Index &rhs) -> bool;
[[nodiscard]] auto operator>(const Index &lhs, const Index &rhs) -> bool;
[[nodiscard]] auto operator>=(const Index &lhs, const Index &rhs) -> bool;

auto operator<<(std::ostream &stream, const Index &index) -> std::ostream &;

} // namespace lizard

template<> struct std::hash< lizard::Index > {
	[[nodiscard]] auto operator()(const lizard::Index &index) -> std::size_t;
};
