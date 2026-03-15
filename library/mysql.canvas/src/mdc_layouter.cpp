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

#include "mdc_layouter.h"
#include "mdc_figure.h"
#include "mdc_draw_util.h"

using namespace mdc;
using namespace base;

Layouter::Layouter(Layer *layer) : CanvasItem(layer) {
  _corner_mask = CNone;
  _corner_radius = 0.0;
  _draw_background = false;
}

Layouter::~Layouter() {
}

auto Layouter::render(CairoCtx *cr) -> void {
  draw_state(cr);

  if (_draw_background) {
    stroke_outline(cr);
    cr->set_line_width(1.0);
    cr->set_color(_background_color);
    cr->fill_preserve();
    cr->set_color(_border_color);
    cr->stroke();
  }
}

auto Layouter::render_gl(mdc::CairoCtx *cr) -> void {
  if (_draw_background)
    gl_box(get_bounds(), _border_color, _background_color);
  draw_state_gl();
}

auto Layouter::stroke_outline(CairoCtx *cr, float offset) const -> void {
  stroke_rounded_rectangle(cr, get_bounds(), _corner_mask, _corner_radius, offset);
}

auto Layouter::stroke_outline_gl(float offset) const -> void {
  stroke_rounded_rectangle_gl(get_bounds(), _corner_mask, _corner_radius, offset);
}

auto Layouter::remove_all() -> void {
  foreach (std::bind(&Layouter::remove, this, std::placeholders::_1))
    ;

  set_needs_relayout();
}

static auto find_item(mdc::CanvasItem *item, const std::string &tag, mdc::CanvasItem **found_item) -> void {
  if (*found_item)
    return;

  if (item->get_tag() == tag) {
    *found_item = item;
    return;
  }

  Layouter *sub = dynamic_cast<Layouter *>(item);
  if (sub)
    *found_item = sub->find_item_with_tag(tag);
}

auto Layouter::find_item_with_tag(const std::string &tag) -> CanvasItem * {
  CanvasItem *ret = 0;
  foreach (std::bind(&find_item, std::placeholders::_1, tag, &ret))
    ;
  return ret;
}

auto Layouter::set_draw_background(bool flag) -> void {
  _draw_background = flag;
  set_needs_render();
}

auto Layouter::set_background_corners(mdc::CornerMask mask, float radius) -> void {
  _corner_mask = mask;
  _corner_radius = radius;
}

auto Layouter::set_background_color(const Color &color) -> void {
  _background_color = color;
  set_needs_render();
}

auto Layouter::set_border_color(const Color &color) -> void {
  _border_color = color;
  set_needs_render();
}
