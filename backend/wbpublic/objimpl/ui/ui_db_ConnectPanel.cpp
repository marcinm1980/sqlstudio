/*
 * Copyright (c) 2012, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include <grts/structs.ui.h>
#include <grts/structs.db.mgmt.h>

#include <grtpp_util.h>
#include "grtui/grtdb_connect_panel.h"
#include "../wrapper/mforms_ObjectReference_impl.h"

//================================================================================
// ui_db_ConnectPanel

class ui_db_ConnectPanel::ImplData {
  grtui::DbConnectPanel *_panel;

public:
  ImplData() : _panel(0) {
  }

  auto init(const db_mgmt_ManagementRef &mgmt) -> void {
    if (!_panel) {
      _panel = new grtui::DbConnectPanel();
      _panel->init(mgmt);
    }
  }

  auto init(const db_mgmt_ManagementRef &mgmt, const grt::ListRef<db_mgmt_Rdbms> &rdbms_list) -> void {
    if (!_panel) {
      _panel =
        new grtui::DbConnectPanel(grtui::DbConnectPanelShowConnectionCombo | grtui::DbConnectPanelShowRDBMSCombo);
      _panel->init(mgmt, rdbms_list);
    }
  }

  auto panel() -> grtui::DbConnectPanel * {
    return _panel;
  }

  ~ImplData() {
    delete _panel;
  }
};

auto ui_db_ConnectPanel::init() -> void {
  _data = new ImplData();
}

ui_db_ConnectPanel::~ui_db_ConnectPanel() {
  delete _data;
}

auto ui_db_ConnectPanel::set_data(ImplData *data) -> void {
  throw std::logic_error("wrong call to set_data()");
}

auto ui_db_ConnectPanel::initialize(const grt::Ref<db_mgmt_Management> &mgmt) -> void {
  _data->init(mgmt);
}

auto ui_db_ConnectPanel::initializeWithRDBMSSelector(const grt::Ref<db_mgmt_Management> &mgmt,
                                                     const grt::ListRef<db_mgmt_Rdbms> &rdbms_list) -> void {
  _data->init(mgmt, rdbms_list);
}

auto ui_db_ConnectPanel::connection() const -> grt::Ref<db_mgmt_Connection> {
  if (_data && _data->panel()) {
    _data->panel()->get_be()->save_changes();
    return _data->panel()->get_connection();
  }
  return db_mgmt_ConnectionRef();
}

auto ui_db_ConnectPanel::connection(const grt::Ref<db_mgmt_Connection> &value) -> void {
  if (_data && _data->panel())
    _data->panel()->set_connection(value);
  throw std::logic_error("Cannot set connection value to non-initialized ui.db.ConnectionPanel instance");
}

auto ui_db_ConnectPanel::view() const -> grt::Ref<mforms_ObjectReference> {
  if (_data && _data->panel())
    return mforms_to_grt(_data->panel(), "Box");
  return grt::Ref<mforms_ObjectReference>();
}

auto ui_db_ConnectPanel::saveConnectionAs(const std::string &name) -> void {
  if (_data && _data->panel())
    _data->panel()->save_connection_as(name);
}
