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

#include "../stub_listbox.h"

using namespace mforms;
using namespace mforms::stub;

//--------------------------------------------------------------------------------------------------

ListBoxWrapper::ListBoxWrapper(mforms::ListBox *self, bool multi_select) : ViewWrapper(self) {
}


//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::create(mforms::ListBox *self, bool multi_select) -> bool {
  return true;
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::clear(mforms::ListBox *self) -> void {
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::add_item(mforms::ListBox *self, const std::string &item) -> size_t {
  return 0;
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::add_items(mforms::ListBox *self, const std::list<std::string> &items) -> void {
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::remove_index(mforms::ListBox *self, size_t index) -> void {
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::remove_indexes(mforms::ListBox *self, const std::vector<size_t> &items) -> void {
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::get_text(mforms::ListBox *self) -> std::string {
  return "";
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::set_index(mforms::ListBox *self, ssize_t index) -> void {
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::get_index(mforms::ListBox *self) -> ssize_t {
  return -1;
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::set_heading(ListBox *self, const std::string &text) -> void {
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::get_selected_indices(ListBox *self) -> std::vector<size_t> {
  std::vector<size_t> res;
  return res;
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::getCount(ListBox *self) -> size_t {
  return 0;
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::getStringValueFromIndex(ListBox *self, size_t index) -> std::string {
  return "";
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::init() -> void {
  mforms::ControlFactory *f = mforms::ControlFactory::get_instance();

  f->_listbox_impl.create = &ListBoxWrapper::create;
  f->_listbox_impl.clear = &ListBoxWrapper::clear;
  f->_listbox_impl.add_item = &ListBoxWrapper::add_item;
  f->_listbox_impl.add_items = &ListBoxWrapper::add_items;
  f->_listbox_impl.remove_index = &ListBoxWrapper::remove_index;
  f->_listbox_impl.remove_indexes = &ListBoxWrapper::remove_indexes;
  f->_listbox_impl.set_index = &ListBoxWrapper::set_index;
  f->_listbox_impl.get_index = &ListBoxWrapper::get_index;
  f->_listbox_impl.get_selected_indices = &ListBoxWrapper::get_selected_indices;
  f->_listbox_impl.get_text = &ListBoxWrapper::get_text;
  f->_listbox_impl.set_heading = &ListBoxWrapper::set_heading;
  f->_listbox_impl.get_count = &ListBoxWrapper::getCount;
  f->_listbox_impl.get_string_value_from_index = &ListBoxWrapper::getStringValueFromIndex;
}

//--------------------------------------------------------------------------------------------------
