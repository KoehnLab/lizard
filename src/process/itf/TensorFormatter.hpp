// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/format/FormatSupport.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/Spin.hpp"
#include "lizard/symbolic/Tensor.hpp"
#include "lizard/symbolic/TensorBlock.hpp"
#include "lizard/symbolic/TensorElement.hpp"

#include <algorithm>

namespace lizard::itf {

struct TensorFormatter : details::SymbolicFormatter< TensorElement > {
	using details::SymbolicFormatter< TensorElement >::SymbolicFormatter;
};

} // namespace lizard::itf


template<> struct fmt::formatter< lizard::itf::TensorFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext > auto format(const lizard::itf::TensorFormatter &formatter, FormatContext &ctx) {
		const lizard::TensorElement &element              = formatter.get();
		const lizard::TensorBlock::IndexSlots &slots      = element.getBlock().getIndexSlots();
		const nonstd::span< const lizard::Index > indices = element.getIndices();
		const lizard::IndexSpaceManager &manager          = formatter.getManager();
		std::string indexSpec;
		std::string spaceSpec;
		std::string spinSpec;

		// Note: We only print spin information about tensor elements, if any of the spins is actually interesting
		const bool includeSpin =
			std::find_if(slots.begin(), slots.end(),
						 [](const lizard::IndexSpace &space) {
							 return space.getSpin() == lizard::Spin::Alpha || space.getSpin() == lizard::Spin::Beta;
						 })
			!= slots.end();

		if (includeSpin) {
			// Since ITF doesn't allow underscores in tensor names, we have to resort to concatenating the
			// tensor's name and its spin case by a different character
			spinSpec = "0";
			for (const lizard::IndexSpace &current : slots) {
				switch (current.getSpin()) {
					case lizard::Spin::Alpha:
						spinSpec += "a";
						break;
					case lizard::Spin::Beta:
						spinSpec += "b";
						break;
					case lizard::Spin::None:
						spinSpec += "n";
						break;
					case lizard::Spin::Both:
						throw std::runtime_error("Encountered indices in spin-orbit formalism during ITF export");
						break;
				}
			}
		}

		for (const lizard::Index &currentIndex : indices) {
			indexSpec += fmt::format("{}", lizard::IndexFormatter(currentIndex, manager));

			const lizard::IndexSpaceData &data = manager.getData(currentIndex.getSpace());
			spaceSpec += data.getShortName();
		}

		if (!spaceSpec.empty()) {
			spaceSpec = ":" + spaceSpec;
		}

		return fmt::formatter< std::string_view >::format(
			fmt::format("{}{}{}[{}]", element.getBlock().getTensor(), spinSpec, spaceSpec, indexSpec), ctx);
	}
};
