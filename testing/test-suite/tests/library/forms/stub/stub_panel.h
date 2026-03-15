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

#ifndef _STUB_PANEL_H_
#define _STUB_PANEL_H_

#include "stub_container.h"

namespace mforms {
  namespace stub {

    class PanelWrapper : public ContainerWrapper {
    protected:
      PanelWrapper(::mforms::Panel *self, ::mforms::PanelType type) : ContainerWrapper(self) {
      }

      static auto create(::mforms::Panel *self, ::mforms::PanelType type) -> bool {
        return true;
      }

      static auto set_title(::mforms::Panel *self, const std::string &title) -> void {
      }

      static auto set_active(::mforms::Panel *self, bool flag) -> void {
      }

      static auto get_active(::mforms::Panel *self) -> bool {
        return false;
      }

      static auto set_back_color(::mforms::Panel *self, const std::string &color) -> void {
      }

      static auto add(::mforms::Panel *self, ::mforms::View *child) -> void {
      }

      static auto remove(::mforms::Panel *self, ::mforms::View *child) -> void {
      }

    public:
      static auto init() -> void {
        ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

        f->_panel_impl.create = &PanelWrapper::create;
        f->_panel_impl.set_title = &PanelWrapper::set_title;
        f->_panel_impl.set_back_color = &PanelWrapper::set_back_color;

        f->_panel_impl.add = &PanelWrapper::add;
        f->_panel_impl.remove = &PanelWrapper::remove;

        f->_panel_impl.set_active = &PanelWrapper::set_active;
        f->_panel_impl.get_active = &PanelWrapper::get_active;
      }
    };
  };
};

#endif
