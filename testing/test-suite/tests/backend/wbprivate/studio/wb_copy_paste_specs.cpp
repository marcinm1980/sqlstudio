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

#include "wb_test_helpers.h"
#include "grtdb/db_object_helpers.h"
#include "grt_test_helpers.h"
#include "grtpp_util.h"
#include "grt/clipboard.h"
#include "base/string_utilities.h"

#include "gtest/gtest.h"

namespace {

  using namespace base;
  using namespace wb;

  static auto match_member(const grt::MetaClass::Member *member, const grt::ObjectRef &copy,
                           const grt::ObjectRef &source) -> bool {
    if (!grt::is_simple_type(member->type.base.type))
      return true;

    if (member->name == "guid") // it's always true, as it's unique per GrtObject
      return true;
    grt::ValueRef value1;
    grt::ValueRef value2;

    value1 = source.get_member(member->name);
    value2 = copy.get_member(member->name);

    EXPECT_EQ(value1.toString(), value2.toString());

    return true;
  }

  static auto ensure_simple_contents_match(const grt::ObjectRef &copy, const grt::ObjectRef &source) -> void {
    grt::MetaClass *mc = copy.get_metaclass();

    mc->foreach_member(std::bind(&match_member, std::placeholders::_1, copy, source));
  }

  static auto ensure_list_contents_copy(const grt::BaseListRef &copy, const grt::BaseListRef &source) -> void {
    EXPECT_TRUE(copy.valueptr() != source.valueptr());

    EXPECT_EQ(copy.count(), source.count());

    for (size_t c = copy.count(), i = 0; i < c; i++) {
      EXPECT_TRUE(copy[i].valueptr() != source[i].valueptr());

      grt::ObjectRef copyRef = grt::ObjectRef::cast_from(copy[i]);
      grt::ObjectRef sourceRef = grt::ObjectRef::cast_from(source[i]);
      ensure_simple_contents_match(grt::ObjectRef(copyRef), grt::ObjectRef(sourceRef));
    }
  }

  struct TestData {
    std::unique_ptr<MySqlStudioTester> tester;
  };

  class CopyPasteRelatedTestsTest : public ::testing::Test {
  protected:
    TestData *data = new TestData();

    void SetUp() override {
      data->tester.reset(new MySqlStudioTester());
      data->tester->initializeRuntime();
    }

    void TearDown() override {
      delete data;
    }
  };

  TEST_F(CopyPasteRelatedTestsTest, CopyToClipboard) {
    data->tester->wb->open_document("data/studio/all_objects.mwb");

    EXPECT_EQ(data->tester->getPview()->figures().count(), 6U);

    studio_physical_TableFigureRef source, copy;
    source = studio_physical_TableFigureRef::cast_from(
      grt::find_named_object_in_list(data->tester->getPview()->figures(), "table1"));

    EXPECT_TRUE(source.is_valid());

    wb::WBComponent *compo = data->tester->wb->get_component_handling(source);
    EXPECT_TRUE(compo != 0);

    grt::CopyContext context;

    compo->copy_object_to_clipboard(source, context);

    EXPECT_TRUE(bec::GRTManager::get()->get_clipboard()->get_data().empty() == false);
    copy = studio_physical_TableFigureRef::cast_from(bec::GRTManager::get()->get_clipboard()->get_data().front());

    EXPECT_TRUE(copy.is_valid());
    EXPECT_TRUE(copy.id() != source.id());

    EXPECT_TRUE(copy->owner() == source->owner());
    EXPECT_TRUE(copy->layer() == source->layer());

    EXPECT_TRUE(copy.valueptr() != source.valueptr());

    EXPECT_TRUE(copy->table() == source->table());

    data->tester->wb->close_document();
    data->tester->wb->close_document_finish();
  }

  TEST_F(CopyPasteRelatedTestsTest, MakeCopyOfTable) {
    // create a table with PK and make sure that a copy will contain
    // proper refs to the copied objects
    // data->tester->create_new_document();
    data->tester->wb->open_document("data/studio/all_objects.mwb");

    db_mysql_TableRef table(grt::Initialized);
    table->name("person");

    for (int i = 0; i < 5; i++) {
      db_mysql_ColumnRef column(grt::Initialized);

      column->owner(table);
      column->name(strfmt("col%i", i));
      if (i > 2)
        column->setParseType("VARCHAR(32)", data->tester->getRdbms()->simpleDatatypes());
      else
        column->setParseType("INT", data->tester->getRdbms()->simpleDatatypes());
      table->columns().insert(column);

      if (i == 0)
        table->addPrimaryKeyColumn(column);
    }

    db_mysql_TableRef copy = db_mysql_TableRef::cast_from(grt::copy_object(table));

    EXPECT_TRUE(copy.is_valid());
    EXPECT_TRUE(copy.valueptr() != table.valueptr());

    ensure_list_contents_copy(table->columns(), copy->columns());

    EXPECT_TRUE(copy->primaryKey().is_valid());
    EXPECT_TRUE(copy->primaryKey().valueptr() != table->primaryKey().valueptr());
    EXPECT_TRUE(copy->primaryKey()->columns()[0].valueptr() != table->primaryKey()->columns()[0].valueptr());
    EXPECT_TRUE(copy->indices().get(0).valueptr() == copy->primaryKey().valueptr());

    EXPECT_EQ(*copy->columns().get(0)->name(), "col0");
    EXPECT_TRUE(copy->columns().get(0)->owner() == copy);

    EXPECT_EQ(copy->columns().get(0).valueptr(),
      copy->primaryKey()->columns().get(0)->referencedColumn().valueptr());

    data->tester->wb->close_document();
    data->tester->wb->close_document_finish();
  }
}
