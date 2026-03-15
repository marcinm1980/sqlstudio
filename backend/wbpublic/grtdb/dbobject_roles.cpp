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

#include "dbobject_roles.h"

#include "base/string_utilities.h"

using namespace bec;
using namespace grt;

//--------------------------------------------------------------------------------------------------

ObjectRoleListBE::ObjectRoleListBE(DBObjectEditorBE *owner, const db_mgmt_RdbmsRef &rdbms)
  : _owner(owner), _privilege_list(this, rdbms) {
  _rdbms = rdbms;
  refresh();
}

//--------------------------------------------------------------------------------------------------

auto ObjectRoleListBE::refresh() -> void {
  _role_privs.clear();

  db_DatabaseObjectRef object(_owner->get_dbobject());

  grt::ListRef<db_Role> roles(_owner->get_catalog()->roles());
  for (size_t c = roles.count(), i = 0; i < c; i++) {
    for (size_t d = roles[i]->privileges().count(), j = 0; j < d; j++) {
      if (roles[i]->privileges()[j]->databaseObject() == object) {
        _role_privs.push_back(roles[i]->privileges()[j]);
        break;
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

auto ObjectRoleListBE::count() -> size_t {
  return _role_privs.size();
}

//--------------------------------------------------------------------------------------------------

auto ObjectRoleListBE::get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool {
  switch ((Columns)column) {
    case Name: {
      db_RolePrivilegeRef role_privs(_role_privs[node[0]]);
      std::string name;

      grt::ListRef<db_mgmt_PrivilegeMapping> mappings(_rdbms->privilegeNames());
      grt::StringListRef privileges;

      for (size_t c = mappings.count(), i = 0; i < c; i++) {
        if (role_privs->databaseObject().is_valid() &&
            role_privs->databaseObject().is_instance(mappings[i]->structName())) {
          privileges = mappings[i]->privileges();
          break;
        }
      }

      if (privileges.is_valid()) {
        for (grt::StringListRef::const_iterator iter = privileges.begin(); iter != privileges.end(); ++iter) {
          const size_t idx = role_privs->privileges().get_index(*iter);
          if (idx != BaseListRef::npos) {
            if (!name.empty())
              name.append(", ");
            name.append((*iter).c_str());
          }
        }
      }

      if (name.empty())
        name = db_RoleRef::cast_from(role_privs->owner())->name();
      else
        name =
          std::string(db_RoleRef::cast_from(role_privs->owner())->name().c_str()).append(" (").append(name).append(")");
      value = grt::StringRef(name);
    }
      return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------------

auto ObjectRoleListBE::add_role_for_privileges(const db_RoleRef &role) -> void {
  grt::ListRef<db_RolePrivilege> role_privs(role->privileges());
  db_DatabaseObjectRef object(_owner->get_dbobject());

  for (size_t c = role_privs.count(), i = 0; i < c; i++) {
    if (role_privs[i]->databaseObject() == object)
      return;
  }

  db_RolePrivilegeRef role_priv(grt::Initialized);

  role_priv->owner(role);
  role_priv->databaseObject(_owner->get_dbobject());

  AutoUndoEdit undo(_owner);

  role->privileges().insert(role_priv);

  undo.end(_("Add Role to Object Privileges"));

  refresh();
}

//--------------------------------------------------------------------------------------------------

auto ObjectRoleListBE::remove_role_from_privileges(const db_RoleRef &role) -> void {
  grt::ListRef<db_RolePrivilege> role_privs(role->privileges());
  db_DatabaseObjectRef object(_owner->get_dbobject());

  for (size_t c = role_privs.count(), i = 0; i < c; i++) {
    if (role_privs[i]->databaseObject() == object) {
      // grt::AutoUndo undo;
      AutoUndoEdit undo(_owner);
      role_privs.remove(i);
      undo.end(_("Remove Role from Object Privileges"));
      break;
    }
  }

  refresh();
}

//--------------------------------------------------------------------------------------------------

auto ObjectRoleListBE::select_role(const NodeId &node) -> void {
  _selected_node = node;

  _privilege_list.refresh();
}

//--------------------------------------------------------------------------------------------------

auto ObjectRoleListBE::get_selected() -> db_RolePrivilegeRef {
  if (_selected_node.is_valid()) {
    if (_selected_node[0] < count())
      return _role_privs[_selected_node[0]];
  }
  return db_RolePrivilegeRef();
}

//--------------------------------------------------------------------------------------------------

ObjectPrivilegeListBE::ObjectPrivilegeListBE(ObjectRoleListBE *owner, const db_mgmt_RdbmsRef &rdbms)
  : _owner(owner), _rdbms(rdbms) {
}

//--------------------------------------------------------------------------------------------------

auto ObjectPrivilegeListBE::count() -> size_t {
  if (_privileges.is_valid())
    return _privileges.count();
  return 0;
}

//--------------------------------------------------------------------------------------------------

auto ObjectPrivilegeListBE::refresh() -> void {
  db_RolePrivilegeRef role_privilege(_owner->get_selected());

  _privileges.clear();
  if (role_privilege.is_valid()) {
    grt::ListRef<db_mgmt_PrivilegeMapping> mappings(_rdbms->privilegeNames());

    for (size_t c = mappings.count(), i = 0; i < c; i++) {
      if (role_privilege->databaseObject().is_valid() &&
          role_privilege->databaseObject().is_instance(mappings[i]->structName())) {
        _privileges = mappings[i]->privileges();
        break;
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

auto ObjectPrivilegeListBE::get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool {
  if (node[0] >= count())
    return false;

  db_RolePrivilegeRef role_privilege(_owner->get_selected());

  switch ((Columns)column) {
    case Enabled: {
      // Check that requested privilege is valid.
      const int ret = (role_privilege.is_valid() &&
                       (BaseListRef::npos != role_privilege->privileges().get_index(_privileges.get(node[0]))));

      value = grt::IntegerRef(ret);

      return true;
    }
    case Name:
      value = _privileges.get(node[0]);
      return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------------

auto ObjectPrivilegeListBE::set_field(const NodeId &node, ColumnId column, ssize_t value) -> bool {
  db_RolePrivilegeRef role_privilege(_owner->get_selected());
  size_t index;

  if (node[0] >= count())
    return false;

  switch ((Columns)column) {
    case Enabled:
      if ((index = role_privilege->privileges().get_index(_privileges.get(node[0]))) == BaseListRef::npos) {
        if (value) {
          AutoUndoEdit undo(_owner->get_owner());
          role_privilege->privileges().insert(_privileges.get(node[0]));
          undo.end(_("Add object privilege to role"));
        }
      } else {
        if (!value) {
          AutoUndoEdit undo(_owner->get_owner());
          role_privilege->privileges().remove(index);
          undo.end(_("Remove object privilege from role"));
        }
      }
      return true;

    case Name:
      return false;
  }
  return false;
}

//--------------------------------------------------------------------------------------------------
