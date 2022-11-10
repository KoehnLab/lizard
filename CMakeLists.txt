# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

cmake_minimum_required(VERSION 3.15)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "The type of build to produce")


project(lizard,
	VERSION "1.0.0"
	LANGUAGES "CXX"
)

set(CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/cmake"
)

# Make sure that libraries and executables end up in the correct directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Create a compile_commands.json DB for use with clang-tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/3rdParty")

include(compiler)
include(setup_dependencies)
include(setup_tools)


option(tests "Whether to build test cases" ON)

include_directories("${CMAKE_SOURCE_DIR}/include")

add_subdirectory(grammars)
add_subdirectory(src)
#add_subdirectory(tests)

