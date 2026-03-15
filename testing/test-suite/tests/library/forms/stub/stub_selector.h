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

#ifndef _STUB_SELECTOR_H_
#define _STUB_SELECTOR_H_

#include "stub_view.h"

namespace mforms {
  namespace stub {

    class SelectorWrapper : public ViewWrapper {
    protected:
      SelectorWrapper(::mforms::Selector *self, ::mforms::SelectorStyle style);
      static auto callback(::mforms::Selector *self) -> void;
      static auto create(::mforms::Selector *self, ::mforms::SelectorStyle style) -> bool;
      static auto clear(::mforms::Selector *self) -> void;
      static auto add_item(::mforms::Selector *self, const std::string &item) -> int;
      static auto add_items(::mforms::Selector *self, const std::list<std::string> &items) -> void;
      static auto get_item(::mforms::Selector *self, int index) -> std::string;
      static auto get_text(::mforms::Selector *self) -> std::string;
      static auto set_index(::mforms::Selector *self, int index) -> void;
      static auto get_index(::mforms::Selector *self) -> int;
      static auto get_item_count(::mforms::Selector *self) -> int;
      static auto set_value(::mforms::Selector *self, const std::string &) -> void;

    public:
      ~SelectorWrapper();
      static auto init() -> void;
    };
  }
}

#endif
