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

#include "wb_test_helpers.h"
#include "context.h"
#include "grt_test_helpers.h"
#include "gtest/gtest.h"

#include "model/wb_history_tree.h"
#include "grtdb/db_object_helpers.h"
#include "../../../../../../plugins/db.mysql.editors/backend/mysql_table_editor.h"

using namespace wb;

namespace {
struct WbUndoEditorsData {
  std::unique_ptr<MySqlStudioTester> tester;
  grt::UndoManager* um = nullptr;
  OverviewBE* overview = nullptr;
  db_SchemaRef schema;
  db_mgmt_RdbmsRef rdbms;

  size_t lastUndoStackSize = 0;
  size_t lastRedoStackSize = 0;

  #include "wb_undo_helpers.h"

};

class Undo_Tests_for_EditorsTest : public ::testing::Test {
protected:
  static std::unique_ptr<WbUndoEditorsData> data;

  static void SetUpTestSuite() {
    data = std::make_unique<WbUndoEditorsData>();
    data->tester.reset(new MySqlStudioTester());
    data->um = grt::GRT::get()->get_undo_manager();
    data->overview = WBContextUI::get()->get_physical_overview();

    WBContextUI::get()->set_active_form(data->overview);

    std::string dataDir = testing::Context::get().tmpDataDir();
    bool flag = data->tester->wb->open_document(dataDir + "/studio/undo_test_model2.mwb");
    EXPECT_TRUE(flag) << "open_document";

    EXPECT_EQ(data->tester->getCatalog()->schemata().count(), 1U) << "schemas";

    data->schema = data->tester->getCatalog()->schemata()[0];
    data->rdbms = db_mgmt_RdbmsRef::cast_from(grt::GRT::get()->unserialize(dataDir + "/res/mysql_rdbms_info.xml"));

    // make sure the loaded model contains expected number of things
    EXPECT_EQ(data->schema->tables().count(), 2U) << "tables";
    EXPECT_EQ(data->schema->tables()[0]->columns()->count(), 2U) << "tables";

    EXPECT_EQ(data->um->get_undo_stack().size(), 0U) << "undo stack is empty";
  }

  static void TearDownTestSuite() {
    data.reset();
  }

};

std::unique_ptr<WbUndoEditorsData> Undo_Tests_for_EditorsTest::data;

TEST_F(Undo_Tests_for_EditorsTest, Editors_general) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  const std::string old_name = table->name();
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  // Rename
  be->set_name("new_name");
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(table->name(), "new_name") << "Table name set to new value";
  data->checkUndo();
  EXPECT_EQ(table->name(), old_name) << "Table name change undo failed";

  // Change Comment
  be->set_comment("comment");
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(be->get_comment(), "comment") << "Table comment set to new value";
  data->checkUndo();
  EXPECT_EQ(be->get_comment(), "test_table_comment") << "Table comment undo failed";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_general) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  // Change Collation
  std::string old_value = be->get_table_option_by_name("CHARACTER SET - COLLATE");
  std::vector<std::string> collations(be->get_charset_collation_list());
  if (collations.size() > 2) {
    size_t new_coll_idx = collations.size() / 2;
    if (collations[new_coll_idx] != old_value) {
      be->set_table_option_by_name("CHARACTER SET - COLLATE", collations[new_coll_idx]);
      data->checkOnlyOneUndoAdded();
      EXPECT_NE(old_value, be->get_table_option_by_name("CHARACTER SET - COLLATE"))
        << "Table collation change failed";
      data->checkUndo();
      EXPECT_EQ(old_value, be->get_table_option_by_name("CHARACTER SET - COLLATE"))
        << "Table collation change undo failed";
    } else
      FAIL() << "Cannot test collation";
  } else
    FAIL() << "Cannot test collation. list empty";

  // Change Engine
  old_value = be->get_table_option_by_name("ENGINE");
  std::vector<std::string> engines(be->get_engines_list());
  if (engines.size() > 2) {
    size_t new_eng_idx = engines.size() / 2;
    if (engines[new_eng_idx] != old_value) {
      be->set_table_option_by_name("ENGINE", engines[new_eng_idx]);
      data->checkOnlyOneUndoAdded();
      EXPECT_NE(old_value, be->get_table_option_by_name("ENGINE"))
        << "Table engine change failed";
      data->checkUndo();
      EXPECT_EQ(old_value, be->get_table_option_by_name("ENGINE"))
        << "Table engine change undo failed";
    } else
      FAIL() << "Cannot test engine change";
  } else
    FAIL() << "Cannot test engine change. list empty";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_add_column) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  // Add
  const size_t existing_cols_nr = table->columns().count();
  EXPECT_EQ(existing_cols_nr, 2U) << "Table has extra columns before test";
  be->add_column("test_column");
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(table->columns()[existing_cols_nr]->name(), "test_column") << "Column was not added";
  data->checkUndo();
  EXPECT_EQ(table->columns().count(), existing_cols_nr) << "Table has extra columns after undo";
  data->checkRedo();
  EXPECT_EQ(table->columns()[existing_cols_nr]->name(), "test_column") << "Column was not added after redo";
  data->checkUndo();
}

TEST_F(Undo_Tests_for_EditorsTest, Table_rename_column) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  const std::string old_name = table->columns()[0]->name();
  // Rename
  bec::TableColumnsListBE* cols = be->get_columns();
  cols->set_field(bec::NodeId(0), MySQLTableColumnsListBE::Name, std::string("abyrvalg"));
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(table->columns()[0]->name(), "abyrvalg") << "Column was not renamed";
  data->checkUndo();
  EXPECT_EQ(table->columns()[0]->name(), old_name) << "Column name change undo failed";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_column_type_change) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  bec::TableColumnsListBE* cols = be->get_columns();

  // Change Type
  // get original type
  const std::string old_value = table->columns()[0]->formattedType();

  // change type via BE, adding undo action
  cols->set_field(bec::NodeId(0), MySQLTableColumnsListBE::Type, std::string("FLOAT"));
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(table->columns()[0]->formattedType(), "FLOAT") << "Column type was not set to FLOAT";
  // Store new value to check redo later
  const std::string new_value = table->columns()[0]->formattedType();
  data->checkUndo();

  // verify that type was reverted to original
  EXPECT_EQ(table->columns()[0]->formattedType(), old_value) << "Column type change undo failed";

  data->checkRedo();
  EXPECT_EQ(table->columns()[0]->formattedType(), new_value) << "Column type was not reset to " + old_value + " after redo";

  // revert all and do a last check
  data->checkUndo();
  EXPECT_EQ(table->columns()[0]->formattedType(), old_value) << "Column type change undo after redo failed";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_column_flag_change) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  bec::TableColumnsListBE* cols = be->get_columns();

  // Change Flag
  int flag = cols->get_column_flag(bec::NodeId(0), "UNSIGNED");
  cols->set_column_flag(bec::NodeId(0), "UNSIGNED", !flag);
  data->checkOnlyOneUndoAdded();
  EXPECT_NE(flag, cols->get_column_flag(bec::NodeId(0), "UNSIGNED")) << "Column flag was not changed";

  data->checkUndo();
  EXPECT_EQ(flag, cols->get_column_flag(bec::NodeId(0), "UNSIGNED")) << "Column flag change undo failed";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_column_NN_change) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  bec::TableColumnsListBE* cols = be->get_columns();

  // Change NN, get original flag and revert it
  ssize_t nn = -1;
  cols->get_field(bec::NodeId(0), MySQLTableColumnsListBE::IsNotNull, nn);
  cols->set_field(bec::NodeId(0), MySQLTableColumnsListBE::IsNotNull, !nn);
  data->checkOnlyOneUndoAdded();

  // get value and check that it was changed, they should match
  ssize_t nn2 = -1;
  cols->get_field(bec::NodeId(0), MySQLTableColumnsListBE::IsNotNull, nn2);
  EXPECT_EQ(static_cast<bool>(nn2), !nn) << "NN was not set";

  data->checkUndo();
  cols->get_field(bec::NodeId(0), MySQLTableColumnsListBE::IsNotNull, nn2);
  EXPECT_EQ(nn2, nn) << "NN was not set back on undo";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_column_default_change) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  bec::TableColumnsListBE* cols = be->get_columns();

  std::string value;
  // Change Default
  cols->get_field(bec::NodeId(0), MySQLTableColumnsListBE::Default, value);
  cols->set_field(bec::NodeId(0), MySQLTableColumnsListBE::Default, "2");
  data->checkOnlyOneUndoAdded();

  data->checkUndo();
  std::string value2;
  cols->get_field(bec::NodeId(0), MySQLTableColumnsListBE::Default, value2);
  EXPECT_EQ(value, value2) << "Default value undo failed";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_column_remove) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  const size_t ncols = table->columns().count();
  EXPECT_EQ(ncols, 2U) << "tables";
  // Remove
  be->remove_column(bec::NodeId(1));
  data->checkOnlyOneUndoAdded();
  // In file we had 2 columns and one index(PRIMARY KEY). We removed column on which PK was.
  data->checkUndo();
  EXPECT_EQ(table->columns().count(), ncols) << "Table column remove undo failed";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_index_add) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  const size_t nidx = table->indices().count();
  // Add
  be->add_index("idx");
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(table->indices()[nidx]->name(), "idx") << "Index was not added";
  data->checkUndo();
  EXPECT_EQ(table->indices().count(), nidx) << "Index add undo failed";
  data->checkRedo();
  EXPECT_EQ(table->indices()[nidx]->name(), "idx") << "Index add redo failed";
  data->checkUndo();
  EXPECT_EQ(table->indices().count(), nidx) << "Index add undo2 failed";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_index_rename) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  const size_t nidx = table->indices().count();
  be->add_index("idx");
  data->checkOnlyOneUndoAdded();

  // Rename
  MySQLTableIndexListBE* indices = be->get_indexes();
  indices->set_field(bec::NodeId((int)nidx), bec::IndexListBE::Name, "idx_new");
  data->checkOnlyOneUndoAdded();
  data->checkUndo();
  EXPECT_EQ(table->indices()[nidx]->name(), "idx") << "Index rename undo failed";

  data->checkUndo(); // undo index addition
}

TEST_F(Undo_Tests_for_EditorsTest, Table_index_remove) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  const size_t nidx = table->indices().count();
  be->add_index("idx");
  data->checkOnlyOneUndoAdded();

  // Remove
  be->remove_index(bec::NodeId((int)nidx), false);
  data->checkOnlyOneUndoAdded();
  data->checkUndo();
  EXPECT_EQ(table->indices().count(), nidx + 1) << "Index removal undo failed";

  data->checkUndo();
}

TEST_F(Undo_Tests_for_EditorsTest, Table_index_type_change) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  // set engine to something that supports FULLTEXT and RTREE index options
  table->tableEngine("MyISAM");

  const size_t nidx = table->indices().count();
  be->add_index("idx");
  data->checkOnlyOneUndoAdded();

  MySQLTableIndexListBE* indices = be->get_indexes();

  indices->select_index(bec::NodeId((int)nidx));

  indices->set_field(bec::NodeId((int)nidx), bec::IndexListBE::Type, "FULLTEXT");
  data->checkOnlyOneUndoAdded();
  data->checkUndo();
  EXPECT_EQ(table->indices()[nidx]->indexType(), "INDEX") << "Index type change undo failed";

  data->checkUndo();
}

TEST_F(Undo_Tests_for_EditorsTest, Table_index_storage_change) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  const size_t nidx = table->indices().count();
  be->add_index("idx");
  data->checkOnlyOneUndoAdded();

  MySQLTableIndexListBE* indices = be->get_indexes();

  indices->select_index(bec::NodeId((int)nidx));

  // Change Storage Type
  indices->set_field(bec::NodeId((int)nidx), MySQLTableIndexListBE::StorageType, "RTREE");
  data->checkOnlyOneUndoAdded();
  data->checkUndo();
  EXPECT_EQ(*table->indices()[nidx]->indexKind(), "") << "Index storage type change undo failed";

  data->checkUndo();
}

TEST_F(Undo_Tests_for_EditorsTest, Table_index_column_add) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));

  const size_t nidx = table->indices().count();
  be->add_index("idx");
  data->checkOnlyOneUndoAdded();

  MySQLTableIndexListBE* indices = be->get_indexes();
  indices->select_index(bec::NodeId((int)nidx));

  bec::IndexColumnsListBE* icols = indices->get_columns();
  icols->set_column_enabled(bec::NodeId(icols->count() - 1), true);
  data->checkOnlyOneUndoAdded();
  data->checkUndo();
  EXPECT_FALSE(icols->get_column_enabled(bec::NodeId(bec::NodeId((int)table->columns().count() - 1)))) << "Undo adding column to index failed";
  data->checkRedo();
  data->checkUndo();

  data->checkUndo();
}

TEST_F(Undo_Tests_for_EditorsTest, Table_FK_add) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));
  const size_t nfks = table->foreignKeys().count();

  be->add_fk("fk1");
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(table->foreignKeys()[nfks]->name(), "fk1") << "FK was not added";
  data->checkUndo();
  data->checkRedo();
  EXPECT_EQ(table->foreignKeys()[nfks]->name(), "fk1") << "adding FK redo failed";
  data->checkUndo();
}

TEST_F(Undo_Tests_for_EditorsTest, Table_FK_remove) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));
  const size_t nfks = table->foreignKeys().count();

  be->add_fk("fk1");
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(nfks + 1, table->foreignKeys().count()) << "FK was added";
  be->remove_fk(bec::NodeId((int)nfks));
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(nfks, table->foreignKeys().count()) << "FK was removed";
  data->checkUndo();
  EXPECT_EQ(nfks + 1, table->foreignKeys().count()) << "FK remove undo";

  data->checkUndo();
}

TEST_F(Undo_Tests_for_EditorsTest, Table_FK_rename) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));
  const size_t nfks = table->foreignKeys().count();
  bec::FKConstraintListBE* fks = be->get_fks();

  be->add_fk("fk1");
  data->checkOnlyOneUndoAdded();

  fks->set_field(bec::NodeId((int)nfks), bec::FKConstraintListBE::Name, "fk_new");
  data->checkOnlyOneUndoAdded();
  data->checkUndo();
  EXPECT_EQ(table->foreignKeys()[nfks]->name(), "fk1") << "Index rename undo failed";
  data->checkRedo();
  data->checkUndo();

  data->checkUndo();
}

TEST_F(Undo_Tests_for_EditorsTest, Table_FK_set_ref_table) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));
  const size_t nfks = table->foreignKeys().count();
  bec::FKConstraintListBE* fks = be->get_fks();

  be->add_fk("fk1");
  data->checkOnlyOneUndoAdded();
  fks->select_fk(bec::NodeId((int)nfks));

  fks->set_field(bec::NodeId((int)nfks), bec::FKConstraintListBE::RefTable, "table2");
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(fks->get_selected_fk()->referencedTable()->name(), "table2") << "FK ref table was not set";
  data->checkUndo();

  data->checkUndo();
}

TEST_F(Undo_Tests_for_EditorsTest, Table_FK_set_ref_column) {
  db_mysql_TableRef table(db_mysql_TableRef::cast_from(data->schema->tables()[0]));
  db_mysql_TableRef table2(db_mysql_TableRef::cast_from(data->schema->tables()[1]));
  std::unique_ptr<MySQLTableEditorBE> be(new MySQLTableEditorBE(table));
  const size_t nfks = table->foreignKeys().count();
  bec::FKConstraintListBE* fks = be->get_fks();

  be->add_fk("fk1");
  data->checkOnlyOneUndoAdded();
  fks->select_fk(bec::NodeId((int)nfks));

  fks->set_field(bec::NodeId((int)nfks), bec::FKConstraintListBE::RefTable, "table2");
  data->checkOnlyOneUndoAdded();
  EXPECT_EQ(fks->get_selected_fk()->referencedTable()->name(), "table2") << "FK ref table was not set";

  bec::FKConstraintColumnsListBE* fkcols = fks->get_columns();
  fkcols->set_field(bec::NodeId(0), bec::FKConstraintColumnsListBE::Enabled, 1);
  data->checkOnlyOneUndoAdded();
  fkcols->set_field(bec::NodeId(0), bec::FKConstraintColumnsListBE::RefColumn,
                    table2->columns()[table2->columns()->count() - 1]->name());
  data->checkOnlyOneUndoAdded();

  // TODO: Change On Update
  // TODO: Change On Delete

  data->checkUndo(); // set column
  data->checkUndo(); // set ref column

  data->checkUndo(); // set ref table

  data->checkUndo(); // add fk
}

TEST_F(Undo_Tests_for_EditorsTest, Table_inserts) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_partitioning) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Table_options) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, View) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Routine) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Routine_group) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Schema) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Relationship) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Image) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Note) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Stored_script) {
  GTEST_SKIP() << "not implemented";
}

TEST_F(Undo_Tests_for_EditorsTest, Stored_note) {
  GTEST_SKIP() << "not implemented";
}


}
