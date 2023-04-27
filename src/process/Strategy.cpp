// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/Strategy.hpp"
#include "lizard/process/StrategyType.hpp"

#include <cassert>
#include <iostream>

namespace lizard {

void Strategy::setLogger(std::shared_ptr< spdlog::logger > logger) {
	assert(logger); // NOLINT

	m_log = std::move(logger);
}

auto Strategy::getLogger() const -> spdlog::logger & {
	assert(m_log); // NOLINT

	return *m_log;
}

auto operator<<(std::ostream &stream, const Strategy &strategy) -> std::ostream & {
	switch (strategy.getType()) {
		case StrategyType::Export:
			stream << "Export-";
			break;
		case StrategyType::Import:
			stream << "Import-";
			break;
		case StrategyType::Optimization:
			stream << "Opt-";
			break;
		case StrategyType::SpinProcessing:
			stream << "SpinProcess-";
			break;
		case StrategyType::Substitution:
			stream << "Subst-";
			break;
	}

	return stream << "Strategy: " << strategy.getName();
}

} // namespace lizard
