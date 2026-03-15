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

//--------------------------------------------------------------------------------------------------

ListBox::ListBox(bool multi_select) : _updating(false) {
  _listbox_impl = &ControlFactory::get_instance()->_listbox_impl;

  _listbox_impl->create(this, multi_select);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::clear() -> void {
  _updating = true;
  _listbox_impl->clear(this);
  _updating = false;
}

//--------------------------------------------------------------------------------------------------

auto ListBox::set_heading(const std::string &text) -> void {
  _listbox_impl->set_heading(this, text);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::add_item(const std::string &item) -> size_t {
  return _listbox_impl->add_item(this, item);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::add_items(const std::list<std::string> &items) -> void {
  _listbox_impl->add_items(this, items);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::remove_index(size_t index) -> void {
  _listbox_impl->remove_index(this, index);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::remove_indexes(const std::vector<size_t> &indexes) -> void {
  _listbox_impl->remove_indexes(this, indexes);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::set_selected(ssize_t index) -> void {
  _updating = true;
  _listbox_impl->set_index(this, index);
  _updating = false;
}

//--------------------------------------------------------------------------------------------------

auto ListBox::get_string_value() -> std::string {
  return _listbox_impl->get_text(this);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::get_selected_index() -> ssize_t {
  return _listbox_impl->get_index(this);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::get_selected_indices() -> std::vector<size_t> {
  return _listbox_impl->get_selected_indices(this);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::selection_changed() -> void {
  if (!_updating)
    _signal_changed();
}

//--------------------------------------------------------------------------------------------------

auto ListBox::get_count() -> size_t {
  return _listbox_impl->get_count(this);
}

//--------------------------------------------------------------------------------------------------

auto ListBox::get_string_value_from_index(size_t index) -> std::string {
  return _listbox_impl->get_string_value_from_index(this, index);
}
