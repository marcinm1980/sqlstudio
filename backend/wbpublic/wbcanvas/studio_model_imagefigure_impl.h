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

#ifndef _MYSQLSTUDIO_MODEL_IMAGEFIGURE_IMPL_H_
#define _MYSQLSTUDIO_MODEL_IMAGEFIGURE_IMPL_H_

#include "model_figure_impl.h"
#include "grts/structs.studio.model.h"

#include "image_figure.h"

class WBPUBLICBACKEND_PUBLIC_FUNC studio_model_ImageFigure::ImplData : public model_Figure::ImplData {
  typedef model_Figure::ImplData super;

protected:
  wbfig::Image *_figure;
  cairo_surface_t *_thumbnail;
  mdc::Timestamp _last_click;

  virtual auto realize() -> bool;
  auto shrink_if_needed() -> bool;

public:
  ImplData(studio_model_ImageFigure *self);
  virtual ~ImplData(){};

  virtual auto get_canvas_item() const -> mdc::CanvasItem * {
    return _figure;
  }

  auto set_filename(const std::string &fn) -> std::string;
  auto set_keep_aspect_ratio(bool flag) -> void;

  virtual auto unrealize() -> void;

  virtual auto render_mini(mdc::CairoCtx *cr) -> void;

private:
  auto self() const -> studio_model_ImageFigure * {
    return (studio_model_ImageFigure *)_self;
  }
};

#endif
