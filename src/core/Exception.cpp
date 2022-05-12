// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include "lizard/core/Exception.hpp"

#include <exception>
#include <iostream>

namespace lizard::core {

void print_exception_message(const std::exception &e, std::ostream &out) {
	out << e.what() << std::endl;

	try {
		std::rethrow_if_nested(e);
	} catch (const std::exception &inner) {
		print_exception_message(inner);
	}
}

} // namespace lizard::core
