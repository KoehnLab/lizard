# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

include(CheckIPOSupported)

check_ipo_supported(RESULT LTO_DEFAULT)


option(LIZARD_LTO "Whether to use link-time optimizations (if available)" ${LTO_DEFAULT})
option(LIZARD_DISABLE_WARNINGS "Whether to disable compiler warnings" OFF)
option(LIZARD_WARNINGS_AS_ERRORS "Whether to disable compiler warnings" OFF)


# Use cpp17 and error if that is not available
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# On some compilers, we have to use some special flags in order to get support
# for std::filesystem. That's why this explicit step is necessary.
find_package(Filesystem REQUIRED QUIET)
link_libraries(std::filesystem)


set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ${LIZARD_LTO})


# Configure warning-related options
if (MSVC)
	add_compile_options(/W4 /Wall)

	if (LIZARD_DISABLE_WARNINGS)
		add_compile_options(/w)
	elseif (LIZARD_WARNINGS_AS_ERRORS)
		add_compile_options(/WX)
	endif()
else()
	add_compile_options(-Wpedantic -Wall -Wextra)

	if (LIZARD_DISABLE_WARNINGS)
		add_compile_options(-w)
	elseif (LIZARD_WARNINGS_AS_ERRORS)
		add_compile_options(-Werror)
	endif()
endif()
