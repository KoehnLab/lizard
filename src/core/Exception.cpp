// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/core/Exception.hpp"

#include <exception>
#include <iostream>

namespace lizard {

void print_exception_message(const std::exception &exception, std::ostream &out) {
	out << exception.what() << std::endl;

	try {
		std::rethrow_if_nested(exception);
	} catch (const std::exception &inner) {
		print_exception_message(inner, out);
	}
}

} // namespace lizard
