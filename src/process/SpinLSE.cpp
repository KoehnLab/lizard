// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "SpinLSE.hpp"

#include "lizard/process/ProcessingException.hpp"

#include <Eigen/Dense>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <iterator>
#include <limits>

namespace lizard {

void SpinLSE::beginEquation() {
	// New equation corresponds to a new row in our LSE which will start out as
	// having zero coefficients for all variables known so far (aka: as an empty equation)
	m_equations.emplace_back(m_variables.size(), 0);
}

void SpinLSE::endEquation(int result) {
	m_inhomogeneity.push_back(result);

	assert(m_equations.size() == m_inhomogeneity.size()); // NOLINT
}

void SpinLSE::addTerm(const Index &index, int coefficient) {
	auto iter = std::find_if(m_variables.begin(), m_variables.end(), Index::FindByName{ index });

	if (iter == m_variables.end()) {
		// New variable in our system -> extend the already existing equations with a zero entry for it
		for (std::vector< int > &coefficients : m_equations) {
			coefficients.push_back(0);
		}

		// Store this variable
		m_variables.push_back(index);

		// Set coefficient in the current equation
		std::vector< int > &activeEquation        = m_equations.at(m_equations.size() - 1);
		activeEquation[activeEquation.size() - 1] = coefficient;
	} else {
		std::iterator_traits< decltype(iter) >::difference_type position = std::distance(m_variables.begin(), iter);
		assert(position >= 0); // NOLINT
		m_equations.at(m_equations.size() - 1)[static_cast< std::size_t >(position)] = coefficient;
	}
}

auto SpinLSE::getVariables() const -> const std::vector< Index > & {
	return m_variables;
}

auto SpinLSE::solve() const -> std::vector< std::vector< Spin > > {
	// The system of equations is given in matrix form Ax = b
	// where A = m_equations and b = m_inhomogeneity
	// x are the possible solutions to this system that we want to determine

	// We know that the entries in x can only be Alpha or Beta, therefore the space of
	// possible solutions is 2^n with n being the amount of variables

	// The current approach for solving this system is brute-force checking all possible
	// x vectors and then check whether they are valid solutions.
	// In order to handle stepping through all possible vectors, we make use of the fact that
	// in binary, a regular integer can be seen as a bijection to our x vector, where e.g.
	// 0 corresponds to Alpha and 1 corresponds to Beta.
	// Therefore, we can enumerate all possible x vectors by considering the binary representation
	// of the 2^n possible values of an n-bit integer.

	assert(m_equations.size() == m_inhomogeneity.size()); // NOLINT

	if (m_variables.empty()) {
		// Special case of a system without variables -> verify that the system is homogenous
		for (int current : m_inhomogeneity) {
			if (current != 0) {
				throw ProcessingException("Encountered unsolvable spin LSE (no variables but not homogenous)");
			}
		}

		// This system has no solution, which translates to "there are no spin labels to be distributed"
		return {};
	}

	std::size_t nSolutionCandidates = static_cast< std::size_t >(1) << m_variables.size();
	assert(nSolutionCandidates < std::numeric_limits< std::uint32_t >::max()); // NOLINT

	// Convert properties into proper matrices and vectors
	Eigen::Matrix< int, Eigen::Dynamic, Eigen::Dynamic > coefficientMatrix(m_equations.size(), m_variables.size());
	Eigen::Vector< int, Eigen::Dynamic > expectedResult(m_equations.size());
	Eigen::Vector< int, Eigen::Dynamic > solutionCandidate(m_variables.size());

	for (std::size_t row = 0; row < m_equations.size(); ++row) {
		for (std::size_t col = 0; col < m_variables.size(); ++col) {
			coefficientMatrix(static_cast< Eigen::Index >(row), static_cast< Eigen::Index >(col)) =
				m_equations[row][col];
		}

		expectedResult(static_cast< Eigen::Index >(row)) = m_inhomogeneity[row];
	}

	std::vector< std::vector< Spin > > solutions;

	for (std::uint32_t i = 0; i < nSolutionCandidates; ++i) {
		std::bitset< 32 > bitset(i);

		// Assemble solution candidate
		// We a zero in the binary representation is taken to mean Alpha spin whereas a one shall mean Beta
		for (std::size_t k = 0; k < m_variables.size(); ++k) {
			solutionCandidate(static_cast< Eigen::Index >(k)) = bitset.test(k) ? -1 : +1;
		}

		Eigen::Vector< int, Eigen::Dynamic > result = coefficientMatrix * solutionCandidate;

		if (result != expectedResult) {
			// The current solution candidate is no solution to our LSE
			continue;
		}

		// Convert into actual spin representation
		std::vector< Spin > spinSolution;
		spinSolution.reserve(m_variables.size());
		for (int entry : solutionCandidate) {
			assert(entry == -1 || entry == +1); // NOLINT
			spinSolution.push_back(entry == -1 ? Spin::Beta : Spin::Alpha);
		}

		solutions.push_back(std::move(spinSolution));
	}

	return solutions;
}

} // namespace lizard
