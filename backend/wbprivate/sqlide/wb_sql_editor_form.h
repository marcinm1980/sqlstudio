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

#pragma once

#include "studio/wb_backend_public_interface.h"

#include "base/file_utilities.h"
#include "base/ui_form.h"
#include "base/threaded_timer.h"

#include "grts/structs.studio.h"
#include "grts/structs.db.mgmt.h"
#include "grtpp_notifications.h"

#include "sqlide/recordset_be.h"
#include "sqlide/sql_editor_be.h"
#include "sqlide/db_sql_editor_log.h"
#include "sqlide/db_sql_editor_history_be.h"
#include "sqlide/wb_context_sqlide.h"
#include "sqlide/wb_live_schema_tree.h"

#include "cppdbc.h"

#include "mforms/view.h"

#include "SymbolTable.h"

namespace mforms {
  class ToolBar;
  class AppView;
  class View;
  class MenuItem;
  class DockingPoint;
}; // namespace mforms

namespace bec {
  class DBObjectEditorBE;
}

#define MAIN_DOCKING_POINT "db.query.Editor:main"
#define RESULT_DOCKING_POINT "db.Query.QueryEditor:result"

class QuerySidePalette;
class SqlEditorTreeController;
class ColumnWidthCache;
class SqlEditorPanel;
class SqlEditorResult;

namespace wb {
  class SSHTunnel;
}

using Recordsets = std::vector<Recordset::Ref>;
using RecordsetsRef = std::shared_ptr<Recordsets>;

auto getServerInstance(const db_mgmt_ConnectionRef &connection) -> db_mgmt_ServerInstanceRef;

class MYSQLWBBACKEND_PUBLIC_FUNC SqlEditorForm : public bec::UIForm,
                                                 grt::GRTObserver,
                                                 public std::enable_shared_from_this<SqlEditorForm>,
                                                 mforms::DropDelegate {
public:
#if defined(ENABLE_TESTING)
  friend class EditorFormTester;
  friend class LocalEditorFormTester;
#endif

  enum ServerState { UnknownState, RunningState, PossiblyStoppedState, OfflineState };

  struct PSStage {
    std::string name;
    double wait_time;
  };

  struct PSWait {
    std::string name;
    double wait_time;
  };

  class RecordsetData : public Recordset::ClientData {
  public:
    SqlEditorResult *result_panel;
    std::string generator_query;

    double duration;
    std::string ps_stat_error;
    std::map<std::string, std::int64_t> ps_stat_info;
    std::vector<PSStage> ps_stage_info;
    std::vector<PSWait> ps_wait_info;
  };

public:
  using Ref = std::shared_ptr<SqlEditorForm>;
  using Ptr = std::weak_ptr<SqlEditorForm>;
  static auto create(wb::WBContextSQLIDE *wbsql, const db_mgmt_ConnectionRef &conn) -> SqlEditorForm::Ref;
  static auto report_connection_failure(const std::string &error, const db_mgmt_ConnectionRef &target) -> void;
  static auto report_connection_failure(const grt::server_denied &info, const db_mgmt_ConnectionRef &target) -> void;

  auto set_tab_dock(mforms::DockingPoint *dp) -> void;

  /* Callback must be set by frontend to show a busy indicator on the tab with the given index. -1 means remove it from
   * all */
  std::function<void(int)> set_busy_tab;

  auto databaseSymbols() -> parsers::SymbolTable * {
    return &_databaseSymbols;
  }

protected:
  SqlEditorForm(wb::WBContextSQLIDE *wbsql);

  auto update_menu_and_toolbar() -> void;
  auto update_toolbar_icons() -> void;

  auto save_workspace_order(const std::string &prefix) -> void;
  auto find_workspace_state(const std::string &workspace_name, std::unique_ptr<base::LockFile> &lock_file) -> std::string;

public:
  virtual ~SqlEditorForm();

  auto cancel_connect() -> void;
  virtual auto close() -> void;
  virtual auto is_main_form() -> bool {
    return true;
  }
  virtual auto get_form_context_name() const -> std::string;

  virtual auto get_menubar() -> mforms::MenuBar *;
  virtual auto get_toolbar() -> mforms::ToolBar *;
  auto get_session_name() -> std::string;

  auto auto_save() -> void;
  auto save_workspace(const std::string &workspace_name, bool is_autosave) -> void;
  auto load_workspace(const std::string &workspace_name) -> bool;

  auto restore_last_workspace() -> void;

public:
  auto wbsql() const -> wb::WBContextSQLIDE * {
    return _wbsql;
  }

  auto grtobj() -> db_query_EditorRef;

  auto validate_menubar() -> void;

  auto handle_tab_menu_action(const std::string &action, int tab_index) -> void;
  auto handle_history_action(const std::string &action, const std::string &sql) -> void;

public:
  // do NOT use rdbms->version().. it's not specific for this connection
  auto rdbms() -> db_mgmt_RdbmsRef;
  auto rdbms_version() const -> GrtVersionRef;

  auto get_connection_info() const -> std::string {
    return _connectionInfo;
  }

public:
  auto active_sql_editor_panel() -> SqlEditorPanel *;

  auto sql_editor_reordered(SqlEditorPanel *editor, int new_index) -> void;

  auto is_closing() const -> bool {
    return _closing;
  }

private:
  int _sql_editors_serial = 0;
  int _scratch_editors_serial = 0;
  std::shared_ptr<wb::SSHTunnel> _tunnel;
  db_mgmt_SSHConnectionRef _sshConnection;

  auto sql_editor_panel_switched() -> void;
  auto sql_editor_panel_closed(mforms::AppView *view) -> void;

  auto set_editor_tool_items_enbled(const std::string &name, bool flag) -> void;
  auto set_editor_tool_items_checked(const std::string &name, bool flag) -> void;

public:
  auto set_tool_item_checked(const std::string &name, bool flag) -> void;

  boost::signals2::signal<void(MySQLEditor::Ref, bool)> sql_editor_list_changed;

  auto run_sql_in_scratch_tab(const std::string &sql, bool reuse_if_possible, bool start_collapsed) -> SqlEditorPanel *;
  auto add_sql_editor(bool scratch = false,
                                 bool start_collapsed = false) -> SqlEditorPanel *; // returns index of the added sql_editor
  auto remove_sql_editor(SqlEditorPanel *panel) -> void;
  auto sql_editor_panel(int index) -> SqlEditorPanel *;
  auto sql_editor_count() -> int;
  auto sql_editor_panel_index(SqlEditorPanel *panel) -> int;

  virtual auto drag_over(mforms::View *sender, base::Point p, mforms::DragOperation allowedOperations,
                                          const std::vector<std::string> &formats) -> mforms::DragOperation;
  virtual auto files_dropped(mforms::View *sender, base::Point p,
                                              mforms::DragOperation allowedOperations,
                                              const std::vector<std::string> &file_names) -> mforms::DragOperation;

private:
  auto count_connection_editors(const std::string &conn_name) -> int;

protected:
  auto create_title() -> std::string;
  auto title_changed() -> void;
  auto check_server_problems() -> void;

public:
  virtual auto get_title() -> std::string {
    return _title;
  }
  auto update_title() -> void;

  auto getTunnelPort() const -> int;

  auto connection_details() -> std::map<std::string, std::string> & {
    return _connection_details;
  }
  auto server_version() -> int;
  auto valid_charsets() -> std::set<std::string>;

private:
  auto do_connect(std::shared_ptr<wb::SSHTunnel> tunnel, sql::Authentication::Ref &auth,
                            struct ConnectionErrorInfo *autherr_ptr) -> grt::StringRef;
  auto get_client_lib_version() -> std::string;
  auto do_disconnect() -> grt::StringRef;

  auto update_connected_state() -> void;

public:
  auto connect(std::shared_ptr<wb::SSHTunnel> tunnel) -> bool;
  auto connected() const -> bool;
  auto connectionIsValid() const -> bool {
    return _connection.is_valid();
  }
  auto checkIfOffline() -> void;
  auto offline() -> bool;
  auto ping() const -> bool;
  auto finish_startup() -> void;
  auto cancel_query() -> void;
  auto reset() -> void;
  auto commit() -> void;
  auto rollback() -> void;
  auto auto_commit() -> bool;
  auto auto_commit(bool value) -> void;
  auto toggle_autocommit() -> void;
  auto toggle_collect_field_info() -> void;
  auto collect_field_info() const -> bool;
  auto toggle_collect_ps_statement_events() -> void;
  auto collect_ps_statement_events() const -> bool;

  auto set_connection(db_mgmt_ConnectionRef conn) -> void;

  auto run_editor_contents(bool current_statement_only) -> void;

  auto limit_rows(const std::string &limit_text) -> void;

  auto sql_mode() const -> std::string {
    return _sql_mode;
  };
  auto lower_case_table_names() const -> int {
    return _lower_case_table_names;
  }

private:
  auto do_commit() -> void;

public:
  auto connection_descriptor() const -> db_mgmt_ConnectionRef {
    return _connection;
  }

  auto getSSHConnection() -> db_mgmt_SSHConnectionRef;

  auto get_session_variable(sql::Connection *dbc_conn, const std::string &name, std::string &value) -> bool;

private:
  auto cache_sql_mode() -> void;
  auto update_sql_mode_for_editors() -> void;

  auto query_ps_statistics(std::int64_t conn_id, std::map<std::string, std::int64_t> &stats) -> void;

  auto query_ps_stages(std::int64_t stmt_event_id) -> std::vector<SqlEditorForm::PSStage>;
  auto query_ps_waits(std::int64_t stmt_event_id) -> std::vector<SqlEditorForm::PSWait>;

private:
  auto create_connection(sql::Dbc_connection_handler::Ref &dbc_conn, db_mgmt_ConnectionRef db_mgmt_conn,
                         std::shared_ptr<wb::SSHTunnel> tunnel, sql::Authentication::Ref auth, bool autocommit_mode,
                         bool user_connection) -> void;
  auto init_connection(sql::Connection *dbc_conn_ref, const db_mgmt_ConnectionRef &connectionProperties,
                       sql::Dbc_connection_handler::Ref &dbc_conn, bool user_connection) -> void;
  auto close_connection(sql::Dbc_connection_handler::Ref &dbc_conn) -> void;
  auto ensure_valid_dbc_connection(sql::Dbc_connection_handler::Ref &dbc_conn,
                                                 base::RecMutex &dbc_conn_mutex, bool throw_on_block = false,
                                                 bool lockOnly = false) -> base::RecMutexLock;
  auto ensure_valid_usr_connection(bool throw_on_block = false, bool lockOnly = false) -> base::RecMutexLock;
  auto ensure_valid_aux_connection(bool throw_on_block = false, bool lockOnly = false) -> base::RecMutexLock;

  std::vector<std::pair<std::string, std::string>> runQueryForCache(const std::string &query);

public:
  auto ensure_valid_aux_connection(sql::Dbc_connection_handler::Ref &conn, bool lockOnly = false) -> base::RecMutexLock;
  auto work_parser_context() -> parsers::MySQLParserContext::Ref {
    return _work_parser_context;
  };

private:
  auto send_message_keep_alive() -> void;
  auto send_message_keep_alive_bool_wrapper() -> bool {
    send_message_keep_alive();
    return false;
  } // need it for ThreadedTimer, which expects callbacks to return bool
  auto reset_keep_alive_thread() -> void;

  auto getAuxConnection(sql::Dbc_connection_handler::Ref &conn, bool lockOnly = false) -> base::RecMutexLock;
  auto getUserConnection(sql::Dbc_connection_handler::Ref &conn, bool lockOnly = false) -> base::RecMutexLock;

  auto onCacheAction(bool active) -> void;

public:
  auto column_width_cache() -> ColumnWidthCache * {
    return _column_width_cache;
  }

  auto exec_editor_sql(SqlEditorPanel *editor, bool sync, bool current_statement_only = false,
                       bool wrap_with_non_std_delimiter = false, bool dont_add_limit_clause = false,
                       SqlEditorResult *into_result = NULL) -> bool;
  auto exec_sql_retaining_editor_contents(const std::string &sql_script, SqlEditorPanel *editor, bool sync,
                                          bool dont_add_limit_clause = false) -> void;

  auto exec_sql_returning_results(const std::string &sql_script, bool dont_add_limit_clause) -> RecordsetsRef;

  auto exec_management_sql(const std::string &sql, bool log) -> void;
  auto exec_management_query(const std::string &sql, bool log) -> db_query_ResultsetRef;

  auto exec_main_sql(const std::string &sql, bool log) -> void;
  auto exec_main_query(const std::string &sql, bool log) -> db_query_ResultsetRef;

  auto explain_current_statement() -> void;
  auto is_running_query() -> bool;

  auto dbc_auth_data() -> sql::Authentication::Ref {
    return _dbc_auth;
  }

private:
  enum ExecFlags { NeedNonStdDelimiter = 1 << 1, DontAddLimitClause = 1 << 2, ShowWarnings = 1 << 3 };
  auto update_live_schema_tree(const std::string &sql) -> void;

  auto do_exec_sql(Ptr self_ptr, std::shared_ptr<std::string> sql, SqlEditorPanel *editor, ExecFlags flags,
                             RecordsetsRef result_list) -> grt::StringRef;

  auto handle_command_side_effects(const std::string &sql) -> void;

public:
  GrtThreadedTask::Ref exec_sql_task;

  std::function<void()> post_query_slot; // called after a query is executed
private:
  auto on_exec_sql_finished() -> int;

public:
  auto continue_on_error() -> bool {
    return _continueOnError;
  }
  auto continue_on_error(bool val) -> void;

private:
  using Error_cb =
    boost::signals2::signal<int(long long, const std::string &, const std::string &), boost::signals2::last_value<int>>;
  using Batch_exec_progress_cb = boost::signals2::signal<int(float), boost::signals2::last_value<int>>;
  using Batch_exec_stat_cb = boost::signals2::signal<int(long, long), boost::signals2::last_value<int>>;

public:
  Error_cb on_sql_script_run_error;

private:
  auto sql_script_apply_error(long long, const std::string &, const std::string &, std::string &) -> int;
  int sql_script_apply_progress(float);
  int sql_script_stats(long, long);

  auto abort_apply_object_alter_script() -> void;

public:
  auto apply_object_alter_script(const std::string &alter_script, bec::DBObjectEditorBE *obj_editor, RowId log_id) -> void;
  auto run_live_object_alteration_wizard(const std::string &alter_script, bec::DBObjectEditorBE *obj_editor,
                                         RowId log_id, const std::string &log_context) -> bool;

private:
  auto apply_changes_to_recordset(Recordset::Ptr rs_ptr) -> void;
  auto run_data_changes_commit_wizard(Recordset::Ptr rs_ptr, bool skip_commit) -> bool;
  auto apply_data_changes_commit(const std::string &sql_script_text, Recordset::Ptr rs_ptr, bool skip_commit) -> void;
  auto update_editor_title_schema(const std::string &schema) -> void;

public:
  auto can_close() -> bool;
  auto can_close_(bool interactive) -> bool;

  auto check_external_file_changes() -> void;

public:
  auto new_sql_script_file() -> SqlEditorPanel *;
  auto new_sql_scratch_area(bool start_collapsed = false) -> SqlEditorPanel *;
  auto new_scratch_area() -> void {
    new_sql_scratch_area(false);
  }
  auto open_file(const std::string &path, bool in_new_tab, bool askForFile = true) -> void;
  void open_file(const std::string &path = "") {
    open_file(path, true, !path.empty());
  }

public:
  auto active_schema(const std::string &value) -> void;
  auto active_schema() const -> std::string;

  auto schemaListRefreshed(std::vector<std::string> const &schemas) -> void;

  auto schema_meta_data_refreshed(const std::string &schema_name, base::StringListPtr tables, base::StringListPtr views,
                                  base::StringListPtr procedures, base::StringListPtr functions) -> void;

private:
  auto cache_active_schema_name() -> void;

public:
  auto request_refresh_schema_tree() -> void;

public:
  auto fetch_data_from_stored_procedure(std::string proc_call, std::shared_ptr<sql::ResultSet> &rs) -> std::string;

  auto log() -> DbSqlEditorLog::Ref {
    return _log;
  }
  auto history() -> DbSqlEditorHistory::Ref {
    return _history;
  }
  auto restore_sql_from_history(int entry_index, std::list<int> &detail_indexes) -> std::string;
  auto exec_sql_error_count() -> int {
    return _exec_sql_error_count;
  }

  auto get_live_tree() -> std::shared_ptr<SqlEditorTreeController> {
    return _live_tree;
  }
  auto schema_tree_did_populate() -> void;

  std::function<void(const std::string &, bool)> output_text_slot;

public:
  // Result should be RowId but that requires to change the task callback type (at least for 64bit builds).
  auto add_log_message(int msg_type, const std::string &msg, const std::string &context, const std::string &duration) -> int;
  auto set_log_message(RowId log_message_index, int msg_type, const std::string &msg, const std::string &context,
                       const std::string &duration) -> void;
  auto refresh_log_messages(bool ignore_last_message_timestamp) -> void;

protected:
  DbSqlEditorLog::Ref _log;
  DbSqlEditorHistory::Ref _history;
  bool _serverIsOffline = false;

  std::string _title;

private:
  virtual auto handle_grt_notification(const std::string &name, grt::ObjectRef sender, grt::DictRef info) -> void;
  virtual auto handle_notification(const std::string &name, void *sender, base::NotificationInfo &info) -> void;
  auto setup_side_palette() -> void;

  auto schema_row_selected() -> void;
  auto side_bar_filter_changed(const std::string &filter) -> void;

  auto note_connection_open_outcome(int error) -> void;

public:
  auto inspect_object(const std::string &name, const std::string &object, const std::string &type) -> void;

  auto toolbar_command(const std::string &command) -> void;

  auto save_snippet() -> bool;

  auto show_output_area() -> void;

  auto get_sidebar() -> mforms::View *;
  auto get_side_palette() -> mforms::View *;

  auto set_autosave_disabled(const bool autosave_disabled) -> void;
  auto get_autosave_disabled(void) -> bool;

private:
  wb::WBContextSQLIDE *_wbsql;
  GrtVersionRef _version;
  mforms::MenuBar *_menu = nullptr;
  mforms::ToolBar *_toolbar = nullptr;
  std::string _connectionInfo;
  base::LockFile *_autosave_lock = nullptr;
  std::string _autosave_path;

  mforms::DockingPoint *_tabdock = nullptr;

  // Set when we triggered a refresh asynchronously.
  boost::signals2::connection _overviewRefreshPending;
  boost::signals2::connection _editorRefreshPending;

  int _keep_alive_task_id = 0;
  base::Mutex _keep_alive_thread_mutex;

  Batch_exec_progress_cb on_sql_script_run_progress;
  Batch_exec_stat_cb on_sql_script_run_statistics;

  bool _autosave_disabled = false;
  bool _loading_workspace = false;
  bool _cancel_connect = false;
  bool _closing = false;
  bool _startup_done = false;
  bool _is_running_query = false;
  bool _continueOnError = false;
  bool _has_pending_log_messages = false;

  double _last_log_message_timestamp;
  int _exec_sql_error_count;

  std::shared_ptr<SqlEditorTreeController> _live_tree;

  mforms::View *_side_palette_host = nullptr;
  QuerySidePalette *_side_palette = nullptr;
  std::string _pending_expand_nodes;

  std::map<std::string, std::string> _connection_details;
  std::set<std::string> _charsets;

  std::string _sql_mode;
  int _lower_case_table_names;
  parsers::MySQLParserContext::Ref _work_parser_context; // Never use in a background thread.

  db_mgmt_ConnectionRef _connection;
  // connection for maintenance operations, fetching schema contents & live editors (DDL only)
  sql::Dbc_connection_handler::Ref _aux_dbc_conn;
  base::RecMutex _aux_dbc_conn_mutex;

  // connection for running sql scripts
  sql::Dbc_connection_handler::Ref _usr_dbc_conn;
  mutable base::RecMutex _usr_dbc_conn_mutex;

  sql::Authentication::Ref _dbc_auth;

  ServerState _last_server_running_state = UnknownState;

  ColumnWidthCache *_column_width_cache = nullptr;

  parsers::SymbolTable _staticServerSymbols; // Charsets, collations, engines.
  parsers::SymbolTable _databaseSymbols;     // All available db objects reachable via the current connection.

  auto activate_command(const std::string &command) -> void;
  auto readStaticServerSymbols() -> void;

  // workaround for managed code windows
  struct PrivateMutex;
  std::unique_ptr<PrivateMutex> _pimplMutex;
};
