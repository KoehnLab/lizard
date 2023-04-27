// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/Index.hpp"
#include "lizard/symbolic/Spin.hpp"

#include <vector>

namespace lizard {

/**
 * This class describes the linear system of equations whose solutions describe possible spin
 * distributions over specific indices.
 */
class SpinLSE {
public:
	SpinLSE() = default;

	/**
	 * Starts a new equation that should be added to this system
	 */
	void beginEquation();

	/**
	 * Finishes the current equation
	 *
	 * @param result The result that the ended equation must produce
	 */
	void endEquation(int result);

	/**
	 * Adds a term to the current equation. Note that subsequent calls to this function with the
	 * same Index will overwrite the coefficient in the current equation that has been set by
	 * the prior call.
	 *
	 * @param index The Index (considered as the variable in the given term) to add
	 * @param coefficient The coefficient with which the Index enters the current equation
	 */
	void addTerm(const Index &index, int coefficient);

	/**
	 * @returns A list of variables (Index instances) that appear in this system
	 */
	[[nodiscard]] auto getVariables() const -> const std::vector< Index > &;

	/**
	 * Solves this system
	 *
	 * @returns A list of all possible solutions of the represented system. The solutions are given in
	 * terms of Spin instances, where their numeric value represents the numeric solution.
	 * The spins are given in the same order as the variables obtained via getVariables()
	 */
	[[nodiscard]] auto solve() const -> std::vector< std::vector< Spin > >;

private:
	std::vector< std::vector< int > > m_equations;
	std::vector< int > m_inhomogeneity;
	std::vector< Index > m_variables;
};

} // namespace lizard
