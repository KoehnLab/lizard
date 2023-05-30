// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <string>

namespace lizard {
class IndexSpaceManager;
}

namespace lizard::itf {

/**
 * Base class for all ITF operations
 */
class Operation {
public:
	Operation()          = default;
	virtual ~Operation() = default;

	Operation(const Operation &) = default;
	Operation(Operation &&)      = default;

	auto operator=(const Operation &) -> Operation & = default;
	auto operator=(Operation &&) -> Operation & = default;

	/**
	 * @returns The corresponding ITF code (in textual representation)
	 */
	[[nodiscard]] virtual auto stringify(const IndexSpaceManager &manager) const -> std::string = 0;
};

} // namespace lizard::itf
