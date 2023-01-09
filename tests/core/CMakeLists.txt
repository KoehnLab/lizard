# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

add_executable(CoreTest
	ExpressionTest.cpp
	FractionTest.cpp
	MetaprogrammingTest.cpp
	MultiEnumTest.cpp
	SignedCastTest.cpp
)

target_link_libraries(CoreTest PRIVATE lizard::core)

register_test(TARGETS CoreTest)
