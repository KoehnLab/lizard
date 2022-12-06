// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_CORE_MULTIENUM_HPP_
#define LIZARD_CORE_MULTIENUM_HPP_

#include "lizard/core/CumulativeSize.hpp"
#include "lizard/core/SizedStorage.hpp"
#include "lizard/core/VariadicHelpers.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace lizard::core {

namespace details {
	/**
	 * Gets the cumulative size of the first N types of the provided parameter pack in bits
	 */
	template< std::size_t N, typename T, typename... Ts > constexpr auto bit_width_of_first_n() -> std::size_t {
		static_assert(sizeof...(Ts) + 1 > N, "bit_width_of_first_n: Given N is larger than parameter pack");

		if constexpr (N > 0) {
			return sizeof(T) * 8 + bit_width_of_first_n< N - 1, Ts... >();
		}
		return 0;
	}

	/**
	 * Gets a bit pattern by setting bits_to_mask consecutive bits to 1 and then shifting that by bits_to_skip to the
	 * left. Example: bits_to_mask = 4 and bits_to_skip = 4 results in 0b11110000
	 */
	template< typename T, std::size_t bits_to_skip, std::size_t bits_to_mask > constexpr auto bit_pattern() -> T {
		static_assert(bits_to_skip + bits_to_mask <= sizeof(T) * 8,
					  "bit_pattern: Provided type is not wide enough to create desired mask");
		std::size_t mask  = 0;
		std::size_t value = 1;
		for (std::size_t i = 0; i < bits_to_skip; ++i) {
			value *= 2;
		}
		for (std::size_t i = 0; i < bits_to_mask; ++i) {
			mask += value;
			value *= 2;
		}

		return mask;
	}
} // namespace details

/**
 * This class represents a wrapper type that can be used to combine the storage of the provided enum types into a single
 * numeric value. This is done by abusing that often times enum types use much smaller data types (e.g. std::uint8_t)
 * for their representation, but when including such types as members of a class or struct, this will usually introduce
 * padding anyway, if larger data types are present as well. Therefore, it can make sense to pack multiple enum values
 * together instead of listing them all explicitly, where every enum occurrence introduces additional padding.
 */
template< typename... Enums > class MultiEnum {
public:
	// Ensure that Enums actually is a list of enums
	static_assert((std::is_enum_v< Enums > && ...), "MultiEnum can only be used with enum types");
	static_assert(sizeof...(Enums) > 0, "MultiEnum requires at least one template argument");

	/**
	 * The type used to represent the combined enum values
	 */
	using underlying_type = sized_storage_t< cumulative_bit_size_v< Enums... > >;

	/**
	 * @returns The current value of the specified enum type
	 */
	template< typename Enum > constexpr auto get() const -> Enum {
		static_assert(is_contained_v< Enum, Enums... >,
					  "MultiEnum: Used get<>() with a type not wrapped by this instance");
		constexpr std::size_t shiftAmount = details::bit_width_of_first_n< index_of_v< Enum, Enums... >, Enums... >();

		// Mask out all unrelated bits and then shift the remaining bits down as if they had never been changed in the
		// first place
		std::underlying_type_t< Enum > numericRep = static_cast< std::underlying_type_t< Enum > >(
			(m_value & details::bit_pattern< underlying_type, shiftAmount, sizeof(Enum) * 8 >()) >> shiftAmount);
		return static_cast< Enum >(numericRep);
	}

	/**
	 * Sets the value of the given enum type in this wrapper to the given value
	 */
	template< typename Enum > constexpr void set(Enum val) {
		static_assert(is_contained_v< Enum, Enums... >,
					  "MultiEnum: Used set<>() with a type not wrapped by this instance");
		constexpr std::size_t shiftAmount = details::bit_width_of_first_n< index_of_v< Enum, Enums... >, Enums... >();

		// Shift the relevant bits up so that they reach their position for storage inside this wrapper.
		underlying_type numericRep = static_cast< underlying_type >(val) << shiftAmount;

		// Create a bit pattern that masks out the bits corresponding to Enum in our internal storage
		underlying_type zeroPattern = std::numeric_limits< underlying_type >::max()
									  & (~details::bit_pattern< underlying_type, shiftAmount, sizeof(Enum) * 8 >());

		// Zero-out the bits for Enum
		m_value &= zeroPattern;
		// Then set these bits to the requested value
		m_value |= numericRep;
	}

private:
	underlying_type m_value = 0;
};

} // namespace lizard::core

#endif
