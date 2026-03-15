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

Selector::Selector(SelectorStyle style) : _updating(false), _editable(style == SelectorCombobox) {
  _selector_impl = &ControlFactory::get_instance()->_selector_impl;

  _selector_impl->create(this, style);
}

auto Selector::clear() -> void {
  _selector_impl->clear(this);
}

auto Selector::add_item(const std::string &item) -> int {
  _updating = true;
  int r = _selector_impl->add_item(this, item);
  _updating = false;
  return r;
}

auto Selector::add_items(const std::list<std::string> &items) -> void {
  _updating = true;
  _selector_impl->add_items(this, items);
  _updating = false;
}

auto Selector::set_selected(int index) -> void {
  _updating = true;
  _selector_impl->set_index(this, index);
  _updating = false;
}

auto Selector::get_item_title(int index) -> std::string {
  return _selector_impl->get_item(this, index);
}

auto Selector::get_string_value() -> std::string {
  return _selector_impl->get_text(this);
}

auto Selector::index_of_item_with_title(const std::string &title) -> int {
  for (int i = 0; i < get_item_count(); i++)
    if (get_item_title(i) == title)
      return i;
  return -1;
}

auto Selector::get_selected_index() -> int {
  return _selector_impl->get_index(this);
}

auto Selector::get_item_count() -> int {
  return _selector_impl->get_item_count(this);
}

auto Selector::callback() -> void {
  if (!_updating)
    _signal_changed();
}

auto Selector::set_value(const std::string &value) -> void {
  // Try to set text from the list
  // Otherwise force text to be set (only for SelectorCombo)
  const int i = index_of_item_with_title(value);
  if (i >= 0)
    set_selected(i);
  else if (_editable)
    _selector_impl->set_value(this, value);
}
