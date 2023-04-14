// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/SpinSummationStrategy.hpp"

namespace lizard {

auto SpinSummationStrategy::getType() const -> StrategyType {
	return StrategyType::SpinSummation;
}

} // namespace lizard
