// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <cstdint>
#include <iosfwd>

namespace lizard {

/**
 * Enumeration for representing the possible spin cases a given Index can be in
 */
enum class Spin : std::int8_t {
	/**
	 * Index runs only over beta-spin orbitals
	 */
	Beta = -1,
	/**
	 * Index is not associated with a spin
	 */
	None = 0,
	/**
	 * Index runs only over alpha-spin orbitals
	 */
	Alpha = 1,
	/**
	 * Index runs over both alpha- and beta-spin orbitals (spin-orbit formalism)
	 */
	Both,
};

auto operator<<(std::ostream &stream, Spin spin) -> std::ostream &;

} // namespace lizard
