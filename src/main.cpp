// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

#include <CLI/CLI.hpp>

#include <fmt/core.h>

#include <spdlog/spdlog.h>

auto main(int argc, char **argv) -> int {
	CLI::App app("A quantum chemistry application used for the symbolic derivation and manipulation of equations based "
				 "on second quantization.");
	app.set_version_flag("--version", std::string(LIZARD_VERSION_STR));

	CLI11_PARSE(app, argc, argv);

	spdlog::info("This is lizard v{}", LIZARD_VERSION_STR);

	return 0;
}
