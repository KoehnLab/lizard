# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/cmake/antlr4"
)

message(STATUS "Fetching and building dependencies...")

# Add ANTLR4 cpp runtime
set(ANTLR4_TAG "v4.11.1")
include(ExternalAntlr4Cpp)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# Set ANTLR jar
set(ANTLR_EXECUTABLE "${3RDPARTY_DIR}/antlr4/antlr-4.11.1-complete.jar")

if (NOT EXISTS "${ANTLR_EXECUTABLE}")
	message(FATAL_ERROR "ANTLR jar-file not found. Was looking for ${ANTLR_EXECUTABLE}")
endif()

find_package(ANTLR REQUIRED)


FetchContent_Declare(
	cmake_compiler_flags
	GIT_REPOSITORY https://github.com/Krzmbrzl/cmake-compiler-flags.git
	GIT_TAG        v1.2.1
	GIT_SHALLOW    true
)

FetchContent_MakeAvailable(cmake_compiler_flags)

# Append the compiler flags CMake module to the module path
FetchContent_GetProperties(cmake_compiler_flags SOURCE_DIR COMPILER_FLAGS_SRC_DIR)
list(APPEND CMAKE_MODULE_PATH "${COMPILER_FLAGS_SRC_DIR}")
