/*
 * Copyright (c) 2011, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _MFORMS_LF_TOOLBAR_H_
#define _MFORMS_LF_TOOLBAR_H_

#include "mforms/toolbar.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <gtkmm.h>
#pragma GCC diagnostic pop

namespace mforms {
  Gtk::Widget *widget_for_toolbar(mforms::ToolBar *);
  auto widget_for_toolbar_item_named(mforms::ToolBar *, const std::string &) -> Gtk::Widget *;

  namespace gtk {

    auto lf_toolbar_init() -> void;
    struct ToolBarImpl {
      static auto create_tool_bar(ToolBar *item, ToolBarType type) -> bool;
      static auto insert_item(ToolBar *toolbar, int index, ToolBarItem *item) -> void;
      static auto remove_item(ToolBar *toolbar, ToolBarItem *item) -> void;

      static auto create_tool_item(ToolBarItem *item, ToolBarItemType type) -> bool;
      static auto set_item_icon(ToolBarItem *item, const std::string &) -> void;
      static auto set_item_alt_icon(ToolBarItem *item, const std::string &) -> void;
      static auto set_item_text(ToolBarItem *item, const std::string &) -> void;
      static auto get_item_text(ToolBarItem *item) -> std::string;
      static auto set_item_name(ToolBarItem *item, const std::string &) -> void;
      static auto set_item_enabled(ToolBarItem *item, bool) -> void;
      static auto get_item_enabled(ToolBarItem *item) -> bool;
      static auto set_item_checked(ToolBarItem *item, bool) -> void;
      static auto get_item_checked(ToolBarItem *item) -> bool;
      static auto set_item_tooltip(ToolBarItem *item, const std::string &) -> void;

      static auto set_selector_items(ToolBarItem *item, const std::vector<std::string> &values) -> void;
    };

  } // ns gtk
} // ns mforms

#endif
