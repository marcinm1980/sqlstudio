/*
 * Copyright (c) 2010, 2018, Oracle and/or its affiliates. All rights reserved.
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

Splitter::Splitter(bool horiz, bool thin) {
  _splitter_impl = &ControlFactory::get_instance()->_splitter_impl;

#ifdef __APPLE__
  _splitter_impl->create(this, horiz, thin);
#else
  _splitter_impl->create(this, horiz);
#endif
}

auto Splitter::add(View *subview, int minsize, bool fixed) -> void {
  cache_view(subview);
  _splitter_impl->add(this, subview, minsize, fixed);
}

auto Splitter::remove(View *subview) -> void {
  _splitter_impl->remove(this, subview);
  remove_from_cache(subview);
}

auto Splitter::set_divider_position(int pos) -> void {
  _splitter_impl->set_divider_position(this, pos);
}

auto Splitter::get_divider_position() -> int {
  return _splitter_impl->get_divider_position(this);
}

auto Splitter::set_expanded(bool first, bool expand) -> void {
  _splitter_impl->set_expanded(this, first, expand);
}

auto Splitter::position_changed() -> void {
  _position_changed_signal();
}
