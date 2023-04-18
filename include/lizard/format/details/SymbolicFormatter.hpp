// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/IndexSpaceManager.hpp"

namespace lizard::details {

/**
 * Formatting helper for everything that requires the context of an IndexSpaceManager in order
 * to be properly formatted.
 */
template< typename T > class SymbolicFormatter {
public:
	SymbolicFormatter(const T &t, const IndexSpaceManager &manager) : m_t(t), m_manager(manager) {}

	/**
	 * @returns The object that shall be formatted
	 */
	[[nodiscard]] auto get() const -> const T & { return m_t; }

	/**
	 * @returns The corresponding IndexSpaceManager
	 */
	[[nodiscard]] auto getManager() const -> const IndexSpaceManager & { return m_manager; }

private:
	const T &m_t;
	const IndexSpaceManager &m_manager;
};

} // namespace lizard::details
