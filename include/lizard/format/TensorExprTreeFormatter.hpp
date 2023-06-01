// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/format/TensorExprFormatter.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <fmt/core.h>


namespace lizard {

struct TensorExprTreeFormatter : details::SymbolicFormatter< TensorExprTree > {
	using details::SymbolicFormatter< TensorExprTree >::SymbolicFormatter;
};

} // namespace lizard



template<> struct fmt::formatter< lizard::TensorExprTreeFormatter > : fmt::formatter< lizard::TensorExprFormatter > {
	template< typename FormatContext >
	auto format(const lizard::TensorExprTreeFormatter &formatter, FormatContext &ctx) {
		return fmt::formatter< lizard::TensorExprFormatter >::format(
			lizard::TensorExprFormatter(formatter.get().getRoot(), formatter.getManager()), ctx);
	}
};
