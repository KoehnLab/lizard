// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#pragma once

#include "lizard/process/Strategy.hpp"
#include "lizard/symbolic/IndexSpaceManager.hpp"

#include <memory>
#include <vector>

namespace lizard {

/**
 * This class is intended to control the processing flow. Different processing Strategy objects
 * can be queued with this processor, which are then used to subsequently perform the different
 * processing steps that were previously queued.
 * Since the class knows how to deal with the different Strategy classes, the user doesn't have to
 * worry about how to invoke them and can instead let this class take care of executing everything.
 */
class Processor {
public:
	Processor(IndexSpaceManager manager = {});

	/**
	 * Sets the IndexSpaceManager that shall be used during processing
	 */
	void setIndexSpaceManager(IndexSpaceManager manager);

	/**
	 * Queues the provided processing step to be executed after all steps that have been queued before
	 */
	void enqueue(std::unique_ptr< Strategy > step);

	/**
	 * Inserts the provided processing step into the queue at the provided position
	 */
	void insert(std::unique_ptr< Strategy > step, std::size_t position);

	/**
	 * Runs all queued processing steps (in order)
	 */
	void run() const;

private:
	IndexSpaceManager m_spaceManager;
	std::vector< std::unique_ptr< Strategy > > m_processingSteps;
};

} // namespace lizard
