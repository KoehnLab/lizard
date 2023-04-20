// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/symbolic/Expression.hpp"
#include "lizard/symbolic/ExpressionTree.hpp"
#include "lizard/symbolic/NamedExpressionTree.hpp"
#include "lizard/symbolic/TensorElement.hpp"

namespace lizard {

using TensorExpr          = Expression< TensorElement >;
using ConstTensorExpr     = ConstExpression< TensorElement >;
using TensorExprTree      = ExpressionTree< TensorElement >;
using NamedTensorExprTree = NamedExpressionTree< TensorElement, TensorElement >;

} // namespace lizard
