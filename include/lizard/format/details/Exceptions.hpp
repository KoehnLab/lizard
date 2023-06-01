// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <exception>
#include <fmt/format.h>
#include <string>

// Newer versions of the fmt library will ship with support for formatting exceptions out of the box
// so once we upgrade to a newer release, this function has to be removed (it'll probably create a
// compile error anyway).

template< typename T, typename Char >
struct fmt::formatter< T, Char, typename std::enable_if< std::is_base_of< std::exception, T >::value >::type >
	: fmt::formatter< string_view > {
	[[nodiscard]] static auto getMessage(const std::exception &exception) -> std::string {
		std::string message(exception.what());

		try {
			std::rethrow_if_nested(exception);
		} catch (const std::exception &nested) {
			message += "\n  caused by: " + getMessage(nested);
		}

		return message;
	}

	template< typename FormatContext >
	auto format(const std::exception &exception, FormatContext &ctx) const -> typename FormatContext::iterator {
		return fmt::formatter< string_view >::format(getMessage(exception), ctx);
	}
};
