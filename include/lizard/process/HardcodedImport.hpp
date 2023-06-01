// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/ImportStrategy.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <vector>

namespace lizard {

class IndexSpaceManager;

/**
 * This import strategy allows one to simply choose from a set of hardcoded term definitions.
 * It is mainly intended for testing purposes, where it saves you the hassle of having to have
 * a corresponding input file at hand.
 */
class HardcodedImport : public ImportStrategy {
public:
	enum ImportTarget {
		CCD_ENERGY,
	};

	HardcodedImport(ImportTarget target);

	[[nodiscard]] auto getName() const -> std::string final;

	[[nodiscard]] auto importExpressions(const IndexSpaceManager &manager) const
		-> std::vector< NamedTensorExprTree > final;

private:
	ImportTarget m_target;
};

} // namespace lizard
