// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/format/IndexSpaceFormatter.hpp"

#include <fmt/core.h>

template<> struct fmt::formatter< lizard::IndexSpaceFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext > auto format(const lizard::IndexSpaceFormatter &formatter, FormatContext &ctx) {
		return fmt::formatter< std::string_view >::format(
			formatter.getManager().getData(formatter.getSpace()).getName(), ctx);
	}
};
