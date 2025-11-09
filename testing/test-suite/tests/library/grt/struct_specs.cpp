/*
 * Copyright (c) 2018, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "structs.test.h"

#include "gtest/gtest.h"
#include "wb_test_helpers.h"

namespace testing {

class GRTStructsMetaclassesTest : public ::testing::Test {
protected:
  void SetUp() override {
    EXPECT_THROW({ test_Book book; }, std::exception);
    register_structs_test_xml();
    grt::GRT::get()->load_metaclasses(Context::get().tmpDataDir() + "/structs.test.xml");
    grt::GRT::get()->end_loading_metaclasses();
  }

  void TearDown() override {
    MySqlStudioTester::reinitGRT();
  }
};

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, LoadStructures) {
  EXPECT_EQ(grt::GRT::get()->get_metaclasses().size(), 6U);
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, TestValidStructCreationAndComparisonToAnotherStruct) {
  grt::MetaClass *book(grt::GRT::get()->get_metaclass("test.Book"));

  EXPECT_NE(book, nullptr);
  EXPECT_EQ(book->name(), "test.Book");

  EXPECT_TRUE(book->is_a(grt::GRT::get()->get_metaclass("test.Publication")));
  EXPECT_TRUE(book->is_a(grt::GRT::get()->get_metaclass("test.Base")));
  EXPECT_TRUE(book->is_a("test.Base"));
  EXPECT_FALSE(book->is_a("XXXX"));

  EXPECT_EQ(book->get_attribute("caption"), "Book");
  EXPECT_EQ(book->get_attribute("xxx"), "");

  EXPECT_EQ(book->parent()->name(), "test.Publication");
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, CheckGetMember) {
  grt::MetaClass *book = grt::GRT::get()->get_metaclass("test.Book");
  const grt::MetaClass::Member *mem;

  mem = book->get_member_info("pages");
  EXPECT_NE(mem, nullptr);

  mem = book->get_member_info("title");
  EXPECT_NE(mem, nullptr);

  test_BookRef book_obj(grt::Initialized);

  book_obj->pages(1234);

  EXPECT_EQ(*grt::IntegerRef::cast_from(book->get_member_value(&book_obj.content(), "pages")), 1234);
  EXPECT_EQ(*book_obj->pages(), 1234);
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, CheckHasMember) {
  GTEST_SKIP() << "it needs an implementation";
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, CheckGetMember2) {
  GTEST_SKIP() << "it needs an implementation";
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, CheckSetMember) {
  GTEST_SKIP() << "it needs an implementation";
  // check set_member

  // from parent class

  // with override
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, CheckAllocation) {
  GTEST_SKIP() << "it needs an implementation";
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, CheckMethodCall) {
  GTEST_SKIP() << "it needs an implementation";
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, CheckForeachMember) {
  GTEST_SKIP() << "it needs an implementation";
}

//-----------------------------------------------------------------------------------------------------

TEST_F(GRTStructsMetaclassesTest, TestStructMembersAndTheirAttributes) {
  grt::MetaClass *book(grt::GRT::get()->get_metaclass("test.Book"));
  const grt::MetaClass::Member *m;
  grt::TypeSpec t;
  std::string a;

  m = book->get_member_info("authors");
  EXPECT_NE(m, nullptr);

  m = book->get_member_info("title");
  EXPECT_NE(m, nullptr);

  t = book->get_member_type("authors");
  EXPECT_EQ((int)t.base.type, grt::ListType);

  t = book->get_member_type("title");
  EXPECT_EQ((int)t.base.type, grt::StringType);

  // Member attributes.
  a = book->get_member_attribute("authors", "caption");
  EXPECT_EQ(a, "Authors");

  a = book->get_member_attribute("authors", "desc");
  EXPECT_EQ(a, "the list of authors");

  a = book->get_member_attribute("authors", "group");
  EXPECT_EQ(a, "group1");

  a = book->get_member_attribute("title", "caption");
  EXPECT_EQ(a, "Title");

  a = book->get_member_attribute("title", "desc");
  EXPECT_EQ(a, "title of the book");

  a = book->get_member_attribute("title", "group");
  EXPECT_EQ(a, "");
}

//-----------------------------------------------------------------------------------------------------

}


