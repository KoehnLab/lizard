// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace lizard {
namespace details {
	/**
	 * This dummy template is used to prevent the compiler from automatic template parameter deduction, therefore making
	 * specifying them explicitly a requirement to using the template.
	 */
	template< typename T > using dont_deduce = std::common_type_t< T >;
} // namespace details

enum class RotateDirection { Left, Right };

/**
 * Bit-rotate the given integer by the given amount into the given direction
 */
template< typename Integer, RotateDirection direction = RotateDirection::Right >
constexpr auto bit_rotate(details::dont_deduce< Integer > value, unsigned int amount = 1) -> Integer {
	// Implementation mostly copied from https://gist.github.com/pabigot/7550454
	// See also https://blog.regehr.org/archives/1063
	// Note: Modern compilers should be able to see through this and replace with the appropriate
	// rotate instruction, if the hardware supports it
	static_assert(std::is_integral_v< Integer >, "bit_rotate can only deal with integers");
	static_assert(!std::is_signed_v< Integer >, "Bit rotations only make sense for unsigned types");

	constexpr const unsigned int bitWidth = static_cast< unsigned int >(std::numeric_limits< Integer >::digits);

	static_assert(bitWidth > 0);
	static_assert((bitWidth & (bitWidth - 1)) == 0,
				  "To-be-rotated integer type has a bitWidth that is not a power of two");

	using promoted_type          = std::common_type_t< int, Integer >;
	using unsigned_promoted_type = std::make_unsigned_t< promoted_type >;

	// The shift amount must always be < the number of bits in the shifted integer
	constexpr unsigned int countMask{ bitWidth - 1 };
	amount &= countMask;

	if constexpr (direction == RotateDirection::Left) {
		return (unsigned_promoted_type{ value } << amount) | (unsigned_promoted_type{ value } >> (-amount & countMask));
	} else {
		return (unsigned_promoted_type{ value } >> amount) | (unsigned_promoted_type{ value } << (-amount & countMask));
	}
}

} // namespace lizard
