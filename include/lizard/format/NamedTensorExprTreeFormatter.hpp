// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/format/TensorElementFormatter.hpp"
#include "lizard/format/TensorExprTreeFormatter.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"
#include "lizard/symbolic/IndexSpaceManager.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <fmt/core.h>


namespace lizard {

struct NamedTensorExprTreeFormatter : details::SymbolicFormatter< NamedTensorExprTree > {
	using details::SymbolicFormatter< NamedTensorExprTree >::SymbolicFormatter;
};

} // namespace lizard


template<> struct fmt::formatter< lizard::NamedTensorExprTreeFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext >
	auto format(const lizard::NamedTensorExprTreeFormatter &formatter, FormatContext &ctx) {
		const lizard::NamedTensorExprTree &tree  = formatter.get();
		const lizard::IndexSpaceManager &manager = formatter.getManager();

		return fmt::formatter< std::string_view >::format(
			fmt::format("{} = {}", lizard::TensorElementFormatter(tree.getName(), manager),
						lizard::TensorExprTreeFormatter(tree, manager)),
			ctx);
	}
};
