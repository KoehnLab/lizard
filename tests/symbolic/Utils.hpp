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


inline auto getSpins() -> std::array< Spin, 4 > {
	return { Spin::Alpha, Spin::Beta, Spin::None, Spin::Both };
}

inline auto getIndexTypes() -> std::array< IndexType, 3 > {
	return { IndexType::Annihilator, IndexType::Creator, IndexType::External };
}

inline auto indexSpaceSequence(std::size_t size) -> std::vector< IndexSpace > {
	std::vector< IndexSpace > spaces;
	spaces.reserve(size);

	const std::array< Spin, 4 > spins = getSpins();

	for (std::size_t i = 0; i < size; ++i) {
		spaces.emplace_back(i, spins.at(i % spins.size()));
	}

	return spaces;
}


inline auto indexSequence(std::size_t size, std::size_t variant = 1) -> std::vector< Index > {
	std::vector< Index > indices;
	indices.reserve(size);

	std::vector< IndexSpace > spaces = indexSpaceSequence(std::max(size / 2, static_cast< std::size_t >(1)));
	std::array< IndexType, 3 > types = getIndexTypes();

	for (std::size_t i = 0; i < size; ++i) {
		indices.emplace_back(i, spaces.at(variant * i % spaces.size()), types.at(variant * i % types.size()));
	}

	return indices;
}

} // namespace lizard::test
