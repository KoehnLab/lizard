// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/Strategy.hpp"
#include "lizard/process/StrategyType.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <nonstd/span.hpp>

namespace lizard {

class IndexSpaceManager;

/**
 * Base class for all processing strategies that are related to exporting a given set of tensor expressions
 * (where exporting is defined as anything that transports the information about the expressions to somewhere
 * outside of the passed data structure)
 */
class ExportStrategy : public Strategy {
public:
	ExportStrategy() = default;

	[[nodiscard]] auto getType() const -> StrategyType final;

	/**
	 * Performs the actual export operation. The export format and target (e.g. a file or printing to console)
	 * depends on the concrete strategy that is used and potentially also on parameters passed to that
	 * strategy' constructor.
	 *
	 * @param manager The current IndexSpaceManager
	 * @param expressions The list of tensor expressions that shall be exported
	 *
	 * @throws ExportException if something goes wrong during the export
	 */
	virtual void exportExpressions(nonstd::span< const NamedTensorExprTree > expressions,
								   const IndexSpaceManager &manager) = 0;
};

} // namespace lizard
