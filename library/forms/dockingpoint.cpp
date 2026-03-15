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

#include "mforms/mforms.h"

using namespace mforms;

//--------------------------------------------------------------------------------------------------

DockingPoint::DockingPoint(DockingPointDelegate *delegate, bool delete_on_destroy)
  : _delegate(delegate), _delete_delegate(delete_on_destroy) {
  _delegate->_dpoint = this;
}

//--------------------------------------------------------------------------------------------------

DockingPoint::~DockingPoint() {
  if (_delete_delegate)
    delete _delegate;
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::get_type() -> std::string {
  return _delegate->get_type();
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::set_name(const std::string &name) -> void {
  return _delegate->set_name(name);
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::dock_view(AppView *view, const std::string &arg1, int arg2) -> void {
  view->set_containing_docking_point(this);
  _delegate->dock_view(view, arg1, arg2);
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::select_view(AppView *view) -> bool {
  return _delegate->select_view(view);
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::undock_view(AppView *view) -> void {
  view->retain();
  _delegate->undock_view(view);
  view->set_containing_docking_point(NULL);
  _view_undocked(view);
  view->release();
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::set_view_title(AppView *view, const std::string &title) -> void {
  _delegate->set_view_title(view, title);
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::get_size() -> std::pair<int, int> {
  return _delegate->get_size();
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::close_view_at_index(int index) -> void {
  AppView *view = _delegate->view_at_index(index);
  if (view != NULL)
    view->close();
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::close_all_views() -> bool {
  // Two loops here. First determine if all views accept to close before you actually close any.
  // Otherwise we might end up with some views closed and some not if one refuses to close.
  for (int i = view_count() - 1; i >= 0; --i) {
    AppView *view = _delegate->view_at_index(i);
    if (view == NULL)
      continue;

    if (!view->on_close())
      return false;
  }

  for (int i = view_count() - 1; i >= 0; --i) {
    AppView *view = _delegate->view_at_index(i);
    if (view != NULL)
      view->close();
  }

  return true;
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::selected_view() -> AppView * {
  return _delegate->selected_view();
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::view_count() -> int {
  return _delegate->view_count();
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::view_at_index(int index) -> AppView * {
  return _delegate->view_at_index(index);
}

//--------------------------------------------------------------------------------------------------

auto DockingPoint::view_switched() -> void {
  _view_switched();
}

//--------------------------------------------------------------------------------------------------
