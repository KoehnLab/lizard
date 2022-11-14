# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

cmake_minimum_required(VERSION 3.18)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "The type of build to produce")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


project(lizard,
	VERSION "1.0.0"
	LANGUAGES "CXX"
)


option(LIZARD_BUILD_TESTS "Whether to build test cases" ON)


set(CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/cmake"
)

include(CTest)

# Use cpp17 and error if that is not available
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Make sure that libraries and executables end up in the correct directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Create a compile_commands.json DB for use with clang-tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(3RDPARTY_DIR "${PROJECT_SOURCE_DIR}/3rdParty")

include(setup_dependencies)
include(compiler)
include(setup_tools)



include_directories("${CMAKE_SOURCE_DIR}/include")

add_subdirectory(grammars)
add_subdirectory(src)
#add_subdirectory(tests)

