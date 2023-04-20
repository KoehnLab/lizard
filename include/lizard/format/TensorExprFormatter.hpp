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

#include <stack>
#include <string_view>

namespace lizard {

struct TensorExprFormatter : details::SymbolicFormatter< ConstTensorExpr > {
	using details::SymbolicFormatter< ConstTensorExpr >::SymbolicFormatter;
};

} // namespace lizard

template<> struct fmt::formatter< lizard::Fraction > : fmt::ostream_formatter {};


template<> struct fmt::formatter< lizard::TensorExprFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext > auto format(const lizard::TensorExprFormatter &formatter, FormatContext &ctx) {
		const lizard::ConstTensorExpr &expr = formatter.get();

		std::string formatted;

		using InOrderCore = lizard::details::ExpressionTreeIteratorCore< lizard::TensorElement, true,
																		 lizard::TreeTraversal::DepthFirst_InOrder >;
		using InOrderIter = iterators::iterator_facade< InOrderCore >;

		std::stack< InOrderIter > parenClosePositions;

		for (auto iter = expr.cbegin< lizard::TreeTraversal::DepthFirst_InOrder >();
			 iter != expr.cend< lizard::TreeTraversal::DepthFirst_InOrder >(); ++iter) {
			const lizard::ConstTensorExpr &current = *iter;

			if (!parenClosePositions.empty() && parenClosePositions.top() == iter) {
				formatted += ") ";
			}

			switch (current.getType()) {
				case lizard::ExpressionType::Literal:
					formatted += fmt::format("{} ", current.getLiteral());
					break;
				case lizard::ExpressionType::Operator: {
					assert(current.getCardinality() == lizard::ExpressionCardinality::Binary);

					switch (current.getOperator()) {
						// Peek at next
						case lizard::ExpressionOperator::Plus:
							formatted += "+ ";
							break;
						case lizard::ExpressionOperator::Times: {
							const lizard::ConstTensorExpr peekExpr = current.getRightArg();

							const bool followedByAddition =
								peekExpr.getType() == lizard::ExpressionType::Operator
									? peekExpr.getOperator() == lizard::ExpressionOperator::Plus
									: false;

							if (followedByAddition) {
								formatted += "* ( ";

								parenClosePositions.emplace(
									InOrderCore::afterRoot(peekExpr.getContainingTree(), peekExpr));
							} else {
								// We take multiplication as implicit
							}
						} break;
					}
					break;
				}
				case lizard::ExpressionType::Variable:
					formatted += fmt::format(
						"{} ", lizard::TensorElementFormatter(current.getVariable(), formatter.getManager()));
					break;
			}
		}

		// Add trailing closing parenthesis
		while (!parenClosePositions.empty()) {
			formatted += ") ";
			parenClosePositions.pop();
		}

		// Make sure to exclude the trailing space that appears in non-empty formatted texts
		return fmt::formatter< std::string_view >::format(
			std::string_view{ formatted.data(), formatted.empty() ? 0 : formatted.size() - 1 }, ctx);
	}
};
