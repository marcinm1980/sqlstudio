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

#include "grt/tree_model.h"

#include "gtest/gtest.h"
#include "wb_test_helpers.h"

using namespace grt;
using namespace bec;

namespace {

TEST(GrtTreeModelBase, BaseTests) {
  NodeId node, node2;

  EXPECT_FALSE(node.is_valid()) << "clean node";

  EXPECT_EQ(0U, node.depth()) << "clean node depth";

  node = NodeId(5);
  EXPECT_TRUE(node.is_valid()) << "node(5)";
  EXPECT_EQ(1U, node.depth()) << "node(5).depth()";
  EXPECT_EQ(5U, node[0]) << "node(5)[0]";

  node2 = node.append(7);
  EXPECT_EQ(2U, node.depth()) << "node append";
  EXPECT_EQ(5U, node[0]) << "node append[0]";
  EXPECT_EQ(7U, node[1]) << "node append[1]";

  EXPECT_EQ(2U, node2.depth()) << "node append ret";
  EXPECT_EQ(5U, node2[0]) << "node append ret[0]";
  EXPECT_EQ(7U, node2[1]) << "node append ret[1]";

  EXPECT_EQ(node2, node) << "node compare";

  node2 = NodeId(5);
  EXPECT_NE(node2, node) << "node compare";

  node2.append(7);
  node2.append(11);

  EXPECT_NE(node2, node) << "node compare";

  node = node2;
  EXPECT_EQ(node2, node) << "node assign/compare";
}

TEST(GrtTreeModelBase, Serialization) {
  // serialization
  NodeId node;
  std::string s;

  s = node.toString();
  EXPECT_EQ("", s) << "() toString";
  EXPECT_EQ(node, NodeId(s)) << "() parse";

  node.append(3);
  s = node.toString();
  EXPECT_EQ(NodeId(3), node) << "(3) check";
  EXPECT_EQ("3", s) << "(3) toString";
  EXPECT_EQ(node.toString(), NodeId(s).toString()) << "(3) parse";

  node.append(0);
  s = node.toString();
  EXPECT_EQ("3.0", s) << "(3,0) toString";
  EXPECT_EQ(node.toString(), NodeId(s).toString()) << "(3,0) parse";

  node.append(1);
  s = node.toString();
  EXPECT_EQ("3.0.1", s) << "(3,0,1) toString";
  EXPECT_EQ(node.toString(), NodeId(s).toString()) << "(3,0,1) parse";
}

TEST(GrtTreeModelBase, CommonTreeModelMethods) {
  GTEST_SKIP() << "needs implementation";
}

}

