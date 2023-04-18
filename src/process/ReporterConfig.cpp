// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/ReporterConfig.hpp"

namespace lizard {

ReporterConfig::ReporterConfig(LogLevel logLevel, std::optional< std::filesystem::path > logFilePath)
	: m_logLevel(std::move(logLevel)), m_logFile(std::move(logFilePath)) {
}

auto ReporterConfig::getLogLevel() const -> LogLevel {
	return m_logLevel;
}

auto ReporterConfig::logToFile() const -> bool {
	return m_logFile.has_value();
}

auto ReporterConfig::getFilePath() const -> const std::optional< std::filesystem::path > & {
	return m_logFile;
}

} // namespace lizard
