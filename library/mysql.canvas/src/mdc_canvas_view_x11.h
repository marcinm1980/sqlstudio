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

#ifndef _MDC_CANVAS_MANAGER_X11_H_
#define _MDC_CANVAS_MANAGER_X11_H_

#include "mdc_canvas_view.h"
#include "base/geometry.h"

#include <X11/Xlib.h>

namespace mdc {

  class XlibCanvasView : public CanvasView {
  public:
    XlibCanvasView(Display *dpy, Window win, Visual *visual, int width, int height);

    virtual auto initialize() -> bool;

    virtual auto update_view_size(int width, int height) -> void;

  private:
    virtual auto has_gl() const -> bool {
      return false;
    }
    virtual auto begin_repaint(int x, int y, int w, int h) -> void;
    virtual auto end_repaint() -> void;
  };

  class BufferedXlibCanvasView : public CanvasView {
  public:
    BufferedXlibCanvasView(Display *dpy, Window win, Visual *visual, int depth, int width, int height);
    virtual ~BufferedXlibCanvasView();

    virtual auto initialize() -> bool;

    virtual auto has_gl() const -> bool {
      return false;
    }

    // virtual Surface *create_temp_surface(const Size &size) const;

  protected:
    Display *_display;
    Window _window;
    Pixmap _back_buffer;
    Visual *_visual;
    GC _copy_gc;
    int _depth;
    int _clip_x, _clip_y, _clip_w, _clip_h;

    virtual auto scroll_to(const base::Point &offs) -> void;
    virtual auto update_view_size(int width, int height) -> void;
    virtual auto make_current() -> void;
    virtual auto begin_repaint(int x, int y, int w, int h) -> void;
    virtual auto end_repaint() -> void;
  };
};

#endif /* _MDC_CANVAS_MANAGER_X11_H_ */
