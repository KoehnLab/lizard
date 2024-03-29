// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <exception>
#include <iostream>
#include <stdexcept>

namespace lizard {

/**
 * Base class for all exceptions used in lizard
 */
class Exception : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

/**
 * Prints out the full exception message (including any potentially nested exceptions).
 *
 * @param e The exception to print out
 * @param out The ostream to print the message to (defaults to std::err)
 */
void print_exception_message(const std::exception &e, std::ostream &out = std::cerr);

} // namespace lizard
