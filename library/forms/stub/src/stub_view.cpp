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

#include "../stub_view.h"

namespace mforms {
  namespace stub {

    ViewWrapper::ViewWrapper(mforms::View *view) : ObjectWrapper(view) {
    }

    auto ViewWrapper::show(mforms::View *self, bool show) -> void {
    }

    auto ViewWrapper::is_shown(mforms::View *self) -> bool {
      return false;
    }

    auto ViewWrapper::set_tooltip(mforms::View *self, const std::string &text) -> void {
    }

    auto ViewWrapper::get_width(const mforms::View *self) -> int {
      return 0;
    }

    auto ViewWrapper::get_height(const mforms::View *self) -> int {
      return 0;
    }

    auto ViewWrapper::get_preferred_width(mforms::View *self) -> int {
      return 0;
    }

    auto ViewWrapper::get_preferred_width() -> int {
      return 0;
    }

    auto ViewWrapper::get_preferred_height(mforms::View *self) -> int {
      return 0;
    }

    auto ViewWrapper::get_preferred_height() -> int {
      return 0;
    }

    auto ViewWrapper::get_x(const mforms::View *self) -> int {
      return 0;
    }

    auto ViewWrapper::get_y(const mforms::View *self) -> int {
      return 0;
    }

    auto ViewWrapper::set_size(mforms::View *self, int w, int h) -> void {
    }

    auto ViewWrapper::set_size(int width, int height) -> void {
    }

    auto ViewWrapper::set_min_size(mforms::View *self, int width, int height) -> void {
    }

    auto ViewWrapper::set_position(mforms::View *self, int x, int y) -> void {
    }

    auto ViewWrapper::set_enabled(mforms::View *self, bool flag) -> void {
    }

    auto ViewWrapper::is_enabled(mforms::View *self) -> bool {
      return true;
    }

    auto ViewWrapper::set_name(mforms::View *view, const std::string &name) -> void {
    }

    auto ViewWrapper::set_font(mforms::View *view, const std::string &font) -> void {
    }

    auto ViewWrapper::relayout(mforms::View *view) -> void {
    }

    auto ViewWrapper::set_needs_repaint(mforms::View *view) -> void {
    }

    auto ViewWrapper::size_changed() -> void {
    }

    auto ViewWrapper::suspend_layout(mforms::View *self, bool) -> void {
    }

    auto ViewWrapper::set_front_color(mforms::View *self, const std::string &color) -> void {
    }

    auto ViewWrapper::get_front_color(mforms::View *self) -> std::string {
      return "#000000";
    }

    auto ViewWrapper::set_back_color(mforms::View *self, const std::string &color) -> void {
    }

    auto ViewWrapper::get_back_color(mforms::View *self) -> std::string {
      return "#FFFFFF";
    }

    auto ViewWrapper::set_back_image(mforms::View *self, const std::string &path, mforms::Alignment layout) -> void {
    }

    auto ViewWrapper::flush_events(mforms::View *self) -> void {
    }

    auto ViewWrapper::focus(mforms::View *self) -> void {
    }

    auto ViewWrapper::destroy(mforms::View *self) -> void {
    }

    auto ViewWrapper::set_padding(mforms::View *self, int left, int top, int right, int bottom) -> void {
    }

    auto ViewWrapper::client_to_screen(mforms::View *self, int x, int y) -> std::pair<int, int> {
      return std::make_pair(0, 0);
    }

    auto ViewWrapper::screen_to_client(mforms::View *self, int x, int y) -> std::pair<int, int> {
      return std::make_pair(0, 0);
    }

    auto ViewWrapper::register_drop_formats(View *self, DropDelegate *target, const std::vector<std::string> &) -> void {
    }

    auto ViewWrapper::drag_text(View *self, DragDetails details, const std::string &text) -> DragOperation {
      return mforms::DragOperationNone;
    }

    auto ViewWrapper::drag_data(View *self, DragDetails details, void *data, const std::string &format) -> DragOperation {
      return mforms::DragOperationNone;
    }

    auto ViewWrapper::init() -> void {
      mforms::ControlFactory *f = mforms::ControlFactory::get_instance();

      f->_view_impl.destroy = &ViewWrapper::destroy;

      f->_view_impl.get_width = &ViewWrapper::get_width;
      f->_view_impl.get_height = &ViewWrapper::get_height;
      f->_view_impl.get_preferred_width = &ViewWrapper::get_preferred_width;
      f->_view_impl.get_preferred_height = &ViewWrapper::get_preferred_height;
      f->_view_impl.set_size = &ViewWrapper::set_size;
      f->_view_impl.set_min_size = &ViewWrapper::set_min_size;
      f->_view_impl.set_padding = &ViewWrapper::set_padding;

      f->_view_impl.get_x = &ViewWrapper::get_x;
      f->_view_impl.get_y = &ViewWrapper::get_y;
      f->_view_impl.set_position = &ViewWrapper::set_position;
      f->_view_impl.client_to_screen = &ViewWrapper::client_to_screen;
      f->_view_impl.screen_to_client = &ViewWrapper::screen_to_client;

      f->_view_impl.show = &ViewWrapper::show;
      f->_view_impl.is_shown = &ViewWrapper::is_shown;

      f->_view_impl.set_tooltip = &ViewWrapper::set_tooltip;
      f->_view_impl.set_name = &ViewWrapper::set_name;
      f->_view_impl.set_font = &ViewWrapper::set_font;

      f->_view_impl.set_enabled = &ViewWrapper::set_enabled;
      f->_view_impl.is_enabled = &ViewWrapper::is_enabled;
      f->_view_impl.relayout = &ViewWrapper::relayout;
      f->_view_impl.set_needs_repaint = &ViewWrapper::set_needs_repaint;

      f->_view_impl.suspend_layout = &ViewWrapper::suspend_layout;
      f->_view_impl.set_front_color = &ViewWrapper::set_front_color;
      f->_view_impl.get_front_color = &ViewWrapper::get_front_color;
      f->_view_impl.set_back_color = &ViewWrapper::set_back_color;
      f->_view_impl.get_back_color = &ViewWrapper::get_back_color;
      f->_view_impl.set_back_image = &ViewWrapper::set_back_image;

      f->_view_impl.flush_events = &ViewWrapper::flush_events;
      f->_view_impl.focus = &ViewWrapper::focus;

      f->_view_impl.register_drop_formats = &ViewWrapper::register_drop_formats;
      f->_view_impl.drag_text = &ViewWrapper::drag_text;
      f->_view_impl.drag_data = &ViewWrapper::drag_data;
    };
  };
};
