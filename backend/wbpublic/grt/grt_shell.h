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
#include "grtpp_shell.h"

#include "grt_dispatcher.h"
#include "wbpublic_public_interface.h"

#define ShellBE_VERSION 4

namespace bec {
  class GRTManager;

  class WBPUBLICBACKEND_PUBLIC_FUNC ShellBE {
  public:
    ShellBE(const GRTDispatcher::Ref dispatcher);
    ~ShellBE();

    auto setup(const std::string &lang) -> bool;

    auto set_save_directory(const std::string &path) -> void;
    auto start() -> void;

    auto process_line_async(const std::string &line) -> void;

    auto run_script_file(const std::string &path) -> void;
    auto run_script(const std::string &script, const std::string &language) -> bool;

    auto previous_history_line(const std::string &current_line, std::string &line) -> bool;
    auto next_history_line(std::string &line) -> bool;
    auto reset_history_position() -> void;

    auto get_grt_tree_bookmarks() -> std::vector<std::string>;
    auto add_grt_tree_bookmark(const std::string &path) -> void;
    auto delete_grt_tree_bookmark(const std::string &path) -> void;

    auto write_line(const std::string &line) -> void;
    auto write(const std::string &text) -> void;
    auto writef(const char *fmt, ...) -> void;

    auto set_output_handler(const std::function<void(const std::string &)> &slot) -> void;
    auto set_ready_handler(const std::function<void(const std::string &)> &slot) -> void;

    auto flush_shell_output() -> void;

    auto set_saves_history(int line_count) -> void;

    auto complete_line(const std::string &line, std::string &nprefix) -> std::vector<std::string>;

    auto get_shell_variable(const std::string &varname) -> grt::ValueRef;

    auto clear_history() -> void;
    auto save_history_line(const std::string &line) -> void;

    auto get_snippet_data() -> std::string;
    auto set_snippet_data(const std::string &data) -> void;

    auto store_state() -> void;
    auto restore_state() -> void;

    auto handle_msg(const grt::Message &msgs) -> void;

  protected:
    grt::Shell *_shell;
    GRTDispatcher::Ref _dispatcher;
    std::vector<std::string> _grt_tree_bookmarks;

    std::string _savedata_dir;

    std::string _current_statement;

    std::list<std::string> _history; // most recent first
    std::list<std::string>::iterator _history_ptr;

    std::function<void(const std::string &)> _ready_slot;

    std::function<void(const std::string &)> _output_slot;

    base::Mutex _text_queue_mutex;

    std::list<std::string> _text_queue;

    int _save_history_size;
    int _skip_history;

  private:
    auto shell_finished_cb(grt::ShellCommand result, const std::string &prompt, const std::string &line) -> void;
  };
};
