// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/StrategyType.hpp"

#include <hedley.h>

#include <iostream>

namespace lizard {

auto operator<<(std::ostream &stream, StrategyType type) -> std::ostream & {
	switch (type) {
		case StrategyType::Export:
			return stream << "Export";
		case StrategyType::Import:
			return stream << "Import";
		case StrategyType::Optimization:
			return stream << "Optimization";
		case StrategyType::SpinProcessing:
			return stream << "Spin processing";
		case StrategyType::Substitution:
			return stream << "Substitution";
	}

	HEDLEY_UNREACHABLE();
}

} // namespace lizard
