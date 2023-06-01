// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/Spin.hpp"

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <limits>

namespace lizard {

/**
 * Representation of an index space. The index space determines over which values a given index iterates.
 */
class IndexSpace {
public:
	using Id = std::uint8_t;

	/**
	 * Helper class that can be used as a unary predicate to find the provided IndexSpace by its ID.
	 * That means that the search explicitly ignores the space's spin.
	 */
	class FindByID {
	public:
		FindByID(const IndexSpace &space) : m_id(space.getID()) {}

		/**
		 * @returns Whether the given IndexSpace satisfies the search criteria
		 */
		[[nodiscard]] auto operator()(const IndexSpace &other) const -> bool { return m_id == other.getID(); }

	private:
		IndexSpace::Id m_id;
	};

	IndexSpace() = default;
	IndexSpace(Id identifier, Spin spin);

	/**
	 * @returns The ID of this space
	 */
	[[nodiscard]] auto getID() const -> Id;
	/**
	 * Sets the ID of this space
	 */
	void setID(Id identifier);

	/**
	 * @returns The Spin associated with this space
	 */
	[[nodiscard]] auto getSpin() const -> Spin;
	/**
	 * Sets the spin that is associated with this space
	 */
	void setSpin(Spin spin);

private:
	Id m_id     = std::numeric_limits< Id >::max();
	Spin m_spin = Spin::None;
};

[[nodiscard]] auto operator==(const IndexSpace &lhs, const IndexSpace &rhs) -> bool;
[[nodiscard]] auto operator!=(const IndexSpace &lhs, const IndexSpace &rhs) -> bool;
[[nodiscard]] auto operator<(const IndexSpace &lhs, const IndexSpace &rhs) -> bool;
[[nodiscard]] auto operator<=(const IndexSpace &lhs, const IndexSpace &rhs) -> bool;
[[nodiscard]] auto operator>(const IndexSpace &lhs, const IndexSpace &rhs) -> bool;
[[nodiscard]] auto operator>=(const IndexSpace &lhs, const IndexSpace &rhs) -> bool;

auto operator<<(std::ostream &stream, const IndexSpace &space) -> std::ostream &;

} // namespace lizard

template<> struct std::hash< lizard::IndexSpace > {
	[[nodiscard]] auto operator()(const lizard::IndexSpace &space) -> std::size_t;
};
