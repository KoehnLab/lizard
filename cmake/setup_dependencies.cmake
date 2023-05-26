# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/cmake/antlr4"
)

set(LIZARD_DEPENDENCIES_BUILD_TYPE "Release" CACHE STRING "The build type to use for lizard's dependencies")

message(STATUS "Fetching and configuring dependencies...")
message(VERBOSE "External depdendencies build type: ${LIZARD_DEPENDENCIES_BUILD_TYPE}")


# Set the build type to use for dependencies (may be different from lizard's own build type)
set(GENERAL_BUILD_TYPE ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE ${LIZARD_DEPENDENCIES_BUILD_TYPE} CACHE INTERNAL "" FORCE)

FetchContent_Declare(
	cmake_compiler_flags
	GIT_REPOSITORY https://github.com/Krzmbrzl/cmake-compiler-flags.git
	GIT_TAG        v2.0.0
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	antlr4
	GIT_REPOSITORY https://github.com/antlr/antlr4.git
	GIT_TAG        4.12.0
	GIT_SHALLOW    ON
	SOURCE_SUBDIR  runtime/Cpp
)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG        release-1.12.1
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	googlebenchmark
	GIT_REPOSITORY https://github.com/google/benchmark
	GIT_TAG        v1.7.1
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	CLI11
	GIT_REPOSITORY https://github.com/CLIUtils/CLI11
	GIT_TAG        v2.3.2
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt
	GIT_TAG        9.1.0
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog
	GIT_TAG        v1.11.0
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	hedley
	GIT_REPOSITORY https://github.com/nemequ/hedley
	GIT_TAG        v15
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	iterators
	GIT_REPOSITORY https://github.com/Krzmbrzl/iterators
	GIT_TAG        v1.0.0
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	libperm
	GIT_REPOSITORY https://github.com/Krzmbrzl/libPerm
	GIT_TAG        v1.5.0
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	span_lite
	GIT_REPOSITORY https://github.com/martinmoene/span-lite
	GIT_TAG        v0.10.3
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	eigen3
	GIT_REPOSITORY https://gitlab.com/libeigen/eigen
	GIT_TAG        3.4.0
	GIT_SHALLOW    ON
)

# ANTLR options
set(DISABLE_WARNINGS   TRUE  CACHE INTERNAL "")
set(WITH_DEMO          FALSE CACHE INTERNAL "")
set(ANTLR4_INSTALL     FALSE CACHE INTERNAL "")
set(ANTLR_BUILD_SHARED FALSE CACHE INTERNAL "")
set(ANTLR_BUILD_CPP_TESTS ${LIZARD_BUILD_TESTS} CACHE INTERNAL "")
set(ANTLR_EXECUTABLE "${3RDPARTY_DIR}/antlr4/antlr-4.12.0-complete.jar" CACHE INTERNAL "")
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

# fmt options
set(FMT_MASTER_PROJECT OFF CACHE INTERNAL "")
set(FMT_PEDANTIC       OFF CACHE INTERNAL "")
set(FMT_WERROR         OFF CACHE INTERNAL "")
set(FMT_DOC            OFF CACHE INTERNAL "")
set(FMT_INSTALL        OFF CACHE INTERNAL "")
set(FMT_TEST           OFF CACHE INTERNAL "") # Compiling tests creates errors due to duplicate gtest target
set(FMT_FUZZ           OFF CACHE INTERNAL "")
set(FMT_FUZZ           OFF CACHE INTERNAL "")
set(FMT_MODULE         OFF CACHE INTERNAL "")
set(FMT_SYSTEM_HEADERS ON  CACHE INTERNAL "")

# spdlog options
set(SPDLOG_MASTER_PROJECT   OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_ALL        OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_SHARED     OFF CACHE INTERNAL "")
set(SPDLOG_ENABLE_PCH       ON  CACHE INTERNAL "")
set(SPDLOG_BUILD_PIC        ${CMAKE_POSITION_INDEPENDENT_CODE} CACHE INTERNAL "")
set(SPDLOG_BUILD_EXAMPLE    OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_EXAMPLE_HO OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_BENCH      OFF CACHE INTERNAL "")
set(SPDLOG_SANITIZE_ADDRESS OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_WARNINGS   OFF CACHE INTERNAL "")
set(SPDLOG_INSTALL          OFF CACHE INTERNAL "")
set(SPDLOG_FMT_EXTERNAL     ON  CACHE INTERNAL "")
set(SPDLOG_TIDY             OFF CACHE INTERNAL "")

# libPerm options
set(LIBPERM_TESTS ${LIZARD_BUILD_TESTS} CACHE INTERNAL "")
set(LIBPERM_EXAMPLES OFF CACHE INTERNAL "")
set(LIBPERM_DISABLE_WARNINGS ON CACHE INTERNAL "")
set(LIBPERM_WARNINGS_AS_ERRORS OFF CACHE INTERNAL "")

# Eigen options
set(EIGEN_BUILD_DOC     OFF CACHE INTERNAL "")
set(BUILD_TESTING       OFF CACHE INTERNAL "")
set(EIGEN_BUILD_BLAS    OFF CACHE INTERNAL "")
set(EIGEN_TEST_NOQT     ON  CACHE INTERNAL "")
# Prevent Eigen from including any Fortran modules
set(CMAKE_Fortran_COMPILER NOTFOUND CACHE INTERNAL "")

FetchContent_MakeAvailable(cmake_compiler_flags antlr4 CLI11 fmt spdlog hedley iterators libperm span_lite eigen3)


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

# Add hedley to include path
FetchContent_GetProperties(hedley SOURCE_DIR HEDLEY_SOURCE_DIR)
include_directories("${HEDLEY_SOURCE_DIR}")

# Add a custom Eigen target that ensures that we are not flooded with warnings due to Eigen's implementation
add_library(lizard_eigen_interface INTERFACE)
get_target_property(EIGEN_INCLUDES Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(lizard_eigen_interface SYSTEM INTERFACE ${EIGEN_INCLUDES})
add_library(lizard::Eigen3 ALIAS lizard_eigen_interface)

# Restore build type
set(CMAKE_BUILD_TYPE ${GENERAL_BUILD_TYPE} CACHE INTERNAL "" FORCE)
unset(GENERAL_BUILD_TYPE)
