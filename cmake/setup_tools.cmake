# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

option(LIZARD_IWYU "Whether to enable the include-what-you-use tool (if found)" ON)
option(LIZARD_LWYU "Whether to enable the link-what-you-use tool" ON)

message(STATUS "Using Link-what-you-use: ${LIZARD_LWYU}")
set(CMAKE_LINK_WHAT_YOU_USE ${LIZARD_LWYU})

if (LIZARD_IWYU)
	find_program(iwyu_path NAMES include-what-you-use iwyu)
	if (iwyu_path)
		# Use -w to disable any additional compiler warnings when running IWYU
		set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${iwyu_path}" -w)
		set(CMAKE_C_INCLUDE_WHAT_YOU_USE "${iwyu_path}" -w)
	else()
		message(STATUS "Could not find include-what-you-use tool - You'll have to check your includes manually")
	endif()
endif()

