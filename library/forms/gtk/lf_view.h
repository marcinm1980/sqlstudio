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

#ifndef _LF_VIEW_H_
#define _LF_VIEW_H_

#include "mforms/mforms.h"
#include <gdkmm/event.h>

#include "lf_base.h"
#include "lf_mforms.h"
#include "main_app.h"

namespace mforms {
  namespace gtk {

    class DataWrapper {
    private:
      void *_data;

    public:
      DataWrapper(void *data) {
        _data = data;
      }

      auto GetData() -> void * {
        return _data;
      };
    };

    auto GetModifiers(const guint state, const guint keyval) -> mforms::ModifierKey;
    auto GetKeys(const guint keyval) -> mforms::KeyCode;

    class ViewImpl : public ObjectImpl {
    public:
      virtual auto get_outer() const -> Gtk::Widget * = 0;
      // get the widget that does the actual work. most of the time it will be the same as the outer one
      virtual auto get_inner() const -> Gtk::Widget *;

    protected:
      ViewImpl(::mforms::View *view);
      static auto destroy(::mforms::View *self) -> void;
      static auto show(::mforms::View *self, bool show) -> void;
      virtual auto show(bool show) -> void;
      static auto is_shown(::mforms::View *self) -> bool;
      static auto is_fully_visible(::mforms::View *self) -> bool;
      static auto set_tooltip(::mforms::View *self, const std::string &text) -> void;
      static auto set_font(::mforms::View *self, const std::string &fontDescription) -> void;
      static auto get_width(const ::mforms::View *self) -> int;
      static auto get_height(const ::mforms::View *self) -> int;
      static auto get_preferred_width(::mforms::View *self) -> int;
      virtual auto get_preferred_width() -> int;
      static auto get_preferred_height(::mforms::View *self) -> int;
      virtual auto get_preferred_height() -> int;
      static auto get_x(const ::mforms::View *self) -> int;
      static auto get_y(const ::mforms::View *self) -> int;
      static auto set_size(::mforms::View *self, int w, int h) -> void;
      virtual auto set_size(int width, int height) -> void;
      static auto set_min_size(::mforms::View *self, int w, int h) -> void;
      virtual auto set_min_size(int width, int height) -> void;
      static auto set_position(::mforms::View *self, int x, int y) -> void;
      static auto client_to_screen(::mforms::View *self, int x, int y) -> std::pair<int, int>;
      static auto set_enabled(::mforms::View *self, bool flag) -> void;
      static auto is_enabled(::mforms::View *self) -> bool;
      static auto set_name(::mforms::View *self, const std::string &name) -> void;
      virtual auto set_name(const std::string &name) -> void;
      static auto relayout(::mforms::View *view) -> void;
      static auto set_needs_repaint(::mforms::View *view) -> void;
      auto size_changed() -> void;
      auto on_focus_grab() -> void;
      auto on_button_release(GdkEventButton *btn) -> bool;
      auto on_button_press(GdkEventButton *btn) -> bool;

      auto setup() -> void;
      virtual auto move_child(ViewImpl *child, int x, int y) -> void;
      static auto suspend_layout(::mforms::View *view, bool flag) -> void;
      virtual auto suspend_layout(bool flag) -> void {
      }
      static auto set_front_color(::mforms::View *self, const std::string &color) -> void;
      virtual auto set_front_color(const std::string &color) -> void {};
      static auto set_back_color(::mforms::View *self, const std::string &color) -> void;
      static auto get_front_color(::mforms::View *self) -> std::string;
      static auto get_back_color(::mforms::View *self) -> std::string;
      virtual auto set_back_color(const std::string &color) -> void;
      static auto set_back_image(::mforms::View *self, const std::string &path, mforms::Alignment alig) -> void;
      static auto flush_events(::mforms::View *self) -> void;
      static auto set_padding(::mforms::View *self, int left, int top, int right, int bottom) -> void;
      virtual auto set_padding_impl(int left, int top, int right, int bottom) -> void;
      static auto register_drop_formats(::mforms::View *self, DropDelegate *target,
                                        const std::vector<std::string> &formats) -> void;
      auto register_drop_formats(const std::vector<std::string> &formats, DropDelegate *target) -> void;
      auto get_drop_position() -> mforms::DropPosition;
      static auto focus(::mforms::View *view) -> void;
      static auto get_drop_position(::mforms::View *self) -> mforms::DropPosition;
      static auto has_focus(::mforms::View *view) -> bool;
      static auto drag_text(::mforms::View *self, ::mforms::DragDetails details, const std::string &text) -> DragOperation;
      static auto drag_data(::mforms::View *self, ::mforms::DragDetails details, void *data,
                                     const std::string &format) -> DragOperation;
      auto drag_data(::mforms::DragDetails details, void *data, const std::string &format) -> DragOperation;

    protected:
      Glib::RefPtr<Gdk::Pixbuf> _back_image;
      mforms::Alignment _back_image_alignment;
      Gdk::Event *_last_btn_down;

      // need this to find out later the format
      std::map<std::string, size_t> _drop_formats;
      DropDelegate *_target;

      std::map<std::string, DataWrapper> _drop_data;

      // can be null
      cairo_surface_t *_drag_image;
      runtime::loop _loop;

      //
      //  /**
      //   * holds a void ptr to the data being dragged and std::string mime type
      //   */
      //  std::map<std::string, void*> _drag_data;

      // This will only work if the specific subclass supports drawing backgroud images
      // in that case it will add on_expose_event to the expose signal
      virtual auto set_back_image(const std::string &path, mforms::Alignment alig) -> void;

      // for supporting subclasses that support background painting
      auto on_draw_event(const ::Cairo::RefPtr< ::Cairo::Context> &context, Gtk::Widget *target) -> bool;

      auto slot_drag_drop(const Glib::RefPtr<Gdk::DragContext> &context, int x, int y, guint time) -> bool;
      auto slot_drag_data_delete(const Glib::RefPtr<Gdk::DragContext> &context) -> void;
      auto slot_drag_motion(const Glib::RefPtr<Gdk::DragContext> &context, int, int, guint time) -> bool;
      auto slot_drag_data_received(const Glib::RefPtr<Gdk::DragContext> &context, int x, int y,
                                   const Gtk::SelectionData &data, guint info, guint time) -> void;
      auto slot_drag_begin(const Glib::RefPtr<Gdk::DragContext> &context) -> void;
      auto slot_drag_data_get(const Glib::RefPtr<Gdk::DragContext> &context, Gtk::SelectionData &data, guint,
                              guint time) -> void;
      auto slot_drag_end(const Glib::RefPtr<Gdk::DragContext> &context) -> void;
      auto slot_drag_failed(const Glib::RefPtr<Gdk::DragContext> &context, Gtk::DragResult result) -> bool;

    public:
      static auto init() -> void;
      static auto get_widget_for_view(mforms::View *view) -> Gtk::Widget *;
      static auto get_view_for_widget(Gtk::Widget *w) -> mforms::View *;
    };

    auto draw_event_slot(const ::Cairo::RefPtr< ::Cairo::Context> &context, Gtk::Widget *w) -> bool;
    enum WBColor { BG_COLOR, FG_COLOR };

    auto set_color(Gtk::Widget *, const std::string &color, const WBColor col) -> void;
    auto get_color(Gtk::Widget *w, const WBColor colr) -> base::Color *;
  };

  inline auto widget_for_view(mforms::View *view) -> Gtk::Widget * {
    return gtk::ViewImpl::get_widget_for_view(view);
  }

  inline auto view_for_widget(Gtk::Widget *w) -> mforms::View * {
    return gtk::ViewImpl::get_view_for_widget(w);
  }
};

#endif
