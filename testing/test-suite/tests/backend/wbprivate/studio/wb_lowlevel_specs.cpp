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

// High-level testing for MySqlStudio.
// This tests WBContext, which will test the integration of all components.

#include "wb_test_helpers.h"
#include "studio/wb_overview.h"
#include "base/util_functions.h"
#include "wbcanvas/studio_physical_tablefigure_impl.h"

#include "grtdb/db_object_helpers.h"

#include "grt_test_helpers.h"
#include "wbcanvas/table_figure.h"

using namespace wb;

#ifndef _MSC_VER
#include <signal.h>

void signal_handler(int sig) {
  printf("Exiting after signal[%d] was trapped\n", sig);
  exit(0);
}
#endif

#include "gtest/gtest.h"

namespace {

  struct TestData {
    std::unique_ptr<MySqlStudioTester> tester;
  };

} // anonymous namespace

class LowLevelTestsForMySqlStudioContextTest : public ::testing::Test {
protected:
  TestData *data = new TestData();

  void SetUp() override {
    data->tester.reset(new MySqlStudioTester());
    data->tester->initializeRuntime();
#ifndef _MSC_VER
    if (signal(SIGSEGV, signal_handler) == SIG_ERR) {
      printf("Failed to setup the signal handler\n");
    }
  #endif
  }

  void TearDown() override {
    delete data;
  }
};

TEST_F(LowLevelTestsForMySqlStudioContextTest, StoredConnectionsTest) {
  GTEST_SKIP() << "need investigate why connection is not avaiable";
  EXPECT_TRUE(data->tester->wb->get_root()->rdbmsMgmt()->storedConns().is_valid());

  // We cannot check the exact number because on Windows, if there are no server instances yet,
  // instances and connections are created automatically from all installed servers.
  // So we can't know in advance how many connections we will have (but at least 1, that in the test
  // connection file).
  EXPECT_TRUE(data->tester->wb->get_root()->rdbmsMgmt()->storedConns().count() > 0);

  EXPECT_TRUE(data->tester->wb->get_root()->rdbmsMgmt()->rdbms().get(0)->drivers().count() > 0);

  EXPECT_TRUE(data->tester->wb->get_root()->rdbmsMgmt()->storedConns().get(0)->driver().is_valid());
}

TEST_F(LowLevelTestsForMySqlStudioContextTest, CheckIfCreatingAFkBetween2TablesWillCreateTheConnection) {
  data->tester->wb->new_document();
  data->tester->addView();

  db_mysql_TableRef table1(data->tester->addTableFigure("table1", 10, 10));
  db_mysql_TableRef table2(data->tester->addTableFigure("table2", 10, 100));

  EXPECT_EQ(data->tester->getPview()->figures().count(), 2U);
  EXPECT_EQ(data->tester->getPview()->connections().count(), 0U);

  db_mysql_ColumnRef column(grt::Initialized);
  column->owner(table1);
  column->name("id1");
  column->setParseType("int", data->tester->getRdbms()->simpleDatatypes());
  // bec::ColumnHelper::parse_column_type(data->tester->getRdbms(), data->tester->getCatalog()->userDatatypes(), "int",
  // column);
  table1->columns().insert(column);

  column = db_mysql_ColumnRef(grt::Initialized);
  column->owner(table1);
  column->name("col1");
  column->setParseType("varchar(100)", data->tester->getRdbms()->simpleDatatypes());
  //  bec::ColumnHelper::parse_column_type(data->tester->getRdbms(), data->tester->getCatalog()->userDatatypes(),
  //  "varchar(100)", column);
  table1->columns().insert(column);
  table1->addPrimaryKeyColumn(column);
  // bec::TableHelper::make_primary_key(table1, column, true);

  column = db_mysql_ColumnRef(grt::Initialized);
  column->owner(table2);
  column->name("id2");
  column->setParseType("int", data->tester->getRdbms()->simpleDatatypes());
  // bec::ColumnHelper::parse_column_type(data->tester->getRdbms(), data->tester->getCatalog()->userDatatypes(), "int",
  // column);
  table2->columns().insert(column);

  column = db_mysql_ColumnRef(grt::Initialized);
  column->owner(table2);
  column->name("col2");
  column->setParseType("varchar(100)", data->tester->getRdbms()->simpleDatatypes());
  // bec::ColumnHelper::parse_column_type(data->tester->getRdbms(), data->tester->getCatalog()->userDatatypes(),
  // "varchar(100)", column);
  table2->columns().insert(column);
  table2->addPrimaryKeyColumn(column);
  // bec::TableHelper::make_primary_key(table2, column, true);

  bec::TableHelper::create_foreign_key_to_table(table1, table2, true, true, true, true, data->tester->getRdbms(),
                                                grt::DictRef(true), grt::DictRef(true));

  EXPECT_TRUE(table1->foreignKeys().count() > 0);
  EXPECT_TRUE(table2->foreignKeys().count() == 0);

  grt::ListRef<model_Connection> tmp(data->tester->getPview()->connections());

  data->tester->flushUntil(3, std::bind(&grt::ListRef<model_Connection>::count, tmp), 1);

  EXPECT_EQ(data->tester->getPview()->connections().count(), 1U);
  data->tester->wb->close_document();
  data->tester->wb->close_document_finish();
}

TEST_F(LowLevelTestsForMySqlStudioContextTest, BugCheckIfCreatingARecursiveFkWillCreateTheConnection) {
  data->tester->wb->new_document();
  data->tester->addView();

  db_mysql_TableRef table = data->tester->addTableFigure("table", 10, 10);

  EXPECT_EQ(data->tester->getPview()->figures().count(), 1U);
  EXPECT_EQ(data->tester->getPview()->connections().count(), 0U);

  db_mysql_ColumnRef column(grt::Initialized);
  column->owner(table);
  column->name("id");
  table->columns().insert(column);

  column = db_mysql_ColumnRef(grt::Initialized);
  column->owner(table);
  column->name("col2");
  table->columns().insert(column);

  table->addPrimaryKeyColumn(column);
  // bec::TableHelper::make_primary_key(table, column, true);

  bec::TableHelper::create_foreign_key_to_table(table, table, true, true, true, true, data->tester->getRdbms(),
                                                grt::DictRef(true), grt::DictRef(true));

  EXPECT_TRUE(table->foreignKeys().count() > 0);

  grt::ListRef<model_Connection> tmp(data->tester->getPview()->connections());
  data->tester->flushUntil(3, std::bind(&grt::ListRef<model_Connection>::count, tmp), 1);

  EXPECT_EQ(data->tester->getPview()->connections().count(), 1U);

  data->tester->wb->close_document();
  data->tester->wb->close_document_finish();
}

TEST_F(LowLevelTestsForMySqlStudioContextTest, BugCheckIfDeletingAnObjectWithPrivilegesWillDeleteThePrivsToo) {
  data->tester->createNewDocument();

  WBComponentPhysical *phys = data->tester->wb->get_component<WBComponentPhysical>();
  EXPECT_EQ(data->tester->getPmodel()->catalog()->roles().count(), 5U);
  phys->add_new_role(data->tester->getPmodel());
  phys->add_new_role(data->tester->getPmodel());

  db_SchemaRef schema(data->tester->getPmodel()->catalog()->schemata()[0]);

  phys->add_new_db_table(schema);
  phys->add_new_db_table(schema);

  EXPECT_EQ(schema->tables().count(), 2U);
  EXPECT_EQ(data->tester->getPmodel()->catalog()->roles().count(), 2U + 5);

  // add some privs to the table
  db_RoleRef role(data->tester->getPmodel()->catalog()->roles().get(5));
  db_TableRef table(schema->tables().get(0));

  db_TableRef table2(schema->tables().get(1));

  db_RolePrivilegeRef priv(grt::Initialized);

  priv->databaseObject(table);
  priv->databaseObjectType(table.class_name());
  priv->databaseObjectName(table->name());
  priv->privileges().insert("CREATE");
  priv->privileges().insert("DELETE");

  role->privileges().insert(priv);

  db_RolePrivilegeRef priv2(grt::Initialized);

  priv2->databaseObject(table2);
  priv2->databaseObjectType(table.class_name());
  priv2->databaseObjectName(table->name());
  priv2->privileges().insert("CREATE");
  priv2->privileges().insert("INSERT");

  role->privileges().insert(priv2);

  EXPECT_EQ(role->privileges().count(), 2U);

  // delete the 1st table
  phys->delete_db_object(table);

  EXPECT_EQ(data->tester->getPmodel()->catalog()->schemata()[0]->tables().count(), 1U);
  EXPECT_EQ(data->tester->getPmodel()->catalog()->roles().count(), 2U + 5);

  EXPECT_EQ(role->privileges().count(), 1U);

  EXPECT_TRUE(role->privileges().get(0)->databaseObject() == table2);

  data->tester->wb->close_document();
  data->tester->wb->close_document_finish();
}

TEST_F(LowLevelTestsForMySqlStudioContextTest, BugUndoDropTableWillNotResetTableFigure) {
  db_TableRef table;

  data->tester->wb->open_document("data/studio/2tables_1fk.mwb");
  studio_DocumentRef doc = data->tester->wb->get_document();
  EXPECT_TRUE(doc.is_valid());

  data->tester->openAllDiagrams();

  EXPECT_EQ(data->tester->getCatalog()->schemata().count(), 1U);

  std::list<db_DatabaseObjectRef> objects;
  EXPECT_EQ(data->tester->getSchema()->tables().count(), 2U);
  objects.push_back(grt::find_named_object_in_list(data->tester->getSchema()->tables(), "table1"));
  EXPECT_TRUE(objects.front().is_valid());
  EXPECT_EQ(*objects.front()->name(), "table1");
  EXPECT_EQ(doc->physicalModels()[0]->diagrams().count(), 1U);
  data->tester->interactivePlaceDbObjects(10, 150, objects);

  data->tester->flushUntil(2);
  EXPECT_EQ(data->tester->getPview()->figures().count(), 1U);

  grt::GRT::get()->get_undo_manager()->undo();

  EXPECT_EQ(data->tester->getPview()->figures().count(), 0U);
  data->tester->wb->close_document();
  data->tester->wb->close_document_finish();
}
