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

#include "wbpublic_public_interface.h"
#include "sqlide/sqlide_generics.h"
#include "sqlide/var_grid_model_be.h"
#include "grt/action_list.h"
#include <map>
#include <set>
#include <list>

class Recordset_data_storage;
class BinaryDataEditor;

namespace mforms {
  class Form;
  class ContextMenu;
  class ToolBar;
  class ToolBarItem;
}; // namespace mforms

struct WBPUBLICBACKEND_PUBLIC_FUNC Recordset_storage_info {
  std::string name;
  std::string extension;
  std::string description;
  // "label1":SYMBOL1;label2:SYMBOL2;label3:SYMBOL3
  std::list<std::pair<std::string, std::string> > arguments;
};

class WBPUBLICBACKEND_PUBLIC_FUNC Recordset : public VarGridModel {
public:
  using Ref = std::shared_ptr<Recordset>;
  using Ptr = std::weak_ptr<Recordset>;
  static auto create() -> Ref;
  static auto create(GrtThreadedTask::Ref parent_task) -> Ref;
  virtual ~Recordset();

protected:
  Recordset();
  Recordset(GrtThreadedTask::Ref parent_task);

public:
  auto can_close() -> bool;
  auto can_close(bool interactive) -> bool;
  auto close() -> bool;
  boost::signals2::signal<void(Ptr)> on_close;

public:
  using Recordset_data_storage_Ref = std::shared_ptr<Recordset_data_storage>;
  using Recordset_data_storage_Ptr = std::weak_ptr<Recordset_data_storage>;
  friend class Recordset_data_storage;

public:
  auto key() const -> long {
    return _id;
  }

  class WBPUBLICBACKEND_PUBLIC_FUNC ClientData {
  public:
    virtual ~ClientData();
  };

  auto set_client_data(ClientData *cdata) -> void {
    _client_data = cdata;
  }
  auto client_data() -> ClientData * {
    return _client_data;
  }

public:
  auto reset(bool rethrow) -> bool;
  virtual auto reset() -> void;
  virtual auto refresh() -> void;
  boost::signals2::signal<void()> data_edited_signal;

private:
  auto reset(Recordset_data_storage_Ptr data_storage_ptr, bool rethrow) -> bool;
  auto data_edited() -> void;

public:
  auto real_row_count() const -> RowId;

private:
  auto recalc_row_count(sqlite::connection *data_swap_db) -> void;

private:
  size_t _real_row_count;

public:
  auto column_names() const -> const Column_names * {
    return &_column_names;
  }
  virtual auto get_column_count() const -> size_t {
    return (int)(_column_count - _aux_column_count);
  }
  auto aux_column_count() const -> size_t {
    return _aux_column_count;
  }

protected:
  size_t _aux_column_count;
  ColumnId _rowid_column;

public:
  auto min_new_rowid() const -> RowId {
    return _min_new_rowid;
  }

protected:
  RowId _min_new_rowid;
  RowId _next_new_rowid;

private:
  static std::string _add_change_record_statement;

public:
  virtual auto after_set_field(const bec::NodeId &node, ColumnId column, const sqlite::variant_t &value) -> void;
  virtual auto delete_node(const bec::NodeId &node) -> bool;
  virtual auto delete_nodes(std::vector<bec::NodeId> &nodes) -> bool;

private:
  virtual auto cell(RowId row, ColumnId column) -> Cell;
  auto mark_dirty(RowId row, ColumnId column, const sqlite::variant_t &new_value) -> void;

public:
  auto data_storage() -> Recordset_data_storage_Ref {
    return _data_storage;
  }
  auto data_storage(const Recordset_data_storage_Ref &data_storage) -> void {
    _data_storage = data_storage;
  }

protected:
  Recordset_data_storage_Ref _data_storage;

public:
  std::function<void()> apply_changes_cb;
  // force UI to save any ongoing edits by the user
  std::function<void()> flush_ui_changes_cb;

public:
  auto apply_changes_and_gather_messages(std::string &messages) -> bool;
  auto rollback_and_gather_messages(std::string &messages) -> void;

  auto apply_changes_() -> void;
  auto do_apply_changes(Ptr self_ptr, Recordset_data_storage_Ptr data_storage_ptr, bool skip_commit) -> grt::StringRef;
  auto has_pending_changes() -> bool;
  auto pending_changes(int &upd_count, int &ins_count, int &del_count) const -> void;
  auto rollback() -> void;
  auto apply_changes() -> void;

private:
  auto on_apply_changes_finished() -> int;
  auto apply_changes_(Recordset_data_storage_Ptr data_storage_ptr) -> void;

public:
  auto limit_rows() -> bool;
  auto limit_rows(bool value) -> void;
  auto toggle_limit_rows() -> void;
  auto limit_rows_count() -> int;
  auto limit_rows_count(int value) -> void;
  auto limit_rows_applicable() -> bool;
  auto scroll_rows_frame_forward() -> void;
  auto scroll_rows_frame_backward() -> void;

public:
  auto get_context_menu() -> mforms::ContextMenu *;

  auto update_selection_for_menu(const std::vector<int> &rows, int clicked_column) -> void;
  std::function<void(mforms::ContextMenu *, const std::vector<int> &, int)> update_selection_for_menu_extra;

  auto selected_rows() -> std::vector<int> {
    return _selected_rows;
  }
  auto selected_column() -> int {
    return _selected_column;
  }

private:
  std::vector<int> _selected_rows;
  int _selected_column;

  auto activate_menu_item(const std::string &action, const std::vector<int> &rows, int clicked_column) -> void;

public:
  void copy_rows_to_clipboard(const std::vector<int> &indeces, std::string sep = ", ", bool quoted = true,
                              bool with_header = false);
  auto copy_field_to_clipboard(int row, ColumnId column, bool quoted = true) -> void;

  auto paste_rows_from_clipboard(ssize_t dest_row) -> void;
  auto showPointInBrowser(const bec::NodeId &node, ColumnId column) -> void;
  auto data_storages_for_export() -> std::vector<Recordset_storage_info>;
  auto data_storage_for_export(const std::string &format_name) -> Recordset_data_storage_Ref;

protected:
  Recordset_data_storage_Ref _data_storage_for_export;
  using Data_storages_for_export = std::map<std::string, std::string>;
  Data_storages_for_export _data_storages_for_export;

  auto load_from_file(const bec::NodeId &node, ColumnId column) -> void;
  auto save_to_file(const bec::NodeId &node, ColumnId column) -> void;

public:
  auto load_from_file(const bec::NodeId &node, ColumnId column, const std::string &file) -> void;
  auto save_to_file(const bec::NodeId &node, ColumnId column, const std::string &file) -> void;

  auto get_raw_field(const bec::NodeId &node, ColumnId column, std::string &data_ret) -> bool;

public:
  virtual auto sort_by(ColumnId column, int direction, bool retaining) -> void;
  virtual auto sort_columns() const -> SortColumns {
    return _sort_columns;
  }

private:
  SortColumns _sort_columns; // column:direction(asc/desc)

public:
  auto has_column_filters() const -> bool;
  auto has_column_filter(ColumnId column) const -> bool;
  auto get_column_filter_expr(ColumnId column) const -> std::string;
  auto set_column_filter(ColumnId column, const std::string &filter_expr) -> void;
  auto reset_column_filter(ColumnId column) -> void;
  auto reset_column_filters() -> void;
  auto column_filter_icon_id() const -> size_t;

private:
  using Column_filter_expr_map = std::map<ColumnId, std::string>;
  Column_filter_expr_map _column_filter_expr_map; // column:filter_expr

  auto search_activated(mforms::ToolBarItem *item) -> void;

public:
  auto data_search_string() const -> const std::string &;
  auto set_data_search_string(const std::string &value) -> void;
  auto reset_data_search_string() -> void;
  auto setPreserveRowFilter(bool value) -> void {
    _preserveRowFilters = value;
  }

private:
  std::string _data_search_string;
  bool _preserveRowFilters;

private:
  auto rebuild_data_index(sqlite::connection *data_swap_db, bool do_cache_data_frame, bool do_refresh_ui) -> void;

public:
  auto caption(const std::string &val) -> void {
    _caption = val;
  }
  auto caption() -> std::string;
  auto set_inserts_editor(bool flag) -> void {
    _inserts_editor = flag;
  }
  auto inserts_editor() -> bool {
    return _inserts_editor;
  }

  auto generator_query() const -> std::string {
    return _generator_query;
  }
  auto generator_query(const std::string &query) -> void {
    _generator_query = query;
  }

  auto status_text() -> std::string;
  std::string status_text_trailer;

private:
  bool _inserts_editor;
  std::string _caption;
  std::string _generator_query;
  long _id;
  ClientData *_client_data;
  mforms::ToolBar *_toolbar;

public:
  GrtThreadedTask::Ref task;

public:
  mforms::ContextMenu *_context_menu;

public:
  auto action_list() -> ::ActionList &;

private:
  ::ActionList _action_list;

public:
  auto get_toolbar() -> mforms::ToolBar *;
  auto rebuild_toolbar() -> void;

private:
  auto register_default_actions() -> void;

public:
  auto open_field_data_editor(RowId row, ColumnId column, const std::string &logical_type) -> void;

protected:
  auto set_field_value(RowId row, ColumnId column, BinaryDataEditor *data_editor) -> void;
  auto set_field_raw_data(RowId row, ColumnId column, const char *data, size_t data_length, bool isJson = false) -> void;

public:
  auto getFont() const -> const std::string & {
    return _font;
  }
  auto getFontSize() const -> float {
    return _size;
  }
  auto setFont(const std::string &font, float size) -> void {
    _font = font;
    _size = size;
  }

private:
  std::string _font;
  float _size;
};
