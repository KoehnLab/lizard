// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/format/IndexFormatter.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"
#include "lizard/format/details/Tensor.hpp"
#include "lizard/symbolic/TensorElement.hpp"

namespace lizard {

struct TensorElementFormatter : details::SymbolicFormatter< TensorElement > {
	using details::SymbolicFormatter< TensorElement >::SymbolicFormatter;
};

} // namespace lizard


template<> struct fmt::formatter< lizard::TensorElementFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext >
	auto format(const lizard::TensorElementFormatter &formatter, FormatContext &ctx) {
		std::string formatted = fmt::format("{}[", formatter.get().getBlock().getTensor());

		const std::size_t nIndices = formatter.get().getIndices().size();

		for (std::size_t i = 0; i < nIndices; ++i) {
			formatted +=
				fmt::format("{}{}", lizard::IndexFormatter(formatter.get().getIndices()[i], formatter.getManager()),
							(i + i < nIndices ? " " : ""));
		}

		formatted += "]";

		return fmt::formatter< std::string_view >::format(formatted, ctx);
	}
};
