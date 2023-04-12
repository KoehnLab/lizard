// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <string>
#include <string_view>

namespace lizard {

/**
 * This class is meant to represent the most basic properties of a general tensor.
 * It is only meant to represent the tensor symbolically, so no representation of the underlying numerical
 * values is implemented.
 */
class Tensor {
public:
	Tensor() = default;
	Tensor(std::string name);

	/**
	 * Set the name of this Tensor
	 */
	void setName(std::string name);
	/**
	 * @returns This Tensor's name
	 */
	[[nodiscard]] auto getName() const -> std::string_view;

private:
	std::string m_name;
};

[[nodiscard]] auto operator==(const Tensor &lhs, const Tensor &rhs) -> bool;
[[nodiscard]] auto operator!=(const Tensor &lhs, const Tensor &rhs) -> bool;

auto operator<<(std::ostream &stream, const Tensor &tensor) -> std::ostream &;

} // namespace lizard

// Specialize std::hash for Tensors
template<> struct std::hash< lizard::Tensor > {
	[[nodiscard]] auto operator()(const lizard::Tensor &tensor) const -> std::size_t;
};
