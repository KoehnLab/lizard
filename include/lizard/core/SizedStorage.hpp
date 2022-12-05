// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_CORE_SIZED_STORAGE_HPP_
#define LIZARD_CORE_SIZED_STORAGE_HPP_

#include <cstddef>
#include <cstdint>

namespace lizard::core {

/**
 * A metaprogramming helper that returns an integral type that can hold data consisting up to the
 * given amount of octets (~bytes).
 * The respective type can be found in the public type definition "type".
 */
template< std::size_t octets > struct sized_octet_storage {
	static_assert(octets == -1, "sized_octet_storage has no specialization for the requested size");
};

// The default implementation of sized_octet_storage doesn't contain a "type" field in order
// to indicate that we don't know what type to use in the general case.
// What follows are template specializations that map a given amount of octets to a suitable
// type.
template<> struct sized_octet_storage< 1 > { using type = std::uint8_t; };  // NOLINT
template<> struct sized_octet_storage< 2 > { using type = std::uint16_t; }; // NOLINT
template<> struct sized_octet_storage< 3 > { using type = std::uint32_t; }; // NOLINT
template<> struct sized_octet_storage< 4 > { using type = std::uint32_t; }; // NOLINT
template<> struct sized_octet_storage< 5 > { using type = std::uint64_t; }; // NOLINT
template<> struct sized_octet_storage< 6 > { using type = std::uint64_t; }; // NOLINT
template<> struct sized_octet_storage< 7 > { using type = std::uint64_t; }; // NOLINT
template<> struct sized_octet_storage< 8 > { using type = std::uint64_t; }; // NOLINT

/**
 * Gets an integral type that is suitable for storing the given amount of octets (~bytes).
 */
template< std::size_t octets > using sized_octet_storage_t = typename sized_octet_storage< octets >::type;

/**
 * A metaprogramming helper that returns an integral type that can hold data consisting up to the
 * given amount of bits.
 * The respective type can be found in the public type definition "type".
 */
template< std::size_t bits > struct sized_storage {
	// Note: (bits + 7) / 8 is the same as rounding the result of bits / 8 to the next
	// highest integer (unless the result itself is already an integer)
	using type = sized_octet_storage_t< (bits + 7) / 8 >; // NOLINT
};

/**
 * Gets an integral type that is suitable for storing the given amount of bits.
 */
template< std::size_t bits > using sized_storage_t = typename sized_storage< bits >::type;

} // namespace lizard::core

#endif // LIZARD_CORE_SIZED_STORAGE_HPP_
