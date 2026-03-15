/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2026 dev4fun. All rights reserved.
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

#include "structs.test.h"
#include "grtpp_util.h"

#include "gtest/gtest.h"
#include "wb_test_helpers.h"
#include "context.h"

namespace testing {

using namespace grt;

class GRTUtilFunctionsTest : public ::testing::Test {
protected:
  void SetUp() override {
    MySqlStudioTester::reinitGRT();
    register_structs_test_xml();
    grt::GRT::get()->load_metaclasses(Context::get().tmpDataDir() + "/structs.test.xml");
    grt::GRT::get()->end_loading_metaclasses();
    EXPECT_EQ(grt::GRT::get()->get_metaclasses().size(), 6U);
  }

  void TearDown() override {
    MySqlStudioTester::reinitGRT();
  }
};

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTUtilFunctionsTest, SetValueByPath) {
  test_BookRef book(grt::Initialized);
  bool flag;

  flag = set_value_by_path(book, "/title", StringRef("TITLE"));
  EXPECT_TRUE(flag);
  EXPECT_EQ(*book->title(), "TITLE");

  flag = set_value_by_path(book, "/", StringRef("TITLE"));
  EXPECT_TRUE(!flag);

  try {
    set_value_by_path(book, "/xxx", StringRef("TITLE"));
    EXPECT_TRUE(false);
  } catch (grt::bad_item &) {
  }

  flag = set_value_by_path(book, "/title/x", StringRef("TITLE"));
  EXPECT_TRUE(!flag);

  try {
    set_value_by_path(book, "/title", IntegerRef(1234));
    EXPECT_TRUE(false);
  } catch (grt::type_error &) {
  }
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTUtilFunctionsTest, RegressionTestForBug17324160) {
  test_PublisherRef publisher(grt::Initialized);
  test_BookRef book(grt::Initialized);

  book->title("testbook");
  publisher->name("testpub");
  publisher->books().insert(book);
  book->publisher(publisher);

  test_PublisherRef publisher_copy(grt::shallow_copy_object(publisher));

  EXPECT_TRUE(publisher_copy.id() != publisher.id());
  EXPECT_EQ(*publisher_copy->name(), *publisher->name());
  EXPECT_EQ(publisher_copy->books().count(), 1U);
  EXPECT_EQ(publisher_copy->books()[0].id(), book.id());
  // The bug was that a shallow_copy would modify the referenced objects that would back-reference the copied object
  EXPECT_EQ(book->publisher().id(), publisher.id());
}

//-----------------------------------------------------------------------------------------------------

}


