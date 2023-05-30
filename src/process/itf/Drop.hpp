// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/TensorElement.hpp"

#include "Operation.hpp"

namespace lizard::itf {

/**
 * Represents the drop operation in ITF
 */
class Drop : public Operation {
public:
	Drop(TensorElement element);

	[[nodiscard]] auto stringify(const IndexSpaceManager &manager) const -> std::string override;

private:
	TensorElement m_element;
};

} // namespace lizard::itf
