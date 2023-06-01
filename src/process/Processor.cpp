// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/process/Processor.hpp"
#include "lizard/core/SignedCast.hpp"
#include "lizard/format/FormatSupport.hpp"
#include "lizard/process/ExportStrategy.hpp"
#include "lizard/process/ImportStrategy.hpp"
#include "lizard/process/ReporterConfig.hpp"
#include "lizard/process/RewriteStrategy.hpp"
#include "lizard/symbolic/TensorExpressions.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <iterator>
#include <type_traits>
#include <vector>


namespace lizard {

Processor::Processor(IndexSpaceManager manager, std::shared_ptr< spdlog::logger > logger)
	: m_spaceManager(std::move(manager)), m_log(std::move(logger)) {
	assert(m_log); // NOLINT
}

void Processor::setIndexSpaceManager(IndexSpaceManager manager) {
	m_spaceManager = std::move(manager);
}

void Processor::setLogger(std::shared_ptr< spdlog::logger > logger) {
	assert(logger); // NOLINT
	m_log = std::move(logger);
}

void Processor::enqueue(ProcessingStep step) {
	m_steps.push_back(std::move(step));
}

void Processor::insert(ProcessingStep step, std::size_t position) {
	assert(position <= m_steps.size()); // NOLINT

	using SignedSize = std::make_signed_t< std::size_t >;

	m_steps.insert(m_steps.begin() + signed_cast< SignedSize >(position), std::move(step));
}

auto createLogger(const Strategy &strategy, const ReporterConfig &config) -> std::shared_ptr< spdlog::logger > {
	std::shared_ptr< spdlog::logger > logger;

	if (!config.logToFile()) {
		logger = spdlog::stdout_color_mt(std::string(strategy.getName()));
		logger->set_pattern("[%n] [%^%l%$]: %v");
	} else {
		logger = spdlog::basic_logger_mt(std::string(strategy.getName()), config.getFilePath().value().string());
		logger->set_pattern("[%l]: %v");
	}

	return logger;
}

void Processor::run() {
	std::vector< NamedTensorExprTree > expressions;

	std::size_t nSteps = m_steps.size();

	for (std::size_t i = 0; i < nSteps; ++i) {
		ProcessingStep &currentStep = m_steps[i];
		Strategy &strategy          = currentStep.getStep();

		const std::size_t nExpressions = expressions.size();

		m_log->info(fmt::format("{}/{}: {}", i + 1, nSteps, strategy));

		std::shared_ptr< spdlog::logger > subLogger = createLogger(strategy, currentStep.getReporterConfig());

		strategy.setLogger(subLogger);

		switch (strategy.getType()) {
			case StrategyType::Import: {
				const auto &importStrategy = dynamic_cast< const ImportStrategy & >(strategy);

				std::vector< NamedTensorExprTree > newExpressions = importStrategy.importExpressions(m_spaceManager);

				if (expressions.empty()) {
					expressions = std::move(newExpressions);
				} else {
					expressions.reserve(expressions.size() + newExpressions.size());
					expressions.insert(expressions.end(), std::move_iterator(newExpressions.begin()),
									   std::move_iterator(newExpressions.end()));
				}
			} break;
			case StrategyType::Export: {
				auto &exportStrategy = dynamic_cast< ExportStrategy & >(strategy);

				exportStrategy.exportExpressions(expressions, m_spaceManager);
			} break;
			case StrategyType::Optimization:
			case StrategyType::SpinProcessing:
			case StrategyType::Substitution: {
				auto &rewriteStrategy = dynamic_cast< RewriteStrategy & >(strategy);

				rewriteStrategy.process(expressions, m_spaceManager);
				break;
			}
		}

		// unregister logger again
		spdlog::drop(subLogger->name());

		if (nExpressions < expressions.size()) {
			const std::size_t diff = expressions.size() - nExpressions;
			m_log->info("-> Added {} {}", diff, diff > 1 ? "expressions" : "expression");
		} else if (nExpressions > expressions.size()) {
			const std::size_t diff = nExpressions - expressions.size();
			m_log->info("-> Removed {} {}", diff, diff > 1 ? "expressions" : "expression");
		}
	}
}

} // namespace lizard
