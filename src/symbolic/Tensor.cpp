// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/Tensor.hpp"

#include <cassert>
#include <iostream>

namespace lizard {

Tensor::Tensor(std::string name) : m_name(std::move(name)) {
}

void Tensor::setName(std::string name) {
	m_name = std::move(name);
}
auto Tensor::getName() const -> std::string_view {
	return m_name;
}

auto Tensor::getSpinProjection() const -> int { // NOLINT (*-convert-member-functions-to-static)
	// For now, all tensors are assumed to belong to singlet operators
	return 0;
}

auto operator==(const Tensor &lhs, const Tensor &rhs) -> bool {
	return lhs.getName() == rhs.getName();
}

auto operator!=(const Tensor &lhs, const Tensor &rhs) -> bool {
	return !(lhs == rhs);
}

auto operator<<(std::ostream &stream, const Tensor &tensor) -> std::ostream & {
	return stream << tensor.getName();
}


} // namespace lizard

auto std::hash< lizard::Tensor >::operator()(const lizard::Tensor &tensor) const -> std::size_t {
	return std::hash< decltype(tensor.getName()) >{}(tensor.getName());
}
