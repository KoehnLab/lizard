// This file is part of the lizard quantum chemistry software.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file at the root of the lizard source
// tree or at <https://github.com/KoehnLab/lizard/blob/main/LICENSE>.

#include "lizard/symbolic/IndexSpaceManager.hpp"
#include "lizard/symbolic/IndexSpace.hpp"
#include "lizard/symbolic/IndexSpaceData.hpp"
#include "lizard/symbolic/InvalidIndexSpaceException.hpp"
#include "lizard/symbolic/Spin.hpp"

#include <gtest/gtest.h>

using namespace lizard;



////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// TEST CASES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

TEST(IndexSpaceManager, registerSpace) {
	IndexSpaceManager manager;

	IndexSpace space1(0, Spin::Alpha);
	IndexSpace space2(0, Spin::Both);
	IndexSpace space3(1, Spin::Alpha);

	IndexSpaceData data1("dummy", 'd', 2, Spin::Both, {});
	IndexSpaceData data2("other_dummy", 'o', 2, Spin::Both, {});

	ASSERT_NO_THROW(manager.registerSpace(space1, data1));

	// Duplicating the same IndexSpace (disregarding spin) is not allowed
	ASSERT_THROW(manager.registerSpace(space1, data1), InvalidIndexSpaceException);
	ASSERT_THROW(manager.registerSpace(space1, data2), InvalidIndexSpaceException);
	ASSERT_THROW(manager.registerSpace(space2, data1), InvalidIndexSpaceException);
	ASSERT_THROW(manager.registerSpace(space2, data2), InvalidIndexSpaceException);
	// Registering a different IndexSpace with the same name is not allowed
	ASSERT_THROW(manager.registerSpace(space3, data1), InvalidIndexSpaceException);

	ASSERT_NO_THROW(manager.registerSpace(space3, data2));
}

TEST(IndexSpaceManager, query) {
	IndexSpaceManager manager;

	IndexSpace space1(0, Spin::Alpha);
	IndexSpace space2(1, Spin::Both);

	IndexSpaceData data1("space1", '1', 2, Spin::Both, { 'a', 'b' });
	IndexSpaceData data2("space2", '2', 2, Spin::Both, { 'i', 'j' });

	IndexSpaceData dummyData({}, {}, {}, {}, {});
	ASSERT_THROW(dummyData = manager.getData(space1), InvalidIndexSpaceException);

	manager.registerSpace(space1, data1);
	manager.registerSpace(space2, data2);

	ASSERT_EQ(manager.getData(space1).getName(), data1.getName());
	ASSERT_EQ(manager.getData(space2).getName(), data2.getName());

	// Even though the IndexSpace instance to register this space had Alpha spin, the one created
	// here will have the default spin instead
	ASSERT_EQ(manager.createFromName(data1.getName()).getSpin(), Spin::Both);
	ASSERT_EQ(manager.createFromName(data1.getName()).getID(), space1.getID());
	ASSERT_EQ(manager.createFromName(data2.getName()), space2);

	ASSERT_EQ(manager.createFromLabel('a'), manager.createFromName("space1"));
	ASSERT_EQ(manager.createFromLabel('i'), manager.createFromName("space2"));
}
