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

#include "mdc_common.h"
#include "mdc_magnet.h"
#include "mdc_canvas_item.h"
#include "mdc_connector.h"

using namespace mdc;

Magnet::Magnet(CanvasItem *owner) : _owner(owner) {
  scoped_connect(_owner->signal_bounds_changed(),
                 std::bind(&Magnet::owner_bounds_changed, this, std::placeholders::_1));
  scoped_connect(_owner->signal_parent_bounds_changed(),
                 std::bind(&Magnet::owner_parent_bounds_changed, this, std::placeholders::_1, std::placeholders::_2));
}

Magnet::~Magnet() {
  remove_all_connectors();
}

auto Magnet::remove_all_connectors() -> void {
  std::list<Connector *>::iterator iter;
  while ((iter = _connectors.begin()) != _connectors.end()) {
    (*iter)->disconnect();
  }
}

auto Magnet::add_connector(Connector *conn) -> bool {
  _connectors.push_back(conn);

  return true;
}

auto Magnet::remove_connector(Connector *conn) -> void {
  _connectors.remove(conn);
}

auto Magnet::notify_connected() -> void {
  std::list<Connector *> list(_connectors);

  for (std::list<Connector *>::iterator iter = list.begin(); iter != list.end(); ++iter)
    (*iter)->magnet_moved(this);
}

auto Magnet::set_connection_validator(const std::function<bool(Connector *)> &slot) -> void {
  _connection_slot = slot;
}

auto Magnet::set_disconnection_validator(const std::function<bool(Connector *)> &slot) -> void {
  _disconnection_slot = slot;
}

auto Magnet::allows_connection(Connector *conn) const -> bool {
  if (_connection_slot)
    return _connection_slot(conn);
  return true;
}

auto Magnet::allows_disconnection(Connector *conn) const -> bool {
  if (_disconnection_slot)
    return _disconnection_slot(conn);

  return true;
}

auto Magnet::get_position_for_connector(Connector *conn, const base::Point &srcpos) const -> base::Point {
  return _owner->get_intersection_with_line_to(srcpos);
}

auto Magnet::get_position() const -> base::Point {
  return _owner->get_root_bounds().center();
}

auto Magnet::owner_parent_bounds_changed(CanvasItem *item, const base::Rect &obounds) -> void {
  if (item->get_bounds() != obounds)
    notify_connected();
}

auto Magnet::owner_bounds_changed(const base::Rect &obounds) -> void {
  // notify listening connections that we have been moved
  if (obounds != _owner->get_bounds())
    notify_connected();
}
