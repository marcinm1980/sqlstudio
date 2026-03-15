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

#include "mysql_sql_parser_base.h"
#include "grtsqlparser/sql_statement_decomposer.h"

/** Implements DBMS specifics.
 *
 * @ingroup sqlparser
 */
class MYSQL_SQL_PARSER_PUBLIC_FUNC Mysql_sql_statement_decomposer : protected Mysql_sql_parser_base,
                                                                    public Sql_statement_decomposer {
public:
  typedef std::shared_ptr<Mysql_sql_statement_decomposer> Ref;
  static auto create(grt::DictRef db_opts = grt::DictRef()) -> Ref {
    Ref decomposer(new Mysql_sql_statement_decomposer);
    decomposer->set_options(db_opts);
    return decomposer;
  }
  virtual ~Mysql_sql_statement_decomposer() {
  }

protected:
  Mysql_sql_statement_decomposer();
  auto set_options(const grt::DictRef &opts) -> void;
  auto decompose_query(const std::string &sql, SelectStatement::Ref select_statement) -> int;
  auto decompose_view(const std::string &ddl, SelectStatement::Ref select_statement) -> int;
  auto decompose_view(db_ViewRef view, SelectStatement::Ref select_statement) -> int;

protected:
  typedef boost::function<Parse_result(const SqlAstNode *)> ProcessSqlStatement;
  auto process_sql_statement(const std::string &sql, SelectStatement::Ref select_statement,
                            ProcessSqlStatement do_process_sql_statement_cb) -> int;
  auto process_sql_statement(const std::string &sql, SelectStatement::Ref select_statement,
                            Mysql_sql_parser_fe &sql_parser_fe) -> int;
  auto do_process_sql_statement(const SqlAstNode *tree) -> int;
  ProcessSqlStatement _do_process_sql_statement;

protected:
  auto decompose_query(const SqlAstNode *select_init) -> Parse_result;
  auto do_decompose_query(const SqlAstNode *tree) -> Parse_result;
  SelectStatement::Ref _select_statement;

protected:
  auto do_decompose_view(const SqlAstNode *tree) -> Parse_result;
  auto expand_wildcards(SelectStatement::Ref select_statement, db_SchemaRef &db_schema,
                        grt::ListRef<db_Schema> &db_schemata) -> void;
  std::list<std::string> _view_columns_names;

protected:
  class Null_state_keeper : Mysql_sql_parser_base::Null_state_keeper {
  public:
    Null_state_keeper(Mysql_sql_statement_decomposer *sql_parser)
      : Mysql_sql_parser_base::Null_state_keeper(sql_parser), _sql_parser(sql_parser) {
    }
    ~Null_state_keeper();

  private:
    Mysql_sql_statement_decomposer *_sql_parser;
  };
};
