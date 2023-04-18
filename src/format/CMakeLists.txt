# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

add_library(lizard_format INTERFACE)

add_library(lizard::format ALIAS lizard_format)

target_link_libraries(lizard_format INTERFACE lizard::core lizard::symbolic lizard::process fmt::fmt)
