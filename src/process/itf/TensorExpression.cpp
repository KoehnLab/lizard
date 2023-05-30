// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "TensorExpression.hpp"
#include "TensorFormatter.hpp"

#include "lizard/format/FormatSupport.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <cassert>
#include <variant>

namespace lizard {
class ScaledTensorFormatter : public details::SymbolicFormatter< itf::TensorExpression::ScaledTensorElement > {
public:
	using details::SymbolicFormatter< itf::TensorExpression::ScaledTensorElement >::SymbolicFormatter;
};
} // namespace lizard

template<> struct fmt::formatter< lizard::ScaledTensorFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext > auto format(const lizard::ScaledTensorFormatter &formatter, FormatContext &ctx) {
		if (formatter.get().factor == 1) {
			return fmt::formatter< std::string_view >::format(
				fmt::format("{}", lizard::itf::TensorFormatter(formatter.get().tensor, formatter.getManager())), ctx);
		}

		return fmt::formatter< std::string_view >::format(
			fmt::format("{}*{}", formatter.get().factor,
						lizard::itf::TensorFormatter(formatter.get().tensor, formatter.getManager())),
			ctx);
	}
};


namespace lizard::itf {

TensorExpression::TensorExpression(TensorElement element, Fraction factor)
	: Multiplyable(std::move(factor)), m_expression(std::move(element)) {
}

auto TensorExpression::getCardinality() const -> ExpressionCardinality {
	if (std::holds_alternative< TensorElement >(m_expression)) {
		return ExpressionCardinality::Nullary;
	}

	return ExpressionCardinality::Binary;
}

auto TensorExpression::getResult() const -> const TensorElement & {
	if (std::holds_alternative< TensorElement >(m_expression)) {
		return std::get< TensorElement >(m_expression);
	}

	const Addition &addition = std::get< Addition >(m_expression);
	const TensorElement &lhs = addition.lhs.tensor;
	const TensorElement &rhs = addition.rhs.tensor;

	// We are assuming that the added tensors generally have the same indices (potentially in a different order)
	// and that their index symmetries are equivalent as well

	// NOLINTNEXTLINE
	assert(lhs.getIndices().size() == rhs.getIndices().size()
		   && std::is_permutation(lhs.getIndices().begin(), lhs.getIndices().end(), rhs.getIndices().begin()));
	assert(lhs.getBlock().getSlotSymmetry() == rhs.getBlock().getSlotSymmetry()); // NOLINT
	(void) rhs;

	// Under these assumptions, the result tensor element can be chosen to look the same as either
	// of the individual elements
	return lhs;
}

auto TensorExpression::stringify(const IndexSpaceManager &manager) const -> std::string {
	if (std::holds_alternative< TensorElement >(m_expression)) {
		if (getFactor() == 1) {
			return fmt::format("{}", TensorFormatter(std::get< TensorElement >(m_expression), manager));
		}

		return fmt::format("{}*{}", getFactor(), TensorFormatter(std::get< TensorElement >(m_expression), manager));
	}

	const Addition &addition = std::get< Addition >(m_expression);

	if (getFactor() == 1) {
		return fmt::format("({} + {})", ScaledTensorFormatter(addition.lhs, manager),
						   ScaledTensorFormatter(addition.rhs, manager));
	}

	return fmt::format("{}*({} + {})", getFactor(), ScaledTensorFormatter(addition.lhs, manager),
					   ScaledTensorFormatter(addition.rhs, manager));
}


auto TensorExpression::operator+=(const TensorExpression &rhs) -> TensorExpression & {
	assert(std::holds_alternative< TensorElement >(m_expression));     // NOLINT
	assert(std::holds_alternative< TensorElement >(rhs.m_expression)); // NOLINT

	m_expression = Addition{ ScaledTensorElement{ std::move(std::get< TensorElement >(m_expression)), getFactor() },
							 ScaledTensorElement{ std::get< TensorElement >(rhs.m_expression), rhs.getFactor() } };

	setFactor(1);

	return *this;
}

auto TensorExpression::operator+(const TensorExpression &rhs) -> TensorExpression {
	TensorExpression copy = *this;

	copy += rhs;

	return rhs;
}

} // namespace lizard::itf
