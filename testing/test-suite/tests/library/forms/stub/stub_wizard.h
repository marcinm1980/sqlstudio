/*
 * Copyright (c) 2011, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _STUB_WIZARD_H_
#define _STUB_WIZARD_H_

#include "stub_base.h"

namespace mforms {
  namespace stub {

    class WizardWrapper : public ObjectWrapper {
      auto refresh_step_list(const std::vector<std::string> &steps) -> void;

      static auto cancel(::mforms::Wizard *wiz) -> void;

    protected:
      WizardWrapper(::mforms::Wizard *wiz);

      static auto create(::mforms::Wizard *self, Form *owner) -> bool;
      static auto set_title(::mforms::Wizard *self, const std::string &title) -> void;
      static auto run_modal(::mforms::Wizard *self) -> void;
      static auto close(::mforms::Wizard *self) -> void;
      static auto flush_events(::mforms::Wizard *self) -> void;
      static auto set_content(::mforms::Wizard *self, View *view) -> void;
      static auto set_heading(::mforms::Wizard *self, const std::string &) -> void;
      static auto set_step_list(::mforms::Wizard *self, const std::vector<std::string> &) -> void;
      static auto set_allow_cancel(::mforms::Wizard *self, bool flag) -> void;
      static auto set_allow_back(::mforms::Wizard *self, bool flag) -> void;
      static auto set_allow_next(::mforms::Wizard *self, bool flag) -> void;
      static auto set_show_extra(::mforms::Wizard *self, bool flag) -> void;
      static auto set_extra_caption(::mforms::Wizard *self, const std::string &) -> void;
      static auto set_next_caption(::mforms::Wizard *self, const std::string &) -> void;

    public:
      static auto init() -> void;

      static auto set_icon_path(const std::string &path) -> void;
    };

  } // end of stub namespace
} // end of mforms namespace

#endif
