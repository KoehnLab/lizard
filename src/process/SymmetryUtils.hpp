// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <libperm/AbstractPermutationGroup.hpp>
#include <libperm/Cycle.hpp>
#include <libperm/ExplicitPermutation.hpp>
#include <libperm/PrimitivePermutationGroup.hpp>

#include <nonstd/span.hpp>

#include <vector>

namespace lizard {

/**
 * Creates a group generated by all pairwise antisymmetric exchanges of the provided positions
 */
[[nodiscard]] auto makeAntisymmetricExchanges(nonstd::span< const std::size_t > elementPositions)
	-> perm::PrimitivePermutationGroup;

/**
 * Creates a group which is generated by all individual groups of antisymmetric exchanges within the provided
 * position lists.
 */
[[nodiscard]] auto makeDiscontinousAntisymmetricRanges(const std::vector< nonstd::span< const std::size_t > > &ranges)
	-> perm::PrimitivePermutationGroup;

/**
 * Generates a group containing the column-symmetric exchanges within the two groups. Column-symmetric means that
 * corresponding entries between the two groups can be exchanges with any other pair of corresponding entries
 * (without sign change).
 * The correspondence of positions is determined by their index in the respective position ranges.
 */
[[nodiscard]] auto makeColumnsymmetricExchanges(nonstd::span< const std::size_t > firstGroup,
												nonstd::span< const std::size_t > secondGroup)
	-> perm::PrimitivePermutationGroup;

[[nodiscard]] auto containsAntisymmetryOf(const perm::AbstractPermutationGroup &symmetry,
										  nonstd::span< const std::size_t > positions) -> bool;

} // namespace lizard
