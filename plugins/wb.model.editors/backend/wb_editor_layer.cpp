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

#include "wb_editor_layer.h"
#include "base/string_utilities.h"

LayerEditorBE::LayerEditorBE(const studio_physical_LayerRef &layer) : BaseEditor(layer), _layer(layer) {
}

auto LayerEditorBE::should_close_on_delete_of(const std::string &oid) -> bool {
  if (_layer.id() == oid || _layer->owner().id() == oid)
    return true;

  return false;
}

auto LayerEditorBE::set_color(const std::string &color) -> void {
  if (_layer->color() != color) {
    bec::AutoUndoEdit undo(this, _layer, "color");
    _layer->color(color);
    undo.end(_("Change Layer Color"));
  }
}

auto LayerEditorBE::get_color() -> std::string {
  return _layer->color();
}

auto LayerEditorBE::set_name(const std::string &name) -> void {
  if (_layer->name() != name) {
    bec::AutoUndoEdit undo(this, _layer, "name");
    _layer->name(name);
    undo.end(_("Change Layer Name"));
  }
}

auto LayerEditorBE::get_name() -> std::string {
  return _layer->name();
}

auto LayerEditorBE::get_title() -> std::string {
  return base::strfmt("%s - Layer", get_name().c_str());
}
