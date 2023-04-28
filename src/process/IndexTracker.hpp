// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/Index.hpp"

#include <nonstd/span.hpp>

#include <cstdint>
#include <vector>

namespace lizard {

/**
 * Helper class used to track the positions of indices of different type in a given index sequence
 */
class IndexTracker {
public:
	IndexTracker() = default;
	IndexTracker(nonstd::span< const Index > indices);

	/**
	 * Tracks the indices in the provided sequence
	 */
	void track(nonstd::span< const Index > indices);

	/**
	 * @returns A list of creator index positions in the currently tracked index sequence
	 */
	[[nodiscard]] auto creators() const -> const std::vector< std::size_t > &;

	/**
	 * @returns A list of annihilator index positions in the currently tracked index sequence
	 */
	[[nodiscard]] auto annihilators() const -> const std::vector< std::size_t > &;

	/**
	 * @returns A list of external index positions in the currently tracked index sequence
	 */
	[[nodiscard]] auto externals() const -> const std::vector< std::size_t > &;

private:
	std::vector< std::size_t > m_creators;
	std::vector< std::size_t > m_annihilators;
	std::vector< std::size_t > m_externals;
};

} // namespace lizard
