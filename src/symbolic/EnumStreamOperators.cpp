// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/ExpressionOperator.hpp"
#include "lizard/symbolic/ExpressionType.hpp"
#include "lizard/symbolic/IndexType.hpp"
#include "lizard/symbolic/Spin.hpp"
#include "lizard/symbolic/TreeTraversal.hpp"

#include <hedley.h>

#include <iostream>

namespace lizard {

auto operator<<(std::ostream &stream, IndexType type) -> std::ostream & {
	switch (type) {
		case IndexType::Annihilator:
			return stream << "Ann";
		case IndexType::External:
			return stream << "Ext";
		case IndexType::Creator:
			return stream << "Cre";
	}

	HEDLEY_UNREACHABLE();
}

auto operator<<(std::ostream &stream, Spin spin) -> std::ostream & {
	switch (spin) {
		case Spin::Alpha:
			return stream << "Al";
		case Spin::Beta:
			return stream << "Be";
		case Spin::Both:
			return stream << "Bo";
		case Spin::None:
			return stream << "No";
	}

	HEDLEY_UNREACHABLE();
}

auto operator<<(std::ostream &stream, const ExpressionOperator &operatorType) -> std::ostream & {
	switch (operatorType) {
		case ExpressionOperator::Plus:
			return stream << "+";
		case ExpressionOperator::Times:
			return stream << "*";
	}

	HEDLEY_UNREACHABLE();
}

auto operator<<(std::ostream &stream, const ExpressionType &type) -> std::ostream & {
	switch (type) {
		case ExpressionType::Operator:
			return stream << "Operator";
		case ExpressionType::Literal:
			return stream << "Literal";
		case ExpressionType::Variable:
			return stream << "Variable";
	}

	HEDLEY_UNREACHABLE();
}

auto operator<<(std::ostream &stream, const TreeTraversal &traversal) -> std::ostream & {
	switch (traversal) {
		case TreeTraversal::DepthFirst_InOrder:
			return stream << "depth-first-in-order";
		case TreeTraversal::DepthFirst_PreOrder:
			return stream << "depth-first-pre-order";
		case TreeTraversal::DepthFirst_PostOrder:
			return stream << "depth-first-post-order";
	}

	HEDLEY_UNREACHABLE();
}

} // namespace lizard
