/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "grt.h"
#include "mysql_table_editor.h"
#include "model_mockup.h"

using namespace grt;
using namespace bec;

namespace {

  struct TestData {
    std::unique_ptr<MySqlStudioTester> tester;
  };

class MySQLTableEditorTest : public ::testing::Test {
protected:
  TestData *data = new TestData();

  void SetUp() override {
    data->tester.reset(new MySqlStudioTester());
    data->tester->initializeRuntime();
    data->tester->flushUntil(0.5);
    data->tester->createNewDocument();
  }

  void TearDown() override {
    delete data;
  }
};

TEST_F(MySQLTableEditorTest, ValidRDBMSAfterRenewingDocument) {
  data->tester->renewDocument();
  EXPECT_TRUE(data->tester->getRdbms().is_valid()) << "db_mgmt_RdbmsRef initialization";
  }

  TEST_F(MySQLTableEditorTest, TriggerParsing) {
    // Note: this test relied on content of a code editor (which is checked when setting trigger sql).
    //       However in tests we only have a stub implementation, so this doesn't work.
    //       The test shouldn't be about parsing trigger sql, as this is a low level parser test.
    //       Instead test if trigger addition works (removal is a simple grt call).
    data->tester->renewDocument();
    SyntheticMySQLModel model(data->tester.get());

    model.schema->name("test_schema");
    model.table->name("film");
    MySQLTableEditorBE t(model.table);
    model.table->triggers().remove_all();

    t.add_trigger("after", "delete");
    t.add_trigger("before", "delete");
    t.add_trigger("after", "update");

    EXPECT_EQ(3U, model.table->triggers().count());
    std::vector<std::string> names = { "film_after_delete", "film_before_delete", "film_after_update" };

    for (size_t i = 0, size = model.table->triggers().count(); i < size; i++) {
      std::string name = model.table->triggers().get(i)->name();
      EXPECT_EQ(names[i], name);
    }
  }

  TEST_F(MySQLTableEditorTest, AddColumnsIndicesForeignKeysBySettingNameOfPlaceholderItems) {
    data->tester->renewDocument();
    SyntheticMySQLModel model(data->tester.get());

    db_mysql_TableRef table = model.table;
    table->name("table");
    table->columns().remove_all();
    table->indices().remove_all();
    table->foreignKeys().remove_all();

    MySQLTableEditorBE editor(table);

    EXPECT_EQ(0U, table->columns().count()) << "add column";
    ((bec::TableColumnsListBE *)editor.get_columns())->set_field(0, 0, "newcol");
    ((bec::TableColumnsListBE *)editor.get_columns())->set_field(0, 1, "int(11)");
    EXPECT_EQ(1U, table->columns().count()) << "add column";

    editor.get_indexes()->select_index(0);
    EXPECT_EQ(0U, table->indices().count()) << "add index";
    editor.get_indexes()->set_field(0, 0, "index");
    EXPECT_EQ(1U, table->indices().count()) << "add index";

    editor.get_fks()->select_fk(0);
    EXPECT_EQ(0U, table->foreignKeys().count()) << "add fk";
    editor.get_fks()->set_field(0, 0, "newfk");
    EXPECT_EQ(1U, table->foreignKeys().count()) << "add fk";
  }
}
