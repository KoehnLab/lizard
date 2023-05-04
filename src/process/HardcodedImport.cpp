// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/HardcodedImport.hpp"
#include "lizard/process/ImportException.hpp"
#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexSpaceManager.hpp"
#include "lizard/symbolic/Tensor.hpp"
#include "lizard/symbolic/TensorBlock.hpp"
#include "lizard/symbolic/TensorElement.hpp"
#include "lizard/symbolic/TreeNode.hpp"

#include <libperm/Cycle.hpp>
#include <libperm/ExplicitPermutation.hpp>
#include <libperm/Permutation.hpp>
#include <libperm/PrimitivePermutationGroup.hpp>
#include <libperm/SpecialGroups.hpp>

#include <fmt/format.h>

namespace lizard {

HardcodedImport::HardcodedImport(ImportTarget target) : m_target(target) {
}

auto HardcodedImport::getName() const -> std::string {
	return "HardcodedImport";
}

auto HardcodedImport::importExpressions(const IndexSpaceManager &manager) const -> std::vector< NamedTensorExprTree > {
	// Define common symmetries. These definitions assume that the creators are listed before the annihilators
	const perm::PrimitivePermutationGroup fourIdxAntisymmetry = perm::antisymmetricRanges({ { 0, 1 }, { 2, 3 } });
	const perm::PrimitivePermutationGroup twoElectronIntSym   = [&fourIdxAntisymmetry]() {
        auto group = fourIdxAntisymmetry;
        // Symmetry that exchanges creators with annihilators
        group.addGenerator(perm::ExplicitPermutation(perm::Cycle({ { 0, 2 }, { 1, 3 } })));
        return group;
	}();

	switch (m_target) {
		case ImportTarget::CCD_ENERGY: {
			getLogger().info("Importing CCD energy expresion");

			// E = H + 0.25 * H[ijab] T2[abij]
			IndexSpace occ  = manager.createFromName("occ");
			IndexSpace virt = manager.createFromName("virt");

			TensorElement resultElement(Tensor("E"));

			NamedTensorExprTree expression(std::move(resultElement));

			Tensor H("H");
			Tensor T("T");

			TensorElement referenceEnergy     = std::get< 0 >(TensorElement::create(TensorBlock(H), {}));
			auto [twoElectronInt, twoIntSign] = TensorElement::create(
				H,
				{ Index(0, occ, IndexType::Creator), Index(1, occ, IndexType::Creator),
				  Index(0, virt, IndexType::Annihilator), Index(1, virt, IndexType::Annihilator) },
				twoElectronIntSym);
			auto [amplitude, amplitudeSign] =
				TensorElement::create(T,
									  { Index(0, virt, IndexType::Creator), Index(1, virt, IndexType::Creator),
										Index(0, occ, IndexType::Annihilator), Index(1, occ, IndexType::Annihilator) },
									  fourIdxAntisymmetry);

			// Note: the elements in our expression have to be added in postfix order
			expression.add(std::move(referenceEnergy));
			expression.add(TreeNode(twoIntSign * amplitudeSign * 1, 4)); // +/- 1/4
			expression.add(std::move(twoElectronInt));
			expression.add(std::move(amplitude));
			expression.add(TreeNode(ExpressionOperator::Times));
			expression.add(TreeNode(ExpressionOperator::Times));
			expression.add(TreeNode(ExpressionOperator::Plus));

			return { expression };
		}
	}

	throw ImportException(fmt::format("Unknown import target ({})", fmt::underlying(m_target)));
}

} // namespace lizard
