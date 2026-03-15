/*
 * Copyright (c) 2007, 2019, Oracle and/or its affiliates. All rights reserved.
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

#ifndef __DB_SQL_EDITOR_VIEW_H__
#define __DB_SQL_EDITOR_VIEW_H__

#include "mforms/toolbar.h"
#include "sqlide/wb_sql_editor_form.h"
#include "gtk_helpers.h"
#include "form_view_base.h"
#include "notebook_dockingpoint.h"
#include "overview_panel.h"
#include "active_label.h"
#include "sqlide_output_view.h"
#include <glib.h>

class SqlSnippetsView;
class ToolbarManager;
class QueryView;

//==============================================================================
//
//==============================================================================
class DbSqlEditorView : public Gtk::Box, public FormViewBase {
public:
  DbSqlEditorView(SqlEditorForm::Ref editor_be);
  static auto create(SqlEditorForm::Ref editor_be) -> DbSqlEditorView *;
  virtual ~DbSqlEditorView();

  virtual auto init() -> void;
  virtual auto on_close() -> bool;
  virtual auto dispose() -> void;

  virtual auto get_be() -> bec::BaseEditor * {
    return NULL;
  }
  virtual auto get_form() const -> bec::UIForm * {
    return _be.get();
  }
  virtual auto get_panel() -> Gtk::Widget * {
    return this;
  }
  // virtual void toggle_sidebar();

  virtual auto perform_command(const std::string &command) -> bool;

  auto be() -> SqlEditorForm::Ref {
    return _be;
  }

  auto close_appview_tab(mforms::AppView *aview) -> void;
  auto output_text(const std::string &text, bool bring_to_front) -> void;

  virtual auto close_focused_tab() -> bool;

protected:
  virtual auto plugin_tab_added(PluginEditorBase *plugin) -> void;

private:
  auto polish() -> void;
  void set_busy_tab(int);
  auto on_exec_sql_done() -> void;

  auto editor_page_switched(Gtk::Widget *page, guint index) -> void;
  auto editor_page_reordered(Gtk::Widget *page, guint index) -> void;
  auto editor_page_added(Gtk::Widget *page, guint index) -> void;
  auto editor_page_removed(Gtk::Widget *page, guint index) -> void;

  auto init_tab_menu(Gtk::Widget *w) -> mforms::Menu *;
  auto tab_menu_handler(const std::string &action, ActiveLabel *sender, Gtk::Widget *widget) -> void;
  auto reenable_items_in_tab_menus() -> void;

  auto set_maximized_editor_mode(bool flag, bool hide_schemas = false) -> void;

  SqlEditorForm::Ref _be;
  Gtk::Paned _top_pane;
  Gtk::Paned _top_right_pane;
  Gtk::Paned _main_pane;
  QueryOutputView _output;
  Gtk::Widget *_side_palette;

  sigc::connection _polish_conn;
  sigc::connection _sig_restore_sidebar;

  NotebookDockingPoint _dock_delegate;
  mforms::DockingPoint *_dpoint;

  ActiveLabel *_busy_tab;

  const bool _right_aligned;
  bool _editor_maximized;
};

#endif // __DB_SQL_EDITOR_VIEW_H__
