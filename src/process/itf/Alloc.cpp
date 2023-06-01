// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "Alloc.hpp"
#include "TensorFormatter.hpp"

#include <fmt/core.h>


namespace lizard::itf {

Alloc::Alloc(TensorElement element) : m_element(std::move(element)) {
}

auto Alloc::stringify(const IndexSpaceManager &manager) const -> std::string {
	return fmt::format("alloc {}", TensorFormatter(m_element, manager));
}

} // namespace lizard::itf
