// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/ReporterConfig.hpp"
#include "lizard/process/Strategy.hpp"

#include <memory>

namespace lizard {

/**
 * This class represents a single step in the processing chain
 */
class ProcessingStep {
public:
	ProcessingStep(std::unique_ptr< Strategy > strategy, ReporterConfig config = {});

	/**
	 * @returns The actual Strategy that shall be executed in this step
	 */
	[[nodiscard]] auto getStep() const -> const Strategy &;
	/**
	 * @returns The actual Strategy that shall be executed in this step
	 */
	[[nodiscard]] auto getStep() -> Strategy &;

	/**
	 * @returns The config for the logger/reporter that shall be used for this step
	 */
	[[nodiscard]] auto getReporterConfig() const -> const ReporterConfig &;

private:
	std::unique_ptr< Strategy > m_strategy;
	ReporterConfig m_config;
};

} // namespace lizard
