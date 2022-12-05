// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_CORE_VARIADICHELPERS_HPP_
#define LIZARD_CORE_VARIADICHELPERS_HPP_

#include <cstddef>
#include <type_traits>

namespace lizard::core {

//////////////////////////////////
/// nth_element
//////////////////////////////////

/**
 * Metaprogramming helper to select the Nth element from a variadic parameter/argument pack.
 * The selected type will be stored in the "type" member field. If no such type exists
 * (the given N exceeds the list length) no such member field will exist.
 */
template< std::size_t N, typename T, typename... Ts > struct nth_element {
	using type = typename nth_element< N - 1, Ts... >::type;
};
// Specialization for N == 0, while the list still has more than a single entry
template< typename T, typename... Ts > struct nth_element< 0, T, Ts... > { using type = T; };
// Specialization for the case where the parameter list consists only of a single entry,
// but N != 0
template< std::size_t N, typename T > struct nth_element< N, T > {
	// This condition will never be true as the case N == 0 is handled separately via
	// template specialization. Therefore, we use this construct to provide a nicer error
	// message for out-of-bounds indices.
	static_assert(N == 0, "nth_element: Chosen index out of scope");
};
// Specialization for single-argument parameter list and N == 0
template< typename T > struct nth_element< 0, T > { using type = T; };


/**
 * Gets the Nth element from the provided variadic argument list/pack
 */
template< std::size_t N, typename... Ts > using nth_element_t = typename nth_element< N, Ts... >::type;


//////////////////////////////////
/// get_first, get_last
//////////////////////////////////

/**
 * Metaprogramming helper to select the first type inside a parameter pack.
 * The found type can be accessed via the "type" member. If no such type can be
 * found (i.e. the provided pack is empty), no such member exists.
 */
template< typename... Ts > struct get_first : nth_element< 0, Ts... > {};

/**
 * Gets the first entry in the provided parameter pack.
 */
template< typename... Ts > using get_first_t = typename get_first< Ts... >::type;


/**
 * Metaprogramming helper to select the last type inside a parameter pack.
 * The found type can be accessed via the "type" member. If no such type can be
 * found (i.e. the provided pack is empty), no such member exists.
 */
template< typename... Ts > struct get_last : nth_element< sizeof...(Ts) - 1, Ts... > {};

/**
 * Gets the first entry in the provided parameter pack.
 */
template< typename... Ts > using get_last_t = typename get_last< Ts... >::type;


//////////////////////////////////
/// index_of
//////////////////////////////////

namespace details {
	/**
	 * This is the underlying implementation for the index_of struct defined further down. It requires a few
	 * extra template parameters that we don't want to force users to provide the correct defaults for when
	 * using index_of.
	 */
	template< typename T, std::size_t N, bool head_is_t, typename Head, typename... Tail > struct index_of_impl {
	private:
		using nested_call =
			index_of_impl< T, N + 1, std::is_same_v< std::decay_t< T >, std::decay_t< nth_element_t< 0, Tail... > > >,
						   Tail... >;

	public:
		using type                               = typename nested_call::type;
		static constexpr const std::size_t value = nested_call::value;
	};
	// Specialization for when the T matches Head
	template< typename T, std::size_t N, typename Head, typename... Tail >
	struct index_of_impl< T, N, true, Head, Tail... > {
		using type                               = Head;
		static constexpr const std::size_t value = N;
	};
	// Specialization for when the end of the list has been reached, but still T != Head
	template< typename T, std::size_t N, typename Head > struct index_of_impl< T, N, false, Head > {
		static_assert(N == -1, "index_of: Searched for entry not found");
	};
} // namespace details

/**
 * Metaprogramming helper to find the index of a given type in a list of variadic parameter (parameter pack)
 * The found index is stored in the "value" member and the precise type matched inside the searched
 * pack, is stored in the "type" member.
 * If no such entry was found in the list, these member fields are not defined.
 */
template< typename T, typename Head, typename... Tail >
struct index_of
	: details::index_of_impl< T, 0, std::is_same_v< std::decay_t< T >, std::decay_t< Head > >, Head, Tail... > {};

/**
 * Finds the index of the given type in the provided parameter pack
 */
template< typename T, typename Head, typename... Tail >
constexpr const std::size_t index_of_v = index_of< T, Head, Tail... >::value;

} // namespace lizard::core

#endif // LIZARD_CORE_VARIADICHELPERS_HPP_
