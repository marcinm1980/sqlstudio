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
#include "sqlide/recordset_sql_storage.h"
#include "cppdbc.h"

class WBPUBLICBACKEND_PUBLIC_FUNC Recordset_cdbc_storage : public Recordset_sql_storage {
public:
  struct FieldInfo {
    std::string catalog;
    std::string schema;
    std::string table;
    std::string field;
    std::string type;
    std::string charset;
    int display_size;
    int precision;
    int scale;
  };

  typedef std::shared_ptr<Recordset_cdbc_storage> Ref;
  static auto create() -> Ref {
    return Ref(new Recordset_cdbc_storage());
  }
  virtual ~Recordset_cdbc_storage();

protected:
  Recordset_cdbc_storage();

protected:
  virtual auto do_unserialize(Recordset *recordset, sqlite::connection *data_swap_db) -> void;
  virtual auto do_fetch_blob_value(Recordset *recordset, sqlite::connection *data_swap_db, RowId rowid, ColumnId column,
                                   sqlite::variant_t &blob_value) -> void;

protected:
  virtual auto run_sql_script(const Sql_script &sql_script, bool skip_transaction) -> void;

public:
  auto decorated_sql_query() -> std::string; // adds limit clause if defined by options

public:
  auto setAuxConnectionGetter(
    std::function<base::RecMutexLock(sql::Dbc_connection_handler::Ref &, bool)> getConnection) -> void {
    _getAuxConnection = getConnection;
  };
  auto setUserConnectionGetter(
    std::function<base::RecMutexLock(sql::Dbc_connection_handler::Ref &, bool)> getConnection) -> void {
    _getUserConnection = getConnection;
  };

  auto dbc_resultset(std::shared_ptr<sql::ResultSet> &value) -> void {
    _dbc_resultset = value;
  }
  auto dbc_statement(std::shared_ptr<sql::Statement> &value) -> void {
    _dbc_statement = value;
  }
  auto reloadable() const -> bool {
    return _reloadable;
  }
  auto reloadable(bool val) -> void {
    _reloadable = val;
  }

  auto set_gather_field_info(bool flag) -> void {
    _gather_field_info = flag;
  }
  auto field_info() -> std::vector<FieldInfo> & {
    return _field_info;
  }

private:
  std::function<base::RecMutexLock(sql::Dbc_connection_handler::Ref &, bool)> _getAuxConnection;
  std::function<base::RecMutexLock(sql::Dbc_connection_handler::Ref &, bool)> _getUserConnection;

  std::shared_ptr<sql::ResultSet> _dbc_resultset; // for 1-time unserialization
  std::shared_ptr<sql::Statement> _dbc_statement; // for 1-time unserialization
  std::vector<FieldInfo> _field_info;
  bool _reloadable; // whether can be reloaded using stored sql query
  bool _gather_field_info;

  auto determine_pkey_columns(Recordset::Column_names &column_names, Recordset::Column_types &column_types,
                                Recordset::Column_types &real_column_types) -> size_t;
  auto determine_pkey_columns_alt(Recordset::Column_names &column_names, Recordset::Column_types &column_types,
                                    Recordset::Column_types &real_column_types) -> size_t;
};
