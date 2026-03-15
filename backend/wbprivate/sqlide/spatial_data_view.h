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

#pragma once

#include "studio/wb_backend_public_interface.h"

#include "sqlide/recordset_be.h"

#include "mforms/box.h"
#include "mforms/utilities.h"
#include "mforms/splitter.h"
#include "mforms/treeview.h"

#include <deque>

namespace mforms {
  class ToolBar;
  class ToolBarItem;
  class Selector;
  class TreeView;
  struct TreeNodeRef;
  class Label;
  class ContextMenu;
  class MenuItem;
  class TextBox;
};

class SpatialDrawBox;
class SqlEditorResult;
class SqlEditorForm;
class ProgressPanel;

typedef int
  LayerId; // must be the same as spatial::LayerId, which we can't import b/c gdal creates some weird dependencies

class SpatialDataView : public mforms::Box {
public:
  struct SpatialDataSource {
    std::string source;
    Recordset::Ptr resultset;
    std::string column;
    int column_index;
    std::string type;
  };

private:
  SqlEditorResult *_owner;
  bool _activated;
  mforms::ToolBar *_toolbar;

  mforms::Box *_main_box;
  mforms::Box *_option_box;
  mforms::Splitter *_splitter;

  mforms::ToolBarItem *_projection_picker;
  mforms::TreeView *_layer_tree;
  mforms::ContextMenu *_layer_menu;
  mforms::ContextMenu *_map_menu;

  mforms::TextBox *_info_box;

  LayerId _active_layer;
  LayerId _grid_layer;

  mforms::Label *_mouse_pos_label;

  SpatialDrawBox *_viewer;

  mforms::TimeoutHandle _spliter_change_timeout;
  bool _rendering;

  auto call_refresh_viewer() -> void;

  auto refresh_viewer() -> bool;

  auto tree_toggled(const mforms::TreeNodeRef &node, const std::string &value) -> void;

  auto set_color_icon(mforms::TreeNodeRef node, int column, const base::Color &color) -> void;

  auto work_started(mforms::View *progress, bool reprojecting) -> void;
  auto work_finished(mforms::View *progress) -> void;

  auto update_coordinates(base::Point p) -> void;
  auto handle_click(base::Point p) -> void;

  auto jump_to() -> void;
  auto export_image() -> void;
  auto auto_zoom(LayerId layer) -> void;
  auto copy_coordinates() -> void;

  auto change_tool(mforms::ToolBarItem *item) -> void;

  auto layer_overlay_handler(mforms::TreeNodeRef node) -> std::vector<std::string>;

  // layer currently selected in the treeview
  auto get_selected_layer_id() -> LayerId;
  // layer that's currently set as the active one (bolded in treeview)
  class RecordsetLayer *active_layer();
  auto set_active_layer(LayerId layer) -> void;

  auto row_id_for_action(class RecordsetLayer *&layer) -> int;
  auto copy_record() -> void;
  auto view_record() -> void;

  auto map_menu_will_show() -> void;
  auto layer_menu_will_show() -> void;

  auto area_selected() -> void;
  auto activate_layer(mforms::TreeNodeRef, int column) -> void;

public:
  SpatialDataView(SqlEditorResult *owner);
  virtual ~SpatialDataView();

  auto get_toolbar() -> mforms::ToolBar * {
    return _toolbar;
  }

  auto set_geometry_columns(const std::vector<SpatialDataSource> &columns) -> void;
  auto get_option(const char *opt_name, int default_value) -> int;

  auto fillup_polygon(mforms::MenuItem *mitem) -> void;
  auto projection_item_activated(mforms::ToolBarItem *item) -> void;

  auto activate() -> void;
  auto refresh_layers() -> void;

  auto layer_menu_action(const std::string &action) -> void;
};
