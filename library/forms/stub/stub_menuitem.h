/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _STUB_MENUITEM_H_
#define _STUB_MENUITEM_H_

#include "stub_base.h"

namespace mforms {
  namespace stub {

    class MenuItemWrapper : public ObjectWrapper {
    protected:
      MenuItemWrapper(::mforms::MenuItem *self) : ObjectWrapper(self) {
      }

      static auto create_menu_bar(MenuBar *item) -> bool {
        return true;
      }

      static auto create_menu_item(MenuItem *item, const std::string &, const MenuItemType type) -> bool {
        return true;
      }

      static auto set_title(MenuItem *item, const std::string &) -> void {
      }

      static auto set_name(MenuItem *item, const std::string &) -> void {
      }

      static auto get_title(MenuItem *item) -> std::string {
        return "";
      }

      static auto set_shortcut(MenuItem *item, const std::string &) -> void {
      }

      static auto set_enabled(MenuBase *item, bool) -> void {
      }

      static auto get_enabled(MenuBase *item) -> bool {
        return true;
      }

      static auto set_checked(MenuItem *item, bool) -> void {
      }

      static auto get_checked(MenuItem *item) -> bool {
        return true;
      }

      static auto insert_item(MenuBase *menu, int index, MenuItem *item) -> void {
      }

      static auto remove_item(MenuBase *menu, MenuItem *item) -> void {
      }

      static auto create_context_menu(mforms::ContextMenu *item) -> bool {
        return false;
      }

    public:
      static auto init() -> void {
        ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

        f->_menu_item_impl.create_menu_bar = &MenuItemWrapper::create_menu_bar;
        f->_menu_item_impl.create_menu_item = &MenuItemWrapper::create_menu_item;
        f->_menu_item_impl.get_checked = &MenuItemWrapper::get_checked;
        f->_menu_item_impl.get_enabled = &MenuItemWrapper::get_enabled;
        f->_menu_item_impl.get_title = &MenuItemWrapper::get_title;
        f->_menu_item_impl.insert_item = &MenuItemWrapper::insert_item;
        f->_menu_item_impl.remove_item = &MenuItemWrapper::remove_item;
        f->_menu_item_impl.set_checked = &MenuItemWrapper::set_checked;
        f->_menu_item_impl.set_enabled = &MenuItemWrapper::set_enabled;
        f->_menu_item_impl.set_shortcut = &MenuItemWrapper::set_shortcut;
        f->_menu_item_impl.set_title = &MenuItemWrapper::set_title;
        f->_menu_item_impl.set_name = &MenuItemWrapper::set_name;
        f->_menu_item_impl.create_context_menu = &MenuItemWrapper::create_context_menu;
      }
    };
  }
}

#endif
