# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

include(CheckIPOSupported)
include(CompilerFlags)

check_ipo_supported(RESULT LTO_DEFAULT)


option(LIZARD_LTO "Whether to use link-time optimizations (if available)" ${LTO_DEFAULT})
option(LIZARD_DISABLE_WARNINGS "Whether to disable compiler warnings" OFF)
option(LIZARD_WARNINGS_AS_ERRORS "Whether to disable compiler warnings" OFF)


set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ${LIZARD_LTO})


# On some compilers, we have to use some special flags in order to get support
# for std::filesystem. That's why this explicit step is necessary.
find_package(Filesystem REQUIRED QUIET)
link_libraries(std::filesystem)


set(WANTED_FEATURES "")
if (LIZARD_DISABLE_WARNINGS)
	list(APPEND WANTED_FEATURES "DISABLE_ALL_WARNINGS")
else()
	list(APPEND WANTED_FEATURES "ENABLE_MOST_WARNINGS")
endif()

if (LIZARD_WARNINGS_AS_ERRORS)
	list(APPEND WANTED_FEATURES "ENABLE_WARNINGS_AS_ERRORS")
endif()

get_compiler_flags(
	${WANTED_FEATURES}
	OUTPUT_VARIABLE COMPILER_FLAGS
)

message(STATUS "Used compiler flags: ${COMPILER_FLAGS}")

add_compile_options(${COMPILER_FLAGS})
