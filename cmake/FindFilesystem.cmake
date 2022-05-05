# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

# Inspired by https://github.com/lethal-guitar/RigelEngine/blob/master/cmake/Modules/FindFilesystem.cmake

cmake_policy(PUSH)

if(POLICY CMP0067)
    # pass CMAKE_CXX_STANDARD to check_cxx_source_compiles()
    # has to appear before including CheckCXXSourceCompiles module
    cmake_policy(SET CMP0067 NEW)
endif()

include(CMakePushCheckState)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

cmake_push_check_state()

set(CMAKE_REQUIRED_QUIET ${Filesystem_FIND_QUIETLY})

check_include_file_cxx("filesystem" _FILESYSTEM_HEADER_FOUND)
mark_as_advanced(_FILESYSTEM_HEADER_FOUND)

set(_USABLE_CONFIG_FOUND FALSE)
set(_REQUIRED_LIB "")

if (_FILESYSTEM_HEADER_FOUND)
	if (NOT Filesystem_FOUND)
		# Only print on first (successful) iteration of this module
		message(STATUS "Found std::filesystem headers")
	endif()

	# We have some filesystem library available. Do link checks
	string(CONFIGURE [[
		#include <filesystem>

		int main() {
			auto cwd = std::filesystem::current_path();
			return static_cast<int>(cwd.string().size());
		}
	]] code @ONLY)

	# Try to compile a simple filesystem program without any linker flags
	check_cxx_source_compiles("${code}" _FILESYSTEM_NO_LINK_NEEDED)

	if (_FILESYSTEM_NO_LINK_NEEDED)
		set(_USABLE_CONFIG_FOUND TRUE)

		# on certain Linux distros we have a version of libstdc++ which has the final code for c++17 fs in the
		# libstdc++.so.*, BUT when compiling with g++ < 9, we MUST still link with libstdc++fs.a 
		# libc++ should not suffer from this issue, so, in theory we should be fine with only checking for
		# GCC's libstdc++ 
		if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0.0"))
			set(_REQUIRED_LIB "stdc++fs")
		endif()
	else()
		set(prev_libraries ${CMAKE_REQUIRED_LIBRARIES})

		# Add the libstdc++ flag
		set(CMAKE_REQUIRED_LIBRARIES ${prev_libraries} -lstdc++fs)

		check_cxx_source_compiles("${code}" _FILESYSTEM_STDCPPFS_NEEDED)

		if (_FILESYSTEM_STDCPPFS_NEEDED)
			set(_USABLE_CONFIG_FOUND TRUE)
			set(_REQUIRED_LIB "stdc++fs")
		else()
			# Try the libc++ flag
			set(CMAKE_REQUIRED_LIBRARIES ${prev_libraries} -lc++fs)

			check_cxx_source_compiles("${code}" _FILESYSTEM_CPPFS_NEEDED)

			if (_FILESYSTEM_CPPFS_NEEDED)
				set(_USABLE_CONFIG_FOUND TRUE)
				set(_REQUIRED_LIB "c++fs")
			else()
				if (NOT Filesystem_FIND_QUIETLY)
					message(STATUS "Unable to get filesystem library to link")
				endif()
			endif()
		endif()

		# Restore to defaults
		set(CMAKE_REQUIRED_LIBRARIES ${prev_libraries})
	endif()

	if (_USABLE_CONFIG_FOUND)
		add_library(cxx_std_filesystem INTERFACE IMPORTED)
		add_library(std::filesystem ALIAS cxx_std_filesystem)

		if (_REQUIRED_LIB)
			target_link_libraries(std::filesystem INTERFACE ${_REQUIRED_LIB})
		endif()

		set(Filesystem_FOUND TRUE CACHE BOOL "TRUE if we can compile and link a program using std::filesystem" FORCE)
	endif()
endif()

cmake_pop_check_state()

if(Filesystem_FIND_REQUIRED AND NOT Filesystem_FOUND)
	message(FATAL_ERROR "Unable to find usable configuration for std::filesystem")
endif()

cmake_policy(POP)
