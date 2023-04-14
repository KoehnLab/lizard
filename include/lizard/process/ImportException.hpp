// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/ProcessingException.hpp"

namespace lizard {

/**
 * Exception thrown if something goes wrong during import
 */
class ImportException : public ProcessingException {
public:
	using ProcessingException::ProcessingException;
};

} // namespace lizard
