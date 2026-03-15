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

#include "mforms/mforms.h"

using namespace mforms;

ScrollPanel::ScrollPanel(ScrollPanelFlags flags) : Container() {
  _spanel_impl = &ControlFactory::get_instance()->_spanel_impl;

  _spanel_impl->create(this, flags);
}

ScrollPanel::~ScrollPanel() {
  set_destroying();
  _spanel_impl->remove(this);
}

auto ScrollPanel::add(View* child) -> void {
  cache_view(child);
  _spanel_impl->add(this, child);
  child->show();
}

auto ScrollPanel::remove() -> void {
  _spanel_impl->remove(this);
  clear_subviews();
}

auto ScrollPanel::set_visible_scrollers(bool vertical, bool horizontal) -> void {
  _spanel_impl->set_visible_scrollers(this, vertical, horizontal);
}

auto ScrollPanel::set_autohide_scrollers(bool flag) -> void {
  _spanel_impl->set_autohide_scrollers(this, flag);
}

auto ScrollPanel::scroll_to_view(View* child) -> void {
  if (_spanel_impl->scroll_to_view)
    return _spanel_impl->scroll_to_view(this, child);

  throw std::logic_error("ScrollPanel::scroll_to_view: not implemented");
}

auto ScrollPanel::get_content_rect() -> base::Rect {
  return _spanel_impl->get_content_rect(this);
}

auto ScrollPanel::scroll_to(int x, int y) -> void {
  _spanel_impl->scroll_to(this, x, y);
}
