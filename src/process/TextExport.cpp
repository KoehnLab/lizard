// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/TextExport.hpp"
#include "lizard/format/FormatSupport.hpp"
#include "lizard/format/details/SymbolicFormatter.hpp"

#include <fmt/core.h>



struct TreeSpanFormatter : lizard::details::SymbolicFormatter< nonstd::span< const lizard::NamedTensorExprTree > > {
	using lizard::details::SymbolicFormatter< nonstd::span< const lizard::NamedTensorExprTree > >::SymbolicFormatter;
};


template<> struct fmt::formatter< TreeSpanFormatter > : fmt::formatter< std::string_view > {
	template< typename FormatContext > auto format(const TreeSpanFormatter &formatter, FormatContext &ctx) {
		const lizard::IndexSpaceManager &manager = formatter.getManager();

		std::string formatted = fmt::format("Total amount of expressions: {}", formatter.get().size());

		for (const lizard::NamedTensorExprTree &tree : formatter.get()) {
			formatted += fmt::format("\n  {}", lizard::NamedTensorExprTreeFormatter(tree, manager));
		}

		return fmt::formatter< std::string_view >::format(formatted, ctx);
	}
};

namespace lizard {
auto TextExport::getName() const -> std::string_view {
	return "TextExport";
}

void TextExport::exportExpressions(nonstd::span< const NamedTensorExprTree > expressions,
								   const IndexSpaceManager &manager) {
	getLogger().info("{}", TreeSpanFormatter(expressions, manager));
}

} // namespace lizard
