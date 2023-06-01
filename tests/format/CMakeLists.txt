# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

add_executable(FormatTest
	FormatTest.cpp
)

target_link_libraries(FormatTest PRIVATE lizard::format)

register_test(TARGETS FormatTest)
