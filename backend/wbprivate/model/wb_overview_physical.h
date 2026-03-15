/*
 * Copyright (c) 2010, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "studio/wb_overview.h"

#include "base/notifications.h"

namespace mforms {
  class MenuBar;
  class ToolBar;
};

namespace wb {
  class PhysicalOverviewBE;

  namespace internal {
    class PhysicalSchemaNode;

    class PhysicalSchemataNode : public OverviewBE::ContainerNode {
      virtual auto create_child_node(db_SchemaRef schema) -> OverviewBE::Node *;

      studio_physical_ModelRef model;

      virtual auto add_object(WBContext *wb) -> bool;
      virtual auto delete_object(WBContext *wb) -> void;
      virtual auto refresh_children() -> void;

    public:
      PhysicalSchemataNode(studio_physical_ModelRef model);
      virtual auto init() -> void;
    };

    class SQLScriptsNode : public OverviewBE::ContainerNode {
    private:
      PhysicalOverviewBE *_owner;
      std::string id;
      studio_physical_ModelRef _model;

      auto add_new(WBContext *wb) -> bool;

    public:
      SQLScriptsNode(studio_physical_ModelRef model, PhysicalOverviewBE *owner);

      virtual auto refresh_children() -> void;

      virtual auto get_popup_menu_items(WBContext *wb, bec::MenuItemList &items) -> int;
      virtual auto get_unique_id() -> std::string {
        return id;
      }
    };

    class NotesNode : public OverviewBE::ContainerNode {
    private:
      PhysicalOverviewBE *_owner;
      std::string id;
      studio_physical_ModelRef _model;

      auto add_new(WBContext *wb) -> bool;

    public:
      NotesNode(studio_physical_ModelRef model, PhysicalOverviewBE *owner);

      virtual auto refresh_children() -> void;

      virtual auto get_popup_menu_items(WBContext *wb, bec::MenuItemList &items) -> int;
      virtual auto get_unique_id() -> std::string {
        return id;
      }
    };
  };

  //
  // Node Layout:
  //
  // -root: ORoot  (model)   (stable nodeid)
  //   -diagrams: ODivision  (stable noteid)  <RefreshChildren>
  //     -[diagram nodes]: OItem   <RefreshNode>
  //   -schemata: ODivision     (stable nodeid) <RefreshChildren>
  //     -[schema]: OGroup   (kind of stable by oid*)  <RefreshNode>
  //       -table: OSection (stable for parent)   <RefreshChildren>
  //         -[table nodes]: OItem      <RefreshNode>
  //       -view: OSection (stable for parent)    <RefreshChildren>
  //         -[view nodes]: OItem       <RefreshNode>
  //       -routine: OSection (stable for parent) <RefreshChildren>
  //         -[routine nodes]: OItem    <RefreshNode>
  //       -routineGroup: OSection (stable for parent)<RefreshChildren>
  //         -[routineGroup nodes]: OItem <RefreshNode>
  //   -privileges: ODivision  (stable nodeid)  <RefreshChildren>
  //     -users: OSection (stable nodeid)
  //       -[users]: OItem
  //     -roles: OSection (stable nodeid) <RefreshChildren>
  //       -[roles]: OItem
  //   -scripts: ODivision (stable nodeid)  <RefreshChildren>
  //     -[scripts] OItem
  //   -notes: ODivision (stable nodeid)  <RefreshChildren>
  //     -[notes] OItem
  //
  // Nodes marked <RefreshChildren> must have their contents refreshable when
  // Refresh message is received by the frontend. Arguments are paths.
  class MYSQLWBBACKEND_PUBLIC_FUNC PhysicalOverviewBE : public OverviewBE, public base::Observer {
    studio_physical_ModelRef _model;

    virtual auto create_root_node(studio_physical_ModelRef model, PhysicalOverviewBE *owner) -> OverviewBE::ContainerNode *;

  protected:
    mforms::MenuBar *_menu;
    mforms::ToolBar *_toolbar;
    int _schemata_node_index;

    auto handle_notification(const std::string &name, void *sender, base::NotificationInfo &info) -> void;
    auto update_toolbar_icons() -> void;

  public: // backend internal
    virtual auto identifier() const -> std::string;
    virtual auto get_title() -> std::string;

    virtual auto get_form_context_name() const -> std::string;

    virtual auto send_refresh_diagram(const model_DiagramRef &view) -> void;
    auto send_refresh_users() -> void;
    auto send_refresh_roles() -> void;
    auto send_refresh_scripts() -> void;
    auto send_refresh_notes() -> void;
    auto send_refresh_schema_list() -> void;
    auto send_refresh_for_schema(const db_SchemaRef &schema, bool refresh_object_itself) -> void;
    auto send_refresh_for_schema_object(const GrtObjectRef &object, bool refresh_object_itself) -> void;

    auto set_model(studio_physical_ModelRef model) -> void;

    virtual auto get_toolbar() -> mforms::ToolBar *;
    virtual auto get_menubar() -> mforms::MenuBar *;

  public:
    PhysicalOverviewBE(WBContext *wb);
    virtual ~PhysicalOverviewBE();

    virtual auto can_undo() -> bool;
    virtual auto can_redo() -> bool;
    virtual auto undo() -> void;
    virtual auto redo() -> void;

    virtual auto can_close() -> bool;
    virtual auto close() -> void;

    virtual auto get_model() -> model_ModelRef;
    auto get_active_schema_node() -> internal::PhysicalSchemaNode *;

    virtual auto get_default_tab_page_index() -> int;

    virtual auto get_node_drag_type(const bec::NodeId &node) -> std::string;
    virtual auto should_accept_file_drop_to_node(const bec::NodeId &node, const std::string &path) -> bool;
    virtual auto add_file_to_node(const bec::NodeId &node, const std::string &path) -> void;
    virtual auto get_file_data_for_node(const bec::NodeId &node, char *&data, size_t &length) -> bool;
    virtual auto get_file_for_node(const bec::NodeId &node) -> std::string;

    virtual auto refresh_node(const bec::NodeId &node, bool children) -> void;
  };
};
