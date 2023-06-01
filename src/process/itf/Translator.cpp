// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "Translator.hpp"
#include "Contract.hpp"
#include "TensorExpression.hpp"

#include "lizard/process/ProcessingException.hpp"
#include "lizard/symbolic/Contraction.hpp"
#include "lizard/symbolic/ExpressionOperator.hpp"
#include "lizard/symbolic/ExpressionType.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"
#include "lizard/symbolic/TreeTraversal.hpp"

#include <fmt/core.h>

#include <functional>
#include <memory>
#include <stack>
#include <variant>


namespace lizard::itf {

void translateContraction(const ConstTensorExpr &root, const TensorElement &result,
						  std::vector< std::unique_ptr< Operation > > &operations);

auto translate(const NamedTensorExprTree &tree) -> std::vector< std::unique_ptr< Operation > > {
	std::vector< std::unique_ptr< Operation > > operations;

	std::stack< ConstTensorExpr > toVisit;

	toVisit.push(tree.getRoot());

	while (!toVisit.empty()) {
		ConstTensorExpr currentExpr = std::move(toVisit.top());
		toVisit.pop();

		switch (currentExpr.getType()) {
			case ExpressionType::Literal:
				throw ProcessingException("Exporting additions with scalar constants to ITF is not supported");
				break;
			case ExpressionType::Variable: {
				static TensorElement One(Tensor("One"));
				operations.push_back(std::make_unique< Contract >(tree.getResult(), currentExpr.getVariable(), One));
				break;
			}
			case ExpressionType::Operator:
				switch (currentExpr.getOperator()) {
					case ExpressionOperator::Plus:
						toVisit.push(currentExpr.getRightArg());
						toVisit.push(currentExpr.getLeftArg());
						break;
					case ExpressionOperator::Times:
						translateContraction(currentExpr, tree.getResult(), operations);
						break;
				}

				break;
		}
	}

	return operations;
}

using ContractRef = std::reference_wrapper< Contract >;
using Argument    = std::variant< Fraction, TensorExpression, ContractRef >;

[[nodiscard]] auto handleAddition(Argument lhs, Argument rhs) -> Argument {
	if (std::holds_alternative< Fraction >(rhs)) {
		throw ProcessingException("Can't export addition involving a scalar to ITF");
	}

	assert(std::get< TensorExpression >(lhs).getCardinality() == ExpressionCardinality::Nullary); // NOLINT
	assert(std::get< TensorExpression >(rhs).getCardinality() == ExpressionCardinality::Nullary); // NOLINT
	assert(!std::holds_alternative< ContractRef >(lhs));                                          // NOLINT
	assert(!std::holds_alternative< ContractRef >(rhs));                                          // NOLINT

	std::get< TensorExpression >(lhs) += std::get< TensorExpression >(rhs);

	return lhs;
}

[[nodiscard]] auto getContractionArg(const Argument &arg) -> TensorExpression {
	assert(!std::holds_alternative< Fraction >(arg)); // NOLINT

	if (const TensorExpression *expr = std::get_if< TensorExpression >(&arg); expr != nullptr) {
		return *expr;
	}

	assert(std::holds_alternative< ContractRef >(arg)); // NOLINT

	Contract &contraction = std::get< ContractRef >(arg).get();

	static std::size_t intermediateCounter = 1;

	contraction.setResult(contract(contraction.getLhs().getResult(), contraction.getRhs().getResult(),
								   fmt::format("STIN_{:06d}", intermediateCounter++)));

	return { contraction.getResult() };
}

[[nodiscard]] auto handleMultiplication(Argument lhs, Argument rhs, const TensorElement &result,
										std::vector< std::unique_ptr< Operation > > &operations) -> Argument {
	if (std::holds_alternative< Fraction >(rhs)) {
		// Multiplication with a scalar
		if (std::holds_alternative< Fraction >(lhs)) {
			std::get< Fraction >(lhs) *= std::get< Fraction >(rhs);
		} else if (std::holds_alternative< ContractRef >(lhs)) {
			std::get< ContractRef >(lhs).get().multiplyBy(std::get< Fraction >(rhs));
		} else {
			std::get< TensorExpression >(lhs) *= std::get< Fraction >(rhs);
		}

		return lhs;
	}

	assert(!std::holds_alternative< Fraction >(lhs)); // NOLINT

	// Contraction

	std::unique_ptr< Contract > contraction =
		std::make_unique< Contract >(result, getContractionArg(lhs), getContractionArg(rhs));

	Argument arg = *contraction;
	operations.push_back(std::move(contraction));

	return arg;
}

void translateContraction(const ConstTensorExpr &root, const TensorElement &result,
						  std::vector< std::unique_ptr< Operation > > &operations) {
	auto iter = root.cbegin< TreeTraversal::DepthFirst_PostOrder >();
	auto end  = root.cend< TreeTraversal::DepthFirst_PostOrder >();

	std::stack< Argument > arguments;

	for (; iter != end; ++iter) {
		ConstTensorExpr current = *iter;

		switch (current.getType()) {
			case ExpressionType::Literal:
				arguments.push(current.getLiteral());
				continue;
			case ExpressionType::Variable:
				arguments.push(TensorExpression(current.getVariable()));
				continue;
			case ExpressionType::Operator:
				break;
		}

		assert(current.getType() == ExpressionType::Operator); // NOLINT
		assert(arguments.size() >= 2);                         // NOLINT

		Argument rhs = std::move(arguments.top());
		arguments.pop();
		Argument lhs = std::move(arguments.top());
		arguments.pop();

		if (std::holds_alternative< Fraction >(lhs)) {
			// Ensure that in case there is only a single scalar constant, it will be the rhs
			std::swap(lhs, rhs);
		}

		switch (current.getOperator()) {
			case ExpressionOperator::Plus:
				arguments.push(handleAddition(std::move(lhs), std::move(rhs)));
				break;
			case ExpressionOperator::Times:
				arguments.push(handleMultiplication(std::move(lhs), std::move(rhs), result, operations));
				break;
		}
	}

	while (!arguments.empty() && std::holds_alternative< ContractRef >(arguments.top())) {
		arguments.pop();
	}

	if (!arguments.empty()) {
		throw ProcessingException(
			"Encountered left-over (unprocessed) arguments during translation of contraction to ITF");
	}
}

} // namespace lizard::itf
