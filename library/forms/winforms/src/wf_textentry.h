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

  public
    class TextEntryWrapper : public ViewWrapper {
    protected:
      TextEntryWrapper(mforms::TextEntry *text);

      static auto create(mforms::TextEntry *backend, mforms::TextEntryType type) -> bool;
      static auto set_text(mforms::TextEntry *backend, const std::string &text) -> void;
      static auto set_placeholder_text(mforms::TextEntry *backend, const std::string &text) -> void;
      static auto set_placeholder_color(mforms::TextEntry *backend, const std::string &color) -> void;
      static auto get_text(mforms::TextEntry *backend) -> std::string;
      static auto set_max_length(mforms::TextEntry *backend, int length) -> void;
      static auto set_read_only(mforms::TextEntry *backend, bool flag) -> void;
      static auto set_bordered(mforms::TextEntry *backend, bool flag) -> void;

      static auto cut(mforms::TextEntry *self) -> void;
      static auto copy(mforms::TextEntry *self) -> void;
      static auto paste(mforms::TextEntry *self) -> void;
      static auto select(mforms::TextEntry *self, const base::Range &range) -> void;
      static auto get_selection(mforms::TextEntry *self) -> base::Range;

      virtual void set_front_color(String ^ color);

    public:
      static auto init() -> void;
    };
  };
};
