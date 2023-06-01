// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/ITFExport.hpp"

#include "itf/Operation.hpp"
#include "itf/Translator.hpp"

#include <memory>
#include <string>
#include <vector>


namespace lizard {

auto ITFExport::getName() const -> std::string {
	return "ITF";
}

void ITFExport::exportExpressions(nonstd::span< const NamedTensorExprTree > expressions,
								  const IndexSpaceManager &manager) {
	std::string output;
	for (const NamedTensorExprTree &tree : expressions) {
		std::vector< std::unique_ptr< itf::Operation > > operations = itf::translate(tree);

		// TODO

		// Insert operations for loading, storing and dropping tensors

		// Prepend operations for importing/declaring tensors

		// Stringify expressions (into a code-block)
		output += "----code(\"";
		output += tree.getResult().getBlock().getTensor().getName();
		output += "\")\n";
		for (const std::unique_ptr< itf::Operation > &currentOperation : operations) {
			output += currentOperation->stringify(manager);
			output += "\n";
		}
	}

	getLogger().info("Resulting ITF code:\n{}", output);
}

} // namespace lizard
