// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/core/SignedCast.hpp"

namespace lizard {

template<>
auto signed_cast< std::make_signed_t< Numeric::numeric_type >, Numeric >(Numeric value)
	-> std::make_signed_t< Numeric::numeric_type > {
	return signed_cast< std::make_signed_t< Numeric::numeric_type > >(static_cast< Numeric::numeric_type >(value));
}

} // namespace lizard
