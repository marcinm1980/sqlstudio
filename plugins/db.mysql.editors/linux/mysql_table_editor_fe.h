/*
 * Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
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

#ifndef __WB_MYSQL_TABLE_EDITOR_H__
#define __WB_MYSQL_TABLE_EDITOR_H__

#include <gtkmm/notebook.h>
#include "plugin_editor_base.h"
#include "../backend/mysql_table_editor.h"

#include <gtkmm/notebook.h>

class DbMySQLTableEditorColumnPage;
class DbMySQLTableEditorIndexPage;
class DbMySQLTableEditorFKPage;
class DbMySQLTableEditorTriggerPage;
class DbMySQLTableEditorPartPage;
class DbMySQLTableEditorOptPage;
class DbMySQLEditorPrivPage;

//==============================================================================
//
//==============================================================================
class DbMySQLTableEditor : public PluginEditorBase {
  friend class DbMySQLTableEditorColumnPage;

  MySQLTableEditorBE *_be;
  DbMySQLTableEditorColumnPage *_columns_page;
  DbMySQLTableEditorIndexPage *_indexes_page;
  DbMySQLTableEditorFKPage *_fks_page;
  DbMySQLTableEditorTriggerPage *_triggers_page;
  DbMySQLTableEditorPartPage *_part_page;
  DbMySQLTableEditorOptPage *_opts_page;
  mforms::View *_inserts_panel;
  DbMySQLEditorPrivPage *_privs_page;
  Gtk::Widget *_main_page_widget;

  auto create_table_page() -> void;
  auto charset_combo_changed(const std::string &name, const std::string &value) -> void;

  auto refresh_table_page() -> void;
  auto partial_refresh(const int what) -> void;

  auto set_table_collation(Gtk::ComboBoxText *combo) -> void;
  auto set_table_engine(Gtk::ComboBoxText *combo) -> void;

  virtual auto get_be() -> bec::BaseEditor *;

  bool event_from_table_name_entry(GdkEvent *);

  auto page_changed(Gtk::Widget *page, guint page_num) -> void;

  auto set_table_name(const std::string &) -> void;

  // TESTING
  auto refresh_indices() -> void;
  //\TESTING
  auto set_table_option_by_name(const std::string &name, const std::string &value) -> void;
  auto set_comment(const std::string &cmt) -> void;

  auto toggle_header_part() -> void;

protected:
  virtual auto decorate_object_editor() -> void;

public:
  DbMySQLTableEditor(grt::Module *m, const grt::BaseListRef &args);

  virtual ~DbMySQLTableEditor();
  virtual auto do_refresh_form_data() -> void; // That's called from PluginEditorBase::refresh_form_data
                                       // which is passed to the backend refresh slot
  virtual auto can_close() -> bool;
  virtual auto switch_edited_object(const grt::BaseListRef &args) -> bool;
};

#endif
