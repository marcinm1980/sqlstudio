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

#ifndef _WBCONTEXT_SQLIDE_H_
#define _WBCONTEXT_SQLIDE_H_

#include "studio/wb_backend_public_interface.h"
#include "grt/plugin_manager.h"
#include "grts/structs.db.mgmt.h"
#include "grts/structs.app.h"
//#include "dialogs/search_replace.h"
#include "base/trackable.h"
#include "base/notifications.h"
#include "mforms/utilities.h"

class SqlEditorPanel;
class SqlEditorForm;

// Central point of management for SQLIDE (sql editor)
namespace wb {
  class WBContextUI;
  class CommandUI;

  class MYSQLWBBACKEND_PUBLIC_FUNC WBContextSQLIDE : public base::trackable, base::Observer {
    std::list<std::weak_ptr<SqlEditorForm> > _open_editors;
    app_ToolbarRef _toolbar;

    mforms::TimeoutHandle _auto_save_handle;
    ssize_t _auto_save_interval;
    bool _auto_save_active;
    bool _option_change_signal_connected;

  public:
    WBContextSQLIDE();
    virtual ~WBContextSQLIDE();
    auto init() -> void;
    auto finalize() -> void;

    static auto detect_auto_save_files(const std::string &autosave_dir) -> void;
    static auto auto_save_sessions() -> std::map<std::string, std::string>;

    auto create_connected_editor(const db_mgmt_ConnectionRef &conn) -> std::shared_ptr<SqlEditorForm>;

    auto get_cmdui() -> CommandUI *;

    auto get_active_sql_editor() -> SqlEditorForm *;

    auto activate_live_object(GrtObjectRef object) -> bool;

    auto open_document(const std::string &path) -> void;

    auto request_quit() -> bool;

    auto reconnect_editor(SqlEditorForm *editor) -> void;

    void editor_will_close(SqlEditorForm *); // to be called by SqlEditorForm

  public:
    auto update_plugin_arguments_pool(bec::ArgumentPool &args) -> void;

    auto get_grt_editor_object(SqlEditorForm *editor) -> db_query_EditorRef;
    auto get_open_editors() -> std::list<std::weak_ptr<SqlEditorForm> > * {
      return &_open_editors;
    }

  private:
    void call_in_editor(void (SqlEditorForm::*method)());
    auto call_in_editor_str(void (SqlEditorForm::*method)(const std::string &arg), const std::string &arg) -> void;
    auto call_in_editor_str2(void (SqlEditorForm::*method)(const std::string &arg1, bool arg2, bool arg3),
                             const std::string &arg1, bool arg2, bool arg3) -> void;
    auto call_in_editor_bool(void (SqlEditorForm::*method)(bool arg), bool arg) -> void;

    void call_in_editor_panel(void (SqlEditorPanel::*method)());

    auto auto_save_workspaces() -> bool;
    auto option_changed(grt::internal::OwnedDict *dict, bool, const std::string &key) -> void;

    virtual auto handle_notification(const std::string &name, void *sender, base::NotificationInfo &info) -> void;
  };
};

#endif
