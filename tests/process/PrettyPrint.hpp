// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "Utils.hpp"

#include "lizard/format/FormatSupport.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <fmt/core.h>

#include <ostream>

namespace lizard {

inline void PrintTo(const TensorExpr &expr, std::ostream *stream) {
	*stream << fmt::format("{}", TensorExprFormatter(expr, test::getIndexSpaceManager()));
}

inline void PrintTo(const TensorExprTree &tree, std::ostream *stream) {
	*stream << fmt::format("{}", TensorExprTreeFormatter(tree, test::getIndexSpaceManager()));
}

inline void PrintTo(const NamedTensorExprTree &tree, std::ostream *stream) {
	*stream << fmt::format("{}", NamedTensorExprTreeFormatter(tree, test::getIndexSpaceManager()));
}

} // namespace lizard
