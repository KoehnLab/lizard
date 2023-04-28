// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/core/Fraction.hpp"
#include "lizard/format/TensorElementFormatter.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"
#include "lizard/symbolic/ExpressionOperator.hpp"
#include "lizard/symbolic/ExpressionType.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"
#include "lizard/symbolic/TreeTraversal.hpp"
#include "lizard/symbolic/details/ExpressionTreeIteratorCore.hpp"

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <iterators/iterator_facade.hpp>

#include <optional>
#include <stack>
#include <string>
#include <string_view>

namespace lizard {

struct TensorExprFormatter : details::SymbolicFormatter< ConstTensorExpr > {
	using details::SymbolicFormatter< ConstTensorExpr >::SymbolicFormatter;
};

} // namespace lizard

template<> struct fmt::formatter< lizard::Fraction > : fmt::ostream_formatter {};


template<> struct fmt::formatter< lizard::TensorExprFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext > auto format(const lizard::TensorExprFormatter &formatter, FormatContext &ctx) {
		using namespace lizard;

		// In order to format an expression, we have to iterate in a defined order and then translate that
		// into a suitable infix expression.
		// The easiest way to do this is to iterate in post order and effectively "evaluate" each encountered
		// element by converting that into the appropriate string contribution.
		// See also e.g. https://stackoverflow.com/a/19053203/3907364

		const ConstTensorExpr &expr = formatter.get();

		std::stack< std::string > formattedPieces;
		std::stack< std::optional< ExpressionOperator > > encounteredOperators;

		auto iter      = expr.cbegin< TreeTraversal::DepthFirst_PostOrder >();
		const auto end = expr.cend< TreeTraversal::DepthFirst_PostOrder >();

		for (; iter != end; ++iter) {
			const ConstTensorExpr &current = *iter;

			switch (current.getType()) {
				case ExpressionType::Literal:
					formattedPieces.push(fmt::format("{}", current.getLiteral()));
					encounteredOperators.push({});
					break;
				case ExpressionType::Variable:
					formattedPieces.push(
						fmt::format("{}", TensorElementFormatter(current.getVariable(), formatter.getManager())));
					encounteredOperators.push({});
					break;
				case ExpressionType::Operator: {
					assert(current.getCardinality() == ExpressionCardinality::Binary);
					assert(formattedPieces.size() >= 2);
					assert(formattedPieces.size() == encounteredOperators.size());

					std::string rhs = std::move(formattedPieces.top());
					formattedPieces.pop();
					std::string lhs = std::move(formattedPieces.top());
					formattedPieces.pop();

					std::optional< ExpressionOperator > rhsOp = std::move(encounteredOperators.top());
					encounteredOperators.pop();
					std::optional< ExpressionOperator > lhsOp = std::move(encounteredOperators.top());
					encounteredOperators.pop();

					switch (current.getOperator()) {
						// Peek at next
						case ExpressionOperator::Plus: {
							// Since Plus has the lowest precedence of all possible operators, we can
							// always insert it without any need for parenthesis
							// Formally, we'd have to use parenthesis to encode the order of operations (same as for
							// multiplication/contraction) but since we don't care about the order of addition, we
							// don't do that here.
							lhs += " + ";
							lhs += rhs;
							formattedPieces.push(std::move(lhs));
							encounteredOperators.push(ExpressionOperator::Plus);
						} break;
						case ExpressionOperator::Times: {
							// If any of lhs or rhs is the result of a prior operator acting on some arguments, that
							// result will have to be put in parenthesis in order to
							// 1) preserver order of operations in case it was a Plus
							// 2) encode order of contraction in case it was a Times
							if (lhsOp.has_value()) {
								lhs.insert(0, "( ");
								lhs += " )";
							}
							if (rhsOp.has_value()) {
								rhs.insert(0, "( ");
								rhs += " )";
							}

							lhs += " * ";
							lhs += rhs;

							formattedPieces.push(std::move(lhs));
							encounteredOperators.push(ExpressionOperator::Times);
						} break;
					}
					break;
				}
			}
		}

		assert(formattedPieces.size() == 1);

		// Make sure to exclude the trailing space that appears in non-empty formatted texts
		return fmt::formatter< std::string_view >::format(std::move(formattedPieces.top()), ctx);
	}
};
