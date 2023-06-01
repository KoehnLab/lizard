// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/ExportStrategy.hpp"

namespace lizard {

/**
 * An exporter that will convert the given expressions into the ITF format
 */
class ITFExport : public ExportStrategy {
public:
	ITFExport() = default;

	[[nodiscard]] auto getName() const -> std::string final;

	void exportExpressions(nonstd::span< const NamedTensorExprTree > expressions,
						   const IndexSpaceManager &manager) final;
};

} // namespace lizard
