# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

include(setup_tools)

add_subdirectory(core)
add_subdirectory(parser)


add_executable(lizard main.cpp)

target_compile_definitions(
	lizard
		PRIVATE
			LIZARD_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR}
			LIZARD_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR}
			LIZARD_VERSION_PATCH=${CMAKE_PROJECT_VERSION_PATCH}
			LIZARD_VERSION_STR="${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}"
)

target_link_libraries(
	lizard
		PUBLIC
		lizard::core
		lizard::parser
)
target_link_libraries(
	lizard
		PRIVATE
			CLI11::CLI11
			fmt::fmt
			spdlog::spdlog
)
