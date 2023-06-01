// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <cstdint>
#include <iosfwd>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>

namespace lizard {

class Fraction;
auto operator<<(std::ostream &stream, const Fraction &fraction) -> std::ostream &;

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
		: m_numerator(numerator), m_denominator(denominator) {
		simplify();

		if (m_denominator < 0) {
			// Sign should be carried by numerator
			m_denominator *= -1;
			m_numerator *= -1;
		}
	}

	/**
	 * @returns The numerator
	 */
	[[nodiscard]] constexpr auto getNumerator() const -> field_type { return m_numerator; }

	/**
	 * Sets the numerator
	 */
	constexpr void setNumerator(field_type numerator) {
		m_numerator = numerator;
		simplify();
	}

	/**
	 * @returns The denominator
	 */
	[[nodiscard]] constexpr auto getDenominator() const -> field_type { return m_denominator; }

	/**
	 * Sets the denominator
	 */
	constexpr void setDenominator(field_type denominator) {
		if (denominator < 0) {
			// Keep sign with the numerator
			denominator *= -1;
			m_numerator *= -1;
		}

		m_denominator = denominator;

		simplify();
	}

	/**
	 * @returns The represented values as a floating point number
	 */
	template< typename Decimal = float >[[nodiscard]] constexpr auto getValue() const -> Decimal {
		return static_cast< Decimal >(m_numerator) / static_cast< Decimal >(m_denominator);
	}

	/**
	 * Simplifies this fraction
	 */
	constexpr void simplify() {
		field_type gcd = std::gcd(m_numerator, m_denominator);
		m_numerator /= gcd;
		m_denominator /= gcd;
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

		Decimal sign = static_cast< Decimal >(value < 0 ? -1 : 1);
		value *= sign; // Make literal positive

		Fraction fraction(static_cast< field_type >(value));

		Decimal fractionalPart = value - static_cast< Decimal >(fraction.m_numerator);

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

					const Decimal upperBound = (fractionalPart + precision) * static_cast< Decimal >(mediantDenom);
					const Decimal lowerBound = (fractionalPart - precision) * static_cast< Decimal >(mediantDenom);

					// If fractionalPart + precision < mediant
					if (upperBound < static_cast< Decimal >(mediantNum)) {
						// mediant is new upper bound
						upperNum   = mediantNum;
						upperDenom = mediantDenom;
					} else if (lowerBound > static_cast< Decimal >(mediantNum)) {
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

		fraction.m_numerator *= static_cast< Fraction::field_type >(sign);

		return fraction;
	}

	explicit operator std::string() const {
		std::stringstream sstream;
		sstream << *this;
		return sstream.str();
	}


private:
	field_type m_numerator   = 0;
	field_type m_denominator = 1;
};

constexpr auto operator==(const Fraction &lhs, const Fraction &rhs) -> bool {
	return lhs.getNumerator() == rhs.getNumerator() && lhs.getDenominator() == rhs.getDenominator();
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

// Arithmetic operators
constexpr auto operator*=(Fraction &lhs, const Fraction &rhs) -> Fraction & {
	lhs.setNumerator(lhs.getNumerator() * rhs.getNumerator());
	lhs.setDenominator(lhs.getDenominator() * rhs.getDenominator());

	return lhs;
}

constexpr auto operator*(Fraction lhs, const Fraction &rhs) -> Fraction {
	lhs *= rhs;
	return lhs;
}

constexpr auto operator/=(Fraction &lhs, const Fraction &rhs) -> Fraction & {
	lhs.setNumerator(lhs.getNumerator() * rhs.getDenominator());
	lhs.setDenominator(lhs.getDenominator() * rhs.getNumerator());

	return lhs;
}

constexpr auto operator/(Fraction lhs, const Fraction &rhs) -> Fraction {
	lhs /= rhs;
	return lhs;
}

} // namespace lizard
