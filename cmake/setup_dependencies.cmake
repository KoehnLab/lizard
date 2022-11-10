# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/Krzmbrzl/lizard/blob/main/LICENSE>.

# Add ANTLR4 cpp runtime
set(ANTLR4_TAG "v4.11.1")
include(ExternalAntlr4Cpp)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# Set ANTLR jar
set(ANTLR_EXECUTABLE "${3RDPARTY_DIR}/antlr4/antlr-4.11.1-complete.jar")

if (NOT EXISTS "${ANTLR_EXECUTABLE}")
	message(FATAL_ERROR "ANTLR jar-file not found. Was looking for ${ANTLR_EXECUTABLE}")
endif()

find_package(ANTLR REQUIRED)
