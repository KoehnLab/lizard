// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/StrategyType.hpp"

#include <spdlog/logger.h>

#include <iosfwd>
#include <memory>
#include <string_view>

namespace lizard {

/**
 * Base interface for all processing strategy used within lizard
 */
class Strategy {
public:
	Strategy()                 = default;
	Strategy(const Strategy &) = default;
	Strategy(Strategy &&)      = default;
	virtual ~Strategy()        = default;

	auto operator=(const Strategy &) -> Strategy & = default;
	auto operator=(Strategy &&) -> Strategy & = default;

	/**
	 * @returns Display name of this strategy
	 */
	[[nodiscard]] virtual auto getName() const -> std::string_view = 0;

	/**
	 * @returns The type of this strategy
	 */
	[[nodiscard]] virtual auto getType() const -> StrategyType = 0;

	/**
	 * Sets the logger that shall be used by this Strategy to report its progress
	 */
	void setLogger(std::shared_ptr< spdlog::logger > logger);

protected:
	/**
	 * @returns The logger that shall be used by this strategy
	 */
	[[nodiscard]] auto getLogger() const -> spdlog::logger &;

private:
	std::shared_ptr< spdlog::logger > m_log;
};

auto operator<<(std::ostream &stream, const Strategy &strategy) -> std::ostream &;

} // namespace lizard
