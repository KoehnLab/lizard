// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "Contract.hpp"
#include "TensorFormatter.hpp"

#include "lizard/format/FormatSupport.hpp"

#include <fmt/core.h>


namespace lizard::itf {

Contract::Contract(TensorElement result, TensorExpression lhs, TensorExpression rhs, Fraction factor)
	: Multiplyable(std::move(factor)), m_result(std::move(result)), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {
}

void Contract::setResult(TensorElement result) {
	m_result = std::move(result);
}

auto Contract::getResult() const -> const TensorElement & {
	return m_result;
}


auto Contract::getLhs() const -> const TensorExpression & {
	return m_lhs;
}

auto Contract::getRhs() const -> const TensorExpression & {
	return m_rhs;
}

auto Contract::stringify(const IndexSpaceManager &manager) const -> std::string {
	if (getFactor() == 1) {
		return fmt::format(".{} += {} {}", itf::TensorFormatter(m_result, manager), m_lhs.stringify(manager),
						   m_rhs.stringify(manager));
	}

	return fmt::format(".{} += {}*{} {}", itf::TensorFormatter(m_result, manager), getFactor(),
					   m_lhs.stringify(manager), m_rhs.stringify(manager));
}

} // namespace lizard::itf
