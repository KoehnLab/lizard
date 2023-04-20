// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/format/IndexFormatter.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"
#include "lizard/format/details/Tensor.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/Spin.hpp"
#include "lizard/symbolic/TensorBlock.hpp"
#include "lizard/symbolic/TensorElement.hpp"

#include <algorithm>

namespace lizard {

struct TensorElementFormatter : details::SymbolicFormatter< TensorElement > {
	using details::SymbolicFormatter< TensorElement >::SymbolicFormatter;
};

} // namespace lizard


template<> struct fmt::formatter< lizard::TensorElementFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext >
	auto format(const lizard::TensorElementFormatter &formatter, FormatContext &ctx) {
		const lizard::TensorElement &element         = formatter.get();
		const lizard::TensorBlock::IndexSlots &slots = element.getBlock().getIndexSlots();

		std::string formatted = fmt::format("{}[", element.getBlock().getTensor());

		const std::size_t nIndices = slots.size();

		for (std::size_t i = 0; i < nIndices; ++i) {
			formatted += fmt::format("{}{}", lizard::IndexFormatter(element.getIndices()[i], formatter.getManager()),
									 (i + 1 < nIndices ? " " : ""));
		}

		formatted += "]";

		// Note: We only print spin information about tensor elements, if any of the spins is actually interesting
		const bool printSpinInformation =
			std::find_if(slots.begin(), slots.end(),
						 [](const lizard::IndexSpace &space) {
							 return space.getSpin() == lizard::Spin::Alpha || space.getSpin() == lizard::Spin::Beta;
						 })
			!= slots.end();

		if (printSpinInformation) {
			formatted += "(";

			for (const lizard::IndexSpace &space : slots) {
				switch (space.getSpin()) {
					case lizard::Spin::Alpha:
						formatted += "/";
						break;
					case lizard::Spin::Beta:
						formatted += "\\";
						break;
					case lizard::Spin::Both:
					case lizard::Spin::None:
						// We don't have to differentiate these spin cases because that can
						// be done by consulting the index space definition
						formatted += ".";
						break;
				}
			}

			formatted += ")";
		}

		return fmt::formatter< std::string_view >::format(formatted, ctx);
	}
};
