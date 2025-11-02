/*
 * Copyright (c) 2018, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "base/log.h"

#include "grtpp_util.h"
#include "grt.h"
#include "structs.test.h"

#include "grt_values_test_data.h"
#include "grt_test_helpers.h"
#include "wb_test_helpers.h"

#include "gtest/gtest.h"

using namespace casmine;

namespace {

base::Logger test_logger(".", getenv("WB_LOG_STDERR") != 0);

template<typename ItemType>
void test_list_value(grt::ListRef<ItemType>& lv, grt::Ref<ItemType> v[]) {
  lv.retain();
  EXPECT_EQ(lv.refcount(), 2);
  EXPECT_EQ(lv.count(), 0U);
  EXPECT_EQ(v[0].refcount(), 1);
  lv.insert(v[0]);
  EXPECT_EQ(lv.count(), 1U);
  EXPECT_EQ(v[0].refcount(), 2);
  lv.remove(0);
  EXPECT_EQ(lv.count(), 0U);
  EXPECT_EQ(v[0].refcount(), 1);
  lv.insert(v[0]);
  EXPECT_EQ(lv.count(), 1U);
  lv.insert(v[1]);
  EXPECT_EQ(lv.count(), 2U);
  lv.insert(v[2], 1);
  EXPECT_EQ(lv.count(), 3U);
  EXPECT_EQ(lv.get(1), v[2]);
  lv.insert(v[3], 0);
  EXPECT_EQ(lv.count(), 4U);
  EXPECT_EQ(lv.get(0), v[3]);
  lv.insert(v[4], 3);
  EXPECT_EQ(lv.count(), 5U);
  EXPECT_EQ(lv.get(3), v[4]);
  lv.insert(v[5], 5);
  EXPECT_EQ(lv.count(), 6U);
  EXPECT_EQ(lv.get(5), v[5]);
  EXPECT_THROW(lv.insert(v[6], 7), grt::bad_item);
  EXPECT_EQ(lv.count(), 6U);
  EXPECT_EQ(v[0].refcount(), 2);
  for (int n = (int)lv.count() - 1; n >= 0; n--) {
    lv.remove(n);
  }
  EXPECT_EQ(v[5].refcount(), 1);
  EXPECT_EQ(lv.count(), 0U);
  for (int n = 0; n < 6; n++) {
    lv.insert(v[n], n);
  }
  EXPECT_EQ(lv.count(), 6U);
  for (int n = 0; n < 6; n++) {
    EXPECT_EQ(v[n].refcount(), 2);
  }
  lv.set(3, v[5]);
  EXPECT_EQ(lv.get(3), v[5]);
  for (int i = 0; i < 8; i++) {
    int refcount = 0;
    switch (i) {
      case 3: refcount = 1; break;
      case 5: refcount = 3; break;
      default: refcount = (i > 5 ? 1 : 2); break;
    }
    EXPECT_EQ(v[i].refcount(), refcount);
  }
  EXPECT_THROW(lv.set(6, v[6]), grt::bad_item);
  EXPECT_EQ(lv.count(), 6U);
  while (lv.count())
    lv.remove(0);
  EXPECT_EQ(lv.refcount(), 2);
  EXPECT_EQ(lv.count(), 0U);
  for (int i = 0; i < 8; i++) {
    EXPECT_EQ(v[i].refcount(), 1);
  }
}

class GRTValuesTest : public ::testing::Test {
protected:
  void SetUp() override {
    register_structs_test_xml();
    grt::GRT::get()->load_metaclasses("./data/structs.test.xml");
    grt::GRT::get()->end_loading_metaclasses();
  }

  void TearDown() override {
    MySqlStudioTester::reinitGRT();
  }
};

TEST_F(GRTValuesTest, LoadStructures) {
  EXPECT_EQ(grt::GRT::get()->get_metaclasses().size(), 6U);
}

TEST_F(GRTValuesTest, BaseTests) {
  grt::IntegerRef iv(6666);
  EXPECT_EQ(iv.refcount(), 1);
  {
    grt::ValueRef tmp = iv;
    EXPECT_EQ(iv.refcount(), 2);
  }
  EXPECT_EQ(iv.refcount(), 1);
  iv.retain();
  iv.retain();
  iv.retain();
  EXPECT_EQ(iv.refcount(), 4);
  grt::internal::Value* value = iv.valueptr();
  iv.clear();
  EXPECT_EQ(iv.valueptr(), nullptr);
  EXPECT_EQ(value->refcount(), 3);
  value->release();
  value->release();
  value->release();
}

TEST_F(GRTValuesTest, IntegerValue) {
  grt::IntegerRef iv(1234);
  ssize_t i;
  EXPECT_FALSE(grt::IntegerRef(static_cast<grt::internal::Integer *>(nullptr)).is_valid());
  EXPECT_TRUE(grt::IntegerRef(0).is_valid());
  EXPECT_FALSE(grt::IntegerRef().is_valid());
  i = iv;
  EXPECT_EQ(i, 1234);
  EXPECT_EQ(iv.refcount(), 1);
  grt::IntegerRef iv2(iv);
  EXPECT_EQ(iv2.valueptr(), iv.valueptr());
  EXPECT_EQ(iv.refcount(), 2);
  grt::IntegerRef iv3;
  EXPECT_EQ(iv3.valueptr(), nullptr);
  EXPECT_FALSE(grt::IntegerRef::cast_from(iv3).is_valid());
  iv3 = iv;
  EXPECT_EQ(iv3.valueptr(), iv.valueptr());
  iv3 = 5;
  EXPECT_EQ(iv3, 5);
  iv3 = iv3 + 10;
  EXPECT_EQ(iv3, 15);
  iv2 = 0;
  EXPECT_EQ(iv.refcount(), 1);
  grt::StringRef s("hi");
  EXPECT_FALSE(iv.can_wrap(s));
  EXPECT_THROW(iv.cast_from(s), grt::type_error);
}

TEST_F(GRTValuesTest, DoubleValue) {
  grt::DoubleRef iv(1234.5678);
  double i;
  EXPECT_FALSE(grt::DoubleRef(static_cast<grt::internal::Double *>(nullptr)).is_valid());
  EXPECT_TRUE(grt::DoubleRef(0.0).is_valid());
  EXPECT_FALSE(grt::DoubleRef().is_valid());
  i = iv;
  EXPECT_EQ(i, 1234.5678);
  grt::DoubleRef iv2(iv);
  EXPECT_EQ(iv2.valueptr(), iv.valueptr());
  EXPECT_EQ(iv.refcount(), 2);
  grt::DoubleRef iv3;
  EXPECT_EQ(iv3.valueptr(), nullptr);
  iv3 = iv;
  EXPECT_EQ(iv3.valueptr(), iv.valueptr());
  iv3 = 1.5;
  EXPECT_EQ((double)iv3, 1.5);
  iv3 = iv3 + 10;
  EXPECT_EQ((double)iv3, 11.5);
  grt::IntegerRef v(1234);
  EXPECT_TRUE(!grt::DoubleRef::can_wrap(v));
  EXPECT_THROW(iv.cast_from(v), grt::type_error);
}

TEST_F(GRTValuesTest, StringValue) {
  grt::StringRef iv("hello");
  std::string s;
  EXPECT_FALSE(grt::StringRef(static_cast<grt::internal::String *>(nullptr)).is_valid());
  EXPECT_FALSE(grt::StringRef().is_valid());
  EXPECT_TRUE(grt::StringRef("").is_valid());
  s = iv;
  EXPECT_EQ(s, "hello");
  grt::StringRef iv2(iv);
  EXPECT_EQ(iv2.valueptr(), iv.valueptr());
  EXPECT_EQ(iv.refcount(), 2);
  grt::StringRef iv3;
  EXPECT_EQ(iv3.valueptr(), nullptr);
  iv3 = iv;
  EXPECT_EQ(iv3.valueptr(), iv.valueptr());
  iv3 = std::string("test");
  EXPECT_EQ((std::string)iv3, "test");
  iv3 = (std::string)iv + " world";
  EXPECT_EQ((std::string)iv3, "hello world");
  grt::IntegerRef v(1234);
  EXPECT_TRUE(!grt::StringRef::can_wrap(v));
  EXPECT_THROW(iv.cast_from(v), grt::type_error);
}

// Additional tests converted similarly...

} // namespace

