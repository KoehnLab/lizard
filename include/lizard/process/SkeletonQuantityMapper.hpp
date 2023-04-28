// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/SpinProcessingStrategy.hpp"

namespace lizard {

/**
 * Processing step that maps spin-integrated quantities to so-called "skeleton" (or "orbital") quantities.
 * The process is described in e.g. J. Chem. Theory Comput. 2013, 9, 2567−2572 (DOI: 10.1021/ct301024v)
 */
class SkeletonQuantityMapper : public SpinProcessingStrategy {
public:
	SkeletonQuantityMapper() = default;

	[[nodiscard]] auto getName() const -> std::string_view final;

	void process(std::vector< NamedTensorExprTree > &expressions, const IndexSpaceManager &manager) final;
};

} // namespace lizard
