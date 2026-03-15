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

//!
//! \addtogroup linuxui Linux UI
//! @{
//!

#ifndef _OVERVIEW_PANEL_H_
#define _OVERVIEW_PANEL_H_

#include <map>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/box.h>
#include <gtkmm/menu.h>

#include "studio/wb_overview.h"
#include "studio/wb_context_ui.h"

class OverviewDivision;
class OverviewSection;
class OverviewItemContainer;
class OverviewGroupContainer;

class OverviewGroupContainer;
class MultiView;

class OverviewPanel : public Gtk::ScrolledWindow {
public:
  OverviewPanel(wb::OverviewBE *overview);

  auto reset() -> void;
  auto rebuild_all() -> void;
  auto update_for_resize() -> void;

  auto select_node(const bec::NodeId &node) -> void;
  auto refresh_node(const bec::NodeId &node) -> void;
  auto refresh_children(const bec::NodeId &node) -> void;

  auto get_be() -> wb::OverviewBE * {
    return _overview_be;
  }

  auto item_popup_menu(const Gtk::TreeModel::Path &path, guint32 time, OverviewItemContainer *sender) -> void;

  virtual auto on_close() -> bool {
    if (!_overview_be->can_close())
      return false;
    _overview_be->close();
    return false;
  }

  auto select_default_group_page() -> void;
  auto refresh_active_group_node_children() -> void;

private:
  Gtk::Box *_container;
  OverviewGroupContainer *_groups;
  Gtk::Menu _context_menu;

  std::map<std::string, OverviewGroupContainer *> _group_containers_by_id;
  std::map<std::string, OverviewItemContainer *> _item_containers_by_id;

  wb::OverviewBE *_overview_be;

  bool _freeze;
  bool _rebuilding;

  auto pre_refresh_groups() -> void;

  auto update_group_note(OverviewGroupContainer *group_container, const bec::NodeId &node) -> void;

  auto build_division(Gtk::Box *container, const bec::NodeId &node) -> void;
  void build_group(OverviewDivision *division, OverviewGroupContainer *group_container, const bec::NodeId &node,
                   int position = -1);
  auto build_group_contents(OverviewDivision *division, Gtk::Box *page, const bec::NodeId &node) -> void;

  auto item_list_selection_changed(const std::vector<bec::NodeId> &nodes, MultiView *mview) -> void;
};

#endif /* _OVERVIEW_PANEL_H_ */

//!
//! @}
//!
