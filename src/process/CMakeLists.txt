# This file is part of the lizard quantum chemistry software.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file at the root of the lizard source
# tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

add_library(lizard_process STATIC
	EnumStreamOperators.cpp
	ExportStrategy.cpp
	HardcodedImport.cpp
	ImportStrategy.cpp
	OptimizationStrategy.cpp
	ProcessingStep.cpp
	Processor.cpp
	ReporterConfig.cpp
	SpinIntegration.cpp
	SpinLSE.cpp
	SpinProcessingStrategy.cpp
	Strategy.cpp
	SubstitutionStrategy.cpp
	TextExport.cpp
)

register_lizard_target(lizard_process)

add_library(lizard::process ALIAS lizard_process)

target_link_libraries(lizard_process
	PUBLIC
		lizard::format
		lizard::symbolic
		lizard::core
		spdlog::spdlog
	PRIVATE
		lizard::Eigen3
)

add_library(lizard_process_private_includes INTERFACE)
target_include_directories(lizard_process_private_includes INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
