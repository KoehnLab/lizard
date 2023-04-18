// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/ProcessingStep.hpp"

#include <cassert>

namespace lizard {

ProcessingStep::ProcessingStep(std::unique_ptr< Strategy > strategy, ReporterConfig config)
	: m_strategy(std::move(strategy)), m_config(std::move(config)) {
}

auto ProcessingStep::getStep() const -> const Strategy & {
	assert(m_strategy); // NOLINT
	return *m_strategy;
}

auto ProcessingStep::getStep() -> Strategy & {
	assert(m_strategy); // NOLINT
	return *m_strategy;
}

auto ProcessingStep::getReporterConfig() const -> const ReporterConfig & {
	return m_config;
}

} // namespace lizard
