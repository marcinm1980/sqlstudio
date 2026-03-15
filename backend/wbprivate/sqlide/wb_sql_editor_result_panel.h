/*
 * Copyright (c) 2013, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "base/notifications.h"

#include "studio/wb_backend_public_interface.h"

#include "mforms/appview.h"
#include "mforms/tabview_dock.h"
#include "mforms/tabswitcher.h"

#include "sqlide/recordset_be.h"

#include "grts/structs.db.query.h"

#include <boost/signals2.hpp>

#include "spatial_data_view.h"
#include "wb_sql_editor_panel.h"

namespace mforms {
  class ToolBar;
  class ToolBarItem;
  class ContextMenu;
  class TreeView;
  class GridView;
  class ScrollPanel;
};

class ResultFormView;

class MYSQLWBBACKEND_PUBLIC_FUNC SqlEditorResult : public mforms::AppView, public base::Observer {
  SqlEditorPanel *_owner;
  Recordset::Ptr _rset;

  class DockingDelegate;

public:
  SqlEditorResult(SqlEditorPanel *owner);
  auto set_recordset(Recordset::Ref rset) -> void;

  virtual ~SqlEditorResult();

  auto recordset() const -> Recordset::Ref;

  auto caption() const -> std::string;

  auto grtobj() -> db_query_ResultPanelRef {
    return _grtobj;
  }
  auto result_grtobj() -> db_query_ResultsetRef {
    return _grtobj->resultset();
  }

  virtual auto can_close() -> bool;
  virtual auto close() -> void;

  auto show_export_recordset() -> void;
  auto show_import_recordset() -> void;
  auto dock_result_grid(mforms::View *view) -> void;
  //  mforms::View *result_grid() { return _result_grid; }

  auto owner() -> SqlEditorPanel * {
    return _owner;
  }

  auto get_spatial_columns() -> std::vector<SpatialDataView::SpatialDataSource>;

  auto result_grid() -> mforms::GridView * {
    return _result_grid;
  }

  auto dock() -> mforms::DockingPoint * {
    return &_tabdock;
  }

  auto apply_changes() -> void;
  auto discard_changes() -> void;
  auto has_pending_changes() -> bool;

  virtual auto set_title(const std::string &title) -> void;

  auto set_pinned(bool flag) -> void {
    _pinned = flag;
  }
  auto pinned() const -> bool {
    return _pinned;
  }

  auto view_record_in_form(int row_id) -> void;

  auto open_field_editor(int row, int column) -> void;

private:
  mforms::TabView _tabview;
  mforms::TabSwitcher _switcher;
  DockingDelegate *_tabdock_delegate;
  mforms::DockingPoint _tabdock;

  mforms::AppView *_column_info_box;
  mforms::AppView *_query_stats_box;
  mforms::ScrollPanel *_query_stats_panel;
  mforms::AppView *_resultset_placeholder;
  mforms::AppView *_execution_plan_placeholder;
  ResultFormView *_form_result_view;
  SpatialDataView *_spatial_result_view;
  mforms::ContextMenu *_column_info_menu;
  mforms::ContextMenu *_grid_header_menu;
  std::list<mforms::ToolBar *> _toolbars;
  mforms::GridView *_result_grid;
  boost::signals2::signal<void(bool)> _collapse_toggled;
  boost::signals2::connection _collapse_toggled_sig;

  db_query_ResultPanelRef _grtobj;

  std::vector<std::string> _column_width_storage_ids;

  bool _column_info_created;
  bool _query_stats_created;
  bool _form_view_created;
  bool _spatial_view_initialized;

  bool _pinned;

  auto handle_notification(const std::string &name, void *sender, base::NotificationInfo &info) -> void;
  auto updateColors() -> void;

  auto update_selection_for_menu_extra(mforms::ContextMenu *menu, const std::vector<int> &rows, int column) -> void;
  auto switch_tab() -> void;

  auto toggle_switcher_collapsed() -> void;
  auto switcher_collapsed() -> void;

  auto create_query_stats_panel() -> void;
  auto create_column_info_panel() -> void;
  auto create_spatial_view_panel_if_needed() -> void;

  auto dock_result_grid(mforms::GridView *view) -> void;

  auto restore_grid_column_widths() -> void;
  auto get_autofit_column_widths(Recordset *rs) -> std::vector<float>;
  auto reset_column_widths() -> void;

  auto add_switch_toggle_toolbar_item(mforms::ToolBar *tbar) -> void;

  auto copy_column_info_name(mforms::TreeView *tree) -> void;
  auto copy_column_info(mforms::TreeView *tree) -> void;

  auto copy_column_name() -> void;
  auto copy_all_column_names() -> void;

  auto reset_sorting() -> void;
  auto on_recordset_column_resized(int column) -> void;
  auto onRecordsetColumnsResized(const std::vector<int> cols) -> void;
};
