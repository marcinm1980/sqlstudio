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

#ifndef _WB_COMPONENT_PHYSICAL_H_
#define _WB_COMPONENT_PHYSICAL_H_

// Physical Model Handling

#include "studio/wb_backend_public_interface.h"

#include "base/trackable.h"
#include "base/notifications.h"

#include "wb_component.h"

#include "grt/icon_manager.h"

#include "grts/structs.studio.h"

#include "wbcanvas/studio_physical_model_impl.h"

namespace wb {

  enum RelationshipType {
    Relationship11Id,
    Relationship1nId,
    RelationshipnmId,
    Relationship11NonId,
    Relationship1nNonId,
    RelationshipPick
  };

  enum ObjectType { ObjectTable, ObjectView, ObjectRoutineGroup };

  class CatalogTreeBE;
  class RelationshipFloater;

#define WB_TOOL_PTABLE "physical/table"
#define WB_TOOL_PROUTINEGROUP "physical/routinegroup"
#define WB_TOOL_PVIEW "physical/view"

#define WB_TOOL_PREL11_NOID "physical/rel11_noid"
#define WB_TOOL_PREL1n_NOID "physical/rel1n_noid"
#define WB_TOOL_PREL11 "physical/rel11"
#define WB_TOOL_PREL1n "physical/rel1n"
#define WB_TOOL_PRELnm "physical/relnm"
#define WB_TOOL_PREL_PICK "physical/relpick"

  class MYSQLWBBACKEND_PUBLIC_FUNC WBComponentPhysical : virtual public WBComponent {
  public:
    WBComponentPhysical(WBContext *wb);
    virtual ~WBComponentPhysical();

    static auto name() -> std::string {
      return "physical";
    }
    virtual auto get_name() -> std::string {
      return WBComponentPhysical::name();
    }
    virtual auto get_diagram_class_name() -> std::string {
      return studio_physical_Diagram::static_class_name();
    }

    // Model
    auto add_new_db_schema(const studio_physical_ModelRef &model) -> db_SchemaRef;
    auto delete_db_schema(const db_SchemaRef &schema) -> void;

    db_DatabaseObjectRef add_new_db_table(const db_SchemaRef &schema, const std::string &template_name = "");
    auto add_new_db_view(const db_SchemaRef &schema) -> db_DatabaseObjectRef;
    auto add_new_db_routine(const db_SchemaRef &schema) -> db_DatabaseObjectRef;
    auto add_new_db_routine_group(const db_SchemaRef &schema) -> db_DatabaseObjectRef;

    db_ScriptRef add_new_stored_script(const studio_physical_ModelRef &model, const std::string &path = "");
    GrtStoredNoteRef add_new_stored_note(const studio_physical_ModelRef &model, const std::string &path = "");

    auto interactive_place_db_objects(ModelDiagramForm *vform, int x, int y,
                                                            const std::list<db_DatabaseObjectRef> &objects) -> std::list<model_FigureRef>;
    auto interactive_place_db_objects(ModelDiagramForm *vform, int x, int y,
                                                            const std::list<db_DatabaseObjectRef> &objects,
                                                            grt::CopyContext &copy_context) -> std::list<model_FigureRef>;

    auto place_db_object(ModelDiagramForm *view, const base::Point &pos, const db_DatabaseObjectRef &object,
                                    bool select_figure = true) -> model_FigureRef;
    auto place_new_db_object(ModelDiagramForm *view, const base::Point &pos, ObjectType type) -> void;

    auto clone_db_object_to_schema(const db_SchemaRef &schema, const db_DatabaseObjectRef &object,
                                                   grt::CopyContext &copy_context) -> db_DatabaseObjectRef;

    virtual auto block_model_notifications() -> void;
    virtual auto unblock_model_notifications() -> void;

    virtual auto delete_db_object(const db_DatabaseObjectRef &object) -> void;

    auto setup_physical_model(studio_DocumentRef &doc, const std::string &rdbms_name,
                              const std::string &rdbms_version) -> void;

    auto has_figure_for_object_in_active_view(const GrtObjectRef &object, ModelDiagramForm *vform = 0) -> bool;

    auto privilege_list_changed(grt::internal::OwnedList *list, bool added, const grt::ValueRef &value,
                                const db_CatalogRef &catalog) -> void;
    auto remove_user(const db_UserRef &user) -> void;
    auto remove_role(const db_RoleRef &role) -> void;

    auto add_new_user(const studio_physical_ModelRef &model) -> db_UserRef;
    auto add_new_role(const studio_physical_ModelRef &model) -> db_RoleRef;

    auto remove_references_to_object(const db_DatabaseObjectRef &object) -> void;
    virtual auto close_document() -> void;

  protected:
    enum RelationshipToolState { RIdle, RPickingStart, RPickingEnd, RFinished, RCancelled };

    class RelationshipToolContext : public base::trackable {
    private:
      WBComponentPhysical *owner;
      ModelDiagramForm *view;
      RelationshipToolState state;
      std::string last_message;
      RelationshipType type;
      studio_physical_TableFigureRef hovering;
      std::vector<db_ColumnRef> columns;
      std::vector<db_ColumnRef> refcolumns;

      RelationshipFloater *floater;

      studio_physical_TableFigureRef itable;
      studio_physical_TableFigureRef ftable;

      auto pick_table(const studio_physical_TableFigureRef &table) -> bool;
      auto pick_reftable(const studio_physical_TableFigureRef &table) -> bool;

      auto pick_column(const studio_physical_TableFigureRef &table, const db_ColumnRef &column) -> bool;
      auto pick_refcolumn(const studio_physical_TableFigureRef &table, const db_ColumnRef &column) -> bool;
      auto done_picking_columns() -> bool {
        return (!columns.empty() && columns.size() == refcolumns.size());
      }

      auto finish_for_tables() -> bool;
      auto finish_for_columns() -> bool;
      auto finish() -> bool;

      auto add_column(const db_ColumnRef &column) -> bool;
      auto add_refcolumn(const db_ColumnRef &column) -> bool;

      auto on_figure_crossed(const model_ObjectRef &owner, mdc::CanvasItem *item, bool enter, const base::Point &pos) -> void;
      auto enter_table(const studio_physical_TableFigureRef &table) -> void;
      auto leave_table(const studio_physical_TableFigureRef &table) -> void;

      auto source_picking_done() -> void;

    public:
      RelationshipToolContext(WBComponentPhysical *owner, ModelDiagramForm *form, RelationshipType type);

      auto cancel() -> void;

      auto button_press(ModelDiagramForm *view, const base::Point &pos) -> bool;
    };

    virtual auto load_app_options(bool update) -> void;

    virtual auto setup_context_grt(WBOptions *options) -> void;

    auto init_catalog_grt(const db_mgmt_RdbmsRef &rdbms, const std::string &db_versionRef,
                          studio_physical_ModelRef &model) -> void;

    auto create_builtin_user_datatypes(const db_CatalogRef &catalog,
                                                                const db_mgmt_RdbmsRef &rdbms) -> grt::ListRef<db_UserDatatype>;

    virtual auto setup_canvas_tool(ModelDiagramForm *view, const std::string &tool) -> void;

    virtual auto get_tools_toolbar() -> app_ToolbarRef;
    virtual auto get_tool_options(const std::string &tool) -> app_ToolbarRef;
    virtual auto get_shortcut_items() -> grt::ListRef<app_ShortcutItem>;

    virtual auto reset_document() -> void;
    virtual auto document_loaded() -> void;
    auto add_schema_listeners(const db_SchemaRef &schema) -> void;
    auto add_schema_object_listeners(const grt::ObjectRef &object) -> void;

    virtual auto delete_model_object(const model_ObjectRef &object, bool figure_only) -> bool;

    virtual auto handles_figure(const model_ObjectRef &figure) -> bool;
    virtual auto can_paste_object(const grt::ObjectRef &object) -> bool;
    virtual auto paste_object(ModelDiagramForm *view, const grt::ObjectRef &object,
                                         grt::CopyContext &copy_context) -> model_ObjectRef;
    virtual auto copy_object_to_clipboard(const grt::ObjectRef &object, grt::CopyContext &copy_context) -> void;

    virtual auto activate_canvas_object(const model_ObjectRef &figure, bool newwindow) -> void;

    virtual auto get_object_tooltip(const model_ObjectRef &object, mdc::CanvasItem *item) -> std::string;

    // Toolbar Handling
    virtual auto get_command_dropdown_items(const std::string &option) -> std::vector<std::string>;

    // drag&drop
    virtual auto accepts_drop(ModelDiagramForm *view, int x, int y, const std::string &type,
                              const std::list<GrtObjectRef> &objects) -> bool;
    virtual auto perform_drop(ModelDiagramForm *view, int x, int y, const std::string &type,
                              const std::list<GrtObjectRef> &objects) -> bool;
    virtual auto perform_drop(ModelDiagramForm *view, int x, int y, const std::string &type, const std::string &data) -> bool;

  public:
    virtual auto get_object_for_figure(const model_ObjectRef &object) -> GrtObjectRef;

  private:
    auto delete_db_schema(const db_SchemaRef &schema, bool check_empty) -> grt::DictRef;

    auto start_relationship(ModelDiagramForm *view, const base::Point &pos, RelationshipType type) -> RelationshipToolContext *;
    auto cancel_relationship(ModelDiagramForm *view, RelationshipToolContext *rctx) -> void;

    auto create_nm_relationship(ModelDiagramForm *view, studio_physical_TableFigureRef table1,
                                studio_physical_TableFigureRef table2, bool imandatory, bool fmandatory) -> bool;

  private:
    std::map<std::string, app_ToolbarRef> _toolbars;
    grt::ListRef<app_ShortcutItem> _shortcuts;

    std::vector<std::string> _collation_list;

    std::map<std::string, boost::signals2::connection> _object_listeners;

    std::map<std::string, boost::signals2::connection> _schema_content_listeners;
    std::map<std::string, boost::signals2::connection> _schema_list_listeners;
    //    std::list<sigc::connection> _blockable_listeners;

    std::map<std::string, boost::signals2::connection> _figure_list_listeners;
    boost::signals2::connection _model_list_listener;
    boost::signals2::connection _catalog_object_list_listener;

    auto refresh_ui_for_object(const GrtObjectRef &object) -> void;

    auto update_table_fk_connection(const db_TableRef &table, const db_ForeignKeyRef &fk, bool added) -> bool;

    // Listeners
    auto model_object_list_changed(grt::internal::OwnedList *list, bool added, const grt::ValueRef &value) -> void;

    auto view_object_list_changed(grt::internal::OwnedList *list, bool added, const grt::ValueRef &value,
                                  const model_DiagramRef &view) -> void;

    auto catalog_object_list_changed(grt::internal::OwnedList *list, bool added, const grt::ValueRef &value,
                                     const db_CatalogRef &catalog) -> void;
    auto schema_object_list_changed(grt::internal::OwnedList *list, bool added, const grt::ValueRef &value,
                                    const db_SchemaRef &schema) -> void;

    auto foreign_key_changed(const db_ForeignKeyRef &fk) -> void;

    auto schema_content_object_changed(const db_DatabaseObjectRef &object) -> void;

    auto schema_member_changed(const std::string &name, const grt::ValueRef &ovalue, const db_SchemaRef &schema) -> void;

    auto handle_button_event(ModelDiagramForm *, mdc::MouseButton, bool, base::Point, mdc::EventState, void *data) -> bool;
  };
};

#endif
