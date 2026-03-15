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

#ifndef _RECORDSET_SQL_STORAGE_BE_H_
#define _RECORDSET_SQL_STORAGE_BE_H_

#include "wbpublic_public_interface.h"
#include "sqlide/recordset_data_storage.h"
#include "grtsqlparser/sql_inserts_loader.h"
#include "grts/structs.db.mgmt.h"
#include <vector>

class WBPUBLICBACKEND_PUBLIC_FUNC Sql_script {
public:
  using Statements = std::list<std::string>;
  using Statement_bindings = std::list<sqlite::variant_t>;
  using Statements_bindings = std::list<Statement_bindings>;
  Statements statements;
  Statements_bindings statements_bindings;
  auto reset() -> void {
    statements.clear();
    statements_bindings.clear();
  }
};

class WBPUBLICBACKEND_PUBLIC_FUNC Recordset_sql_storage : public Recordset_data_storage {
public:
  using Ref = std::shared_ptr<Recordset_sql_storage>;
  static auto create() -> Ref {
    return Ref(new Recordset_sql_storage());
  }
  virtual ~Recordset_sql_storage();

protected:
  Recordset_sql_storage();

protected:
  virtual auto fetch_blob_value(Recordset *recordset, sqlite::connection *data_swap_db, RowId rowid, ColumnId column,
                                sqlite::variant_t &blob_value) -> void;

protected:
  virtual auto do_apply_changes(const Recordset *recordset, sqlite::connection *data_swap_db, bool skip_commit) -> void;
  virtual auto do_serialize(const Recordset *recordset, sqlite::connection *data_swap_db) -> void;
  virtual auto do_unserialize(Recordset *recordset, sqlite::connection *data_swap_db) -> void;
  virtual auto do_fetch_blob_value(Recordset *recordset, sqlite::connection *data_swap_db, RowId rowid, ColumnId column,
                                   sqlite::variant_t &blob_value) -> void;

public:
  static auto statements_as_sql_script(const Sql_script::Statements &sql_statements) -> std::string;

protected:
  virtual auto generate_sql_script(const Recordset *recordset, sqlite::connection *data_swap_db, Sql_script &sql_script,
                                   bool is_update_script, bool binaryAsString = false) -> void;
  virtual auto generate_inserts(const Recordset *recordset, sqlite::connection *data_swap_db, Sql_script &sql_script) -> void;
  virtual auto run_sql_script(const Sql_script &sql_script, bool skip_commit) -> void {
  }
  virtual auto init_variant_quoter(sqlide::QuoteVar &qv) const -> void;

public:
  auto schema_name(const std::string &schema_name) -> void {
    _schema_name = schema_name;
  }
  auto schema_name() const -> std::string {
    return _schema_name;
  }

  auto table_name(const std::string &table_name) -> void {
    _table_name = table_name;
  }
  auto table_name() const -> std::string {
    return _table_name;
  }

  auto full_table_name() const -> std::string;

  auto additional_clauses(const std::string &value) -> void {
    _additional_clauses = value;
  }
  auto additional_clauses() const -> std::string {
    return _additional_clauses;
  }

  auto sql_query(const std::string &sql_query) -> void {
    _sql_query = sql_query;
  }
  auto sql_query() const -> std::string {
    return _sql_query;
  }

  auto sql_script(const std::string &val) -> void {
    _sql_script = val;
  }
  auto sql_script() const -> const std::string & {
    return _sql_script;
  }

  auto affective_columns(const Sql_inserts_loader::Strings &val) -> void {
    _affective_columns = val;
  }
  auto affective_columns() const -> const Sql_inserts_loader::Strings & {
    return _affective_columns;
  }

  virtual auto aux_column_count() -> ColumnId {
    return _pkey_columns.size();
  }

protected:
  std::string _table_name;
  std::string _schema_name;
  std::string _additional_clauses;
  std::string _sql_query;
  std::string _sql_script; // for storing result of serialize
  std::vector<ColumnId> _pkey_columns;
  sqlide::VarCast _var_cast;
  Sql_inserts_loader::Strings _affective_columns; // used to filter irrelevant fields when loading from custom (not
                                                  // validated) sql script, also affects column order

public:
  auto sql_script_substitute(const Sql_script &val) -> void {
    _sql_script_substitute = val;
  }
  auto sql_script_substitute() const -> const Sql_script & {
    return _sql_script_substitute;
  }
  auto is_sql_script_substitute_enabled(bool val) -> void {
    _is_sql_script_substitute_enabled = val;
  }
  auto is_sql_script_substitute_enabled() const -> bool {
    return _is_sql_script_substitute_enabled;
  }

public:
  auto init_sql_script_substitute(const Recordset::Ptr &recordset, bool is_update_script) -> void;
  auto omit_schema_qualifier(bool flag) -> void;

private:
  auto do_init_sql_script_substitute(const Recordset *recordset, sqlite::connection *data_swap_db,
                                     bool is_update_script) -> void;

private:
  Sql_script _sql_script_substitute; // if (_is_sql_script_substitute_enabled) use this value instead of generating sql
                                     // script with generate_sql_script
  bool _is_sql_script_substitute_enabled;
  bool _omit_schema_qualifier;

private:
  using Fields_order = std::map<std::string, int>;
  Fields_order _fields_order;

  auto load_insert_statement(const std::string &sql, const std::pair<std::string, std::string> &schema_table,
                             const Sql_inserts_loader::Strings &fields_names,
                             const Sql_inserts_loader::Strings &fields_values, const std::vector<bool> &null_fields,
                             Recordset::Column_names *column_names, Var_list *var_list) -> void;

public:
  auto rdbms() -> db_mgmt_RdbmsRef {
    return _rdbms;
  }
  auto rdbms(db_mgmt_RdbmsRef rdbms) -> void {
    _rdbms = rdbms;
  }

protected:
  db_mgmt_RdbmsRef _rdbms;

public:
  using Error_cb = boost::signals2::signal<int(long long, const std::string &, const std::string &)>;
  using Batch_exec_progress_cb = boost::signals2::signal<int(float)>;
  using Batch_exec_stat_cb = boost::signals2::signal<int(long, long)>;
  Error_cb on_sql_script_run_error;
  Batch_exec_progress_cb on_sql_script_run_progress;
  Batch_exec_stat_cb on_sql_script_run_statistics;

protected:
  auto get_pkey_predicate_for_data_cache_rowid(Recordset *recordset, sqlite::connection *data_swap_db, RowId rowid,
                                               std::string &pkey_predicate) -> void;

public:
  auto binding_blobs() const -> bool {
    return _binding_blobs;
  }
  auto binding_blobs(bool val) -> void {
    _binding_blobs = val;
  }

private:
  bool _binding_blobs;
};

namespace sqlite {
  struct result;
}

class PrimaryKeyPredicate {
  const Recordset::Column_types *_column_types;
  const Recordset::Column_names *_column_names;
  const std::vector<ColumnId> *_pkey_columns;
  sqlide::QuoteVar *_qv;

public:
  PrimaryKeyPredicate(const Recordset::Column_types *column_types, const Recordset::Column_names *column_names,
                      const std::vector<ColumnId> *pkey_columns, sqlide::QuoteVar *qv);
  std::string operator()(std::vector<std::shared_ptr<sqlite::result> > &data_row_results);
};

#endif /* _RECORDSET_SQL_STORAGE_BE_H_ */
