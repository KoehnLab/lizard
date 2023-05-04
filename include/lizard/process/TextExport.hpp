// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/ExportStrategy.hpp"

namespace lizard {

/**
 * An exporter that will write out the given expressions in an unspecified text format that is
 * meant to be read by humans but is not guaranteed to remain fixed or backwards compatible.
 */
class TextExport : public ExportStrategy {
public:
	TextExport() = default;

	[[nodiscard]] auto getName() const -> std::string final;

	void exportExpressions(nonstd::span< const NamedTensorExprTree > expressions,
						   const IndexSpaceManager &manager) final;
};

} // namespace lizard
