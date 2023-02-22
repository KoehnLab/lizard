// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/core/TreeTraversal.hpp"

#include <iostream>

namespace lizard {

auto operator<<(std::ostream &stream, const TreeTraversal &traversal) -> std::ostream & {
	switch (traversal) {
		case TreeTraversal::DepthFirst_InOrder:
			stream << "depth-first-in-order";
			break;
		case TreeTraversal::DepthFirst_PreOrder:
			stream << "depth-first-pre-order";
			break;
		case TreeTraversal::DepthFirst_PostOrder:
			stream << "depth-first-post-order";
			break;
	}

	return stream;
}
} // namespace lizard
