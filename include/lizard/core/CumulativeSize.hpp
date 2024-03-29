// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <cstddef>

namespace lizard {

/**
 * Metaprogramming helper that stores the cumulative octet (byte) size of the given data types in its static "value"
 * data field. The cumulative size is defined as the sum of the result of the sizeof operator applied to the individual
 * types.
 */
template< typename... Ts > struct cumulative_octet_size {
	static constexpr const std::size_t value = (sizeof(Ts) + ...);
};

// Specialization for an empty list
template<> struct cumulative_octet_size<> { static constexpr const std::size_t value = 0; };

/**
 * The cumulative octet (byte) size of the given types. (sum sizeof(T) for all T in the list of given types)
 */
template< typename... Ts > constexpr std::size_t cumulative_octet_size_v = cumulative_octet_size< Ts... >::value;

/**
 * The cumulative bit size of the given types. (sum sizeof(T)*8 for all T in the list of given types)
 */
template< typename... Ts > constexpr std::size_t cumulative_bit_size_v = cumulative_octet_size< Ts... >::value * 8;

} // namespace lizard
