# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

set(GRAMMARS
	"${CMAKE_CURRENT_SOURCE_DIR}/GeCCoExport.g4"
	"${CMAKE_CURRENT_SOURCE_DIR}/TensorSymmetry.g4"
)

foreach (CURRENT_GRAMMAR IN LISTS GRAMMARS)
	get_filename_component(GRAMMAR_EXT "${CURRENT_GRAMMAR}" EXT)
	if (NOT "${GRAMMAR_EXT}" STREQUAL ".g4")
		message(FATAL_ERROR "Expected grammar to end with .g4 (got \"${GRAMMAR_EXT}\")")
	endif()
	if (NOT EXISTS "${CURRENT_GRAMMAR}")
		message(FATAL_ERROR "Grammar does not exist: ${CURRENT_GRAMMAR}")
	endif()

	get_filename_component(GRAMMAR_NAME "${CURRENT_GRAMMAR}" NAME_WLE)

	set(TARGET_NAME "lizard_${GRAMMAR_NAME}_parser")


	antlr_target("${GRAMMAR_NAME}" "${CURRENT_GRAMMAR}"
		PACKAGE lizard::parser::autogen
		OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${GRAMMAR_NAME}/lizard/parser/autogen"
		LEXER
		PARSER
		LISTENER
		VISITOR
	)

	add_library(${TARGET_NAME} STATIC ${ANTLR_${GRAMMAR_NAME}_CXX_OUTPUTS})

	target_include_directories("${TARGET_NAME}" SYSTEM
		PUBLIC
			"${CMAKE_CURRENT_BINARY_DIR}/${GRAMMAR_NAME}"
	)
	target_link_libraries("${TARGET_NAME}"
		PUBLIC
			antlr4_static
	)

	# Don't run any checks on the auto-generated files
	set_target_properties(${TARGET_NAME}
		PROPERTIES
			CXX_INCLUDE_WHAT_YOU_USE ""
			C_INCLUDE_WHAT_YOU_USE ""
			LINK_WHAT_YOU_USE OFF
	)
endforeach()

