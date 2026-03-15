/*
 * Copyright (c) 2010, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef __grt_code_editor_h__
#define __grt_code_editor_h__

#include "mforms/appview.h"
#include "mforms/code_editor.h"
#include "mforms/splitter.h"
#include "mforms/panel.h"
#include "mforms/tabview.h"
#include "mforms/textentry.h"
#include "mforms/textbox.h"
#include "mforms/filechooser.h"
#include "mforms/find_panel.h"

class GRTShellWindow;
class PythonDebugger;

class GRTCodeEditor : public mforms::Box {
public:
  GRTCodeEditor(GRTShellWindow *owner, bool module, const std::string &language);
  virtual ~GRTCodeEditor();

  auto set_path(const std::string &path) -> void;
  auto get_path() -> const std::string & {
    return _filename;
  };
  auto get_language() -> const std::string & {
    return _language;
  };
  auto set_text(const std::string &text) -> void;
  auto get_title() -> std::string;
  auto get_text() -> std::string;

  auto is_dirty() const -> bool {
    return _dirty;
  }
  auto can_close() -> bool;

  auto load(const std::string &path) -> bool;
  auto save(bool choose_file) -> bool;
  auto execute() -> void;

  auto get_shell_window() -> GRTShellWindow * {
    return _owner;
  }

  auto get_editor() -> mforms::CodeEditor * {
    return &_text;
  }

#ifdef _DEBUG
  auto test_markup() -> void;
#endif
protected:
  GRTShellWindow *_owner;
  mforms::Box _top;
  mforms::CodeEditor _text;

  std::string _filename;
  std::string _language;
  bool _debugging_supported;
  bool _editing_module;
  bool _dirty;

  auto text_changed(int line, int linesAdded) -> void;
};

#endif /* __grt_code_editor.h__ */
