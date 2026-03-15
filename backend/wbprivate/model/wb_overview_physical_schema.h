/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _WB_OVERVIEW_PHYSICAL_SCHEMA_H_
#define _WB_OVERVIEW_PHYSICAL_SCHEMA_H_

#include "studio/wb_overview.h"

namespace wb {
  namespace internal {

    class PhysicalSchemaContentNode;
    class SchemaObjectNode;

    class PhysicalSchemaNode : public OverviewBE::ContainerNode {
    public:
      virtual auto init() -> void;

    protected:
      bool _is_routine_group_enabled;

    public:
      PhysicalSchemaNode(db_SchemaRef schema);
      virtual auto is_pasteable(bec::Clipboard *clip) -> bool;
      virtual auto paste_object(WBContext *wb, bec::Clipboard *clip) -> void;
      virtual auto is_deletable() -> bool;
      virtual auto delete_object(WBContext *wb) -> void;
      virtual auto is_renameable() -> bool;
      virtual auto rename(WBContext *wb, const std::string &name) -> bool;
      virtual auto activate(WBContext *wb) -> bool;
      virtual auto focus(OverviewBE *sender) -> void;
      virtual auto refresh() -> void;

    public:
      virtual auto add_new_db_table(WBContext *wb) -> bool;
      virtual auto add_new_db_view(WBContext *wb) -> bool;
      virtual auto add_new_db_routine_group(WBContext *wb) -> bool;
      virtual auto add_new_db_routine(WBContext *wb) -> bool;

      virtual auto create_table_node(const db_DatabaseObjectRef &dbobject) -> SchemaObjectNode *;
      virtual auto create_view_node(const db_DatabaseObjectRef &dbobject) -> SchemaObjectNode *;
      virtual auto create_routine_node(const db_DatabaseObjectRef &dbobject) -> SchemaObjectNode *;
      virtual auto create_routine_group_node(const db_DatabaseObjectRef &dbobject) -> SchemaObjectNode *;
    };

    class SchemaObjectNode : public OverviewBE::ObjectNode {
      SchemaObjectNode(const SchemaObjectNode &copy) : OverviewBE::ObjectNode(copy) {
      }

    public:
      SchemaObjectNode(const db_DatabaseObjectRef &dbobject);
      virtual auto delete_object(WBContext *wb) -> void;
      virtual auto is_deletable() -> bool;
      virtual auto is_renameable() -> bool;
      virtual auto copy_object(WBContext *wb, bec::Clipboard *clip) -> void;
      virtual auto is_copyable() -> bool;
    };

    class SchemaTableNode : public SchemaObjectNode {
    public:
      SchemaTableNode(const db_DatabaseObjectRef &dbobject) : SchemaObjectNode(dbobject) {
      }
      virtual auto get_detail(int field) -> std::string;
    };

    class SchemaViewNode : public SchemaObjectNode {
    public:
      SchemaViewNode(const db_DatabaseObjectRef &dbobject) : SchemaObjectNode(dbobject) {
      }
      virtual auto is_renameable() -> bool;
      virtual auto get_detail(int field) -> std::string;
    };

    class SchemaRoutineGroupNode : public SchemaObjectNode {
    public:
      SchemaRoutineGroupNode(const db_DatabaseObjectRef &dbobject) : SchemaObjectNode(dbobject) {
      }
      virtual auto get_detail(int field) -> std::string;
    };

    class SchemaRoutineNode : public SchemaObjectNode {
    public:
      SchemaRoutineNode(const db_DatabaseObjectRef &dbobject) : SchemaObjectNode(dbobject) {
      }
      virtual auto get_detail(int field) -> std::string;
      virtual auto is_renameable() -> bool;
    };
  };
};

#endif /* _WB_OVERVIEW_PHYSICAL_SCHEMA_H_ */
