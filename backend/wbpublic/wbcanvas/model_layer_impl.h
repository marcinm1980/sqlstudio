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

#pragma once

#include "mdc.h"
#include "grt.h"

#include <grtpp_undo_manager.h>

#include "grts/structs.model.h"

#include "wbpublic_public_interface.h"

#include "model_object_impl.h"

class WBPUBLICBACKEND_PUBLIC_FUNC model_Layer::ImplData : public model_Object::ImplData {
  using super = model_Object::ImplData;

  friend class ViewBase;

protected:
  mdc::AreaGroup *_area_group;

  auto get_canvas_view() const -> mdc::CanvasView *;
  auto is_canvas_view_valid() -> bool;

  auto layer_bounds_changed(const base::Rect &rect) -> void;
  auto interactive_layer_resized(const base::Rect &rect) -> void;

  virtual auto is_realizable() -> bool;

  auto member_changed(const std::string &name, const grt::ValueRef &ovalue) -> void;

public:
  ImplData(model_Layer *owner);

  virtual ~ImplData();

  auto raise_figure(const model_FigureRef &figure) -> void;
  auto lower_figure(const model_FigureRef &figure) -> void;

public:
  auto get_area_group() const -> mdc::AreaGroup * {
    return _area_group;
  }
  virtual auto get_canvas_item() const -> mdc::CanvasItem * {
    return _area_group;
  }

  virtual auto render_mini(mdc::CairoCtx *cr) -> void;
  virtual auto realize() -> bool;
  virtual auto unrealize() -> void;

private:
  auto self() const -> model_Layer * {
    return (model_Layer *)_self;
  }
};
