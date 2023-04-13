// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <lizard/symbolic/Index.hpp>
#include <lizard/symbolic/IndexSpace.hpp>
#include <lizard/symbolic/IndexType.hpp>
#include <lizard/symbolic/Spin.hpp>

#include <libperm/Permutation.hpp>
#include <libperm/PrimitivePermutationGroup.hpp>
#include <libperm/Utils.hpp>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <random>
#include <type_traits>
#include <vector>

namespace lizard::test {

template< typename Group = perm::PrimitivePermutationGroup, typename Container >
auto generate(const Container &generators) -> Group {
	Group group;

	for (const auto &currentGenerator : generators) {
		group.addGenerator(perm::Permutation(currentGenerator));
	}

	return group;
}

template< typename Perm, typename Group = perm::PrimitivePermutationGroup >
auto generate(const std::initializer_list< Perm > &generators) -> Group {
	return generate< Group, std::initializer_list< Perm > >(generators);
}


template< typename Sequence, typename Group >
auto equivalentSequences(const Sequence &sequence, const Group &symmetry) -> std::vector< Sequence > {
	std::vector< perm::Permutation > elements;
	symmetry.getElementsTo(elements);

	std::vector< Sequence > equivalentSeq;
	equivalentSeq.reserve(elements.size());

	for (const perm::Permutation &currentPerm : elements) {
		Sequence copy = sequence;
		perm::applyPermutation(copy, currentPerm);

		equivalentSeq.push_back(std::move(copy));
	}

	return equivalentSeq;
}

template< typename T, typename Group >
auto equivalentSequences(const std::initializer_list< T > &sequence, const Group &symmetry)
	-> std::vector< std::vector< T > > {
	return equivalentSequences< std::initializer_list< T >, Group >(sequence, symmetry);
}


inline auto getSpins(std::size_t seed = 1) -> std::array< Spin, 4 > {
	std::array< Spin, 4 > spins = { Spin::Alpha, Spin::Beta, Spin::None, Spin::Both };

	std::mt19937 generator(seed);

	std::shuffle(spins.begin(), spins.end(), generator);

	return spins;
}

inline auto getIndexTypes(std::size_t seed = 2) -> std::array< IndexType, 3 > {
	std::array< IndexType, 3 > types = { IndexType::Annihilator, IndexType::Creator, IndexType::External };

	std::mt19937 generator(seed);

	std::shuffle(types.begin(), types.end(), generator);

	return types;
}

inline auto indexSpaceSequence(std::size_t size, std::size_t seed = 4) -> std::vector< IndexSpace > {
	std::vector< IndexSpace > spaces;
	spaces.reserve(size);

	if (size == 0) {
		return spaces;
	}

	const std::array< Spin, 4 > spins = getSpins(seed * 2 + 1);

	std::mt19937 generator(seed + 1);
	std::uniform_int_distribution< std::size_t > dist(0, size - 1);

	for (std::size_t i = 0; i < size; ++i) {
		spaces.emplace_back(i, spins.at(dist(generator)));
	}

	std::shuffle(spaces.begin(), spaces.end(), generator);

	return spaces;
}


inline auto indexSequence(std::size_t size, std::size_t seed = 16) -> std::vector< Index > {
	std::vector< Index > indices;
	indices.reserve(size);

	if (size == 0) {
		return indices;
	}

	std::vector< IndexSpace > spaces = indexSpaceSequence(size);
	std::array< IndexType, 3 > types = getIndexTypes(seed + 3);

	std::mt19937 generator(seed);
	std::uniform_int_distribution< std::size_t > dist(0, std::max(spaces.size(), types.size()) - 1);

	for (std::size_t i = 0; i < size; ++i) {
		const std::size_t spaceIndex = dist(generator) % spaces.size();
		const std::size_t typeIndex  = dist(generator) % types.size();

		indices.emplace_back(i, spaces.at(spaceIndex), types.at(typeIndex));
	}

	std::shuffle(indices.begin(), indices.end(), generator);

	return indices;
}

} // namespace lizard::test
