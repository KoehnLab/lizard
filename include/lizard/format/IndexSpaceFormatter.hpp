// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/format/details/SymbolicFormatter.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexSpaceManager.hpp"

#include <fmt/core.h>

namespace lizard {

struct IndexSpaceFormatter : details::SymbolicFormatter< IndexSpace > {
	using details::SymbolicFormatter< IndexSpace >::SymbolicFormatter;
};

} // namespace lizard


template<> struct fmt::formatter< lizard::IndexSpaceFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext > auto format(const lizard::IndexSpaceFormatter &formatter, FormatContext &ctx) {
		return fmt::formatter< std::string_view >::format(formatter.getManager().getData(formatter.get()).getName(),
														  ctx);
	}
};
