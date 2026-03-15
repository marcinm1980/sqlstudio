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

#ifndef _MFORMS_LF_MENUBAR_H_
#define _MFORMS_LF_MENUBAR_H_

#include <mforms/menubar.h>
#include <glibmm/refptr.h>

namespace Gtk {
  class MenuBar;
  class AccelGroup;
}

namespace mforms {
  auto widget_for_menubar(MenuBar *self) -> Gtk::MenuBar *;
  auto on_add_menubar_to_window(MenuBar *menu, Gtk::Window *window) -> void;

  namespace gtk {
    auto lf_menubar_init() -> void;

    struct MenuItemImpl {
      static auto create_menu_bar(MenuBar *item) -> bool;
      static auto create_context_menu(ContextMenu *item) -> bool;
      static auto create_menu_item(MenuItem *item, const std::string &, const MenuItemType type) -> bool;
      static auto copy_menu_item(MenuItem *item, const MenuItem *other) -> bool;
      static auto set_title(MenuItem *item, const std::string &) -> void;
      static auto get_title(MenuItem *item) -> std::string;
      static auto set_shortcut(MenuItem *item, const std::string &) -> void;
      static auto set_enabled(MenuBase *item, bool) -> void;
      static auto get_enabled(MenuBase *item) -> bool;
      static auto set_checked(MenuItem *item, bool) -> void;
      static auto get_checked(MenuItem *item) -> bool;

      static auto insert_item(MenuBase *menu, int index, MenuItem *item) -> void;
      static auto remove_item(MenuBase *menu, MenuItem *item) -> void; // NULL item to remove all
      static auto popup_menu(mforms::ContextMenu *menu, View *owner, base::Point location) -> void;
      
      static auto set_name(MenuItem *item, const std::string &) -> void;
      static auto get_name(MenuItem *item) -> std::string;
    };

  }; // namespace gtk
} // namespace mforms

#endif
