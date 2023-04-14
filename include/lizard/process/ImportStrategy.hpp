// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/Strategy.hpp"
#include "lizard/process/StrategyType.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <vector>

namespace lizard {

/**
 * Base class for processing strategies that deal with importing a set of tensor expressions
 */
class ImportStrategy : public Strategy {
public:
	ImportStrategy() = default;

	[[nodiscard]] auto getType() const -> StrategyType final;

	/**
	 * Performs the actual step of (parsing and) importing the data. If the import depends on any
	 * configuration (e.g. a file path), these will have been passed to the constructor of this strategy.
	 *
	 * @returns The list of imported expressions
	 *
	 * @throws ImportException if something goes wrong during the import
	 */
	[[nodiscard]] virtual auto importExpressions() const -> std::vector< TensorExprTree > = 0;
};

} // namespace lizard
