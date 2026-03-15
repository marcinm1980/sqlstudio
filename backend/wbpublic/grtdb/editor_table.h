/*
 * Copyright (c) 2007, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "grtdb/editor_dbobject.h"
#include "grt/tree_model.h"

#include "grtdb/charset_list.h"

#include "grts/structs.db.mgmt.h"

#include "wbpublic_public_interface.h"

#define TableEditorBE_VERSION 2

class Recordset;
using RecordsetRef = std::shared_ptr<Recordset>;
class Recordset_table_inserts_storage;
using RecordsetTableInsertsStorageRef = std::shared_ptr<Recordset_table_inserts_storage>;

namespace mforms {
  class Form;
  class View;
  class Box;
  class ContextMenu;
  class GridView;
} // namespace mforms

namespace bec {

  class TableEditorBE;
  class IndexListBE;
  class FKConstraintListBE;

  // ColumnNamesSet sets alias for type which is used to return a set of all column
  // names from all tables in the schema. This type is used by TableColumnsListBE
  using ColumnNamesSet = std::set<std::string>;

  class WBPUBLICBACKEND_PUBLIC_FUNC TableColumnsListBE : public ListModel {
  public:
    enum ColumnListColumns {
      Name,
      Type,
      IsPK,
      IsNotNull,
      IsUnique,
      IsBinary,
      IsUnsigned,
      IsZerofill,
      Flags,
      Default,
      CharsetCollation,
      Charset,
      Collation,
      Comment,
      HasCharset,

      LastColumn
    };

    TableColumnsListBE(TableEditorBE *owner);

    auto get_row(const NodeId &node, std::string &name, std::string &type, bool &ispk, bool &notnull, bool &isunique,
                 bool &isbinary, bool &isunsigned, bool &iszerofill, std::string &flags, std::string &defvalue,
                 std::string &charset, std::string &collation, std::string &comment) -> bool;

    virtual auto get_field_icon(const NodeId &node, size_t column, IconSize size) -> IconId;

    virtual auto refresh() -> void;
    virtual auto count() -> size_t;
    auto real_count() -> size_t;

    auto set_column_type(const NodeId &node, const GrtObjectRef &type) -> bool;

    auto set_column_type_from_string(db_ColumnRef &column, const std::string &type) -> bool;

    virtual auto set_field(const NodeId &node, ColumnId column, const std::string &value) -> bool;
    virtual auto set_field(const NodeId &node, ColumnId column, ssize_t value) -> bool;

    /**
     * This is needed so we can reset placeholder info when then user cancelled the edit operation.
     * Used in gtk frontend.
     */
    auto reset_placeholder() -> void;

    virtual auto reorder(const NodeId &node, size_t nindex) -> void;
    auto reorder_many(const std::vector<std::size_t> &rows, std::size_t nindex) -> void;

    auto get_datatype_flags(const ::bec::NodeId &node, bool all = false) -> std::vector<std::string>;
    auto set_column_flag(const ::bec::NodeId &node, const std::string &flag_name, int is_set) -> bool;
    auto get_column_flag(const ::bec::NodeId &node, const std::string &flag_name) -> int;

    virtual auto quote_value_if_needed(const db_ColumnRef &column, const std::string &value) -> std::string;
    virtual auto get_popup_items_for_nodes(const std::vector<NodeId> &nodes) -> MenuItemList;
    virtual auto activate_popup_item_for_nodes(const std::string &name, const std::vector<NodeId> &nodes) -> bool;

    virtual auto can_delete_node(const NodeId &node) -> bool;
    virtual auto delete_node(const NodeId &node) -> bool;

    virtual auto get_datatype_names() -> std::vector<std::string>;

    auto get_column_names_completion_list() const -> ColumnNamesSet;

    auto has_unique_index(const db_ColumnRef &col) -> bool;
    auto make_unique_index(const db_ColumnRef &col, bool flag) -> bool;

  protected:
    TableEditorBE *_owner;
    size_t _editing_placeholder_row;

    auto update_primary_index_order() -> void;

    // for internal use only
    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC IndexColumnsListBE : public ListModel {
  public:
    enum IndexColumnsListColumns { Name, Descending, Length, OrderIndex };

    IndexColumnsListBE(IndexListBE *owner);

    virtual auto refresh() -> void;
    virtual auto count() -> size_t;

    auto set_column_enabled(const NodeId &node, bool flag) -> void;
    auto get_column_enabled(const NodeId &node) -> bool;

    virtual auto set_field(const NodeId &node, ColumnId column, ssize_t value) -> bool;
    virtual auto set_field(const NodeId &node, ColumnId column, const std::string &value) -> bool;

    auto get_max_order_index() -> size_t;

  protected:
    IndexListBE *_owner;

    // for internal use only
    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;

    auto get_index_column(const db_ColumnRef &column) -> db_IndexColumnRef;
    auto get_index_column_index(const db_ColumnRef &column) -> size_t;
    auto set_index_column_order(const db_IndexColumnRef &column, size_t order) -> void;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC IndexListBE : public ListModel {
    friend class IndexColumnsListBE;
    friend class TableEditorBE;

  public:
    enum IndexListColumns { Name, Type, Visible, Comment, LastColumn };

    IndexListBE(TableEditorBE *owner);

    virtual auto refresh() -> void;
    virtual auto count() -> size_t;
    auto real_count() -> size_t;

    // for editable lists only
    virtual auto set_field(const NodeId &node, ColumnId column, const std::string &value) -> bool;

    auto get_columns() -> IndexColumnsListBE * {
      return &_column_list;
    }

    auto get_selected_index() -> db_IndexRef;
    auto select_index(const NodeId &node) -> void;

    auto index_editable(const db_IndexRef &index) -> bool;
    auto index_belongs_to_fk(const db_IndexRef &index) -> db_ForeignKeyRef;

    auto get_owner() -> TableEditorBE * {
      return _owner;
    }

    virtual auto get_popup_items_for_nodes(const std::vector<NodeId> &nodes) -> MenuItemList;
    virtual auto activate_popup_item_for_nodes(const std::string &name, const std::vector<NodeId> &nodes) -> bool;

    virtual auto can_delete_node(const NodeId &node) -> bool;
    virtual auto delete_node(const NodeId &node) -> bool;

  protected:
    // for internal use only
    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;

    auto add_column(const db_ColumnRef &column, const db_IndexRef &index = db_IndexRef()) -> NodeId;
    auto remove_column(const NodeId &node) -> void;

  protected:
    IndexColumnsListBE _column_list;
    TableEditorBE *_owner;
    NodeId _selected;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC FKConstraintColumnsListBE : public ListModel {
  public:
    enum FKConstraintColumnsListColumns { Enabled, Column, RefColumn };

    FKConstraintColumnsListBE(FKConstraintListBE *owner);

    virtual auto refresh() -> void;
    virtual auto count() -> size_t;

    auto get_ref_columns_list(const NodeId &node, bool filtered = true) -> std::vector<std::string>;

    // for editable lists only
    virtual auto set_field(const NodeId &node, ColumnId column, const std::string &value) -> bool;
    virtual auto set_field(const NodeId &node, ColumnId column, ssize_t value) -> bool;

    auto set_column_is_fk(const NodeId &node, bool flag) -> bool;
    auto get_fk_column_index(const NodeId &node) -> ssize_t;
    auto get_column_is_fk(const NodeId &node) -> bool;

    auto get_owner() -> FKConstraintListBE * {
      return _owner;
    }

  protected:
    // for internal use only
    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;

    auto set_fk_column_pair(const db_ColumnRef &column, const db_ColumnRef &refcolumn) -> bool;

    // temporary list of referenced columns for each FK column
    // if id is in the map, then it's enabled, if column is nil, it's unset
    // only valid entries will be committed to actual table
    std::map<std::string, db_ColumnRef> _referenced_columns;

    FKConstraintListBE *_owner;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC FKConstraintListBE : public ListModel {
    friend class FKConstraintColumnsListBE;

  public:
    enum FKConstraintListColumns { Name, OnDelete, OnUpdate, RefTable, Comment, Index, ModelOnly };
    FKConstraintListBE(TableEditorBE *owner);

    auto add_column(const db_ColumnRef &column, const db_ColumnRef &refcolumn,
                      const db_ForeignKeyRef &fk = db_ForeignKeyRef()) -> NodeId;

    virtual auto remove_column(const NodeId &node) -> void;

    virtual auto refresh() -> void;
    virtual auto count() -> size_t;
    auto real_count() -> size_t;

    // for editable lists only
    virtual auto set_field(const NodeId &node, ColumnId column, const std::string &value) -> bool;
    virtual auto set_field(const NodeId &node, ColumnId column, ssize_t value) -> bool;

    auto select_fk(const NodeId &node) -> void;
    auto get_selected_fk() -> db_ForeignKeyRef;

    auto get_owner() -> TableEditorBE * {
      return _owner;
    }

    auto get_columns() -> FKConstraintColumnsListBE * {
      return &_column_list;
    }

    virtual auto can_delete_node(const NodeId &node) -> bool;
    virtual auto delete_node(const NodeId &node) -> bool;

    virtual auto get_popup_items_for_nodes(const std::vector<NodeId> &nodes) -> MenuItemList;
    virtual auto activate_popup_item_for_nodes(const std::string &name, const std::vector<NodeId> &nodes) -> bool;

  protected:
    // for internal use only
    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;

  protected:
    FKConstraintColumnsListBE _column_list;
    TableEditorBE *_owner;
    NodeId _selected_fk;
    size_t _editing_placeholder_row;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC TableEditorBE : public DBObjectEditorBE {
  public:
    enum PartialRefreshes { RefreshColumnMoveUp, RefreshColumnMoveDown, RefreshColumnList, RefreshColumnCollation };

    TableEditorBE(const db_TableRef &table);

    virtual auto get_title() -> std::string;
    virtual auto can_close() -> bool;

    auto get_table() -> db_TableRef {
      return db_TableRef::cast_from(get_object());
    };

    virtual auto get_columns() -> TableColumnsListBE * = 0;
    virtual auto get_indexes() -> IndexListBE * = 0;
    auto get_fks() -> FKConstraintListBE * {
      return &_fk_list;
    }

    auto get_inserts_model() -> RecordsetRef;
    auto get_inserts_panel() -> mforms::View *;

    virtual auto set_name(const std::string &name) -> void;

    // table options
    virtual auto set_table_option_by_name(const std::string &name, const std::string &value) -> void = 0;
    virtual auto get_table_option_by_name(const std::string &name) -> std::string = 0;

    // column editing
    virtual auto add_column(const std::string &name) -> NodeId;
    virtual auto remove_column(const NodeId &column) -> void;
    auto rename_column(const db_ColumnRef &column, const std::string &name) -> void;
    NodeId duplicate_column(const db_ColumnRef &col, ssize_t insert_after = -1);

    auto get_column_with_name(const std::string &name) -> db_ColumnRef;

    // fk editing
    virtual auto add_fk(const std::string &name) -> NodeId;
    virtual auto remove_fk(const NodeId &fk) -> bool;
    virtual auto add_fk_with_columns(const std::vector<NodeId> &columns) -> NodeId;
    virtual auto check_column_referenceable_by_fk(const db_ColumnRef &column1, const db_ColumnRef &column2) -> bool = 0;

    // index editing
    virtual auto add_index(const std::string &name) -> NodeId;
    virtual auto remove_index(const NodeId &index, bool delete_even_if_foreign) -> bool;

    virtual auto add_index_with_columns(const std::vector<NodeId> &columns) -> NodeId;

    // helper utils for columns
    virtual auto parse_column_type(const std::string &str, db_ColumnRef &column) -> bool;
    virtual auto format_column_type(db_ColumnRef &column) -> std::string;

    virtual auto get_index_types() -> std::vector<std::string> = 0;

    auto show_export_wizard(mforms::Form *owner) -> void;
    auto show_import_wizard() -> void;

    virtual auto get_sql_editor() -> MySQLEditor::Ref;

    virtual auto create_stub_table(const std::string &schema, const std::string &table) -> db_TableRef = 0;

    auto column_count_changed() -> void;
    auto showErrorMessage(const std::string &type) -> bool;

  protected:
    FKConstraintListBE _fk_list;

    auto undo_called(grt::UndoAction *action, grt::UndoAction *expected) -> void;

  private:
    mforms::Box *_inserts_panel;
    mforms::GridView *_inserts_grid;
    RecordsetRef _inserts_model;
    RecordsetTableInsertsStorageRef _inserts_storage;

    void inserts_column_resized(int);
    auto restore_inserts_columns() -> void;
    auto catalogChanged(const std::string &member, const grt::ValueRef &value) -> void;

    auto update_selection_for_menu_extra(mforms::ContextMenu *menu, const std::vector<int> &rows, int column) -> void;
    auto open_field_editor(int row, int column) -> void;
  };
}; // namespace bec
