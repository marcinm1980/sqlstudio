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

#ifndef _FORM_VIEW_BASE_H_
#define _FORM_VIEW_BASE_H_

#include <gtkmm/notebook.h>
#include <gtkmm/widget.h>
#include <gtkmm/paned.h>
#include "base/ui_form.h"

namespace bec {
  class GRTManager;
};

namespace mforms {
  class ToolBar;
};

class PluginEditorBase;

class FormViewBase {
protected:
  sigc::signal<void, std::string> _title_changed;
  Gtk::Notebook *_editor_note;

  mforms::ToolBar *_toolbar;
  Gtk::Paned *_sidebar1_pane;
  Gtk::Paned *_sidebar2_pane;
  std::string _panel_savename;

  FormViewBase(const std::string &savename)
    : _editor_note(0), _toolbar(0), _sidebar1_pane(0), _sidebar2_pane(0), _panel_savename(savename) {
  }
  virtual ~FormViewBase(){};

public:
  auto signal_title_changed() -> sigc::signal<void, std::string> {
    return _title_changed;
  }

  auto get_title() -> std::string {
    return get_form()->get_title();
  }
  virtual auto get_panel() -> Gtk::Widget * = 0;

  virtual auto get_form() const -> bec::UIForm * = 0;

  virtual auto on_close() -> bool {
    return true;
  }
  virtual auto on_activate() -> void {
  }

  virtual auto toggle_sidebar(bool show) -> void;
  virtual auto toggle_secondary_sidebar(bool show) -> void;

  virtual auto reset_layout() -> void {
  }
  // close the selected tab and return true or false if no tab is active
  virtual auto close_focused_tab() -> bool;

  virtual auto find_text(const std::string &text) -> void {
  }

  virtual auto dispose() -> void {
  }

  virtual auto perform_command(const std::string &cmd) -> bool;

protected:
  sigc::slot<void, PluginEditorBase *> _close_editor;

  virtual auto plugin_tab_added(PluginEditorBase *plugin) -> void {};

public:
  auto close_plugin_tab(PluginEditorBase *editor) -> bool;

  auto set_close_editor_callback(const sigc::slot<void, PluginEditorBase *> &handler) -> void;

  auto add_plugin_tab(PluginEditorBase *plugin) -> void;
  auto remove_plugin_tab(PluginEditorBase *plugin) -> void;
  auto close_editors_for_object(const std::string &id) -> bool;

  auto get_focused_plugin_tab() -> PluginEditorBase *;

  auto sidebar_resized(bool primary) -> void;
  virtual auto restore_sidebar_layout(const int firstSidebarDefaultWidth = 200, const int secondSidebarDefaultWidt = 200) -> void;
};

#endif
