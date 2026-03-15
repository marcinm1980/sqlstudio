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
#include "sqlide/recordset_be.h"

namespace sqlite {
  struct command;
}

class WBPUBLICBACKEND_PUBLIC_FUNC Recordset_data_storage {
public:
  using Ref = std::shared_ptr<Recordset_data_storage>;
  using Ptr = std::weak_ptr<Recordset_data_storage>;
  virtual ~Recordset_data_storage();

protected:
  Recordset_data_storage();

  friend class Recordset;

public:
  using Var_list = std::list<sqlite::variant_t>;
  using Var_vector = std::vector<sqlite::variant_t>;

protected:
  auto data_swap_db(const Recordset::Ref &recordset) -> std::shared_ptr<sqlite::connection>;

public:
  auto apply_changes(Recordset::Ptr recordset, bool skip_commit) -> void;
  auto serialize(Recordset::Ptr recordset) -> void;
  auto unserialize(Recordset::Ptr recordset) -> void;
  auto fetch_blob_value(Recordset::Ptr recordset, RowId rowid, ColumnId column, sqlite::variant_t &blob_value) -> void;

protected:
  virtual auto fetch_blob_value(Recordset *recordset, sqlite::connection *data_swap_db, RowId rowid, ColumnId column,
                                sqlite::variant_t &blob_value) -> void;

protected:
  virtual auto do_apply_changes(const Recordset *recordset, sqlite::connection *data_swap_db, bool skip_commit) -> void = 0;
  virtual auto do_serialize(const Recordset *recordset, sqlite::connection *data_swap_db) -> void = 0;
  virtual auto do_unserialize(Recordset *recordset, sqlite::connection *data_swap_db) -> void = 0;
  virtual auto do_fetch_blob_value(Recordset *recordset, sqlite::connection *data_swap_db, RowId rowid, ColumnId column,
                                   sqlite::variant_t &blob_value) -> void = 0;

public:
  auto valid() -> bool {
    return _valid;
  }
  auto readonly() -> bool {
    return _readonly;
  }
  auto readonly_reason() -> std::string {
    return _readonly_reason;
  }
  auto readonly_reason(const std::string &reason) -> void {
    _readonly_reason = reason;
  }
  virtual auto aux_column_count() -> ColumnId = 0;

protected:
  bool _readonly;
  std::string _readonly_reason;
  bool _valid;

public:
  virtual auto reloadable() const -> bool {
    return true;
  }

public:
  static auto create_data_swap_tables(sqlite::connection *data_swap_db, Recordset::Column_names &column_names,
                                      Recordset::Column_types &column_types) -> void;

protected:
  auto prepare_data_swap_record_add_statement(
    sqlite::connection *data_swap_db, Recordset::Column_names &column_names) -> std::list<std::shared_ptr<sqlite::command> >;
  auto add_data_swap_record(std::list<std::shared_ptr<sqlite::command> > &insert_commands, const Var_vector &values) -> void;
  auto update_data_swap_record(sqlite::connection *data_swap_db, RowId rowid, ColumnId column,
                               const sqlite::variant_t &value) -> void;

protected:
  static auto get_column_names(Recordset *recordset) -> Recordset::Column_names & {
    return recordset->_column_names;
  }
  static auto get_column_types(Recordset *recordset) -> Recordset::Column_types & {
    return recordset->_column_types;
  }
  static auto get_real_column_types(Recordset *recordset) -> Recordset::Column_types & {
    return recordset->_real_column_types;
  }
  static auto get_column_flags(Recordset *recordset) -> Recordset::Column_flags & {
    return recordset->_column_flags;
  }
  static auto getDbColumnTypes(Recordset *recordset) -> Recordset::DBColumn_types & {
    return recordset->_dbColumnTypes;
  }
  static auto get_column_names(const Recordset *recordset) -> const Recordset::Column_names & {
    return recordset->_column_names;
  }
  static auto get_column_types(const Recordset *recordset) -> const Recordset::Column_types & {
    return recordset->_column_types;
  }
  static auto get_real_column_types(const Recordset *recordset) -> const Recordset::Column_types & {
    return recordset->_real_column_types;
  }
  static auto get_column_flags(const Recordset *recordset) -> const Recordset::Column_flags & {
    return recordset->_column_flags;
  }
  static auto getDbColumnTypes(const Recordset *recordset) -> const Recordset::DBColumn_types & {
    return recordset->_dbColumnTypes;
  }

public:
  auto limit_rows() -> bool {
    return _limit_rows;
  }
  auto limit_rows(bool value) -> void {
    _limit_rows = value;
  }
  auto limit_rows_count() -> int {
    return _limit_rows_count;
  }
  auto limit_rows_count(RowId value) -> void {
    _limit_rows_count = (int)value;
  }
  auto limit_rows_applicable() -> bool {
    return _limit_rows_applicable;
  }
  auto limit_rows_applicable(bool val) -> void {
    _limit_rows_applicable = val;
  }
  auto limit_rows_offset() -> int {
    return _limit_rows_offset;
  }
  auto scroll_rows_frame_forward() -> void {
    _limit_rows_offset += _limit_rows_count;
  }
  auto scroll_rows_frame_backward() -> void {
    _limit_rows_offset = std::max<int>(0, (_limit_rows_offset - _limit_rows_count));
  }

protected:
  bool _limit_rows;
  int _limit_rows_count;
  int _limit_rows_offset;
  bool _limit_rows_applicable;
};
