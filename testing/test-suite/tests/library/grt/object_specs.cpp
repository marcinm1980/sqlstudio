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



static bool count_member(const grt::MetaClass::Member *member, int *count) {
  (*count)++;
  return true;
}
/*
class TestBridge : public ObjectBridgeBase {
public:
  grt::IntegerRef x;
  grt::IntegerRef y;
  grt::StringRef myname;
  grt::ListRef<test_Book> books;
  bool *flag;

protected:
  virtual void initialize(const DictRef &args)
  {
    x= grt::IntegerRef(0);
    y= grt::IntegerRef(0);
    myname= grt::StringRef("hello");
    books.init();
  }
  virtual void destroy()
  {
    *flag= true;
  }

  virtual ValueRef get_item(const std::string &name) const
  {
    if (name == "x")
      return x;
    if (name == "y")
      return y;
    if (name == "name")
      return myname;
    if (name == "books")
      return books;
    return ValueRef();
  }

  virtual void set_item(const std::string &name, const ValueRef &value)
  {
    if (name == "x")
      assign(x, value);
    if (name == "y")
      assign(y, value);
    if (name == "name")
      assign(myname, value);
    if (name == "books")
      throw std::logic_error(name+" is read-only");
  }

  virtual void serialize(xmlNodePtr node)
  {
  }

  virtual void unserialize(xmlNodePtr node)
  {
  }

  virtual void copy(ObjectBridgeBase *orig)
  {
  }


public:
  TestBridge(grt::ValueRef self, void *data) : ObjectBridgeBase(self, data) {};
};
*/

namespace {

class GRTObjectValuesTest : public ::testing::Test {
protected:
  void SetUp() override {
    register_structs_test_xml();
    grt::GRT::get()->load_metaclasses(casmine::CasmineContext::get()->tmpDataDir() + "/structs.test.xml");
    grt::GRT::get()->end_loading_metaclasses();
  }

  void TearDown() override {
    MySqlStudioTester::reinitGRT();
  }
};

TEST_F(GRTObjectValuesTest, LoadStructures) {
  EXPECT_EQ(grt::GRT::get()->get_metaclasses().size(), 6U);
}

TEST_F(GRTObjectValuesTest, MetaClassSupport) {
  test_BookRef book(grt::Initialized);

  EXPECT_TRUE(book.has_member("title"));
  EXPECT_FALSE(book.has_member("Title"));

  EXPECT_TRUE(book.get_member("title").is_valid());

  book.set_member("title", grt::StringRef("Harry Potter"));
  EXPECT_EQ(book.get_string_member("title"), "Harry Potter");

  book.set_member("price", grt::DoubleRef(123.45));
  EXPECT_EQ(book.get_double_member("price"), grt::DoubleRef(123.45));

  test_AuthorRef author(grt::Initialized);
  author.set_member("name", grt::StringRef("Some One"));

  EXPECT_EQ(author.get_string_member("name"), "Some One");

  EXPECT_NO_THROW(book->authors().insert(author));
}

TEST_F(GRTObjectValuesTest, ExceptionsForInvalidMemberAccess) {
  test_BookRef obj(grt::Initialized);

  EXPECT_THROW(obj.set_member("invalid", grt::StringRef("XXX")), std::exception);
  EXPECT_THROW(obj.get_integer_member("invalid"), std::exception);
  EXPECT_THROW(obj.set_member("title", grt::IntegerRef(1234)), std::exception);
  EXPECT_THROW(obj.set_member("title", grt::DoubleRef(1234.123)), std::exception);
  EXPECT_THROW(obj.set_member("price", grt::StringRef("hello")), std::exception);
  EXPECT_THROW(obj.set_member("authors", grt::StringRef("joe")), std::exception);
  EXPECT_THROW(obj.set_member("pages", grt::DoubleRef(1234.456)), std::exception);
}

TEST_F(GRTObjectValuesTest, ValueMemberAccess) {
  test_BookRef book(grt::Initialized);

  book->title("Harry Potter");
  book->title(*book->title() + " XXV");
  book->price(500.23);
  EXPECT_NE(*book->title(), "Harry Potter");

  EXPECT_EQ(*book->title(), "Harry Potter XXV");

  test_AuthorRef author(grt::Initialized);

  book->authors().insert(author);
  EXPECT_EQ(book->authors().count(), 1U);

  book->authors().get(0)->name("J.K.Bowling");
  EXPECT_EQ(*author->name(), "J.K.Bowling");

  book->authors()[0]->name("ABC");
  EXPECT_EQ(*author->name(), "ABC");

  book->authors().remove(0);
  EXPECT_EQ(book->authors().count(), 0U);
}

TEST_F(GRTObjectValuesTest, CheckIfInheritedValuesAreProperlyInitialized) {
  test_BookRef book(grt::Initialized);

  int count = 0;
  book->get_metaclass()->foreach_member(std::bind(&count_member, std::placeholders::_1, &count));
  EXPECT_EQ(count, 6);
}

/*
TEST_F(GRTObjectValuesTest, BridgeRegistration) {
  bool ret;

  ret= ObjectBridgeBase::register_bridge<TestBridge>;
  EXPECT_TRUE(ret);
}

TEST_F(GRTObjectValuesTest, BridgeInteraction) {
  bool bridge_destroyed= false;

  {
    test_Bridged bridged;
    test_Book book;

    EXPECT_EQ(bridged.get_metaclass().get_metaclass()->bridge, "tut::TestBridge");
    EXPECT_NE(bridged.get_bridge_private(), 0U);

    book.title("Harry Potter");
    book.title(*book.title()+ " XXV");
    book.price(500.23);

    TestBridge *bridge_data;
    EXPECT_EQ(*bridged->name(), "hello");

    bridge_data= (TestBridge*)bridged.get_bridge_private();
    EXPECT_NE(bridge_data, 0U);

    bridge_flag= &bridge_destroyed;
    EXPECT_EQ(bridge_myname, bridged->name());

    bridged.name("xyz");
    EXPECT_EQ(*bridge_myname, "xyz");

    bridged.x(1234);
    EXPECT_EQ(bridged.x(), 1234);
    EXPECT_EQ(bridged.books().count(), 0U);

    bridged.books().insert(book);
    EXPECT_EQ(bridged.books().count(), 1U);
    EXPECT_EQ(*bridged.books().get(0).title(), "Harry Potter XXV");

    bridged.books().remove(0);
    EXPECT_EQ(bridged.books().count(), 0U);
    EXPECT_THROW(bridged.books().remove(0), std::exception);
    EXPECT_EQ(bridged.count_members(), 4U);
  }
  // leaving the context should destroy the objects

  EXPECT_TRUE(bridge_destroyed);
}
*/

}


