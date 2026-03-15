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

#ifndef _MDC_BACK_LAYER_H_
#define _MDC_BACK_LAYER_H_

#include "mdc_layer.h"

namespace mdc {

  class MYSQLCANVAS_PUBLIC_FUNC BackLayer : public Layer {
  public:
    BackLayer(CanvasView *view);
    virtual ~BackLayer();

    virtual auto repaint(const base::Rect &bounds) -> void;

    auto set_grid_visible(bool flag) -> void;
    auto set_paper_visible(bool flag) -> void;

    auto get_grid_visible() -> bool {
      return _grid_visible;
    }
    auto get_paper_visible() -> bool {
      return _paper_visible;
    }

    auto set_color(const base::Color &color) -> void;

  protected:
    base::Color _fill_color;
    base::Color _line1_color;
    base::Color _line2_color;

    // display lists for caching the grid
    GLint _grid1_dl;
    GLint _grid2_dl;
    // canvas position the display lists were generated
    base::Point _grid_dl_start;
    base::Rect _grid_dl_area;
    double _grid_dl_size;

    bool _grid_visible;
    bool _paper_visible;

    auto render_page_borders(const base::Rect &aBounds) -> void;
    auto render_grid(const base::Rect &aBounds) -> void;
  };

} // end of mdc namespace

#endif
