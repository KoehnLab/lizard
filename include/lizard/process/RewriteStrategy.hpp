// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/Strategy.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <vector>

namespace lizard {

class IndexSpaceManager;

/**
 * Base class for all processing strategy that rewrite the expressions given to them in one
 * way or another
 */
class RewriteStrategy : public Strategy {
public:
	RewriteStrategy() = default;

	/**
	 * Performs the respective modifications on the provided tensor expressions
	 *
	 * @param manager The current IndexSpaceManager
	 * @param expressions The expressions to work on
	 *
	 * @throws ProcessingException if something goes wrong during processing
	 */
	virtual void process(std::vector< NamedTensorExprTree > &expressions, const IndexSpaceManager &manager) = 0;
};

} // namespace lizard
