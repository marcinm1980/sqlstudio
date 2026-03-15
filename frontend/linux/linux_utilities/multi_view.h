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

#ifndef _MULTIVIEW_H_
#define _MULTIVIEW_H_

#include <gtkmm/grid.h>
#include "treemodel_wrapper.h"
#include "grt/tree_model.h"
#include "editable_iconview.h"

// Because of Gtk3 bug: https://bugzilla.gnome.org/show_bug.cgi?id=749575
// we need to use Gtk::Grid instead of Gtk::Box parent of MultiView

class MultiView : public Gtk::Grid {
  Gtk::TreeView *_tree_view;
  EditableIconView *_icon_view;
  Glib::RefPtr<TreeModelWrapper> _tv_model;
  Glib::RefPtr<TreeModelWrapper> _iv_model;
  Glib::RefPtr<Gtk::TreeSelection> _selection;

  sigc::signal<void, const std::vector<bec::NodeId> &> _selection_changed;
  sigc::signal<void, Gtk::TreeModel::Path, guint32> _popup_menu;
  sigc::signal<void, Gtk::TreeModel::Path> _activate_item;

  auto tree_row_activated(const Gtk::TreeModel::Path &path, const Gtk::TreeViewColumn *column) -> void;
  auto icon_activated(const Gtk::TreeModel::Path &path) -> void;
  auto icon_button_release_event(GdkEventButton *event) -> void;
  auto tree_button_release_event(GdkEventButton *event) -> void;
  auto icon_selection_changed() -> void;
  auto tree_selection_changed() -> void;

protected:
  virtual auto on_selection_changed(const std::vector<bec::NodeId> &sel) -> void;

public:
  MultiView(bool tree_view, bool icon_view);
  virtual ~MultiView();

  auto get_tree_view() const -> Gtk::TreeView * {
    return _tree_view;
  }
  auto get_icon_view() const -> Gtk::IconView * {
    return _icon_view;
  }

  virtual auto refresh() -> void;
  auto set_tree_model(const Glib::RefPtr<TreeModelWrapper> &model) -> void;
  auto set_icon_model(const Glib::RefPtr<TreeModelWrapper> &model) -> void;
  auto unset_models() -> void;
  auto get_tree_model() -> Glib::RefPtr<TreeModelWrapper> {
    return _tv_model;
  }
  auto get_icon_model() -> Glib::RefPtr<TreeModelWrapper> {
    return _iv_model;
  }

  auto set_icon_mode(bool flag, bool horizontal_icons = false) -> void;

  auto get_selected() -> Gtk::TreeModel::Path;
  auto select_node(const bec::NodeId &node) -> void;

  auto signal_selection_changed() -> sigc::signal<void, const std::vector<bec::NodeId> &> {
    return _selection_changed;
  }
  auto signal_popup_menu() -> sigc::signal<void, Gtk::TreeModel::Path, guint32> {
    return _popup_menu;
  }
  auto signal_activate_item() -> sigc::signal<void, Gtk::TreeModel::Path> {
    return _activate_item;
  }
};

#endif /* _MULTIVIEW_H_ */

//!
//! @}
//!
