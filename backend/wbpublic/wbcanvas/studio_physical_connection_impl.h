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

#ifndef _MYSQLSTUDIO_PHYSICAL_CONNECTION_IMPL_H_
#define _MYSQLSTUDIO_PHYSICAL_CONNECTION_IMPL_H_

#include "model_connection_impl.h"
#include "grts/structs.studio.physical.h"

namespace wbfig {
  class FigureItem;
};

class WBPUBLICBACKEND_PUBLIC_FUNC studio_physical_Connection::ImplData : public model_Connection::ImplData {
  typedef model_Connection::ImplData super;

protected:
  boost::signals2::scoped_connection _realize_conn;

  boost::signals2::scoped_connection _fk_member_changed_conn;
  boost::signals2::scoped_connection _fk_changed_conn;
  boost::signals2::scoped_connection _table_changed_conn;

  bool _highlighting;

  auto fk_changed(const db_ForeignKeyRef &fk) -> void;
  auto member_changed(const std::string &name, const grt::ValueRef &ovalue) -> void;

  virtual auto realize() -> bool;
  virtual auto unrealize() -> void;

  auto update_line_ends() -> void;
  auto layout_changed() -> void;
  auto table_changed(const std::string &detail) -> void;

  virtual auto get_start_canvas_item() -> mdc::CanvasItem *;
  virtual auto get_end_canvas_item() -> mdc::CanvasItem *;

  virtual auto caption_bounds_changed(const base::Rect &obounds, mdc::TextFigure *figure) -> void;

  auto fk_member_changed(const std::string &member, const grt::ValueRef &ovalue) -> void;

  auto object_realized(const model_ObjectRef &object) -> void;

  auto update_connected_tables() -> void;

public:
  ImplData(studio_physical_Connection *self);
  virtual ~ImplData();

  virtual auto highlight(const base::Color *color = 0) -> void;
  virtual auto unhighlight() -> void;

  virtual auto set_in_view(bool flag) -> void;

  auto set_foreign_key(const db_ForeignKeyRef &fk) -> void;

private:
  auto self() const -> studio_physical_Connection * {
    return (studio_physical_Connection *)_self;
  }
};

#endif
