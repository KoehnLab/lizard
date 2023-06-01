// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/format/IndexSpaceFormatter.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"
#include "lizard/format/details/Tensor.hpp"
#include "lizard/symbolic/TensorBlock.hpp"

#include <fmt/core.h>

namespace lizard {

struct TensorBlockFormatter : public details::SymbolicFormatter< TensorBlock > {
	using details::SymbolicFormatter< TensorBlock >::SymbolicFormatter;
};

} // namespace lizard

template<> struct fmt::formatter< lizard::TensorBlockFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext > auto format(const lizard::TensorBlockFormatter &formatter, FormatContext &ctx) {
		std::string formatted = fmt::format("{}{{", formatter.get().getTensor());

		const std::size_t nSlots = formatter.get().dimension();

		for (std::size_t i = 0; i < nSlots; ++i) {
			formatted += fmt::format(
				"{}{}", lizard::IndexSpaceFormatter(formatter.get().getIndexSlots()[i], formatter.getManager()),
				(i + 1 < nSlots ? " " : ""));
		}

		formatted += "}";

		return fmt::formatter< std::string_view >::format(formatted, ctx);
	}
};
