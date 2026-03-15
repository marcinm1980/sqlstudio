/*
 * Copyright (c) 2007, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef __MDC_BUTTON_H__
#define __MDC_BUTTON_H__

#include "mdc_icon_text.h"

namespace mdc {

  enum ButtonType { ActionButton, ToggleButton, ExpanderButton };

  class MYSQLCANVAS_PUBLIC_FUNC Button : public IconTextFigure {
  public:
    Button(Layer *layer, ButtonType type);
    ~Button();

    void set_active(bool flag);
    auto get_active() -> bool;

    void set_image(cairo_surface_t *image);
    void set_alt_image(cairo_surface_t *image);

    virtual void draw_contents(CairoCtx *cr);
    virtual auto calc_min_size() -> base::Size;

    auto signal_activate() -> boost::signals2::signal<void()> * {
      return &_action_signal;
    }

  protected:
    ButtonType _button_type;
    bool _active;
    bool _pressed;
    bool _inside;

    cairo_surface_t *_image;
    cairo_surface_t *_alt_image;

    boost::signals2::signal<void()> _action_signal;

    virtual auto on_button_press(CanvasItem *target, const base::Point &point, MouseButton button, EventState state) -> bool;
    virtual auto on_button_release(CanvasItem *target, const base::Point &point, MouseButton button, EventState state) -> bool;
    virtual auto on_enter(CanvasItem *target, const base::Point &point) -> bool;
    virtual auto on_leave(CanvasItem *target, const base::Point &point) -> bool;
    virtual auto on_drag(CanvasItem *target, const base::Point &point, EventState state) -> bool;
  };

} // end of mdc namespace

#endif /* __MDC_BUTTON_H__ */
