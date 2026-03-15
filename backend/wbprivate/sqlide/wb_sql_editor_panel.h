/*
 * Copyright (c) 2014, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef __MySQLMySqlStudio__wb_sql_editor_panel__
#define __MySQLMySqlStudio__wb_sql_editor_panel__

#include "studio/wb_backend_public_interface.h"

#include "mforms/appview.h"
#include "mforms/box.h"
#include "mforms/splitter.h"
#include "mforms/tabview.h"
#include "mforms/tabview_dock.h"
#include "mforms/button.h"
#include "mforms/label.h"
#include "mforms/imagebox.h"
#include "mforms/dockingpoint.h"
#include "mforms/menubar.h"

#include <boost/signals2.hpp>

namespace mforms {
  class TabView;
  class TabSwitcher;
  class ToolBar;
  class ToolBarItem;
  class ContextMenu;
  class TreeView;
  class RecordGrid;
};

class SqlEditorForm;
class MySQLEditor;

class SqlEditorResult;

class MYSQLWBBACKEND_PUBLIC_FUNC SqlEditorPanel : public mforms::AppView {
  friend class SqlEditorResult;

  SqlEditorForm *_form;
  std::shared_ptr<MySQLEditor> _editor;

  mforms::Box _editor_box;

  mforms::Splitter _splitter;
  mforms::TabView _lower_tabview;
  mforms::TabViewDockingPoint _lower_dock_delegate;
  mforms::DockingPoint _lower_dock;
  mforms::ContextMenu _lower_tab_menu;

  mforms::Box _tab_action_box;
  mforms::Button _tab_action_apply;
  mforms::Button _tab_action_revert;
  mforms::ImageBox _tab_action_icon;
  mforms::Label _tab_action_info;

  std::string _title;
  std::string _filename;
  std::string _orig_encoding;
  std::string _caption;

  std::string _autosave_file_suffix;

  time_t _file_timestamp;

  int _rs_sequence;

  bool _busy;
  bool _was_empty;
  bool _is_scratch;

  auto setup_editor_toolbar() -> mforms::ToolBar *;
  auto update_title() -> void;

  auto dock_result_panel(SqlEditorResult *result) -> void;
  auto show_find_panel(mforms::CodeEditor *editor, bool show) -> void;

  auto dispose_recordset(Recordset::Ptr rs_ptr) -> void;
  auto on_close_by_user() -> bool;
  auto on_recordset_context_menu_show(Recordset::Ptr rs_ptr) -> void;

  auto lower_tab_switched() -> void;
  auto lower_tab_closing(int tab) -> bool;
  auto lower_tab_closed(mforms::View *page, int tab) -> void;
  void lower_tab_reordered(mforms::View *, int, int);

  auto result_removed() -> void;

  auto apply_clicked() -> void;
  auto revert_clicked() -> void;

  auto resultset_edited() -> void;
  auto splitter_resized() -> void;

  auto tab_menu_will_show() -> void;
  auto rename_tab_clicked() -> void;
  auto pin_tab_clicked() -> void;
  auto close_tab_clicked() -> void;
  auto close_other_tabs_clicked() -> void;

  auto is_pinned(int tab) -> bool;
  auto tab_pinned(int tab, bool flag) -> void;

  void limit_rows(mforms::ToolBarItem *);

public:
  typedef std::shared_ptr<SqlEditorPanel> Ref;
  SqlEditorPanel(SqlEditorForm *owner, bool is_scratch, bool start_collapsed);
  ~SqlEditorPanel();

  auto editor_be() -> std::shared_ptr<MySQLEditor> {
    return _editor;
  }
  auto grtobj() -> db_query_QueryEditorRef;

  auto get_toolbar() -> mforms::ToolBar *;
  virtual auto set_title(const std::string &title) -> void;

  auto update_limit_rows() -> void;

  auto owner() -> SqlEditorForm * {
    return _form;
  }

  auto is_scratch() -> bool {
    return _is_scratch;
  }

public:
  struct AutoSaveInfo {
    std::string orig_encoding;
    std::string type;
    std::string title;
    std::string filename;
    size_t first_visible_line;
    size_t caret_pos;
    bool word_wrap;
    bool show_special;

    AutoSaveInfo() : first_visible_line(0), caret_pos(0), word_wrap(false), show_special(false) {
    }
    AutoSaveInfo(const std::string &info_file);

    static auto old_scratch(const std::string &scratch_file) -> AutoSaveInfo;
    static auto old_autosave(const std::string &autosave_file) -> AutoSaveInfo;
  };

  enum LoadResult { Cancelled, Loaded, RunInstead };

  LoadResult load_from(const std::string &file, const std::string &encoding = "", bool keep_dirty = false);
  auto load_autosave(const AutoSaveInfo &info, const std::string &text_file) -> bool;

  virtual auto can_close() -> bool;
  virtual auto close() -> void;

  auto save() -> bool;
  auto save_as(const std::string &file) -> bool;
  auto revert_to_saved() -> void;

  auto auto_save(const std::string &directory) -> void;
  auto delete_auto_save(const std::string &directory) -> void;
  auto autosave_file_suffix() -> std::string;

  auto set_filename(const std::string &f) -> void;
  auto filename() const -> std::string {
    return _filename;
  }

  auto is_dirty() const -> bool;
  auto check_external_file_changes() -> void;

  auto text_data() const -> std::pair<const char *, std::size_t>;

  auto list_members() -> void;
  auto jump_to_placeholder() -> void;

public:
  auto query_started(bool retain_old_recordsets) -> void;
  auto query_finished() -> void;
  auto query_failed(const std::string &message) -> void;

  // recordset management
  auto active_result_panel() -> SqlEditorResult *;

  auto result_panel(int i) -> SqlEditorResult *;

  auto result_panel_count() -> size_t;
  auto resultset_count() -> size_t;

  auto add_panel_for_recordset(Recordset::Ref rset) -> SqlEditorResult *;
  auto add_panel_for_recordset_from_main(Recordset::Ref rset) -> void;

  auto dirty_result_panels() -> std::list<SqlEditorResult *>;
};

#endif /* defined(__MySQLMySqlStudio__wb_sql_editor_panel__) */
