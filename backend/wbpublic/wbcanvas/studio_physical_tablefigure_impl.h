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

#ifndef _MYSQLSTUDIO_PHYSICAL_TABLEFIGURE_IMPL_H_
#define _MYSQLSTUDIO_PHYSICAL_TABLEFIGURE_IMPL_H_

#include "model_figure_impl.h"
#include "grts/structs.studio.physical.h"

#include "table_figure.h"

/** Table figure implementation.
 * Implements inner working of db table figures.
 *
 * Figure contents are updated when the following changes:
 *
 * <li>figure attributes (color, state etc)
 * <li>table name
 * <li>column list (column added, removed)
 * <li>column name changed
 * <li>column type changed
 * <li>primary key
 * <li>foreign key list (fk added, removed)
 * <li>foreign key columns list (column added, removed)
 * <li>index list (index added, removed)
 * <li>index name changed
 *
 * @ingroup Figures
 */
class WBPUBLICBACKEND_PUBLIC_FUNC studio_physical_TableFigure::ImplData : public model_Figure::ImplData {
  typedef model_Figure::ImplData super;

protected:
  wbfig::Table *_figure;

  bool _pending_columns_sync;
  bool _pending_index_sync;
  bool _pending_trigger_sync;

  std::shared_ptr<boost::signals2::shared_connection_block> _table_fk_conn_block;
  std::shared_ptr<boost::signals2::shared_connection_block> _refresh_conn_block;
  std::shared_ptr<boost::signals2::shared_connection_block> _changed_conn_block;

  boost::signals2::scoped_connection _table_fk_conn;
  boost::signals2::scoped_connection _refresh_conn;
  boost::signals2::scoped_connection _changed_conn;

  std::vector<int> _list_listeners;

  virtual auto realize() -> bool;

  virtual auto update_options(const std::string &key) -> void;

  auto sync_columns() -> void;
  auto sync_indexes() -> void;
  auto sync_triggers() -> void;

  auto toggle_title(bool expanded, wbfig::Titlebar *sender) -> void;

  auto member_changed(const std::string &name, const grt::ValueRef &ovalue) -> void;
  auto table_member_changed(const std::string &name, const grt::ValueRef &ovalue) -> void;

  auto content_changed(const std::string &where) -> void;

  auto fk_changed(const db_ForeignKeyRef &fk) -> void;

  virtual auto set_in_view(bool flag) -> void;

public:
  ImplData(studio_physical_TableFigure *self);
  virtual ~ImplData(){};

  auto set_table(const db_TableRef &table) -> void;

  auto get_column_at(mdc::CanvasItem *item) -> db_ColumnRef;
  auto get_index_at(mdc::CanvasItem *item) -> db_IndexRef;

  auto set_column_highlighted(const db_ColumnRef &column, const base::Color *color = 0) -> void;
  auto set_column_unhighlighted(const db_ColumnRef &column) -> void;

  virtual auto get_canvas_item() const -> mdc::CanvasItem * {
    return _figure;
  }
  virtual auto is_realizable() -> bool;

  virtual auto unrealize() -> void;

private:
  auto self() const -> studio_physical_TableFigure * {
    return (studio_physical_TableFigure *)_self;
  }
};

#endif
