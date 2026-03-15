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

#ifndef _STUB_LABEL_H_
#define _STUB_LABEL_H_

#include "stub_view.h"

namespace mforms {
  namespace stub {

    class LabelWrapper : public ViewWrapper {
    protected:
      LabelWrapper(::mforms::Label *self) : ViewWrapper(self) {
      }

      static auto create(::mforms::Label *self) -> bool {
        return true;
      }

      static auto set_style(::mforms::Label *self, ::mforms::LabelStyle style) -> void {
      }

      static auto set_text(::mforms::Label *self, const std::string &text) -> void {
      }

      static auto set_color(::mforms::Label *self, const std::string &text) -> void {
      }

      static auto set_wrap_text(::mforms::Label *self, bool flag) -> void {
      }

      static auto set_text_align(::mforms::Label *self, ::mforms::Alignment align) -> void {
      }

    public:
      static auto init() -> void {
        ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

        f->_label_impl.create = &LabelWrapper::create;
        f->_label_impl.set_style = &LabelWrapper::set_style;
        f->_label_impl.set_text = &LabelWrapper::set_text;
        f->_label_impl.set_text_align = &LabelWrapper::set_text_align;
        f->_label_impl.set_color = &LabelWrapper::set_color;
        f->_label_impl.set_wrap_text = &LabelWrapper::set_wrap_text;
      }
    };
  };
};

#endif
