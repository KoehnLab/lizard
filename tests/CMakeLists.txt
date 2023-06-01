# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

include(FetchContent)
include(GoogleTest)

set(TEST_FILE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_files")

# Helper function for registering test-cases
function(register_test)
	set(options "")
	set(oneValueArgs "")
	set(multiValueArgs TARGETS)

	cmake_parse_arguments(LIZARD_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(LIBPERM_TEST_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unrecognized arguments: ${LIZARD_TEST_UNPARSED_ARGUMENTS}")
	endif()

	foreach(CURRENT IN LISTS LIZARD_TEST_TARGETS)
		if(NOT TARGET "${CURRENT}")
			message(FATAL_ERROR "\"${CURRENT} is not a target")
		endif()

		register_lizard_target("${CURRENT}")
		target_compile_definitions("${CURRENT}" PRIVATE TEST_FILE_DIR="${TEST_FILE_DIR}")
		target_link_libraries("${CURRENT}" PRIVATE gmock gtest_main)
		gtest_discover_tests("${CURRENT}")
	endforeach()
endfunction()

if (LIZARD_BUILD_TESTS)
	FetchContent_MakeAvailable(googletest)

	add_subdirectory(core)
	add_subdirectory(symbolic)
	add_subdirectory(format)
	add_subdirectory(parser)
	add_subdirectory(process)
endif()
