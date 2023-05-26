# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

add_executable(SymbolicTest
	ContractionTest.cpp
	ExpressionTreeTest.cpp
	IndexSpaceTest.cpp
	IndexSpaceManagerTest.cpp
	IndexTest.cpp
	TensorBlockTest.cpp
	TensorElementTest.cpp
	TensorTest.cpp
)

target_link_libraries(SymbolicTest PRIVATE lizard::symbolic)

register_test(TARGETS SymbolicTest)
