// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/core/ExpressionType.hpp"

#include <iostream>

namespace lizard {

auto operator<<(std::ostream &stream, const ExpressionType &type) -> std::ostream & {
	switch (type) {
		case ExpressionType::Operator:
			stream << "Operator";
			break;
		case ExpressionType::Literal:
			stream << "Literal";
			break;
		case ExpressionType::Variable:
			stream << "Variable";
			break;
	}

	return stream;
}

} // namespace lizard
