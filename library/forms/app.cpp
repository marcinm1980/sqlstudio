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
#include "base/log.h"

DEFAULT_LOG_DOMAIN("mforms")

using namespace mforms;
using namespace base;

// Implementation of _app_impl for WB is done directly in main_window.cpp in Linux
// and WBMainWindow.mm in Mac
// In Windows, it's done in wf_app.h/wf_app.cpp wrapper

static App *singleton = 0;

App::App(DockingPointDelegate *delegate, bool delete_on_destroy)
  : DockingPoint(delegate, delete_on_destroy), _app_impl(nullptr) {
}

//--------------------------------------------------------------------------------------------------

auto App::instantiate(DockingPointDelegate *delegate, bool delete_on_destroy) -> void {
  singleton = new App(delegate, delete_on_destroy);
  singleton->_app_impl = &ControlFactory::get_instance()->_app_impl;
}

//--------------------------------------------------------------------------------------------------

auto App::get() -> App * {
  return singleton;
}

//--------------------------------------------------------------------------------------------------

auto App::get_resource_path(const std::string &file) -> std::string {
  std::string ret;
  if (_app_impl->get_resource_path)
    ret = _app_impl->get_resource_path(this, file);
  if (ret == "")
      logWarning("Resource file not found: %s\n", file.c_str());
  return ret;
}

//--------------------------------------------------------------------------------------------------

auto App::get_executable_path(const std::string &file) -> std::string {
  std::string ret;
  if (_app_impl->get_executable_path)
    ret = _app_impl->get_executable_path(this, file);
  else
    ret = get_resource_path(file);
  return ret;
}

//--------------------------------------------------------------------------------------------------

auto App::set_status_text(const std::string &text) -> void {
  if (_app_impl->set_status_text)
    _app_impl->set_status_text(this, text);
}

//--------------------------------------------------------------------------------------------------

/**
 * Returns the bounds of the main application window.
 */
auto App::get_application_bounds() -> base::Rect {
  return _app_impl->get_application_bounds(this);
}

//--------------------------------------------------------------------------------------------------

auto App::enter_event_loop(float timeout) -> int {
  return _app_impl->enter_event_loop(this, timeout);
}

//--------------------------------------------------------------------------------------------------

auto App::exit_event_loop(int retcode) -> void {
  _app_impl->exit_event_loop(this, retcode);
}

//--------------------------------------------------------------------------------------------------

auto App::backing_scale_factor() -> float {
  if (_app_impl->backing_scale_factor != nullptr)
    return _app_impl->backing_scale_factor(this);
  return 1.0;
}

//--------------------------------------------------------------------------------------------------

auto App::isDarkModeActive() -> bool {
  if (_app_impl->isDarkModeActive != nullptr)
    return _app_impl->isDarkModeActive(this);

  return false;
}

//--------------------------------------------------------------------------------------------------
