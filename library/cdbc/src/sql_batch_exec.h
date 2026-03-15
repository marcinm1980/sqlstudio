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

#ifndef _SQL_BATCH_EXEC_H_
#define _SQL_BATCH_EXEC_H_

#include "cppdbc_public_interface.h"
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <list>
#include <string>
#include <functional>

namespace sql {

  class CPPDBC_PUBLIC_FUNC SqlBatchExec {
  public:
    SqlBatchExec();

  public:
    long operator()(sql::Statement *stmt, std::list<std::string> &statements);

  private:
    auto exec_sql_script(sql::Statement *stmt, std::list<std::string> &statements, long &batch_exec_err_count) -> void;

  public:
    using Error_cb = std::function<int(long long, const std::string &, const std::string &)>;
    using Batch_exec_progress_cb = std::function<int(float)>;
    using Batch_exec_stat_cb = std::function<int(long, long)>;

    Error_cb _error_cb;
    Batch_exec_progress_cb _batch_exec_progress_cb;
    Batch_exec_stat_cb _batch_exec_stat_cb;

    auto error_cb(const Error_cb &cb) -> void {
      _error_cb = cb;
    };
    auto batch_exec_progress_cb(const Batch_exec_progress_cb &cb) -> void {
      _batch_exec_progress_cb = cb;
    };
    auto batch_exec_stat_cb(const Batch_exec_stat_cb &cb) -> void {
      _batch_exec_stat_cb = cb;
    };

  private:
    long _batch_exec_success_count;
    long _batch_exec_err_count;
    float _batch_exec_progress_state;
    float _batch_exec_progress_inc;

  public:
    auto stop_on_error(bool value) -> void {
      _stop_on_error = value;
    }
    auto stop_on_error() -> bool {
      return _stop_on_error;
    }

  private:
    bool _stop_on_error;

  public:
    auto failback_statements(const std::list<std::string> &value) -> void {
      _failback_statements = value;
    }
    auto failback_statements() const -> const std::list<std::string> & {
      return _failback_statements;
    }

  private:
    std::list<std::string> _failback_statements;

  public:
    auto sql_log() const -> const std::list<std::string> & {
      return _sql_log;
    }

  private:
    std::list<std::string> _sql_log;
  };

} // namespace sql

#endif // _SQL_BATCH_EXEC_H_
