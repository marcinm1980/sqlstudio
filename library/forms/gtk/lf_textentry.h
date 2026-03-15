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

#ifndef _LF_TEXTENTRY_H_
#define _LF_TEXTENTRY_H_

#include "mforms/mforms.h"

#include "lf_view.h"

namespace mforms {
  namespace gtk {

    class TextEntryImpl : public ViewImpl {
      Gtk::Entry *_entry;
      Gdk::RGBA _text_color;
      Gdk::RGBA _placeholder_color;
      TextEntryType _type;
      bool _has_real_text;
      bool _changing_text;
      virtual auto get_outer() const -> Gtk::Widget * {
        return _entry;
      }

      TextEntryImpl(::mforms::TextEntry *self, TextEntryType type);
      static auto create(::mforms::TextEntry *self, TextEntryType type) -> bool;
      static auto set_text(::mforms::TextEntry *self, const std::string &text) -> void;
      static auto set_placeholder_text(::mforms::TextEntry *self, const std::string &text) -> void;
      static auto set_placeholder_color(::mforms::TextEntry *self, const std::string &color) -> void;
      static auto set_max_length(::mforms::TextEntry *self, int len) -> void;
      static auto get_text(::mforms::TextEntry *self) -> std::string;
      static auto set_read_only(::mforms::TextEntry *self, bool flag) -> void;
      static auto set_bordered(::mforms::TextEntry *self, bool flag) -> void;
      static auto cut(::mforms::TextEntry *self) -> void;
      static auto copy(::mforms::TextEntry *self) -> void;
      static auto paste(::mforms::TextEntry *self) -> void;
      static auto select(::mforms::TextEntry *self, const base::Range &range) -> void;
      static auto get_selection(::mforms::TextEntry *self) -> base::Range;

      auto activated(mforms::TextEntry *self) -> void;
      auto key_press(GdkEventKey *event, mforms::TextEntry *self) -> bool;

      auto icon_pressed(Gtk::EntryIconPosition pos, const GdkEventButton *ev) -> void;
      auto set_placeholder_text(const std::string &text) -> void;
      auto set_text(const std::string &text) -> void;
      void focus_in(GdkEventFocus *);
      void focus_out(GdkEventFocus *);
      void changed(mforms::TextEntry *);

    protected:
      auto set_front_color(const std::string &color) -> void;
      virtual auto set_back_color(const std::string &color) -> void;

    public:
      static auto init() -> void;
    };
  };
};

#endif
