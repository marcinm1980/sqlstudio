/*
 * Copyright (c) 2008, 2019, Oracle and/or its affiliates. All rights reserved.
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

#pragma once

namespace MySQL {
  namespace Forms {

    /*
     * Implements a wrapper class around a .NET button control, so it can be used by the mforms library.
     */
  public
    class ButtonWrapper : public ViewWrapper {
    private:
      gcroot<System::Windows::Forms::Button ^> button;
      bool internal_padding;
      auto enable_internal_padding(bool flag) -> void;

    protected:
      ButtonWrapper(mforms::Button *backend);

      static auto create(mforms::Button *backend, mforms::ButtonType btype) -> bool;
      static auto set_text(mforms::Button *backend, const std::string &text) -> void;
      static auto set_icon(mforms::Button *backend, const std::string &path) -> void;
      static void button_click(System::Object ^ sender, System::EventArgs ^ e);
      static auto enable_internal_padding(mforms::Button *backend, bool flag) -> void;

    public:
      static auto init() -> void;
      virtual auto set_text(const std::string &text) -> int;

      auto uses_internal_padding() -> bool;
    };
  };
};
