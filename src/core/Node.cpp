// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/Node.hpp"
#include "lizard/core/ExpressionCardinality.hpp"
#include "lizard/core/ExpressionOperator.hpp"
#include "lizard/core/ExpressionType.hpp"
#include "lizard/core/Numeric.hpp"
#include "lizard/core/SignedCast.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <utility>

#include <hedley.h>

namespace lizard {

Node::Node(ExpressionType type, Numeric left, Numeric right) noexcept
	: m_flags(type), m_left(std::move(left)), m_right(std::move(right)) {
	assert(type != ExpressionType::Literal || (left.isValid() && right.isValid()));   // NOLINT
	assert(type != ExpressionType::Variable || (left.isValid() && !right.isValid())); // NOLINT
	assert(type != ExpressionType::Operator || (left.isValid() && right.isValid()));  // NOLINT
}

Node::Node(ExpressionOperator operatorType) noexcept : m_flags(ExpressionType::Operator, operatorType) {
}

Node::Node(std::make_signed_t< Numeric::numeric_type > numerator,
		   std::make_signed_t< Numeric::numeric_type > denominator) noexcept
	: m_flags(ExpressionType::Literal), m_left(signed_cast< Numeric::numeric_type >(numerator)),
	  m_right(signed_cast< Numeric::numeric_type >(denominator)) {
}

auto Node::getCardinality() const -> ExpressionCardinality {
	switch (m_flags.get< ExpressionType >()) {
		case ExpressionType::Operator:
			return ExpressionCardinality::Binary;
		case ExpressionType::Variable:
			[[fallthrough]];
		case ExpressionType::Literal:
			return ExpressionCardinality::Nullary;
	}

	assert(false); // NOLINT
	HEDLEY_UNREACHABLE();
}

auto Node::getType() const -> ExpressionType {
	return m_flags.get< ExpressionType >();
}

auto Node::getOperator() const -> ExpressionOperator {
	assert(getType() == ExpressionType::Operator); // NOLINT

	return m_flags.get< ExpressionOperator >();
}

auto Node::hasParent() const -> bool {
	return m_parentID.isValid();
}

auto Node::getParent() const -> Numeric {
	return m_parentID;
}

void Node::setParent(Numeric parentID) {
	m_parentID = std::move(parentID);
}

auto Node::hasLeftChild() const -> bool {
	return getCardinality() != ExpressionCardinality::Nullary && m_left.isValid();
}

auto Node::getLeftChild() const -> Numeric {
	return m_left;
}

void Node::setLeftChild(Numeric childID) {
	assert(getCardinality() != ExpressionCardinality::Nullary); // NOLINT
	m_left = std::move(childID);
}

auto Node::hasRightChild() const -> bool {
	return getCardinality() == ExpressionCardinality::Binary && m_right.isValid();
}

auto Node::getRightChild() const -> Numeric {
	return m_right;
}

void Node::setRightChild(Numeric childID) {
	assert(getCardinality() == ExpressionCardinality::Binary); // NOLINT
	m_right = std::move(childID);
}



auto operator==(const Node &lhs, const Node &rhs) -> bool {
	return lhs.m_flags == rhs.m_flags && lhs.m_parentID == rhs.m_parentID && lhs.m_left == rhs.m_left
		   && lhs.m_right == rhs.m_right;
}

auto operator!=(const Node &lhs, const Node &rhs) -> bool {
	return !(lhs == rhs);
}

auto operator<<(std::ostream &stream, const Node &node) -> std::ostream & {
	stream << node.getType() << "{ ";

	switch (node.getType()) {
		case ExpressionType::Operator:
			stream << "'" << node.getOperator() << "'";
			switch (node.getCardinality()) {
				case ExpressionCardinality::Binary:
					stream << ", " << node.getLeftChild() << ", " << node.getRightChild();
					break;
				case ExpressionCardinality::Unary:
					stream << ", " << node.getLeftChild();
					break;
				case ExpressionCardinality::Nullary:
					break;
			}
			stream << " ";
			break;
		case ExpressionType::Literal:
			stream << signed_cast< std::make_signed_t< Numeric::numeric_type > >(node.getLeftChild()) << " / "
				   << signed_cast< std::make_signed_t< Numeric::numeric_type > >(node.getRightChild()) << " ";
			break;
		case ExpressionType::Variable:
			stream << "stored at idx " << node.getLeftChild() << " ";
			break;
	}

	stream << "}";

	return stream;
}

} // namespace lizard
