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

#include "mforms/dockingpoint.h"
#include "mforms/tabview.h"

namespace mforms {

  class MFORMS_EXPORT TabViewDockingPoint : public mforms::DockingPointDelegate {
  protected:
    TabView *_tabview;
    std::string _type;

    auto close_page(mforms::AppView *page) -> void;

  public:
    TabViewDockingPoint(TabView *tv, const std::string &type) : _tabview(tv), _type(type) {
    }

    virtual auto get_type() -> std::string {
      return _type;
    }

    virtual auto set_name(const std::string &name) -> void;
    virtual auto dock_view(mforms::AppView *view, const std::string &arg1, int arg2) -> void;
    virtual auto select_view(mforms::AppView *view) -> bool;
    virtual auto undock_view(mforms::AppView *view) -> void;
    virtual auto set_view_title(mforms::AppView *view, const std::string &title) -> void;
    virtual auto get_size() -> std::pair<int, int>;

    virtual auto selected_view() -> AppView *;
    virtual auto view_count() -> int;
    virtual auto view_at_index(int index) -> AppView *;
  };
};
