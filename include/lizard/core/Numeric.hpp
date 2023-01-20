// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace lizard {

/**
 * Wrapper type around a numeric type that is assumed to be sufficiently wide for the required
 * ranges that typically have to be used.
 * In particular, this type has a special, invalid state which is also the state in which it
 * is constructed by default. Therefore, this can be seen as a "nullable" numeric type with
 * the notable property of not using zero for the invalid state. Therefore, zero remains
 * a perfectly valid value for this type (the invalid type is the highest possible representable
 * value with the underlying numeric type).
 */
class Numeric {
public:
	/**
	 * The numeric type wrapped by this class
	 */
	using numeric_type = std::uint32_t;

	/**
	 * The value reserved to represent an invalid state
	 */
	static constexpr const numeric_type Invalid = std::numeric_limits< numeric_type >::max();

	constexpr Numeric() noexcept = default;
	explicit constexpr Numeric(numeric_type value) noexcept : m_value(value) {}
	Numeric(const Numeric &) = default;
	Numeric(Numeric &&)      = default;
	~Numeric()               = default;
	auto operator=(const Numeric &) -> Numeric & = default;
	auto operator=(Numeric &&) -> Numeric & = default;

	/**
	 * @returns Whether the currently stored value is considered valid
	 */
	[[nodiscard]] constexpr auto isValid() const -> bool { return m_value != Invalid; }

	/**
	 * Resets the stored value to the invalid state
	 */
	constexpr void reset() { m_value = Invalid; }

	/**
	 * @returns The currently stored value
	 */
	[[nodiscard]] constexpr auto getValue() const -> numeric_type { return m_value; }

	/**
	 * Implicit conversion to the underlying numeric type
	 */
	constexpr operator numeric_type() const { return m_value; }

private:
	numeric_type m_value = Invalid;
};

} // namespace lizard
