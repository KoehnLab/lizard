// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#ifndef LIZARD_CORE_FRACTION_HPP_
#define LIZARD_CORE_FRACTION_HPP_

#include <cstdint>
#include <iosfwd>
#include <limits>

namespace lizard {

/**
 * Simple representation of a fraction using numerator and denominator
 */
class Fraction {
public:
	/**
	 * The type used to store numerator and denominator
	 */
	using field_type = std::int32_t;

	constexpr Fraction() = default;
	constexpr Fraction(field_type numerator, field_type denominator = 1)
		: m_numerator(numerator), m_denominator(denominator) {}

	/**
	 * @returns The numerator
	 */
	[[nodiscard]] constexpr auto getNumerator() const -> field_type { return m_numerator; }

	/**
	 * @returns The denominator
	 */
	[[nodiscard]] constexpr auto getDenominator() const -> field_type { return m_denominator; }

	/**
	 * @returns The represented values as a floating point number
	 */
	template< typename Decimal = float >[[nodiscard]] constexpr auto getValue() const -> Decimal {
		return static_cast< Decimal >(m_numerator) / static_cast< Decimal >(m_denominator);
	}

	/**
	 * Converts the given floating point number into a Fraction that most closely approximates the given number up to
	 * the given precision
	 *
	 * @param value The value to convert
	 * @param precision The maximum error that the conversion may introduce
	 * @returns The corresponding Fraction
	 */
	template< typename Decimal >
	[[nodiscard]] static constexpr auto fromDecimal(Decimal value,
													Decimal precision = std::numeric_limits< Decimal >::epsilon() * 4)
		-> Fraction {
		// We perform the conversion double -> symbolic fraction by walking the "Stern-Brocot" tree
		// until we have found a fraction that represents the given literal to a given precision
		// Adapted from https://stackoverflow.com/a/5128558

		int sign = value < 0 ? -1 : 1;
		value *= sign; // Make literal positive

		Fraction fraction(static_cast< field_type >(value));

		Decimal fractionalPart = value - fraction.m_numerator;

		if (fractionalPart > precision) {
			if (1 - precision < fractionalPart) {
				fraction.m_numerator++;
			} else {
				field_type upperNum   = 1;
				field_type lowerNum   = 0;
				field_type upperDenom = 1;
				field_type lowerDenom = 1;

				// In essence, we are performing a binary search to find the best representation
				// of our fractional part as a symbolic fraction
				while (true) {
					// The mediant our fractions is
					// mediant = (upperNum + lowerNum) / (upperDenom + lowerDenom)
					field_type mediantNum   = upperNum + lowerNum;
					field_type mediantDenom = upperDenom + lowerDenom;

					const Decimal upperBound = (fractionalPart + precision) * mediantDenom;
					const Decimal lowerBound = (fractionalPart - precision) * mediantDenom;

					// If fractionalPart + precision < mediant
					if (upperBound < mediantNum) {
						// mediant is new upper bound
						upperNum   = mediantNum;
						upperDenom = mediantDenom;
					} else if (lowerBound > mediantNum) {
						// mediant is new lower bound
						lowerNum   = mediantNum;
						lowerDenom = mediantDenom;
					} else {
						// middle is within precision of the fractional value that we have to approximate
						fraction.m_numerator   = fraction.m_numerator * mediantDenom + mediantNum;
						fraction.m_denominator = mediantDenom;
						break;
					}
				}
			}
		}

		fraction.m_numerator *= sign;

		return fraction;
	}


private:
	field_type m_numerator   = 0;
	field_type m_denominator = 1;
};

constexpr auto operator==(const Fraction &lhs, const Fraction &rhs) -> bool {
	return lhs.getValue() == rhs.getValue();
}

constexpr auto operator!=(const Fraction &lhs, const Fraction &rhs) -> bool {
	return !(lhs == rhs);
}

constexpr auto operator<(const Fraction &lhs, const Fraction &rhs) -> bool {
	return lhs.getValue() < rhs.getValue();
}

constexpr auto operator<=(const Fraction &lhs, const Fraction &rhs) -> bool {
	return lhs == rhs || lhs < rhs;
}

constexpr auto operator>(const Fraction &lhs, const Fraction &rhs) -> bool {
	return !(lhs <= rhs);
}

constexpr auto operator>=(const Fraction &lhs, const Fraction &rhs) -> bool {
	return lhs == rhs || lhs > rhs;
}

auto operator<<(std::ostream &stream, const Fraction &fraction) -> std::ostream &;

} // namespace lizard

#endif // LIZARD_CORE_FRACTION_HPP_
