// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "IndexTracker.hpp"
#include "SymmetryUtils.hpp"

#include "lizard/format/FormatSupport.hpp"
#include "lizard/process/ProcessingException.hpp"
#include "lizard/process/SkeletonQuantityMapper.hpp"

#include <libperm/Cycle.hpp>
#include <libperm/ExplicitPermutation.hpp>
#include <libperm/PrimitivePermutationGroup.hpp>
#include <libperm/Utils.hpp>

#include <fmt/core.h>

#include <cassert>

namespace lizard {

auto SkeletonQuantityMapper::getName() const -> std::string_view {
	return "SkeletonMapper";
}

void replaceBySkeleton(TensorExpr &expression, const IndexTracker &tracker);

void SkeletonQuantityMapper::process(std::vector< NamedTensorExprTree > &expressions,
									 const IndexSpaceManager &manager) {
	for (NamedTensorExprTree &currentTree : expressions) {
		if (!currentTree.getResult().getIndices().empty()) {
			// In order to implement this, the following considerations have to be taken into account:
			// - 2-index tensors: always equal to the skeleton quantity
			// - 4-index tensors: the abab spin case is equal to the skeleton quantity
			// - 6-index tensors and above: No direct relation with a skeleton quantity and one of the spin-cases of
			//      the result tensor.
			//      Due to having > 2 creator/annihilator indices, there will always be at least two indices with the
			//      same spin within each group, requiring antisymmetrization of skeleton quantities in order to
			//      represent them.
			throw ProcessingException(
				fmt::format("Can't map {} to a skeleton quantity: non-scalar result tensors not yet implemented",
							NamedTensorExprTreeFormatter(currentTree, manager)));
		}

		// We have to use post-order iteration in order to ensure that after having visited a given node,
		// the next node in the iteration is ALWAYS its parent (if any). This allows us to replace the
		// visited node by arbitrary sub-trees without affecting the iteration itself.
		auto currentIter      = currentTree.begin< TreeTraversal::DepthFirst_PostOrder >();
		const auto currentEnd = currentTree.end< TreeTraversal::DepthFirst_PostOrder >();

		for (; currentIter != currentEnd; ++currentIter) {
			TensorExpr currentExpr = *currentIter;

			switch (currentExpr.getType()) {
				case ExpressionType::Literal:
				case ExpressionType::Operator:
					// We don't have to do anything with those
					continue;
				case ExpressionType::Variable:
					break;
			}

			// For every variable, we have to check if and how we want to perform the mapping to a
			// corresponding skeleton quantity
			const TensorElement &currentElement = currentExpr.getVariable();

			if (currentElement.getBlock().getTensor().getSpinProjection() != 0) {
				throw ProcessingException(fmt::format(
					"Can't map {} to skeleton quantity: tensors with non-zero spin projection not yet supported",
					TensorElementFormatter(currentElement, manager)));
			}

			auto iter = std::find_if(
				currentElement.getBlock().getIndexSlots().begin(), currentElement.getBlock().getIndexSlots().end(),
				[](const IndexSpace &space) { return space.getSpin() == Spin::None || space.getSpin() == Spin::Both; });

			if (iter != currentElement.getBlock().getIndexSlots().end()) {
				getLogger().debug(
					"Skipping {} - contains indices that have no spin or are still in spin-orbit formalism",
					TensorElementFormatter(currentElement, manager));
				continue;
			}

			const nonstd::span< const Index > indices = currentElement.getIndices();
			IndexTracker tracker(indices);

			// Verify that the expected symmetries exist
			if (!containsAntisymmetryOf(currentElement.getBlock().getSlotSymmetry(), tracker.creators())) {
				getLogger().debug("Skipping {} - The creator indices are not fully antisymmetric",
								  TensorElementFormatter(currentElement, manager));
				continue;
			}
			if (!containsAntisymmetryOf(currentElement.getBlock().getSlotSymmetry(), tracker.annihilators())) {
				getLogger().debug("Skipping {} - The annihilator indices are not fully antisymmetric",
								  TensorElementFormatter(currentElement, manager));
				continue;
			}

			// All indices are either Alpha or Beta spin and the tensor has a spin projection of zero
			// Therefore, the amount of Alpha and Beta spin in creators and annihilators must be identical

			assert(tracker.creators().size() == tracker.annihilators().size()); // NOLINT
			assert(tracker.externals().empty());                                // NOLINT

			if (tracker.creators().empty()) {
				// Nothing to do for scalar tensors
				continue;
			}

			replaceBySkeleton(currentExpr, tracker);
		}
	}
}

void replaceBySkeleton(TensorExpr &expression, const IndexTracker &tracker) {
	assert(expression.getType() == ExpressionType::Variable); // NOLINT
	const TensorElement &element        = expression.getVariable();
	nonstd::span< const Index > indices = element.getIndices();

	std::vector< std::size_t > alphaCreator;
	std::vector< std::size_t > betaCreator;

	for (std::size_t current : tracker.creators()) {
		assert(current < indices.size()); // NOLINT

		if (indices[current].getSpace().getSpin() == Spin::Alpha) {
			alphaCreator.push_back(current);
		} else {
			assert(indices[current].getSpace().getSpin() == Spin::Beta); // NOLINT
			betaCreator.push_back(current);
		}
	}

	// Create symmetry group describing the linear combination of skeleton tensor index sequences required
	// to map the given TensorElement to skeleton tensors
	// The group is generated by the antisymmetric, pairwise exchanges of indices within the two groups
	perm::PrimitivePermutationGroup group = makeDiscontinousAntisymmetricRanges({ alphaCreator, betaCreator });

	std::vector< perm::Permutation > permutations;
	group.getElementsTo(permutations);

	std::vector< Index > baseIndexSequence;
	baseIndexSequence.reserve(indices.size());
	for (const Index &current : indices) {
		IndexSpace skeletonSpace = current.getSpace();
		skeletonSpace.setSpin(Spin::None);

		baseIndexSequence.emplace_back(current.getID(), std::move(skeletonSpace), current.getType());
	}

	// Create skeleton symmetry that reflects the particle-1,2-symmetry, which skeleton
	// quantities ought to have (same as spin-summed quantities)
	perm::PrimitivePermutationGroup skeletonSymmetry =
		makeColumnsymmetricExchanges(tracker.creators(), tracker.annihilators());


	TensorExprTree replacementTree;
	std::size_t nElements = permutations.size();
	replacementTree.reserve(2 * nElements - 1, nElements);

	bool firstIteration = true;
	for (const perm::Permutation &currentPerm : permutations) {
		std::vector< Index > currentSequence = baseIndexSequence;

		perm::applyPermutation(currentSequence, currentPerm);

		auto [currentElement, sign] =
			TensorElement::create(element.getBlock().getTensor(), std::move(currentSequence), skeletonSymmetry);

		sign *= currentPerm->sign();

		replacementTree.add(std::move(currentElement));

		if (sign != 1) {
			replacementTree.add(TreeNode(sign));
			replacementTree.add(TreeNode(ExpressionOperator::Times));
		}

		if (!firstIteration) {
			// Add the current expression to the previously added one(s)
			replacementTree.add(TreeNode(ExpressionOperator::Plus));
		} else {
			firstIteration = false;
		}
	}

	assert(replacementTree.isValid()); // NOLINT

	if (replacementTree.size() > 1) {
		expression.substituteWith(replacementTree.getRoot());
	} else {
		expression.substituteWith(std::move(replacementTree.getRoot().getVariable()));
	}
}


} // namespace lizard
