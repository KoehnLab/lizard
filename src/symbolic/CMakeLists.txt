# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

add_library(lizard_symbolic STATIC
	DepthFirst.cpp
	EnumStreamOperators.cpp
	Index.cpp
	IndexSpace.cpp
	IndexSpaceData.cpp
	IndexSpaceManager.cpp
	Tensor.cpp
	TensorBlock.cpp
	TensorElement.cpp
	TreeNode.cpp
)
register_lizard_target(lizard_symbolic)

add_library(lizard::symbolic ALIAS lizard_symbolic)

target_link_libraries(lizard_symbolic
	PUBLIC
		lizard::core
		libperm::libperm
		nonstd::span-lite
		iterators::iterators
)
