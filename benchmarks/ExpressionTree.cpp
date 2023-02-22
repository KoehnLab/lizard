// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include <lizard/core/Expression.hpp>
#include <lizard/core/ExpressionOperator.hpp>
#include <lizard/core/ExpressionTree.hpp>
#include <lizard/core/ExpressionType.hpp>
#include <lizard/core/Node.hpp>
#include <lizard/core/Numeric.hpp>
#include <lizard/core/TreeTraversal.hpp>

#include <cstdint>
#include <limits>
#include <list>
#include <random>
#include <type_traits>
#include <vector>

#include <benchmark/benchmark.h>

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution< std::make_signed_t< lizard::Numeric::numeric_type > >
	numericDist(0, std::numeric_limits< std::make_signed_t< lizard::Numeric::numeric_type > >::max());

using ExpressionTree = ::lizard::ExpressionTree< int >;
using Expression     = ::lizard::Expression< int >;

constexpr const std::int64_t minAddends = 8;
constexpr const std::int64_t maxAddends = 1 << 20;

auto buildTreeNodes(std::size_t addends) -> std::vector<::lizard::Node > {
	std::vector<::lizard::Node > nodes;

	// Every addend will result in a literal node and then we have plus nodes between every addend (resulting in addends
	// - 1 plus nodes)
	nodes.reserve(2 * addends - 1);

	if (addends == 0) {
		return nodes;
	}

	nodes.push_back(::lizard::Node(numericDist(rng)));

	for (std::size_t i = 1; i < addends; ++i) {
		nodes.push_back(lizard::Node(numericDist(rng)));
		nodes.push_back(lizard::Node(::lizard::ExpressionOperator::Plus));
	}

	return nodes;
}

auto buildTree(std::size_t addends) -> ExpressionTree {
	std::vector<::lizard::Node > nodes = buildTreeNodes(addends);

	ExpressionTree tree;
	tree.reserve(nodes.size());

	for (::lizard::Node &currentExpr : nodes) {
		tree.add(std::move(currentExpr));
	}

	return tree;
}

template< typename T > auto getData(std::size_t addends) -> T {
	(void) addends;
	static_assert(std::is_same_v< T, int >, "Use specialized func");
}
template<> auto getData< std::vector< lizard::Node > >(std::size_t addends) -> std::vector< lizard::Node > {
	return buildTreeNodes(addends);
}
template<> auto getData< ExpressionTree >(std::size_t addends) -> ExpressionTree {
	return buildTree(addends);
}
template<> auto getData< std::list< lizard::Node > >(std::size_t addends) -> std::list< lizard::Node > {
	std::vector< lizard::Node > vec = getData< decltype(vec) >(addends);
	std::list< lizard::Node > list(vec.begin(), vec.end());

	return list;
}

static void BM_iterateIntVec(benchmark::State &state) {
	std::vector< int > intVec;
	const std::size_t elements = 2 * state.range(0) - 1;
	intVec.reserve(elements);
	for (std::size_t i = 0; i < elements; ++i) {
		intVec.push_back(numericDist(rng));
	}

	for (auto _ : state) {
		for (const int &current : intVec) {
			benchmark::DoNotOptimize(current);
		}
	}

	state.SetBytesProcessed(sizeof(decltype(intVec)::value_type) * state.iterations() * state.range(0));
}

template< typename Container > static void BM_expressionIteration(benchmark::State &state) {
	Container data = getData< Container >(state.range(0));

	for (auto _ : state) {
		std::int64_t sum = 0;
		if constexpr (std::is_same_v< typename Container::iterator::value_type, Expression >) {
			for (const Expression &current : data) {
				if (current.getType() == lizard::ExpressionType::Literal) {
					sum += current.getLiteral().getNumerator();
				}
			}
		} else {
			for (const lizard::Node &current : data) {
				if (current.getType() == lizard::ExpressionType::Literal) {
					sum += current.getLeftChild();
				}
			}
		}
		benchmark::DoNotOptimize(sum);
	}

	state.SetBytesProcessed(sizeof(::lizard::Node) * state.iterations() * state.range(0));
}

static void BM_iterateExpressionVec(benchmark::State &state) {
	std::vector<::lizard::Node > expressions = buildTreeNodes(state.range(0));

	for (auto _ : state) {
		for (const lizard::Node &current : expressions) {
			benchmark::DoNotOptimize(current);
		}
	}

	state.SetBytesProcessed(sizeof(::lizard::Node) * state.iterations() * state.range(0));
}

template< lizard::TreeTraversal iteration_order > static void BM_iterateExpressionTree(benchmark::State &state) {
	ExpressionTree tree = buildTree(state.range(0));

	for (auto _ : state) {
		for (auto it = tree.begin< iteration_order >(); it != tree.end< iteration_order >(); ++it) {
			const Expression &current = *it;
			benchmark::DoNotOptimize(current);
		}
	}

	state.SetBytesProcessed(sizeof(::lizard::Node) * state.iterations() * state.range(0));
}

static void BM_iterateExpressionList(benchmark::State &state) {
	std::list< lizard::Node > nodeList = getData< decltype(nodeList) >(state.range(0));

	for (auto _ : state) {
		for (const lizard::Node &current : nodeList) {
			benchmark::DoNotOptimize(current);
		}
	}

	state.SetBytesProcessed(sizeof(::lizard::Node) * state.iterations() * state.range(0));
}

static void BM_indirectExpressionIteration(benchmark::State &state) {
	std::vector< lizard::Node > expressions = getData< decltype(expressions) >(state.range(0));
	std::vector< unsigned int > indices;
	indices.resize(expressions.size());
	for (unsigned int i = 0; i < indices.size(); ++i) {
		indices[i] = numericDist(rng) % indices.size();
	}

	for (auto _ : state) {
		std::int64_t sum = 0;
		for (const lizard::Node &current : expressions) {
			if (current.getType() == lizard::ExpressionType::Literal) {
				sum += current.getLeftChild();
			}
		}
		benchmark::DoNotOptimize(sum);
	}

	state.SetBytesProcessed(sizeof(::lizard::Node) * state.iterations() * state.range(0));
}

BENCHMARK(BM_iterateIntVec)->Range(minAddends, maxAddends)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_iterateExpressionVec)->Range(minAddends, maxAddends)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_iterateExpressionList)->Range(minAddends, maxAddends)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_iterateExpressionTree< lizard::TreeTraversal::DepthFirst_PostOrder >)
	->Range(minAddends, maxAddends)
	->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_iterateExpressionTree< lizard::TreeTraversal::DepthFirst_PreOrder >)
	->Range(minAddends, maxAddends)
	->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_iterateExpressionTree< lizard::TreeTraversal::DepthFirst_InOrder >)
	->Range(minAddends, maxAddends)
	->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_expressionIteration< std::vector< lizard::Node > >)
	->Range(minAddends, maxAddends)
	->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_expressionIteration< std::list< lizard::Node > >)
	->Range(minAddends, maxAddends)
	->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_expressionIteration< ExpressionTree >)->Range(minAddends, maxAddends)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_indirectExpressionIteration)->Range(minAddends, maxAddends)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
