# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

if (NOT LIZARD_BUILD_BENCHMARKS)
	return()
endif()

include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
set(BENCHMARK_ENABLE_LTO OFF CACHE INTERNAL "${CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE}")
set(BENCHMARK_INSTALL OFF CACHE INTERNAL "")
set(BENCHMARK_INSTALL_DOCS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(googlebenchmark)

add_executable(ExpressionTreeBenchmark ExpressionTree.cpp)
target_link_libraries(ExpressionTreeBenchmark PRIVATE benchmark::benchmark lizard::core)
