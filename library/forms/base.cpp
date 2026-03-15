/*
 * Copyright (c) 2012, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "mforms/base.h"
#include "mforms/mforms.h"

using namespace mforms;

//--------------------------------------------------------------------------------------------------

auto Object::retain() -> Object* {
  g_atomic_int_inc(&_refcount);
  return this;
}

//--------------------------------------------------------------------------------------------------

auto Object::release() -> void {
  if (g_atomic_int_dec_and_test(&_refcount) && _managed) {
    _destroying = true;
    delete this;
  }
}

//--------------------------------------------------------------------------------------------------

auto Object::set_managed() -> void {
  _managed = true;
}

//--------------------------------------------------------------------------------------------------

auto Object::set_release_on_add(bool flag) -> void {
  _release_on_add = flag;
}

//--------------------------------------------------------------------------------------------------

auto Object::is_managed() -> bool {
  return _managed;
}

//--------------------------------------------------------------------------------------------------

auto Object::release_on_add() -> bool {
  return _release_on_add;
}

//--------------------------------------------------------------------------------------------------

auto Object::set_destroying() -> void {
  _destroying = true;
};

//--------------------------------------------------------------------------------------------------

auto Object::is_destroying() -> bool {
  return _destroying;
};

//--------------------------------------------------------------------------------------------------

#ifndef SWIG
#if defined(__APPLE__)

Object::Object() : _data(nil), _refcount(1), _managed(false), _release_on_add(false), _destroying(false) {
}

//--------------------------------------------------------------------------------------------------

auto Object::set_data(id data) -> void {
  _data = data;
}

//--------------------------------------------------------------------------------------------------

auto Object::get_data() const -> id {
  return _data;
}

//--------------------------------------------------------------------------------------------------

Object::~Object() {
}

#else // !__APPLE__

//--------------------------------------------------------------------------------------------------

Object::Object()
  : _data(0), _data_free_fn(0), _refcount(1), _managed(false), _release_on_add(false), _destroying(false) {
}

//--------------------------------------------------------------------------------------------------

Object::~Object() {
  if (_data_free_fn && _data)
    (*_data_free_fn)(_data);
}

//--------------------------------------------------------------------------------------------------

auto Object::set_data(void* data, FreeDataFn free_fn) -> void {
  _data = data;
  _data_free_fn = free_fn;
}

//--------------------------------------------------------------------------------------------------

auto Object::get_data_ptr() const -> void* {
  return _data;
}

//--------------------------------------------------------------------------------------------------

#endif // !__APPLE__
#endif // ifndef SWIG
