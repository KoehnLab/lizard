// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/SpinProcessingStrategy.hpp"

namespace lizard {

/**
 * Processing step that performs spin integration which transforms indices from running over
 * alpha and beta spin orbitals to only running over either alpha or beta orbitals and while
 * doing so removing cases that must be zero due to the orthogonality of the formal spin functions.
 */
class SpinIntegration : public SpinProcessingStrategy {
public:
	SpinIntegration() = default;

	[[nodiscard]] auto getName() const -> std::string final;

	void process(std::vector< NamedTensorExprTree > &expressions, const IndexSpaceManager &manager) final;
};

} // namespace lizard
