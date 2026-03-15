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

#ifndef _EDITOR_USER_ROLE_H_
#define _EDITOR_USER_ROLE_H_

#include "grtdb/editor_dbobject.h"
#include "role_tree_model.h"

#include "grts/structs.db.h"

#include "wbpublic_public_interface.h"

namespace bec {

  class RoleEditorBE;

  class WBPUBLICBACKEND_PUBLIC_FUNC RolePrivilegeListBE : public ListModel {
  public:
    enum Columns { Name, Enabled };

    RolePrivilegeListBE(RoleEditorBE *owner);

    virtual auto refresh() -> void;

    virtual auto count() -> size_t;

    auto add_all() -> void;
    auto remove_all() -> void;

    virtual auto set_field(const NodeId &node, ColumnId column, ssize_t value) -> bool;

  protected:
    RoleEditorBE *_owner;
    db_RolePrivilegeRef _role_privilege;
    grt::StringListRef _privileges;

    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;
  };

  //!
  //! Wraps in ListModel way operations with db_Role::privileges()
  //! Role is obtained from RoleEditorBE owner.
  //!
  class WBPUBLICBACKEND_PUBLIC_FUNC RoleObjectListBE : public ListModel {
  public:
    enum Columns { Name };

    RoleObjectListBE(RoleEditorBE *owner);

    auto set_selected_node(const NodeId &node) -> void;
    auto get_selected_object_info() -> db_RolePrivilegeRef;

    virtual auto count() -> size_t;
    virtual auto refresh() -> void {};

    virtual auto get_popup_items_for_nodes(const std::vector<NodeId> &nodes) -> MenuItemList;
    virtual auto activate_popup_item_for_nodes(const std::string &name, const std::vector<NodeId> &nodes) -> bool;

  protected:
    RoleEditorBE *_owner;
    NodeId _selection;

    virtual auto get_field_icon(const NodeId &node, ColumnId column, IconSize size) -> IconId;
    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;
  };

  //!
  //! Represents Roles, their objects and assigned privileges.
  //! The set of classes: RoleEditorBE, RoleObjectListBE and RolePrivilegeListBE
  //! works using RoleEditorBE::_role field.
  class WBPUBLICBACKEND_PUBLIC_FUNC RoleEditorBE : public BaseEditor {
  protected:
    db_RoleRef _role; //!< Selected role
    db_mgmt_RdbmsRef _rdbms;
    RoleTreeBE _tree;                    //!< List of roles in a schema
    RolePrivilegeListBE _privilege_list; //!< Serves as a source of role's objects privileges for the UIs
    RoleObjectListBE _object_list;       //!< Serves as a source of role's objects for the UI.

  public:
    RoleEditorBE(const db_RoleRef &role, const db_mgmt_RdbmsRef &rdbms);

    auto get_role() -> db_RoleRef {
      return _role;
    }

    auto get_rdbms() -> const db_mgmt_RdbmsRef & {
      return _rdbms;
    }

    virtual auto get_title() -> std::string;

    auto set_name(const std::string &name) -> void;
    auto get_name() -> std::string;

    auto set_parent_role(const std::string &name) -> void;
    auto get_parent_role() -> std::string;
    auto get_role_list() -> std::vector<std::string>;

    auto get_role_tree() -> RoleTreeBE * {
      return &_tree;
    }

    auto get_privilege_list() -> RolePrivilegeListBE * {
      return &_privilege_list;
    }
    auto get_object_list() -> RoleObjectListBE * {
      return &_object_list;
    }

    auto add_dropped_objectdata(const std::string &data) -> bool;
    auto add_object(const std::string &type, const std::string &name) -> bool;
    auto add_object(db_DatabaseObjectRef object) -> bool;
    auto remove_object(const bec::NodeId &object_node_id) -> void;
  };
};

#endif /* _EDITOR_USER_ROLE_H_ */
