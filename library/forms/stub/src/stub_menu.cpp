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

#include "../stub_menu.h"

namespace mforms {
  namespace stub {

    //------------------------------------------------------------------------------
    MenuWrapper::MenuWrapper(Menu* self) : ObjectWrapper(self) {
    }

    //------------------------------------------------------------------------------
    auto MenuWrapper::create(Menu* self) -> bool {
      return true;
    }

    //------------------------------------------------------------------------------
    auto MenuWrapper::remove_item(Menu* self, int i) -> void {
    }

    //------------------------------------------------------------------------------
    auto MenuWrapper::add_item(Menu* self, const std::string& caption, const std::string& action) -> int {
      return 0;
    }

    //------------------------------------------------------------------------------
    auto MenuWrapper::add_separator(Menu* self) -> int {
      return 0;
    }

    //------------------------------------------------------------------------------
    auto MenuWrapper::add_submenu(Menu* self, const std::string& caption, Menu* submenu) -> int {
      return 0;
    }

    //------------------------------------------------------------------------------
    auto MenuWrapper::set_item_enabled(Menu* self, int i, bool flag) -> void {
    }

    //------------------------------------------------------------------------------
    auto MenuWrapper::popup_at(Menu* self, Object* control, int x, int y) -> void {
    }

    auto MenuWrapper::clear(Menu* self) -> void {
    }

    //------------------------------------------------------------------------------
    auto MenuWrapper::init() -> void {
      ::mforms::ControlFactory* f = ::mforms::ControlFactory::get_instance();

      f->_menu_impl.create = &MenuWrapper::create;
      f->_menu_impl.remove_item = &MenuWrapper::remove_item;
      f->_menu_impl.add_item = &MenuWrapper::add_item;
      f->_menu_impl.add_separator = &MenuWrapper::add_separator;
      f->_menu_impl.add_submenu = &MenuWrapper::add_submenu;
      f->_menu_impl.set_item_enabled = &MenuWrapper::set_item_enabled;
      f->_menu_impl.popup_at = &MenuWrapper::popup_at;
      f->_menu_impl.clear = &MenuWrapper::clear;
    }
  }
}