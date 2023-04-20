// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/format/FormatSupport.hpp"
#include "lizard/process/HardcodedImport.hpp"
#include "lizard/process/ProcessingException.hpp"
#include "lizard/process/ProcessingStep.hpp"
#include "lizard/process/Processor.hpp"
#include "lizard/process/TextExport.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexSpaceData.hpp"
#include "lizard/symbolic/IndexSpaceManager.hpp"
#include "lizard/symbolic/Spin.hpp"

#include <CLI/CLI.hpp>

#include <fmt/chrono.h>
#include <fmt/core.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <cstdint>
#include <exception>
#include <memory>


using namespace lizard;

struct LogShutdown {
	LogShutdown()                    = default;
	LogShutdown(const LogShutdown &) = delete;
	LogShutdown(LogShutdown &&)      = delete;

	auto operator=(const LogShutdown &) -> LogShutdown & = delete;
	auto operator=(LogShutdown &&) -> LogShutdown & = delete;

	~LogShutdown() { spdlog::shutdown(); }
};

auto main(int argc, char **argv) -> int {
	CLI::App app("A quantum chemistry application used for the symbolic derivation and manipulation of equations based "
				 "on second quantization.");
	app.set_version_flag("--version", std::string(LIZARD_VERSION_STR));

	CLI11_PARSE(app, argc, argv);

	LogShutdown shutdown;

	std::shared_ptr< spdlog::logger > logger = spdlog::stdout_color_mt("lizard_main");
	logger->set_pattern("[%^%l%$]: %v");

	try {
		spdlog::stopwatch stopwatch;
		logger->info("This is lizard v{}", LIZARD_VERSION_STR);


		IndexSpaceManager spaceManager;
		constexpr std::size_t occSize  = 10;
		constexpr std::size_t virtSize = 100;
		spaceManager.registerSpace(IndexSpace{ 0, Spin::Both },
								   IndexSpaceData{ "occ", occSize, Spin::Both, { 'i', 'j', 'k', 'l', 'm', 'n' } });
		spaceManager.registerSpace(IndexSpace{ 1, Spin::Both },
								   IndexSpaceData{ "virt", virtSize, Spin::Both, { 'a', 'b', 'c', 'd', 'e', 'f' } });

		Processor processor(std::move(spaceManager), logger);


		// Import diagrams
		// & Translate diagrams into algebraic expressions
		processor.enqueue(ProcessingStep{ std::make_unique< HardcodedImport >(HardcodedImport::CCD_ENERGY) });

		processor.enqueue(ProcessingStep{ std::make_unique< TextExport >() });

		// Perform substitutions (e.g. density-fitting)

		// Perform term optimization(s)
		// -> At least: strength-reduction

		// Spin-integration

		// If restricted orbitals: Spin summation

		// Export terms

		processor.run();

		logger->info("Successful termination after {:.3}", stopwatch.elapsed());
	} catch (const ProcessingException &e) {
		logger->error("Processing error: {}", e);
		return 1;
	} catch (const std::exception &e) {
		logger->error("Encountered unexpected error: {}", e);
		return 2;
	}

	return 0;
}
