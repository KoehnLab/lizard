// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#pragma once

#include <type_traits>

namespace lizard {

template< typename > class ConstExpression;
template< typename > class Expression;

template< typename > class ExpressionTree;


// Type trait for checking whether something is an Expression or ConstExpression
template< typename T > struct is_expression : std::false_type {};
template< typename Variable > struct is_expression< Expression< Variable > > : std::true_type {};
template< typename Variable > struct is_expression< ConstExpression< Variable > > : std::true_type {};

// Type trait for extracting an expression's Variable type - can only be used with actual expressions
template< typename T > struct expression_variable {};
template< typename Variable > struct expression_variable< Expression< Variable > > { using type = Variable; };
template< typename Variable > struct expression_variable< ConstExpression< Variable > > { using type = Variable; };

// Type trait to check if a given type is iterable (has begin() and end() members)
template< typename T, typename = void > struct is_iterable : std::false_type {};
template< typename T >
struct is_iterable< T, std::void_t< decltype(std::declval< T >().begin()), decltype(std::declval< T >().end()) > >
	: std::true_type {};

// Type trait to obtain an expression tree's Variable type - can only be used with actual ExpressionTrees
template< typename T > struct expression_tree_variable {};
template< typename Variable > struct expression_tree_variable< ExpressionTree< Variable > > { using type = Variable; };


// Shorthand accessors for the type traits defined above

template< typename T > constexpr bool is_expression_v   = is_expression< T >::value;
template< typename T > using expression_variable_t      = typename expression_variable< T >::type;
template< typename T > constexpr bool is_iterable_v     = is_iterable< T >::value;
template< typename T > using expression_tree_variable_t = typename expression_tree_variable< T >::type;

} // namespace lizard
