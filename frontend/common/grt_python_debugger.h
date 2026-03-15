/*
 * Copyright (c) 2010, 2020, Oracle and/or its affiliates.
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

#ifndef __GRT_PYTHON_DEBUGGER_H_
#define __GRT_PYTHON_DEBUGGER_H_

#include "python_context.h"
#include "grt/grt_manager.h"
#include "base/trackable.h"

#include "mforms/tabview.h"
#include "mforms/treeview.h"
#include "mforms/utilities.h"
#include "mforms/panel.h"
#include "mforms/splitter.h"
#include "mforms/textentry.h"
#include "mforms/textbox.h"

class GRTCodeEditor;
class GRTShellWindow;

class PythonDebugger : public base::trackable {
  GRTShellWindow *_shell;
  mforms::TabView *_lower_tabs;

  mforms::TreeView *_stack_list;
  mforms::TreeView *_breakpoint_list;
  mforms::TreeView *_variable_list;

  GRTCodeEditor *_stack_position_editor;
  int _stack_position_line;

  std::string _tmpfile_name;

  grt::AutoPyObject _pdb;
  std::string _pdb_varname;

  bec::GRTManager::Timer *_heartbeat_timeout_timer;

  bool _pause_clicked;
  bool _program_stopped;

  auto show_stack() -> void;

  auto ensure_code_saved() -> bool;

  auto edit_breakpoint(mforms::TreeNodeRef node, int column, std::string value) -> void;
  auto line_gutter_clicked(int margin, int line, mforms::ModifierKey mods, GRTCodeEditor *editor) -> void;
  auto editor_text_changed(int line, int linesAdded, GRTCodeEditor *editor) -> void;
  auto stack_selected() -> void;

  auto heartbeat_timeout() -> bool;

private:
  auto toggle_breakpoint(const char *file, int line) -> bool;

public:
  static auto from_cobject(PyObject *cobj) -> PythonDebugger *;
  auto as_cobject() -> PyObject *;

  auto debug_print(const std::string &s) -> void;
  auto ui_clear_breakpoints() -> void;
  auto ui_add_breakpoint(const char *file, int line, const char *condition) -> void;
  auto ui_program_stopped(const char *file, int line, int reason) -> const char *;
  auto ui_clear_stack() -> void;
  auto ui_add_stack(const char *location, const char *file, int line) -> void;

  auto ui_clear_variables() -> void;
  auto ui_add_variable(const char *varname, const char *value) -> void;

public:
  PythonDebugger(GRTShellWindow *shell, mforms::TabView *tabview);
  auto init_pdb() -> void;

  auto program_stopped() -> bool {
    return _program_stopped;
  }

  auto editor_added(GRTCodeEditor *editor) -> void;
  auto editor_closed(GRTCodeEditor *editor) -> void;

  auto refresh_file(const std::string &file) -> void;

  auto run(GRTCodeEditor *editor, bool stepping = false) -> void;
  auto stop() -> void;
  auto pause() -> void;
  auto step_into() -> void;
  auto step() -> void;
  auto step_out() -> void;
  auto continue_() -> void;
};

#endif
