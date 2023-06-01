// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <cstdint>
#include <iosfwd>

namespace lizard {

/**
 * Enumeration of possible index types
 */
enum class IndexType : std::int8_t {
	Annihilator = -1,
	External    = 0,
	Creator     = 1,
};

auto operator<<(std::ostream &stream, IndexType type) -> std::ostream &;

} // namespace lizard
