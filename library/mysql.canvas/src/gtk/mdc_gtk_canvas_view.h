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

#ifndef _MDC_GTK_CANVAS_VIEW_H_
#define _MDC_GTK_CANVAS_VIEW_H_

#include <gtkmm/adjustment.h>
#include <gtkmm/layout.h>

#include "mdc_canvas_view_x11.h"
#include "mdc_canvas_view_glx.h"
#include "base/trackable.h"

namespace mdc {

  class GtkCanvas : public Gtk::Layout, public base::trackable {
    typedef Gtk::Layout super;

  public:
    enum CanvasType { OpenGLCanvasType, XlibCanvasType, BufferedXlibCanvasType };

  private:
    CanvasView *_canvas;
    CanvasType _canvas_type;
    bool _reentrance;
    bool _initialized;

  public:
    GtkCanvas(CanvasType type);
    virtual ~GtkCanvas();

    auto get_canvas() -> CanvasView *;

    auto get_event_state(int event_state) -> mdc::EventState;

    auto create_canvas() -> void;
    auto set_vadjustment(const Glib::RefPtr<Gtk::Adjustment> &vadjustment) -> void;
    auto set_hadjustment(const Glib::RefPtr<Gtk::Adjustment> &hadjustment) -> void;

  protected:
    bool redraw(::Cairo::RefPtr< ::Cairo::Context> context);
    virtual auto on_realize() -> void;
    virtual auto on_unrealize() -> void;
    virtual auto on_map() -> void;
    virtual auto on_size_allocate(Gtk::Allocation &alloc) -> void;

    virtual auto on_scroll_event(GdkEventScroll *event) -> bool;

    virtual auto on_zoom_in_event() -> void;
    virtual auto on_zoom_out_event() -> void;
    virtual auto on_button_press_event(GdkEventButton *event) -> bool;
    virtual auto on_button_release_event(GdkEventButton *event) -> bool;
    virtual auto on_motion_notify_event(GdkEventMotion *event) -> bool;
    virtual auto on_event(GdkEvent *event) -> bool;
    virtual auto on_key_press_event(GdkEventKey *event) -> bool;
    virtual auto on_key_release_event(GdkEventKey *event) -> bool;

    auto update_scrollers() -> void;
    auto scroll_canvas() -> void;

    void canvas_view_needs_repaint(int, int, int, int);
    auto canvas_view_viewport_changed() -> void;
  };
};
#endif /* _MDC_GTK_CANVAS_VIEW_H_ */
