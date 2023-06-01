// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include <spdlog/common.h>

#include <filesystem>
#include <optional>
#include <type_traits>

namespace lizard {

/**
 * This class is meant to represent logging/reporting configuration options
 */
class ReporterConfig {
public:
	using LogLevel = std::underlying_type_t< spdlog::level::level_enum >;

	ReporterConfig(LogLevel logLevel = spdlog::level::info, std::optional< std::filesystem::path > logFilePath = {});

	/**
	 * @returns The desired log level
	 */
	[[nodiscard]] auto getLogLevel() const -> LogLevel;

	/**
	 * @returns Whether the logs shall be written into a file
	 */
	[[nodiscard]] auto logToFile() const -> bool;

	/**
	 * @returns The file path of the file the logs shall be written to
	 */
	[[nodiscard]] auto getFilePath() const -> const std::optional< std::filesystem::path > &;

private:
	LogLevel m_logLevel = spdlog::level::info;
	std::optional< std::filesystem::path > m_logFile;
};

} // namespace lizard
