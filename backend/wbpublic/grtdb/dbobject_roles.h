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

#include "editor_dbobject.h"

namespace bec {

  class ObjectRoleListBE;

  class WBPUBLICBACKEND_PUBLIC_FUNC ObjectPrivilegeListBE : public ListModel {
  public:
    enum Columns { Name, Enabled };

    ObjectPrivilegeListBE(ObjectRoleListBE *owner, const db_mgmt_RdbmsRef &rdbms);

    virtual auto refresh() -> void;
    virtual auto count() -> size_t;

    virtual auto set_field(const NodeId &node, ColumnId column, ssize_t value) -> bool;

  protected:
    ObjectRoleListBE *_owner;
    db_mgmt_RdbmsRef _rdbms;
    grt::StringListRef _privileges;

    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC ObjectRoleListBE : public ListModel {
  public:
    enum Columns { Name };

    ObjectRoleListBE(DBObjectEditorBE *owner, const db_mgmt_RdbmsRef &rdbms);

    virtual auto count() -> size_t;
    virtual auto refresh() -> void;

    auto select_role(const NodeId &node) -> void;

    auto add_role_for_privileges(const db_RoleRef &role) -> void;
    auto remove_role_from_privileges(const db_RoleRef &role) -> void;

    auto get_privilege_list() -> ObjectPrivilegeListBE * {
      return &_privilege_list;
    }
    auto get_selected() -> db_RolePrivilegeRef;
    auto get_owner() -> DBObjectEditorBE * {
      return _owner;
    }

  protected:
    DBObjectEditorBE *_owner;
    db_mgmt_RdbmsRef _rdbms;
    std::vector<db_RolePrivilegeRef> _role_privs;

    ObjectPrivilegeListBE _privilege_list;

    NodeId _selected_node;

    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;
  };
};
