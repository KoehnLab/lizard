// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/Processor.hpp"
#include "lizard/core/SignedCast.hpp"
#include "lizard/process/ExportStrategy.hpp"
#include "lizard/process/ImportStrategy.hpp"
#include "lizard/process/RewriteStrategy.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <cassert>
#include <iterator>
#include <type_traits>
#include <vector>


namespace lizard {

void Processor::enqueue(std::unique_ptr< Strategy > step) {
	assert(step); // NOLINT

	m_processingSteps.push_back(std::move(step));
}

void Processor::insert(std::unique_ptr< Strategy > step, std::size_t position) {
	assert(step);                                 // NOLINT
	assert(position <= m_processingSteps.size()); // NOLINT

	using SignedSize = std::make_signed_t< std::size_t >;

	m_processingSteps.insert(m_processingSteps.begin() + signed_cast< SignedSize >(position), std::move(step));
}

void Processor::run() const {
	std::vector< TensorExprTree > expressions;

	for (const std::unique_ptr< Strategy > &currentPtr : m_processingSteps) {
		assert(currentPtr); // NOLINT
		Strategy &current = *currentPtr;

		switch (current.getType()) {
			case StrategyType::Import: {
				const auto &strategy = dynamic_cast< const ImportStrategy & >(current);

				std::vector< TensorExprTree > newExpressions = strategy.importExpressions();

				if (expressions.empty()) {
					expressions = std::move(newExpressions);
				} else {
					expressions.reserve(expressions.size() + newExpressions.size());
					expressions.insert(expressions.end(), std::move_iterator(newExpressions.begin()),
									   std::move_iterator(newExpressions.end()));
				}
			} break;
			case StrategyType::Export: {
				auto &strategy = dynamic_cast< ExportStrategy & >(current);

				strategy.exportExpressions(expressions);
			} break;
			case StrategyType::Optimization:
			case StrategyType::SpinIntegration:
			case StrategyType::SpinSummation:
			case StrategyType::Substitution: {
				auto &strategy = dynamic_cast< RewriteStrategy & >(current);

				strategy.process(expressions);
				break;
			}
		}
	}
}

} // namespace lizard
