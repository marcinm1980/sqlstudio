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

#ifndef _GRTDBCONNECTIONEDITOR_H_
#define _GRTDBCONNECTIONEDITOR_H_

#include "mforms/form.h"
#include "mforms/box.h"
#include "mforms/button.h"
#include "mforms/treeview.h"

#include "db_conn_be.h"
#include "grtdb_connect_panel.h"
#include "grt/grt_manager.h"

namespace grtui {

  class WBPUBLICBACKEND_PUBLIC_FUNC DbConnectionEditor : public mforms::Form {
  public:
    DbConnectionEditor(const db_mgmt_ManagementRef &mgmt);
    //  DbConnectionEditor(const db_mgmt_ManagementRef &mgmt, const grt::ListRef<db_mgmt_Rdbms> &allowed_rdbms);
    ~DbConnectionEditor();

    auto run() -> void;
    auto run(const db_mgmt_ConnectionRef &connection) -> db_mgmt_ConnectionRef;

  protected:
    db_mgmt_ManagementRef _mgmt;
    grt::ListRef<db_mgmt_Connection> _connection_list;
    DbConnectPanel _panel;

    mforms::Box _top_vbox;
    mforms::Box _top_hbox;

    mforms::Box _conn_list_buttons_hbox;
    mforms::Button _add_conn_button;
    mforms::Button _del_conn_button;
    mforms::Button _dup_conn_button;
    mforms::Button _move_up_button;
    mforms::Button _move_down_button;
    mforms::TreeView _stored_connection_list;
    mforms::TextEntry *_conn_name;

    mforms::Box _bottom_hbox;
    mforms::Button _ok_button;
    mforms::Button _cancel_button;
    mforms::Button _test_button;

    bool _mysql_only;

    //! Called from selector/list of connections to tell that things changed
    auto change_active_stored_conn() -> void;

    auto name_changed() -> void;

  private:
    auto add_stored_conn(bool copy) -> void;
    auto del_stored_conn() -> void;

    auto ok_clicked() -> void;
    auto cancel_clicked() -> void;
    auto reorder_conn(bool up) -> void;

    auto reset_stored_conn_list() -> void;

    auto rename_stored_conn(const std::string &oname, const std::string &name) -> bool;

    auto init() -> void;
  };
};

#endif /* _GRTDBCONNECTIONEDITOR_H_ */
