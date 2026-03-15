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

#ifndef __LF_MENU_H__
#define __LF_MENU_H__

#include "mforms/base.h"
#include "mforms/menu.h"
#include <gtkmm/menu.h>
#include "lf_base.h"

namespace mforms {
  namespace gtk {

    class MenuImpl : public ObjectImpl {
      Gtk::Menu _menu;

      auto item_at(const int index) -> Gtk::MenuItem *;

      static auto create(Menu *self) -> bool;

      static auto remove_item(Menu *self, int i) -> void;
      static auto add_item(Menu *self, const std::string &caption, const std::string &action) -> int;
      static auto add_separator(Menu *self) -> int;
      static auto add_submenu(Menu *self, const std::string &caption, Menu *submenu) -> int;
      static auto clear(Menu *self) -> void;

      static auto set_item_enabled(Menu *self, int i, bool flag) -> void;

      static auto popup_at(Menu *self, Object *control, int x, int y) -> void;

    public:
      MenuImpl(Menu *self);

      static auto init() -> void;
    }; // class MenuImpl

  } // namespace gtk
} // namespace mforms

#endif
