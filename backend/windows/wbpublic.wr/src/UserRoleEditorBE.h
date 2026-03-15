/*
 * Copyright (c) 2008, 2019, Oracle and/or its affiliates. All rights reserved.
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

#ifndef __USER_ROLE_EDITOR_H__
#define __USER_ROLE_EDITOR_H__

#include "DBObjectEditorWrapper.h"
#include "RoleTreeBE.h"
#include "GrtTemplates.h"
#include "grtdb/editor_user_role.h"

namespace MySQL {
  namespace Grt {
    namespace Db {

    public
      ref class RolePrivilegeListWrapper : public MySQL::Grt::ListModelWrapper {
      public:
        enum class Columns { Enabled = ::bec::RolePrivilegeListBE::Enabled, Name = ::bec::RolePrivilegeListBE::Name };

        RolePrivilegeListWrapper(::bec::RolePrivilegeListBE *inn);

        auto get_unmanaged_object() -> ::bec::RolePrivilegeListBE *;
      };

    public
      ref class RoleObjectListWrapper : public MySQL::Grt::ListModelWrapper {
      public:
        enum class Columns { Name = ::bec::RoleObjectListBE::Name };

        RoleObjectListWrapper(::bec::RoleObjectListBE *inn);

        auto get_unmanaged_object() -> ::bec::RoleObjectListBE *;
        void set_selected_node(NodeIdWrapper ^ node);
      };

    public
      ref class RoleEditorBE : public BaseEditorWrapper {
      public:
        RoleEditorBE(MySQL::Grt::GrtValue ^ arglist);
        ~RoleEditorBE();

        auto get_unmanaged_object() -> ::bec::RoleEditorBE *;

        auto get_name() -> String ^;
        void set_name(String ^ name);
        void set_parent_role(String ^ name);
        auto get_parent_role() -> String ^;
        auto get_role_tree() -> RoleTreeBE ^;
        auto get_role_list() -> List<String ^> ^;
        auto get_privilege_list() -> RolePrivilegeListWrapper ^;
        auto get_object_list() -> RoleObjectListWrapper ^;
        void add_object(GrtValue ^ object);
        void remove_object(NodeIdWrapper ^ node);
      };

    } // namespace Db
  }   // namespace Grt
} // namespace MySQL

#endif // __USER_ROLE_EDITOR_H__
