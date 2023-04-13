// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <sstream>
#include <string>

namespace lizard {

/**
 * Converts the given object into its string representation by making use of its operator<< overload
 * for inserting it into a stream.
 *
 * @param obj The object whose string representation shall be obtained
 * @returns The string representation of obj
 *
 * @tparam T The type of obj
 */
template< typename T > auto toString(const T &obj) -> std::string {
	std::stringstream stream;
	stream << obj;

	return stream.str();
}

} // namespace lizard
