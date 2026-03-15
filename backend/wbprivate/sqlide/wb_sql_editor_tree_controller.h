/*
 * Copyright (c) 2007, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "studio/wb_backend_public_interface.h"
#include "sqlide/wb_live_schema_tree.h"
#include "sqlide/db_sql_editor_log.h" // for RowId
#include "grt/grt_threaded_task.h"

#include "grts/structs.db.h"
#include "grts/structs.db.mgmt.h"
#include "grts/structs.db.mysql.h"

#include "grtpp_notifications.h"

class SqlEditorForm;

namespace bec {
  class GRTManager;
  class DBObjectEditorBE;
};

namespace mforms {
  class View;
  class Box;
  class Splitter;
  class TabView;
  class HyperText;
  class MenuItem;
};

namespace wb {
  class SimpleSidebar;
  class AdvancedSidebar;
}

class MYSQLWBBACKEND_PUBLIC_FUNC SqlEditorTreeController :
  public base::trackable,
  public grt::GRTObserver,
  public wb::LiveSchemaTree::FetchDelegate,
  public wb::LiveSchemaTree::Delegate,
  public std::enable_shared_from_this<SqlEditorTreeController> {
#if defined(ENABLE_TESTING)
  friend class EditorFormTester;
  friend class LocalEditorFormTester;
#endif
  friend class db_query_EditorConcreteImplData;

public:
  static auto create(SqlEditorForm *owner) -> std::shared_ptr<SqlEditorTreeController>;
  virtual ~SqlEditorTreeController();

  auto finish_init() -> void;
  auto prepare_close() -> void;

private:
  SqlEditorTreeController(SqlEditorForm *owner);

  SqlEditorForm *_owner;

  wb::AdvancedSidebar *_schema_side_bar;
  wb::SimpleSidebar *_admin_side_bar;
  mforms::TabView *_task_tabview;
  mforms::Box *_taskbar_box;

  wb::LiveSchemaTree *_schema_tree;
  wb::LiveSchemaTree _base_schema_tree;
  wb::LiveSchemaTree _filtered_schema_tree;
  base::Mutex _schema_contents_mutex;
  GrtThreadedTask::Ref live_schema_fetch_task;
  GrtThreadedTask::Ref live_schemata_refresh_task;
  bool _is_refreshing_schema_tree;

  bool _use_show_procedure;

  mforms::Splitter *_side_splitter;
  mforms::TabView *_info_tabview;
  mforms::HyperText *_object_info;
  mforms::HyperText *_session_info;

  boost::signals2::scoped_connection _splitter_connection;

  // Observer
  virtual auto handle_grt_notification(const std::string &name, grt::ObjectRef sender, grt::DictRef info) -> void;

  auto updateColors() -> void;

  // LiveSchemaTree::FetchDelegate
  virtual auto fetch_schema_list() -> std::vector<std::string>;
  virtual auto fetch_data_for_filter(const std::string &schema_filter, const std::string &object_filter,
                                     const wb::LiveSchemaTree::NewSchemaContentArrivedSlot &arrived_slot) -> bool;
  virtual auto fetch_schema_contents(const std::string &schema_name,
                                     const wb::LiveSchemaTree::NewSchemaContentArrivedSlot &arrived_slot) -> bool;
  virtual auto fetch_object_details(const std::string &schema_name, const std::string &object_name,
                                    wb::LiveSchemaTree::ObjectType type, short flags,
                                    const wb::LiveSchemaTree::NodeChildrenUpdaterSlot &) -> bool;
  virtual auto fetch_routine_details(const std::string &schema_name, const std::string &obj_name,
                                     wb::LiveSchemaTree::ObjectType type) -> bool;
  // LiveSchemaTree::Delegate
  virtual auto tree_refresh() -> void;
  virtual auto sidebar_action(const std::string &) -> bool;
  virtual auto tree_activate_objects(const std::string &, const std::vector<wb::LiveSchemaTree::ChangeRecord> &changes) -> void;

public:
  virtual auto tree_create_object(wb::LiveSchemaTree::ObjectType type, const std::string &schema_name,
                                  const std::string &obj_name) -> void;

  auto generate_alter_script(const db_mgmt_RdbmsRef &rdbms, db_DatabaseObjectRef db_object,
                                    std::string algorithm, std::string lock) -> std::string;

private:
  auto do_fetch_live_schema_contents(std::weak_ptr<SqlEditorTreeController> self_ptr,
                                               const std::string &schema_name,
                                               wb::LiveSchemaTree::NewSchemaContentArrivedSlot arrived_slot) -> grt::StringRef;
  auto fetch_object_type(const std::string &schema_name, const std::string &obj_name) -> wb::LiveSchemaTree::ObjectType;
  auto fetch_column_data(const std::string &schema_name, const std::string &obj_name,
                         wb::LiveSchemaTree::ObjectType type,
                         const wb::LiveSchemaTree::NodeChildrenUpdaterSlot &updater_slot) -> void;
  auto fetch_trigger_data(const std::string &schema_name, const std::string &obj_name,
                          wb::LiveSchemaTree::ObjectType type,
                          const wb::LiveSchemaTree::NodeChildrenUpdaterSlot &updater_slot) -> void;
  auto fetch_index_data(const std::string &schema_name, const std::string &obj_name,
                        wb::LiveSchemaTree::ObjectType type,
                        const wb::LiveSchemaTree::NodeChildrenUpdaterSlot &updater_slot) -> void;
  auto fetch_foreign_key_data(const std::string &schema_name, const std::string &obj_name,
                              wb::LiveSchemaTree::ObjectType type,
                              const wb::LiveSchemaTree::NodeChildrenUpdaterSlot &updater_slot) -> void;

  auto do_fetch_data_for_filter(std::weak_ptr<SqlEditorTreeController> self_ptr,
                                          const std::string &schema_filter, const std::string &object_filter,
                                          wb::LiveSchemaTree::NewSchemaContentArrivedSlot arrived_slot) -> grt::StringRef;

  auto schema_row_selected() -> void;
  auto side_bar_filter_changed(const std::string &filter) -> void;
  auto sidebar_splitter_changed() -> void;

  auto context_menu_will_show(mforms::MenuItem *parent_item) -> void;

public:
  auto refresh_live_object_in_editor(bec::DBObjectEditorBE *obj_editor, bool using_old_name) -> void;
  auto refresh_live_object_in_overview(wb::LiveSchemaTree::ObjectType type, const std::string schema_name,
                                       const std::string old_obj_name, const std::string new_obj_name) -> void;

  auto on_active_schema_change(const std::string &schema) -> void;
  auto mark_busy(bool busy) -> void;

  auto open_alter_object_editor(db_DatabaseObjectRef object, db_CatalogRef server_state_catalog) -> void;

private:
  auto do_refresh_schema_tree_safe(std::weak_ptr<SqlEditorForm> self_ptr) -> grt::StringRef;

  auto insert_text_to_active_editor(const std::string &str) -> int;

private:
  auto do_alter_live_object(wb::LiveSchemaTree::ObjectType type, const std::string &schema_name,
                            const std::string &obj_name) -> void;
  auto run_execute_routine_wizard(wb::LiveSchemaTree::ObjectType type, const std::string &schema_name,
                                         const std::string &obj_name) -> std::string;

  auto get_object_ddl_script(wb::LiveSchemaTree::ObjectType type, const std::string &schema_name,
                                    const std::string &obj_name) -> std::string;
  auto get_object_create_script(wb::LiveSchemaTree::ObjectType type,
                                                               const std::string &schema_name,
                                                               const std::string &obj_name) -> std::pair<std::string, std::string>;
  auto get_trigger_sql_for_table(const std::string &schema_name, const std::string &table_name) -> std::vector<std::string>;

  auto parse_ddl_into_catalog(db_mysql_CatalogRef catalog, const std::string &objectDescription, const std::string &sql,
                              std::string sqlMode, const std::string &schema) -> bool;

public:
  auto schema_object_activated(const std::string &action, wb::LiveSchemaTree::ObjectType type,
                               const std::string &schema, const std::string &name) -> void;
  auto apply_changes_to_object(bec::DBObjectEditorBE *obj_editor, bool dry_run) -> bool;

  auto get_sidebar() -> mforms::View *;

  auto get_schema_tree() -> wb::LiveSchemaTree *;
  auto request_refresh_schema_tree() -> void;

public:
  auto create_live_table_stubs(bec::DBObjectEditorBE *table_editor) -> void;
  auto expand_live_table_stub(bec::DBObjectEditorBE *table_editor, const std::string &schema_name,
                              const std::string &obj_name) -> bool;

public:
  auto activate_live_object(GrtObjectRef object) -> bool;

private:
  auto create_new_schema(db_CatalogRef owner) -> db_SchemaRef;
  auto create_new_table(db_SchemaRef owner) -> db_TableRef;
  auto create_new_view(db_SchemaRef owner) -> db_ViewRef;
  auto create_new_routine(db_SchemaRef owner, wb::LiveSchemaTree::ObjectType type) -> db_RoutineRef;
};
