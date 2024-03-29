# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

add_library(lizard_parser STATIC
	GeCCoExportParser.cpp
	TensorSymmetryParser.cpp
	ErrorReporter.cpp
)
register_lizard_target(lizard_parser)

add_library(lizard::parser ALIAS lizard_parser)

target_link_libraries(lizard_parser
	PRIVATE
		lizard_GeCCoExport_parser
		lizard_TensorSymmetry_parser
)

target_link_libraries(lizard_parser
	PUBLIC
		lizard::core
)
