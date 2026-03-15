/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

#pragma once

#include "grtdb/editor_table.h"
#include "grts/structs.studio.physical.h"
#include "grts/structs.db.mysql.h"

#include "mysql_support_backend_public_interface.h"

class MySQLTableEditorBE;

namespace mforms {
  class View;
};

class MYSQLWBMYSQLSUPPORTBACKEND_PUBLIC_FUNC MySQLTableColumnsListBE : public ::bec::TableColumnsListBE {
public:
  enum MySQLColumnListColumns {
    IsAutoIncrement = bec::TableColumnsListBE::LastColumn,
    IsAutoIncrementable,
    IsGenerated,
    GeneratedStorageType,
    GeneratedExpression
  };

  virtual auto set_field(const ::bec::NodeId &node, ColumnId column, const std::string &value) -> bool;
  virtual auto set_field(const ::bec::NodeId &node, ColumnId column, ssize_t value) -> bool;

  MySQLTableColumnsListBE(MySQLTableEditorBE *owner);

  virtual auto activate_popup_item_for_nodes(const std::string &name, const std::vector<bec::NodeId> &orig_nodes) -> bool;
  virtual auto get_popup_items_for_nodes(const std::vector<bec::NodeId> &nodes) -> bec::MenuItemList;

protected:
  // for internal use only
  virtual auto get_field_grt(const ::bec::NodeId &node, ColumnId column, ::grt::ValueRef &value) -> bool;
};

class MYSQLWBMYSQLSUPPORTBACKEND_PUBLIC_FUNC MySQLTableIndexListBE : public bec::IndexListBE {
public:
  enum Columns { StorageType = bec::IndexListBE::LastColumn, RowBlockSize, Parser };

  MySQLTableIndexListBE(MySQLTableEditorBE *owner);

  virtual auto set_field(const ::bec::NodeId &node, ColumnId column, const std::string &value) -> bool;
  virtual auto set_field(const ::bec::NodeId &node, ColumnId column, ssize_t value) -> bool;

protected:
  virtual auto get_field_grt(const ::bec::NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;
};

class MYSQLWBMYSQLSUPPORTBACKEND_PUBLIC_FUNC MySQLTablePartitionTreeBE : public bec::TreeModel {
  MySQLTableEditorBE *_owner;

public:
  // all columns are string, including min/maxRows
  enum Columns { Name, Value, MinRows, MaxRows, DataDirectory, IndexDirectory, Comment };

  MySQLTablePartitionTreeBE(MySQLTableEditorBE *owner);

  virtual auto refresh() -> void {};

  virtual auto count_children(const ::bec::NodeId &parent) -> size_t;
  virtual auto get_child(const ::bec::NodeId &parent, size_t index) -> ::bec::NodeId;

  virtual auto set_field(const ::bec::NodeId &node, ColumnId column, const std::string &value) -> bool;

protected:
  virtual auto get_field_grt(const ::bec::NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;
  virtual auto get_field_type(const ::bec::NodeId &node, ColumnId column) -> grt::Type;

  db_mysql_PartitionDefinitionRef get_definition(const ::bec::NodeId &node);
};

class MySQLTriggerPanel;

class MYSQLWBMYSQLSUPPORTBACKEND_PUBLIC_FUNC MySQLTableEditorBE : public ::bec::TableEditorBE {
  friend class MySQLTriggerPanel;

public:
  MySQLTableEditorBE(db_mysql_TableRef table);
  virtual ~MySQLTableEditorBE();

  virtual auto refresh_live_object() -> void;
  virtual auto commit_changes() -> void;

  virtual auto get_columns() -> MySQLTableColumnsListBE * {
    return &_columns;
  }
  virtual auto get_indexes() -> MySQLTableIndexListBE * {
    return &_indexes;
  }

  virtual auto get_index_types() -> std::vector<std::string>;
  virtual auto get_index_storage_types() -> std::vector<std::string>;
  virtual auto get_fk_action_options() -> std::vector<std::string>;

  // table options
  virtual auto set_table_option_by_name(const std::string &name, const std::string &value) -> void;
  virtual auto get_table_option_by_name(const std::string &name) -> std::string;
  auto get_engines_list() -> std::vector<std::string>;
  auto engine_supports_foreign_keys() -> bool;

  virtual auto check_column_referenceable_by_fk(const db_ColumnRef &column1, const db_ColumnRef &column2) -> bool;

  auto load_trigger_sql() -> void;

  // triggers
  auto get_trigger_panel() -> mforms::View *;
  auto add_trigger(const std::string &timing, const std::string &event) -> void;

  virtual auto can_close() -> bool;

  // partitioning
  auto set_partition_type(const std::string &type) -> bool;
  auto get_partition_type() -> std::string;

  auto set_partition_expression(const std::string &expr) -> void;
  auto get_partition_expression() -> std::string;

  auto set_partition_count(int count) -> void;
  auto get_partition_count() -> int;

  auto set_subpartition_type(const std::string &type) -> bool;
  auto get_subpartition_type() -> std::string;

  auto set_subpartition_expression(const std::string &expr) -> bool;
  auto get_subpartition_expression() -> std::string;

  auto subpartition_count_allowed() -> bool;
  auto set_subpartition_count(int count) -> void;
  auto get_subpartition_count() -> int;

  auto get_partitions() -> MySQLTablePartitionTreeBE * {
    return &_partitions;
  }

  // whether partitions and sub partitions will be defined by the user or not
  // if false, only count is needed otherwise the partitions list must be defined
  auto set_explicit_partitions(bool flag) -> void;
  auto get_explicit_partitions() -> bool;
  auto set_explicit_subpartitions(bool flag) -> void;
  auto get_explicit_subpartitions() -> bool;

  virtual auto create_stub_table(const std::string &schema, const std::string &table) -> db_TableRef;

protected:
  MySQLTableColumnsListBE _columns;
  MySQLTablePartitionTreeBE _partitions;
  MySQLTableIndexListBE _indexes;
  MySQLTriggerPanel *_trigger_panel;
  bool _updating_triggers;

  auto reset_partition_definitions(int parts, int subparts) -> void;
};
