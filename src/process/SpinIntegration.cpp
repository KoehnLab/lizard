// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "IndexTracker.hpp"
#include "SpinLSE.hpp"
#include "SymmetryUtils.hpp"

#include "lizard/format/FormatSupport.hpp"
#include "lizard/process/ProcessingException.hpp"
#include "lizard/process/SpinIntegration.hpp"
#include "lizard/symbolic/Expression.hpp"
#include "lizard/symbolic/ExpressionType.hpp"
#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/Spin.hpp"
#include "lizard/symbolic/TensorBlock.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <libperm/Cycle.hpp>
#include <libperm/ExplicitPermutation.hpp>

#include <fmt/core.h>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <limits>
#include <stack>
#include <unordered_map>
#include <vector>

namespace lizard {

auto SpinIntegration::getName() const -> std::string_view {
	return "SpinIntegration";
}

void processProduct(TensorExpr &rootExpression, const IndexSpaceManager &manager);

auto hasNecessaryAntisymmetry(const TensorElement &element, const IndexTracker &tracker) -> bool {
	return containsAntisymmetryOf(element.getBlock().getSlotSymmetry(), tracker.creators())
		   && containsAntisymmetryOf(element.getBlock().getSlotSymmetry(), tracker.annihilators());
}

void SpinIntegration::process(std::vector< NamedTensorExprTree > &expressions, const IndexSpaceManager &manager) {
	for (NamedTensorExprTree &currentTree : expressions) {
		if (!currentTree.getResult().getIndices().empty()) {
			throw ProcessingException(
				fmt::format("Can't spin-integrate {}: Non-scalar result tensors not yet supported",
							TensorElementFormatter(currentTree.getResult(), manager)));
		}

		std::stack< TensorExpr > toVisit;
		toVisit.push(currentTree.getRoot());

		while (!toVisit.empty()) {
			TensorExpr expr = std::move(toVisit.top());
			toVisit.pop();

			switch (expr.getType()) {
				case ExpressionType::Literal:
					continue;
				case ExpressionType::Variable:
					processProduct(expr, manager);
					break;
				case ExpressionType::Operator:
					switch (expr.getOperator()) {
						case ExpressionOperator::Plus:
							toVisit.push(expr.getLeftArg());
							toVisit.push(expr.getRightArg());
							continue;
						case ExpressionOperator::Times:
							processProduct(expr, manager);
							break;
					}
					break;
			}
		}
	}
}

[[nodiscard]] auto setupLSE(const ConstTensorExpr &rootExpression, const IndexSpaceManager &manager,
							std::size_t *nTensorElements = nullptr) -> SpinLSE;

void processProduct(TensorExpr &rootExpression, const IndexSpaceManager &manager) {
	std::size_t nTensorElements = 0;

	const SpinLSE system = setupLSE(rootExpression, manager, &nTensorElements);

	const std::vector< std::vector< Spin > > solutions = system.solve();

	if (solutions.empty()) {
		// There are no spin labels to distribute, so we can return early
		return;
	}

	// Replace the rootExpression with a sum of all possible explicit spin distributions on the indices within the
	// product
	TensorExprTree replacementTree;
	replacementTree.reserve((rootExpression.size() + 1) * solutions.size(), nTensorElements * solutions.size());

	for (const std::vector< Spin > &currentSolution : solutions) {
		// Copy rootExpression, but adapt the spin of affected indices according to currentSolution
		auto iter      = rootExpression.begin< TreeTraversal::DepthFirst_PostOrder >();
		const auto end = rootExpression.end< TreeTraversal::DepthFirst_PostOrder >();

		int factor = 1;
		for (; iter != end; ++iter) {
			const ConstTensorExpr &currentExpr = *iter;

			switch (currentExpr.getType()) {
				case ExpressionType::Literal:
					replacementTree.add(TreeNode(currentExpr.getLiteral()));
					break;
				case ExpressionType::Operator:
					replacementTree.add(TreeNode(currentExpr.getOperator()));
					break;
				case ExpressionType::Variable: {
					// Create a new tensor element that uses indices with the corresponding spins
					const TensorElement &element = currentExpr.getVariable();
					std::vector< Index > indices(element.getIndices().begin(), element.getIndices().end());

					for (Index &currentIndex : indices) {
						auto indexIter = std::find_if(system.getVariables().begin(), system.getVariables().end(),
													  Index::FindByName{ currentIndex });

						if (indexIter == system.getVariables().end()) {
							assert(currentIndex.getSpace().getSpin() != Spin::Both); // NOLINT
							continue;
						}



						// Set spin accordingly
						IndexSpace space = currentIndex.getSpace();
						space.setSpin(currentSolution.at(
							static_cast< std::size_t >(std::distance(system.getVariables().begin(), indexIter))));
						currentIndex.setSpace(std::move(space));
					}

					auto [integratedElement, sign] = TensorElement::create(
						element.getBlock().getTensor(), std::move(indices), element.getBlock().getSlotSymmetry());

					factor *= sign;
					replacementTree.add(std::move(integratedElement));
				} break;
			}
		}

		if (factor != 1) {
			// Add additional pre-factor (arising from re-sorting indices in the spin-integrated tensor elements)
			replacementTree.add(TreeNode(factor));
			replacementTree.add(TreeNode(ExpressionOperator::Times));
		}
	}

	// Add the operators that'll add all of the different spin cases together (and bring the tree into a complete/valid
	// state again)
	for (std::size_t i = 1; i < solutions.size(); ++i) {
		replacementTree.add(TreeNode(ExpressionOperator::Plus));
	}

	assert(replacementTree.isValid()); // NOLINT

	rootExpression.substituteWith(replacementTree.getRoot());
}

auto setupLSE(const ConstTensorExpr &rootExpression, const IndexSpaceManager &manager, std::size_t *nTensorElements)
	-> SpinLSE {
	SpinLSE system;

	std::size_t elementCount = 0;

	for (const ConstTensorExpr &currentExpression : rootExpression) {
		switch (currentExpression.getType()) {
			case ExpressionType::Literal:
				break;
			case ExpressionType::Operator:
				if (currentExpression.getOperator() != ExpressionOperator::Times) {
					// In order to handle spin-integration of things like A * (B + C), we'll have to first ensure
					// that B and C use the exact same indices in which case we can treat the addition's result as
					// just another factor in our spin-integration routine
					throw ProcessingException("Spin integration for products involving sums is not yet implemented");
				}
				break;
			case ExpressionType::Variable: {
				elementCount++;
				const TensorElement &currentElement = currentExpression.getVariable();

				const nonstd::span< const Index > indices = currentElement.getIndices();
				const IndexTracker tracker(currentElement.getIndices());

				system.beginEquation();

				// TODO: This assumes that the tensor allows to permute at least all creators or all annihilators
				// with each other such that the criterion for non-zero spin cases is only given by the sum of spin
				// changes by creators and that of annihilators together yield the tensor's multiplicity (instead of
				// e.g. requiring that a specific creator and a specific annihilator within the tensor have to have
				// the same spin.
				if (!hasNecessaryAntisymmetry(currentElement, tracker)) {
					throw ProcessingException(fmt::format("Can't spin-integrate {}: Missing antisymmetry in the "
														  "set of creator or annihilator indices",
														  TensorElementFormatter(currentElement, manager)));
				}

				for (const Index &currentIdx : indices) {
					if (currentIdx.getSpace().getSpin() != Spin::Both) {
						// This index will remain unchanged
						continue;
					}

					static_assert(static_cast< int >(IndexType::Annihilator) == -1);
					static_assert(static_cast< int >(IndexType::Creator) == 1);

					system.addTerm(currentIdx, static_cast< int >(currentIdx.getType()));
				}

				int spinProjection = currentElement.getBlock().getTensor().getSpinProjection();
				system.endEquation(spinProjection);
			}
		}
	}

	if (nTensorElements != nullptr) {
		*nTensorElements = elementCount;
	}

	return system;
}

} // namespace lizard
