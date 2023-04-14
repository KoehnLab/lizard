// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/format/FormatSupport.hpp"
#include "lizard/process/ProcessingException.hpp"
#include "lizard/process/Processor.hpp"

#include <CLI/CLI.hpp>

#include <fmt/chrono.h>
#include <fmt/core.h>

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <chrono>
#include <cstdint>
#include <exception>


using namespace lizard;


auto main(int argc, char **argv) -> int {
	CLI::App app("A quantum chemistry application used for the symbolic derivation and manipulation of equations based "
				 "on second quantization.");
	app.set_version_flag("--version", std::string(LIZARD_VERSION_STR));

	CLI11_PARSE(app, argc, argv);

	spdlog::stopwatch stopwatch;
	spdlog::info("This is lizard v{}", LIZARD_VERSION_STR);


	Processor processor;

	// Import diagrams

	// Translate diagrams into algebraic expressions

	// Perform substitutions (e.g. density-fitting)

	// Perform term optimization(s)
	// -> At least: strength-reduction

	// Spin-integration

	// If restricted orbitals: Spin summation

	// Export terms

	try {
		processor.run();
	} catch (const ProcessingException &e) {
		spdlog::error("Processing error: {}", e);
		return 1;
	} catch (const std::exception &e) {
		spdlog::error("Encountered unexpected error: {}", e);
		return 2;
	}

	spdlog::info("Successful termination after {:.3}", stopwatch.elapsed());

	return 0;
}
