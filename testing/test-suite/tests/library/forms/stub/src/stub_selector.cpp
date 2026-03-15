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

#include "../stub_selector.h"

namespace mforms {
  namespace stub {

    //------------------------------------------------------------------------------
    SelectorWrapper::SelectorWrapper(::mforms::Selector *self, ::mforms::SelectorStyle style) : ViewWrapper(self) {
    }

    //------------------------------------------------------------------------------
    SelectorWrapper::~SelectorWrapper() {
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::create(::mforms::Selector *self, ::mforms::SelectorStyle style) -> bool {
      return true;
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::clear(::mforms::Selector *self) -> void {
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::add_item(::mforms::Selector *self, const std::string &item) -> int {
      return 0;
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::add_items(::mforms::Selector *self, const std::list<std::string> &items) -> void {
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::get_item(::mforms::Selector *self, int index) -> std::string {
      return "";
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::get_text(::mforms::Selector *self) -> std::string {
      return "";
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::set_index(::mforms::Selector *self, int index) -> void {
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::get_index(::mforms::Selector *self) -> int {
      return -1;
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::get_item_count(::mforms::Selector *self) -> int {
      return -1;
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::set_value(::mforms::Selector *self, const std::string &) -> void {
    }

    //------------------------------------------------------------------------------
    auto SelectorWrapper::init() -> void {
      ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

      f->_selector_impl.create = &SelectorWrapper::create;
      f->_selector_impl.clear = &SelectorWrapper::clear;
      f->_selector_impl.add_item = &SelectorWrapper::add_item;
      f->_selector_impl.add_items = &SelectorWrapper::add_items;
      f->_selector_impl.get_item = &SelectorWrapper::get_item;
      f->_selector_impl.set_index = &SelectorWrapper::set_index;
      f->_selector_impl.get_index = &SelectorWrapper::get_index;
      f->_selector_impl.get_text = &SelectorWrapper::get_text;
      f->_selector_impl.get_item_count = &SelectorWrapper::get_item_count;
      f->_selector_impl.set_value = &SelectorWrapper::set_value;
    }
  }
}
