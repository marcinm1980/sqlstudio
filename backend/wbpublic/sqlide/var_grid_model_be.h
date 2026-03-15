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

#ifndef _VAR_GRID_MODEL_BE_H_
#define _VAR_GRID_MODEL_BE_H_

#include "wbpublic_public_interface.h"
#include "sqlide_generics.h"
#include "grt/grt_threaded_task.h"
#include "grt/tree_model.h"
#include "grt/grt_manager.h"
#include <vector>

class Recordset_data_storage;

namespace sqlite {
  struct query;
  struct result;
} // namespace sqlite

class WBPUBLICBACKEND_PUBLIC_FUNC VarGridModel : public bec::GridModel,
                                                 public std::enable_shared_from_this<VarGridModel> {
public:
  using Ref = std::shared_ptr<VarGridModel>;
  virtual ~VarGridModel();

protected:
  VarGridModel();

  friend class Recordset_data_storage;

public:
  virtual auto reset() -> void;
  virtual auto refresh() -> void {
  }
  std::function<void()> rows_changed;
  boost::signals2::signal<void()> refresh_ui_signal;

protected:
  boost::signals2::scoped_connection _refresh_connection;

  virtual auto refresh_ui() -> int;

public:
  virtual auto row_count() const -> size_t {
    return _row_count;
  }
  virtual auto count() -> size_t;
  virtual auto get_column_count() const -> size_t {
    return _column_count;
  }
  virtual auto get_column_caption(ColumnId index) -> std::string;
  virtual auto get_column_type(ColumnId column) -> ColumnType;
  virtual auto get_real_column_type(ColumnId column) -> ColumnType;
  virtual auto get_column_width_hint(int column) -> int {
    return 0; /* 0 - no hint */
  }
  virtual auto isGeometry(ColumnId) -> bool;

public:
  virtual auto is_readonly() const -> bool {
    return _readonly;
  }
  virtual auto readonly_reason() const -> std::string {
    return _readonly_reason;
  }

protected:
  bool _readonly;
  std::string _readonly_reason;

public:
  enum ColumnFlags { NeedsQuoteFlag = 1, NotNullFlag = 2 };

  using Column_names = std::vector<std::string>;
  using DBColumn_types = std::vector<std::string>;
  using Column_types = std::vector<sqlite::variant_t>;
  using Cell_const = Data::const_iterator;
  using Column_flags = std::vector<int>;

protected:
  using Cell = Data::iterator;

public:
  virtual auto get_field_icon(const bec::NodeId &node, ColumnId column, bec::IconSize size) -> bec::IconId;

private:
  class IconForVal;
  std::unique_ptr<IconForVal> _icon_for_val;

public:
  virtual auto set_field(const bec::NodeId &node, ColumnId column, const sqlite::variant_t &value) -> bool;
  virtual auto set_field(const bec::NodeId &node, ColumnId column, const std::string &value) -> bool;
  virtual auto set_field(const bec::NodeId &node, ColumnId column, double value) -> bool;
  virtual auto set_field(const bec::NodeId &node, ColumnId column, bool value) -> bool;
  virtual auto set_field(const bec::NodeId &node, ColumnId column, ssize_t value) -> bool;
  virtual auto get_field(const bec::NodeId &node, ColumnId column, std::string &value) -> bool;
  virtual auto get_field_repr(const bec::NodeId &node, ColumnId column, std::string &value) -> bool;
  auto get_field_repr_no_truncate(const bec::NodeId &node, ColumnId column, std::string &value) -> bool;
  virtual auto get_field(const bec::NodeId &node, ColumnId column, ssize_t &value) -> bool;
  virtual auto get_field(const bec::NodeId &node, ColumnId column, double &value) -> bool;
  virtual auto get_field(const bec::NodeId &node, ColumnId column, bool &value) -> bool;
  virtual auto get_field(const bec::NodeId &node, ColumnId column, sqlite::variant_t &value) -> bool;

protected:
  auto get_field_(const bec::NodeId &node, ColumnId column, std::string &value) -> bool;
  auto get_field_repr_(const bec::NodeId &node, ColumnId column, std::string &value) -> bool;
  auto get_field_(const bec::NodeId &node, ColumnId column, ssize_t &value) -> bool;
  auto get_field_(const bec::NodeId &node, ColumnId column, double &value) -> bool;
  auto get_field_(const bec::NodeId &node, ColumnId column, bool &value) -> bool;
  auto get_field_(const bec::NodeId &node, ColumnId column, sqlite::variant_t &value) -> bool;

protected:
  virtual auto get_field_grt(const bec::NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;
  virtual auto after_set_field(const bec::NodeId &node, ColumnId column, const sqlite::variant_t &value) -> void {
  }

public:
  virtual auto is_field_null(const bec::NodeId &node, ColumnId column) -> bool;
  virtual auto set_field_null(const bec::NodeId &node, ColumnId column) -> bool;

public:
  virtual auto data() -> const Data & {
    return _data;
  }

protected:
  virtual auto get_cell(Cell &cell, const bec::NodeId &node, ColumnId column, bool allow_new_row) -> bool;
  virtual auto cell(RowId row, ColumnId column) -> Cell;
  auto add_column(const std::string &name, const sqlite::variant_t &type) -> void;

protected:
  Data _data;
  RowId _row_count;
  ColumnId _column_count;
  Column_names _column_names;
  Column_types _column_types;
  Column_types _real_column_types; //! as a temp workaround for quick-fix of #38600: Insert statement calling function
                                   //! is incorrectly parsed
  Column_flags _column_flags; // various flags, such as whether value should be quoted and whether it's NOT NULL (ie
                              // numbers vs strings. special values like functions need extra handling)
  DBColumn_types _dbColumnTypes;

  base::RecMutex _data_mutex;

protected:
  auto data_swap_db() const -> std::shared_ptr<sqlite::connection>;

private:
  auto create_data_swap_db_connection() const -> std::shared_ptr<sqlite::connection>;

private:
  mutable std::shared_ptr<sqlite::connection> _data_swap_db;
  std::string _data_swap_db_path;

public:
  static const int DATA_SWAP_DB_TABLE_MAX_COL_COUNT;

public:
  auto data_swap_db_partition_count() const -> size_t;

public:
  static auto data_swap_db_partition_count(ColumnId column_count) -> size_t;
  static auto data_swap_db_partition_suffix(size_t partition) -> std::string;
  static auto data_swap_db_column_partition(ColumnId column) -> size_t; // returns partition number containing passed column
  static auto translate_data_swap_db_column(
    ListModel::ColumnId column, size_t *partition = NULL) -> bec::ListModel::ColumnId; // returns column number relative to containing partition
  static auto prepare_partition_queries(sqlite::connection *data_swap_db, const std::string &query_text_template,
                                        std::list<std::shared_ptr<sqlite::query> > &queries) -> void;
  static auto emit_partition_queries(sqlite::connection *data_swap_db,
                                     std::list<std::shared_ptr<sqlite::query> > &queries,
                                     std::vector<std::shared_ptr<sqlite::result> > &results,
                                     const std::list<sqlite::variant_t> &bind_vars = std::list<sqlite::variant_t>()) -> bool;
  static auto emit_partition_commands(sqlite::connection *data_swap_db, size_t partition_count,
                                      const std::string &command_text_template,
                                      const std::list<sqlite::variant_t> &bind_vars = std::list<sqlite::variant_t>()) -> void;

protected:
  auto cache_data_frame(RowId center_row, bool force_reload) -> void;

protected:
  RowId _data_frame_begin;
  RowId _data_frame_end;
  sqlide::VarCast _var_cast;

public:
  virtual auto floating_point_visible_scale() -> int;
  auto var2str_convertor() const -> const sqlide::VarToStr * {
    return &_var_to_str;
  }

protected:
  sqlide::VarToStr _var_to_str;
  sqlide::VarToStr _var_to_str_repr; // supposed to be used only by UI part, set to do truncation of long text values
  sqlide::VarToInt _var_to_int;
  sqlide::VarToBool _var_to_bool;
  sqlide::VarToLongDouble _var_to_long_double;

public:
  virtual auto set_edited_field(RowId row_index, ColumnId col_index) -> void;
  auto is_field_value_truncation_enabled(bool val) -> bool;

  auto edited_field_row() -> RowId {
    return _edited_field_row;
  }
  auto edited_field_column() -> ColumnId {
    return _edited_field_col;
  }

  // called when the backend changes the current edited field row/column and the frontend must reselect
  std::function<void()> update_edited_field;

protected:
  bool _is_field_value_truncation_enabled;
  RowId _edited_field_row;
  ColumnId _edited_field_col;

public:
  auto optimized_blob_fetching() const -> bool {
    return _optimized_blob_fetching;
  }

private:
  bool _optimized_blob_fetching;
};

#endif /* _VAR_GRID_MODEL_BE_H_ */
