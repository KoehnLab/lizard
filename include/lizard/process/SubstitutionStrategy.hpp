// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/RewriteStrategy.hpp"
#include "lizard/process/StrategyType.hpp"

namespace lizard {

/**
 * Base class for all processing strategies that deal with substituting one expression by another
 */
class SubstitutionStrategy : public RewriteStrategy {
public:
	SubstitutionStrategy() = default;

	[[nodiscard]] auto getType() const -> StrategyType final;
};

} // namespace lizard
