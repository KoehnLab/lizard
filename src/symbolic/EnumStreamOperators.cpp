// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/IndexType.hpp"
#include "lizard/symbolic/Spin.hpp"

#include <hedley.h>

#include <iostream>

namespace lizard {

auto operator<<(std::ostream &stream, IndexType type) -> std::ostream & {
	switch (type) {
		case IndexType::Annihilator:
			return stream << "Annihilator";
		case IndexType::External:
			return stream << "External";
		case IndexType::Creator:
			return stream << "Creator";
	}

	HEDLEY_UNREACHABLE();
}

auto operator<<(std::ostream &stream, Spin spin) -> std::ostream & {
	switch (spin) {
		case Spin::Alpha:
			return stream << "Alpha";
		case Spin::Beta:
			return stream << "Beta";
		case Spin::Both:
			return stream << "Both";
		case Spin::None:
			return stream << "None";
	}

	HEDLEY_UNREACHABLE();
}

} // namespace lizard
