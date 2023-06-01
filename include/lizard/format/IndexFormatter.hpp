// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/format/FormatException.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"
#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexSpaceManager.hpp"

#include <fmt/core.h>

namespace lizard {

struct IndexFormatter : public details::SymbolicFormatter< Index > {
	using details::SymbolicFormatter< Index >::SymbolicFormatter;
};

} // namespace lizard


template<> struct fmt::formatter< lizard::IndexFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext > auto format(const lizard::IndexFormatter &formatter, FormatContext &ctx) {
		const lizard::IndexSpaceData &data = formatter.getManager().getData(formatter.get().getSpace());
		const std::vector< char > &labels  = data.getLabels();

		if (labels.empty()) {
			throw lizard::FormatException("Can't format index as no labels for its space are provided");
		}

		const std::size_t nLabelExtensions = static_cast< std::size_t >(formatter.get().getID()) / labels.size();
		const std::size_t labelIdx         = static_cast< std::size_t >(formatter.get().getID()) % labels.size();

		return fmt::formatter< std::string_view >::format(
			labels[labelIdx] + std::string(nLabelExtensions, data.getLabelExtension()), ctx);
	}
};
