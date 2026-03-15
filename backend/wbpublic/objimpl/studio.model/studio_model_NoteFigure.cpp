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

#include <grts/structs.studio.model.h>

#include <grtpp_util.h>

#include "base/string_utilities.h"
#include "wbcanvas/studio_model_notefigure_impl.h"

//================================================================================
// studio_model_NoteFigure

auto studio_model_NoteFigure::init() -> void {
  if (!_data)
    _data = new studio_model_NoteFigure::ImplData(this);
  model_Figure::set_data(_data);
}

auto studio_model_NoteFigure::set_data(ImplData *data) -> void {
}

studio_model_NoteFigure::~studio_model_NoteFigure() {
  delete _data;
}

auto studio_model_NoteFigure::text(const grt::StringRef &value) -> void {
  grt::ValueRef ovalue(_text);
  _text = value;
  _data->set_text(_text);
  member_changed("text", ovalue, value);
}

auto studio_model_NoteFigure::textColor(const grt::StringRef &value) -> void {
  grt::ValueRef ovalue(_textColor);
  _textColor = value;
  _data->set_text_color(_textColor);
  member_changed("textColor", ovalue, value);
}

auto studio_model_NoteFigure::font(const grt::StringRef &value) -> void {
  grt::ValueRef ovalue(_font);
  _font = value;
  _data->set_font(*value);
  member_changed("font", ovalue, value);
}
