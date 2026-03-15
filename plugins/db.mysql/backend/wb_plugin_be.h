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

#include "db_mysql_public_interface.h"
#include "grt/grt_manager.h"
#include "base/trackable.h"

class Wb_plugin;
#ifdef _MSC_VER
#pragma make_public(Wb_plugin)
#endif

class WBPLUGINDBMYSQLBE_PUBLIC_FUNC Wb_plugin : public base::trackable {
public:
  Wb_plugin() {
    _options = grt::DictRef(true);
  }
  virtual ~Wb_plugin() {
  }

public:
  virtual auto task_desc() -> std::string = 0;
  auto exec_task(bool sync = false) -> void;

protected:
  typedef std::function<grt::StringRef()> Task_proc_cb;
  virtual auto set_task_proc() -> void = 0;
  Task_proc_cb _task_proc_cb;

public:
  typedef std::function<int(int, const std::string &)> Task_msg_cb;
  typedef std::function<int(float, const std::string &)> Task_progress_cb;
  typedef std::function<int()> Task_finish_cb;
  typedef std::function<int(const std::string &)> Task_fail_cb;

  auto task_msg_cb(Task_msg_cb cb) -> void {
    _task_msg_cb = cb;
  }
  auto task_progress_cb(Task_progress_cb cb) -> void {
    _task_progress_cb = cb;
  }
  auto task_finish_cb(Task_finish_cb cb) -> void {
    _task_finish_cb = cb;
  }
  auto task_fail_cb(Task_fail_cb cb) -> void {
    _task_fail_cb = cb;
  }

  auto process_task_msg(const grt::Message &msgs) -> void;
  auto process_task_finish(grt::ValueRef res) -> void;
  auto process_task_fail(const std::exception &error) -> void;

private:
  Task_msg_cb _task_msg_cb;
  Task_progress_cb _task_progress_cb;
  Task_finish_cb _task_finish_cb;
  Task_fail_cb _task_fail_cb;

public:
  auto set_option(const std::string &name, int val) -> void;
  auto set_option(const std::string &name, const double &val) -> void;
  auto set_option(const std::string &name, const std::string &val) -> void;
  auto get_int_option(const std::string &name) -> int;
  auto get_double_option(const std::string &name) -> double;
  auto get_string_option(const std::string &name) -> std::string;

protected:
  grt::DictRef _options;
};
