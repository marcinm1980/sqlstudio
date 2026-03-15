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

#ifndef _BADGE_FIGURE_H_
#define _BADGE_FIGURE_H_

#include "mdc.h"

#include <algorithm>
#include <boost/signals2.hpp>

#include "wbpublic_public_interface.h"

class WBPUBLICBACKEND_PUBLIC_FUNC BadgeFigure : public mdc::Figure {
  mdc::FontSpec _font;
  std::string _badge_id;
  std::string _text;
  base::Color _fill_color2;
  base::Color _text_color;
  cairo_pattern_t *_gradient;
  base::Size _text_size;

  virtual auto calc_min_size() -> base::Size;

public:
  BadgeFigure(mdc::Layer *layer);
  virtual ~BadgeFigure();
  virtual auto draw_contents(mdc::CairoCtx *cr) -> void;

  auto set_badge_id(const std::string &bid) -> void;
  auto badge_id() const -> std::string {
    return _badge_id;
  }

  auto set_text(const std::string &text) -> void;
  auto set_gradient_from_color(const base::Color &color) -> void;
  auto set_fill_color2(const base::Color &color) -> void;
  auto set_text_color(const base::Color &color) -> void;

  boost::signals2::scoped_connection updater_connection;
};

#endif
