# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/cmake/antlr4"
)

message(STATUS "Fetching and configuring dependencies...")


FetchContent_Declare(
	cmake_compiler_flags
	GIT_REPOSITORY https://github.com/Krzmbrzl/cmake-compiler-flags.git
	GIT_TAG        v1.2.1
	GIT_SHALLOW    true
)
FetchContent_Declare(
	antlr4
	GIT_REPOSITORY https://github.com/antlr/antlr4.git
	GIT_TAG        v4.11.1
	GIT_SHALLOW    true
	SOURCE_SUBDIR  runtime/Cpp
)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG        release-1.12.1
	GIT_SHALLOW    true
)
FetchContent_Declare(
	CLI11
	GIT_REPOSITORY https://github.com/CLIUtils/CLI11
	GIT_TAG        v2.3.1
	GIT_SHALLOW    true
)

# ANTLR options
set(DISABLE_WARNINGS TRUE  CACHE INTERNAL "")
set(WITH_DEMO        FALSE CACHE INTERNAL "")
set(ANTLR4_INSTALL   FALSE CACHE INTERNAL "")
set(ANTLR_BUILD_CPP_TESTS ${LIZARD_BUILD_TESTS} CACHE INTERNAL "")
set(ANTLR_EXECUTABLE "${3RDPARTY_DIR}/antlr4/antlr-4.11.1-complete.jar" CACHE INTERNAL "")
if(MSVC_RUNTIME_LIBRARY MATCHES ".*DLL$")
	set(USE_STATIC_CRT OFF)
else()
	set(USE_STATIC_CRT ON)
endif()
set(WITH_STATIC_CRT "${USE_STATIC_CRT}" CACHE INTERNAL "")

# CLI11 options
set(CLI11_WARNINGS_AS_ERRORS  OFF CACHE INTERNAL "")
set(CLI11_SINGLE_FILE         OFF CACHE INTERNAL "")
set(CLI11_PRECOMPILED         ON  CACHE INTERNAL "")
set(CLI11_BUILD_DOCS          OFF CACHE INTERNAL "")
set(CLI11_BUILD_TESTS         ${LIZARD_BUILD_TESTS} CACHE INTERNAL "")
set(CLI11_BUILD_EXAMPLES      OFF CACHE INTERNAL "")
set(CLI11_BUILD_EXAMPLES_JSON OFF CACHE INTERNAL "")
set(CLI11_INSTALL             OFF CACHE INTERNAL "")


FetchContent_MakeAvailable(cmake_compiler_flags antlr4 CLI11)


# Append the compiler flags CMake module to the module path
FetchContent_GetProperties(cmake_compiler_flags SOURCE_DIR COMPILER_FLAGS_SRC_DIR)
list(APPEND CMAKE_MODULE_PATH "${COMPILER_FLAGS_SRC_DIR}")


# Add antlr's cmake support module to the module path
FetchContent_GetProperties(antlr4 SOURCE_DIR ANTLR_SOURCE_DIR)
list(APPEND CMAKE_MODULE_PATH "${ANTLR_SOURCE_DIR}/runtime/Cpp/cmake")

# Required to be able to access ANTLR's cmake support
find_package(ANTLR REQUIRED)

# ANTLR's CMakeLists.txt does not specify the include directories for 
target_include_directories(antlr4_static PUBLIC "${ANTLR_SOURCE_DIR}/runtime/Cpp/runtime/src")
target_include_directories(antlr4_shared PUBLIC "${ANTLR_SOURCE_DIR}/runtime/Cpp/runtime/src")
