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

#ifndef _STUB_VIEW_H_
#define _STUB_VIEW_H_

#include "stub_base.h"

namespace mforms {
  namespace stub {

    class ViewWrapper : public ObjectWrapper {
    protected:
      static auto destroy(View *self) -> void;

      static auto get_width(const mforms::View *self) -> int;
      static auto get_height(const mforms::View *self) -> int;
      static auto get_preferred_width(mforms::View *self) -> int;
      static auto get_preferred_height(mforms::View *self) -> int;
      static auto set_size(mforms::View *self, int w, int h) -> void;
      static auto set_min_size(mforms::View *self, int w, int h) -> void;
      static auto set_padding(View *self, int, int, int, int) -> void;

      static auto get_x(const mforms::View *self) -> int;
      static auto get_y(const mforms::View *self) -> int;
      static auto set_position(mforms::View *self, int x, int y) -> void;
      static auto client_to_screen(View *self, int, int) -> std::pair<int, int>;
      static auto screen_to_client(View *self, int, int) -> std::pair<int, int>;

      static auto show(mforms::View *self, bool show) -> void;
      static auto is_shown(mforms::View *self) -> bool;

      static auto set_tooltip(mforms::View *self, const std::string &text) -> void;
      static auto set_name(mforms::View *view, const std::string &name) -> void;
      static auto set_font(mforms::View *view, const std::string &font) -> void;

      static auto set_enabled(mforms::View *self, bool flag) -> void;
      static auto is_enabled(View *self) -> bool;
      static auto relayout(mforms::View *view) -> void;
      static auto set_needs_repaint(mforms::View *view) -> void;

      static auto suspend_layout(View *self, bool) -> void;
      static auto set_front_color(mforms::View *self, const std::string &color) -> void;
      static auto get_front_color(View *self) -> std::string;
      static auto set_back_color(mforms::View *self, const std::string &color) -> void;
      static auto get_back_color(View *self) -> std::string;
      static auto set_back_image(mforms::View *self, const std::string &path, mforms::Alignment layout) -> void;

      static auto flush_events(View *self) -> void;

      static auto focus(View *self) -> void;

      static auto register_drop_formats(View *self, DropDelegate *target, const std::vector<std::string> &) -> void;
      static auto drag_text(View *self, DragDetails details, const std::string &text) -> DragOperation;
      static auto drag_data(View *self, DragDetails details, void *data, const std::string &format) -> DragOperation;

      ViewWrapper(mforms::View *view);

      virtual auto get_preferred_width() -> int;
      virtual auto get_preferred_height() -> int;
      virtual auto set_size(int width, int height) -> void;
      auto size_changed() -> void;

    public:
      static auto init() -> void;
    };
  };
};

#endif
