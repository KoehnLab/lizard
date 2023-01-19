// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_CORE_SIGNED_CAST_HPP_
#define LIZARD_CORE_SIGNED_CAST_HPP_

#include "lizard/core/Numeric.hpp"

#include <cstring>
#include <type_traits>

namespace lizard {

/**
 * This function implements a conversion between the signed and unsigned variant of a given integer type.
 * The function is intended to be used for such casts as the regular static_cast has implementation-defined
 * behavior when e.g. casting from an unsigned type to the signed version, if the value represented by the
 * unsigned type can't be represented by the signed type (e.g. 255 as an std::int8_t).
 * This function always performs a conversion such that the binary representation of the integer value remains
 * unchanged. Therefore, it is always guaranteed that when casting e.g. from signed -> unsigned -> signed, the
 * same value is regained (the same also holds in the other direction).
 * Furthermore, the function guarantees that when converting a negative value to an unsigned type, the result
 * will represent a positive value, that is larger than the biggest representable number of the signed type.
 * Likewise, positive numbers that are representable by the signed type are guaranteed to retain their
 * represented value (e.g. unsigned_value == signed_cast< signed_type >(unsigned_value)).
 */
template< typename To, typename From > auto signed_cast(From value) -> To {
	static_assert(sizeof(To) == sizeof(From), "signed_cast can only convert between types of equal bit width");
	static_assert(std::is_integral_v< From > && std::is_integral_v< To >,
				  "signed_cast can only be applied to integral types");
	static_assert(std::is_signed_v< From > != std::is_signed_v< To >,
				  "signed_cast can not be used on two types with equal signedness");
	// In theory, the claims made in the function's description should also hold for other
	// integer representations. However, before we allow them, we have to perform more
	// extensive testing.
	static_assert(-1 == ~0, "signed_cast only implemented for two's complement architecture");

	// Note: Using memcpy seems a little too much overhead for such an operation. However, the vast
	// majority of compilers should optimize the memcpy call out.

	if constexpr (std::is_signed_v< From >) {
		// Convert to unsigned
		std::make_unsigned_t< From > convertedValue;
		std::memcpy(&convertedValue, &value, sizeof(From));

		return convertedValue;
	} else {
		// Convert to signed
		std::make_signed_t< From > convertedValue;
		std::memcpy(&convertedValue, &value, sizeof(From));

		return convertedValue;
	}
}

// Specialization for our Numeric type
template<>
auto signed_cast< std::make_signed_t< Numeric::numeric_type >, Numeric >(Numeric value)
	-> std::make_signed_t< Numeric::numeric_type >;

} // namespace lizard

#endif // LIZARD_CORE_SIGNED_CAST_HPP_
