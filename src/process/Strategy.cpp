// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/Strategy.hpp"
#include "lizard/process/StrategyType.hpp"

#include <iostream>

namespace lizard {

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
		case StrategyType::SpinIntegration:
			stream << "SpinInt-";
			break;
		case StrategyType::SpinSummation:
			stream << "SpinSum-";
			break;
		case StrategyType::Substitution:
			stream << "Subst-";
			break;
	}

	return stream << "Strategy: " << strategy.getName();
}

} // namespace lizard
