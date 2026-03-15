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

#include <grts/structs.studio.physical.h>

#include <grtpp_util.h>

#include "wbcanvas/studio_physical_diagram_impl.h"

#include "wbcanvas/model_layer_impl.h"

//================================================================================
// studio_physical_Diagram

auto studio_physical_Diagram::init() -> void {
  if (!_data)
    _data = new studio_physical_Diagram::ImplData(this);
  model_Diagram::set_data(_data);

  if (_rootLayer.is_valid())
    throw std::logic_error("rootLayer value is already initialized");

  rootLayer(studio_physical_LayerRef(grt::Initialized));
  _rootLayer->owner(this);
  _rootLayer->width(width());
  _rootLayer->height(height());
}

auto studio_physical_Diagram::set_data(ImplData *data) -> void {
  throw std::logic_error("unexpected");
}

studio_physical_Diagram::~studio_physical_Diagram() {
  delete _data;
}

auto studio_physical_Diagram::autoPlaceDBObjects(const grt::ListRef<db_DatabaseObject> &objects) -> void {
  get_data()->auto_place_db_objects(objects);
}

auto studio_physical_Diagram::getFigureForDBObject(const db_DatabaseObjectRef &object) -> model_FigureRef {
  return get_data()->get_figure_for_dbobject(object);
}

auto studio_physical_Diagram::placeNewLayer(double x, double y, double width, double height,
                                                         const std::string &name) -> model_LayerRef {
  return get_data()->place_new_layer(x, y, width, height, name);
}

auto studio_physical_Diagram::placeRoutineGroup(
  const db_RoutineGroupRef &routineGroup, double x, double y) -> studio_physical_RoutineGroupFigureRef {
  return get_data()->place_routine_group(routineGroup, x, y);
}

auto studio_physical_Diagram::placeTable(const db_TableRef &table, double x, double y) -> studio_physical_TableFigureRef {
  return get_data()->place_table(table, x, y);
}

auto studio_physical_Diagram::placeView(const db_ViewRef &view, double x, double y) -> studio_physical_ViewFigureRef {
  return get_data()->place_view(view, x, y);
}

auto studio_physical_Diagram::createConnectionForForeignKey(const db_ForeignKeyRef &fk) -> studio_physical_ConnectionRef {
  return get_data()->create_connection_for_foreign_key(fk);
}

auto studio_physical_Diagram::createConnectionsForTable(const db_TableRef &table) -> grt::IntegerRef {
  return get_data()->create_connections_for_table(table);
}

auto studio_physical_Diagram::deleteConnectionsForTable(const db_TableRef &table) -> void {
  get_data()->delete_connections_for_table(table);
}

auto studio_physical_Diagram::getConnectionForForeignKey(const db_ForeignKeyRef &fk) -> studio_physical_ConnectionRef {
  return get_data()->get_connection_for_foreign_key(fk);
}
