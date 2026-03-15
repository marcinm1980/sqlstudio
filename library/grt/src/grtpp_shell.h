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

#include "grt.h"

namespace grt {

  enum ShellCommand {
    ShellCommandUnknown = -1,
    ShellCommandExit = 0,
    ShellCommandAll,
    ShellCommandError,
    ShellCommandStatement,
    ShellCommandHelp,
    ShellCommandLs,
    ShellCommandCd,
    ShellCommandRun
  };

#define MYX_SHELL_CURNODE "current"

  class MYSQLGRT_PUBLIC Shell {
  public:
    Shell();
    virtual ~Shell();

    auto set_disable_quit(bool flag) -> bool;

    auto execute(const std::string &linebuf) -> ShellCommand;

    virtual auto shell_type() -> std::string = 0;

    virtual auto init() -> void = 0;
    virtual auto print_welcome() -> void = 0;
    virtual auto get_prompt() -> std::string = 0;
    virtual auto execute_line(const std::string &linebuf) -> int = 0;
    virtual auto run_file(const std::string &file_name, bool interactive) -> int = 0;
    virtual auto show_help(const std::string &topic) -> void = 0;

    virtual auto complete_line(const std::string &line, std::string &completed) -> std::vector<std::string> = 0;

    virtual auto get_global_var(const std::string &var_name) -> ValueRef = 0;
    virtual auto set_global_var(const std::string &var_name, const ValueRef &value) -> int = 0;

    static auto get_abspath(const std::string &cwd, const std::string &npath) -> std::string;

    virtual auto print(const std::string &str) -> void;

  protected:
    bool _disable_quit;
  };
}; // namespace grt
