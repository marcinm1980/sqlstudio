/*
 * Copyright (c) 2019, 2025, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2025, dev4fun. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA 
 */

#include "gtest/gtest.h"
#include "wb_test_helpers.h"

#include <stdio.h>
#include "grt.h"

#include "grtdb/editor_table.h"
#include "grtdb/db_object_helpers.h"


using namespace grt;
using namespace bec;
using namespace std;

namespace {

TEST(TreeModel, TestOfConstructors) {
  bec::NodeId n1;
  EXPECT_FALSE(n1.is_valid());

  bec::NodeId n2(3); // NodeId(const int)
  EXPECT_TRUE(n2.is_valid());

  bec::NodeId n3("1.2.3"); // NodeId(const std::string)
  EXPECT_TRUE(n3.is_valid());
  EXPECT_TRUE(n3.depth() == 3);

  bec::NodeId n4("1:2:3"); // NodeId(const std::string)
  EXPECT_TRUE(n4.is_valid());
  EXPECT_EQ(3U, n4.depth());

  bec::NodeId n5(n3);
  EXPECT_TRUE(n5.is_valid());
  EXPECT_EQ(3U, n5.depth());
}

TEST(TreeModel, EqualNodeTest) {
  bec::NodeId n1("1.2.3");
  bec::NodeId n2(n1);

  EXPECT_TRUE(n1 == n2);

  bec::NodeId n3("1:2");
  bec::NodeId n4(n2);
  bec::NodeId n5(n3);
  EXPECT_TRUE(n3 == n5);
  EXPECT_FALSE(n3 == n4);

  bec::NodeId n6("1:2:3");
  EXPECT_TRUE(n2 == n6);
  EXPECT_TRUE(n1 == n6);
}

TEST(TreeModel, ExceptionsTest) {
  bool exception_caught = false;
  try {
    bec::NodeId n1("1,2,3");
  } catch (std::runtime_error &) {
    exception_caught = true;
  }
  EXPECT_TRUE(exception_caught);

  exception_caught = false;
  try {
    bec::NodeId n1("aaaa");
  } catch (std::runtime_error &) {
    exception_caught = true;
  }
  EXPECT_TRUE(exception_caught);

  exception_caught = false;
  try {
    bec::NodeId n1("1.2.#.\0");
  } catch (std::runtime_error &) {
    exception_caught = true;
  }
  EXPECT_TRUE(exception_caught);

  bec::NodeId n2("");
  EXPECT_FALSE(n2.is_valid());

  bec::NodeId n3("..::...");
  EXPECT_FALSE(n3.is_valid());
}

TEST(TreeModel, AssignTest) {
  bec::NodeId n1("1:2:3");
  bec::NodeId n2("4.5.6.7.8.8");

  EXPECT_FALSE(n1 == n2);

  n2 = n1;

  EXPECT_TRUE(n1 == n2);
}

TEST(TreeModel, NodeDepthTest) {
  bec::NodeId n1("1:2:3");

  EXPECT_EQ(3U, n1.depth());

  n1 = n1.parent();
  EXPECT_EQ(2U, n1.depth());
  EXPECT_EQ("1.2", n1.toString());
}

TEST(TreeModel, OperatorBracketTest) {
  bec::NodeId n1("23.56.78.1.43");
  const std::size_t test[] = {23, 56, 78, 1, 43};

  for (unsigned int i = 0U; i < sizeof(test) / sizeof(*test); i++) {
    char buf[64];
    snprintf(buf, sizeof(buf) / sizeof(*buf), "NodeId::operator[] test%i", i);
    EXPECT_TRUE(n1[i] == test[i]);
  }
}

TEST(TreeModel, NodeBackTest) {
  bec::NodeId n1("23.56.78.1.43");
  EXPECT_EQ(43U, n1.back());
}

TEST(TreeModel, NodeNextTest) {
  bec::NodeId n1("23.56.78.1.43");
  n1.next();
  EXPECT_EQ(44U, n1.back());
}

TEST(TreeModel, NodeAppendTest) {
  bec::NodeId n1("23.56.78.1.43");

  n1.append(1111);
  EXPECT_EQ(1111U, n1.back());
}

class TreeModelWithMap : public ::testing::Test {
protected:
  bec::NodeIds map;
};

TEST_F(TreeModelWithMap, ParentNodeTest) {
  bec::NodeId node("1:2:3");
  bec::NodeId::uid uid1 = map.map_node_id(node);

  bec::NodeId node2("1.2.3.5");
  node2 = node2.parent();
  bec::NodeId::uid uid2 = map.map_node_id(node2);

  EXPECT_TRUE(uid1 == uid2);
}

TEST_F(TreeModelWithMap, MapNodeIdTest1) {
  bec::NodeId node("1:2:3");
  bec::NodeId::uid uid1 = map.map_node_id(node);

  bec::NodeId node2(map.map_node_id(uid1));
  EXPECT_TRUE(node == node2);
}

TEST_F(TreeModelWithMap, MapNodeIdTest2) {
  bec::NodeId n1("1.1");
  bec::NodeId::uid uid1 = map.map_node_id(n1);
  n1.next();
  EXPECT_EQ(2U, n1.back());

  bec::NodeId n2(map.map_node_id(uid1));
  for (int i = 0; i < 2; i++) {
    EXPECT_EQ(1U, n2[i]);
  }
}

TEST(TreeModel, SortingNodesTest) {
  std::vector<bec::NodeId> test;
  for (std::size_t i = 1; i < 20; i++)
    test.push_back(bec::NodeId(i));
  std::sort(test.begin(), test.end());

  for (std::size_t i = 1, j = 0; i < test.size(); i++, j++)
    EXPECT_TRUE(i == test[j][0]);
}

}

