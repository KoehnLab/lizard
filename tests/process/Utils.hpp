// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexSpaceManager.hpp"
#include "lizard/symbolic/IndexType.hpp"
#include "lizard/symbolic/Spin.hpp"
#include "lizard/symbolic/Tensor.hpp"
#include "lizard/symbolic/TensorElement.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"
#include "lizard/symbolic/TreeNode.hpp"

#include "SymmetryUtils.hpp"

#include <libperm/SpecialGroups.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <charconv>
#include <stack>
#include <string_view>
#include <type_traits>
#include <vector>

#include <fmt/core.h>

namespace lizard::test {

/**
 * @returns The IndexSpaceManager preconfigured with default index spaces occ, virt and ext
 */
[[nodiscard]] inline auto getIndexSpaceManager() -> IndexSpaceManager & {
	static IndexSpaceManager manager;
	static bool initialized = false;

	if (!initialized) {
		manager.registerSpace(IndexSpace(0, Spin::Both),
							  IndexSpaceData("occ", 16, Spin::Both, { 'i', 'j', 'k', 'l', 'm', 'n' }));
		manager.registerSpace(IndexSpace(1, Spin::Both),
							  IndexSpaceData("virt", 256, Spin::Both, { 'a', 'b', 'c', 'd', 'e', 'f' }));
		manager.registerSpace(IndexSpace(2, Spin::None),
							  IndexSpaceData("ext", 256, Spin::None, { 'P', 'Q', 'R', 'S', 'T', 'U' }));

		initialized = true;
	}

	return manager;
}

/**
 * @param tensorName The name of the tensor the desired block belongs to
 * @param indexCount The amount of indices/index slots of the respective block
 * @param isSkeleton Whether the symmetry shall be created for a skeleton tensor
 * @returns The symmetry that the given tensor block should have
 */
[[nodiscard]] inline auto getDefaultSymmetry(std::string_view tensorName, std::size_t indexCount,
											 bool isSkeleton = false) -> TensorBlock::SlotSymmetry {
	if (indexCount == 0) {
		return {};
	}

	if (tensorName == "H" || tensorName == "T" || tensorName == "O" || tensorName == "R") {
		if (indexCount == 2) {
			return {};
		}
		if (indexCount == 4) {
			if (isSkeleton) {
				const std::vector< std::size_t > creatorPositions     = { 0, 1 };
				const std::vector< std::size_t > annihilatorPositions = { 2, 3 };

				return makeColumnsymmetricExchanges(creatorPositions, annihilatorPositions);
			}

			return perm::antisymmetricRanges({ { 0, 1 }, { 2, 3 } });
		}

		throw std::runtime_error(fmt::format("Invalid index count ({}) for standard tensor", indexCount));
	}

	// All other tensors will be treated as not having index symmetry
	return {};
}

[[nodiscard]] inline auto createIndexSequence(std::string_view spec) -> std::vector< Index > {
	assert(spec.at(0) == '['); // NOLINT
	const std::size_t closingBracket = spec.find(']');
	const std::size_t openingParen   = spec.find('(');
	const std::size_t closingParen   = spec.find(')');

	assert(closingBracket != std::string::npos); // NOLINT

	std::vector< char > indexLabels;
	std::vector< IndexType > indexTypes;
	std::vector< Spin > indexSpins;

	// Parse index labels + types
	for (char c : spec.substr(1, closingBracket - 1)) {
		switch (c) {
			case '+':
				indexTypes.push_back(IndexType::Creator);
				break;
			case '-':
				indexTypes.push_back(IndexType::Annihilator);
				break;
			case ' ':
				break;
			case ',':
				if (indexLabels.size() > indexTypes.size()) {
					// If no type was specified, we assume it's an external index
					assert(indexLabels.size() == indexTypes.size() + 1); // NOLINT
					indexTypes.push_back(IndexType::External);
				}
				break;
			default:
				indexLabels.push_back(c);
				break;
		}
	}

	if (indexLabels.size() > indexTypes.size()) {
		// If no type was specified for the last label, we assume it's an external index
		assert(indexLabels.size() == indexTypes.size() + 1); // NOLINT
		indexTypes.push_back(IndexType::External);
	}
	assert(indexLabels.size() == indexTypes.size()); // NOLINT

	if (openingParen != std::string::npos) {
		assert(closingParen != std::string::npos); // NOLINT
		assert(openingParen < closingParen);       // NOLINT

		for (char c : spec.substr(openingParen + 1, closingParen - openingParen - 1)) {
			switch (c) {
				case '/':
					indexSpins.push_back(Spin::Alpha);
					break;
				case '\\':
					indexSpins.push_back(Spin::Beta);
					break;
				case '|':
					indexSpins.push_back(Spin::Both);
					break;
				case '.':
					indexSpins.push_back(Spin::None);
					break;
				default:
					throw std::runtime_error(fmt::format("Invalid spin specifier '{}'", c));
			}
		}

		assert(indexSpins.size() == indexLabels.size()); // NOLINT
	}

	// Now actually create those indices
	std::vector< Index > indices;
	for (std::size_t i = 0; i < indexLabels.size(); ++i) {
		IndexSpace space           = getIndexSpaceManager().createFromLabel(indexLabels.at(i));
		const IndexSpaceData &data = getIndexSpaceManager().getData(space);

		if (indexSpins.empty()) {
			space.setSpin(Spin::None);
		} else {
			space.setSpin(indexSpins.at(i));
		}

		Index::Id id = static_cast< Index::Id >(std::distance(
			data.getLabels().begin(), std::find(data.getLabels().begin(), data.getLabels().end(), indexLabels.at(i))));

		indices.emplace_back(id, std::move(space), indexTypes.at(i));
	}

	return indices;
}

/**
 * Parses the given tensor element specification and returns the corresponding object
 *
 * @param spec The spec to use
 * @param signPtr A pointer to an integer to which the sign (due to symmetry reordering of indices)
 * @param isSkeletonTensor Whether the created tensor represents a skeleton tensor
 * of the created TensorElement should be written to
 * @returns The created TensorElement
 */
[[nodiscard]] inline auto createTensorElement(std::string_view spec, int *signPtr = nullptr,
											  bool isSkeletonTensor = false) -> TensorElement {
	const std::size_t openingBracket = spec.find('[');

	assert(openingBracket != std::string::npos); // NOLINT

	std::string_view name = spec.substr(0, openingBracket);

	std::vector< Index > indices = createIndexSequence(spec.substr(openingBracket));

	TensorBlock::SlotSymmetry symmetry = getDefaultSymmetry(name, indices.size(), isSkeletonTensor);
	auto [element, sign] = TensorElement::create(Tensor(std::string(name)), std::move(indices), std::move(symmetry));

	if (signPtr != nullptr) {
		*signPtr = sign;
	}

	return element;
}

namespace details {
	[[nodiscard]] inline auto startsLikeNumber(std::string_view input) -> bool {
		return !input.empty()
			   && (static_cast< bool >(std::isdigit(input[0]))
				   || ((input[0] == '-' || input[0] == '+') && input.size() > 1
					   && static_cast< bool >(std::isdigit(input[1]))));
	}

	template< typename TreeType > void addLiteral(TreeType &tree, std::string_view token) {
		int value                     = 0;
		std::from_chars_result result = std::from_chars(token.data(), token.data() + token.size(), value);
		if (result.ptr != (token.data() + token.size())) {
			throw std::runtime_error(fmt::format("Failed to parse '{}' as an integer number", token));
		}

		tree.add(TreeNode(value, 1));
	}

	template< typename TreeType >
	void addTensorElement(TreeType &tree, std::string_view token, bool isSkeleton = false) {
		int sign = 1;
		tree.add(createTensorElement(token, &sign, isSkeleton));

		if (sign != 1) {
			// Add the sign into the expression
			tree.add(TreeNode(sign));
			tree.add(TreeNode(ExpressionOperator::Times));
		}
	}

	template< typename TreeType > void addToken(TreeType &tree, std::string_view token, bool isSkeleton = false) {
		if (token == "+") {
			tree.add(TreeNode(ExpressionOperator::Plus));
		} else if (token == "*") {
			tree.add(TreeNode(ExpressionOperator::Times));
		} else if (details::startsLikeNumber(token)) {
			addLiteral(tree, token);
		} else {
			addTensorElement(tree, token, isSkeleton);
		}
	}

	[[nodiscard]] inline auto nextToken(std::string_view input, std::size_t *skipped = nullptr) -> std::string_view {
		// Gobble away whitespace
		std::size_t skippedChars = 0;
		while (!input.empty() && static_cast< bool >(std::isspace(input[0]))) {
			input = input.substr(1);
			skippedChars++;
		}
		if (skipped != nullptr) {
			*skipped = skippedChars;
		}

		if (input.empty()) {
			return {};
		}

		std::size_t tokenSize = 0;
		if (startsLikeNumber(input)) {
			tokenSize = 1;
			while (tokenSize < input.size() && static_cast< bool >(std::isdigit(input[tokenSize]))) {
				tokenSize++;
			}
		} else if (input[0] == '+' || input[0] == '*' || input[0] == '(' || input[0] == ')') {
			tokenSize = 1;
		} else if (static_cast< bool >(std::isalpha(input[0]))) {
			// Tensor element spec
			// Name
			tokenSize = 1;
			while (tokenSize < input.size() && static_cast< bool >(std::isalpha(input[tokenSize]))) {
				tokenSize++;
			}

			// Opening bracket
			assert(input[tokenSize] == '['); // NOLINT
			tokenSize++;

			// Index specification
			while (tokenSize < input.size() && input[tokenSize] != ']') {
				tokenSize++;
			}

			// Closing bracket
			assert(input[tokenSize] == ']'); // NOLINT
			tokenSize++;

			// Optional index spin specification
			if (tokenSize < input.size() && input[tokenSize] == '(') {
				tokenSize++;

				while (tokenSize < input.size() && input[tokenSize] != ')') {
					tokenSize++;
				}

				assert(input[tokenSize] == ')'); // NOLINT
				tokenSize++;
			}
		}

		assert(tokenSize > 0);             // NOLINT
		assert(tokenSize <= input.size()); // NOLINT

		return input.substr(0, tokenSize);
	}


} // namespace details

/**
 * Parses the given specification and returns the corresponding ExpressionTree
 *
 * @param spec The tree's specification (in infix notation; parenthesis supported)
 * @param skeletonExpressions Whether the tensors appearing in the spec are understood as skeleton quantities
 * @returns The created tree
 *
 * @tparam TreeType the exact type of the tree to create (must be a TensorExprTree or a NamedTensorExprTree)
 */
template< typename TreeType >
[[nodiscard]] auto createTree(std::string_view spec, bool skeletonExpressions = false) -> TreeType {
	TreeType tree = [&]() {
		if constexpr (std::is_same_v< TreeType, NamedTensorExprTree >) {
			std::size_t equalSign = spec.find('=');
			assert(equalSign != std::string::npos); // NOLINT
			std::string_view resultSpec = spec.substr(0, equalSign);
			spec                        = spec.substr(equalSign + 1);
			return NamedTensorExprTree(createTensorElement(resultSpec));
		} else {
			return TreeType{};
		}
	}();

	// Infix to postfix conversion in order to construct the tree from it
	// -> e.g. https://algotree.org/algorithms/stack_based/infix_to_postfix/

	std::stack< std::string_view > tokenStack;
	while (!spec.empty()) {
		std::size_t skipped    = 0;
		std::string_view token = details::nextToken(spec, &skipped);
		spec                   = spec.substr(token.size() + skipped);

		if (token.empty()) {
			continue;
		}

		if (token == "(" || token == "*") {
			tokenStack.push(token);
		} else if (token == ")") {
			while (tokenStack.top() != "(") {
				details::addToken(tree, tokenStack.top(), skeletonExpressions);

				tokenStack.pop();
			}

			assert(tokenStack.top() == "("); // NOLINT
			tokenStack.pop();
		} else if (token == "+") {
			while (!tokenStack.empty() && tokenStack.top() == "*") {
				// Handle operator precedence
				tree.add(TreeNode(ExpressionOperator::Times));
				tokenStack.pop();
			}

			tokenStack.push(token);
		} else if (details::startsLikeNumber(token)) {
			// Literal value
			details::addLiteral(tree, token);
		} else {
			// This is expected to be a tensor element specification
			details::addTensorElement(tree, token, skeletonExpressions);
		}
	}

	// Append missing operators
	while (!tokenStack.empty()) {
		if (tokenStack.top() != "(") {
			details::addToken(tree, tokenStack.top(), skeletonExpressions);
		}

		tokenStack.pop();
	}

	return tree;
}

} // namespace lizard::test
