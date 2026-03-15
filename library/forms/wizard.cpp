/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

Wizard::Wizard() {
  _wizard_impl = NULL;
  _content = NULL;
}

Wizard::Wizard(Form *owner) {
  _wizard_impl = &ControlFactory::get_instance()->_wizard_impl;

  _content = NULL;
  _wizard_impl->create(this, owner);
}

Wizard::~Wizard() {
  if (_content)
    _content->release();
}

auto Wizard::set_title(const std::string &title) -> void {
  _wizard_impl->set_title(this, title);
}

auto Wizard::run() -> void {
  _wizard_impl->run_modal(this);
}

auto Wizard::close() -> void {
  _wizard_impl->close(this);
}

auto Wizard::set_content(View *view) -> void {
  if (_content != view) {
    if (_content)
      _content->release();
    _content = view;
    _content->retain();
    _wizard_impl->set_content(this, view);
  }
}

auto Wizard::set_heading(const std::string &text) -> void {
  _wizard_impl->set_heading(this, text);
}

auto Wizard::set_step_list(const std::vector<std::string> &steps) -> void {
  _wizard_impl->set_step_list(this, steps);
}

auto Wizard::set_allow_cancel(bool flag) -> void {
  _wizard_impl->set_allow_cancel(this, flag);
}

auto Wizard::set_allow_back(bool flag) -> void {
  _wizard_impl->set_allow_back(this, flag);
}

auto Wizard::set_allow_next(bool flag) -> void {
  _wizard_impl->set_allow_next(this, flag);
}

auto Wizard::set_show_extra(bool flag) -> void {
  _wizard_impl->set_show_extra(this, flag);
}

auto Wizard::set_extra_caption(const std::string &caption) -> void {
  _wizard_impl->set_extra_caption(this, caption);
}

auto Wizard::set_next_caption(const std::string &caption) -> void {
  _wizard_impl->set_next_caption(this, caption);
}

auto Wizard::set_cancel_handler(const std::function<bool()> &slot) -> void {
  _cancel_slot = slot;
}
