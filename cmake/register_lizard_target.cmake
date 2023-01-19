# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

include(CompilerFlags)

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
	OUTPUT_VARIABLE LIZARD_COMPILER_FLAGS
)

message(STATUS "Used compiler flags for lizard targets: ${LIZARD_COMPILER_FLAGS}")


# On some compilers, we have to use some special flags in order to get support
# for std::filesystem. That's why this explicit step is necessary.
find_package(Filesystem REQUIRED QUIET)


function(register_lizard_target target)
	if (NOT TARGET "${target}")
		message(FATAL_ERROR "register_lizard_target: Attempted to register non-existing target \"${target}\"")
	endif()

	target_link_libraries("${target}" PUBLIC std::filesystem)
	set_target_properties("${target}" PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE "${LIZARD_LTO}")
	target_compile_options("${target}" PRIVATE ${LIZARD_COMPILER_FLAGS})
	target_include_directories("${target}" PUBLIC "${PROJECT_SOURCE_DIR}/include")
endfunction()

