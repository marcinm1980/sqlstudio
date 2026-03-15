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

#ifndef _MDC_RECTANGLE_H_
#define _MDC_RECTANGLE_H_

#include "mdc_figure.h"
#include "mdc_draw_util.h"

namespace mdc {

  class MYSQLCANVAS_PUBLIC_FUNC RectangleFigure : public Figure {
  public:
    RectangleFigure(Layer *layer);

    virtual auto draw_contents(CairoCtx *cr) -> void;
    virtual auto stroke_outline(CairoCtx *cr, float offset = 0) const -> void;
    virtual auto stroke_outline_gl(float offset = 0) const -> void;

    virtual auto draw_contents_gl() -> void;

    virtual auto can_render_gl() -> bool {
      return true;
    }

    auto set_rounded_corners(float radius, CornerMask corners) -> void;
    auto set_filled(bool flag) -> void;

  protected:
    float _corner_radius;

    CornerMask _corners;
    bool _filled;
  };

} // end of mdc namespace

#endif /* _MDC_RECTANGLE_H_ */
