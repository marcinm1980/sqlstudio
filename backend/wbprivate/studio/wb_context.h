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

#ifndef _MSC_VER
#include <vector>
#endif

#include "base/ui_form.h"
#include "grt/grt_manager.h"
#include "base/notifications.h"

#include "wb_context_names.h"

#include "grts/structs.studio.h"
#include "wb_backend_public_interface.h"

#include "grtpp_undo_manager.h"

#include "mforms/utilities.h"
#include "base/trackable.h"
#include "base/threading.h"
#include "base/data_types.h"

#define WBContext_VERSION 5

#define WB_DBOBJECT_DRAG_TYPE "com.mysql.studio.DatabaseObject"
#define WB_CONTROL_DRAG_TYPE "com.mysql.studio.control"

const int ONE_MB = 1024*1024;

namespace mdc {
  class CanvasView;
  class CanvasItem;
};

namespace bec {
  class Clipboard;
  class IconManager;
};

class SqlEditorForm;

namespace wb {

  class WBContextUI;
  class WBContextModel;
  class WBContextSQLIDE;
  class MySqlStudioImpl;
  class WBComponent;

  class ModelFile;

  class TunnelManager;

  enum RefreshType {
    RefreshNeeded, // Front end should schedule a refresh flush asap (usually called in worker thread).
    RefreshNothing,
    RefreshSchemaNoReload,
    RefreshNewDiagram,
    RefreshSelection,
    RefreshCloseEditor, // argument: object-id (close all if "")

    RefreshNewModel,

    RefreshOverviewNodeInfo,     // argument: node-id, overview ptr (bec::UIForm*)
    RefreshOverviewNodeChildren, // argument: node-id, overview ptr

    RefreshDocument,
    RefreshCloseDocument,
    RefreshZoom,
    RefreshTimer,

    RefreshFinishEdits // Force all ongoing edit operations (eg in TreeView cells) to be committed
  };

  struct MYSQLWBBACKEND_PUBLIC_FUNC WBPaperSize {
    std::string name;
    std::string caption;
    double width;
    double height;
    bool margins_set;
    double margin_top;
    double margin_bottom;
    double margin_left;
    double margin_right;
    std::string description;
  };

// basic toolbar names
#define WB_TOOLBAR_OPTIONS "options"

// main view types
#define WB_MAIN_VIEW_DB_QUERY "dbquery"

  class ModelDiagramForm;
  class FindDialogBE;

  enum PageOrientation { Landscape, Portrait };

  enum RequestInputFlag { InputPassword = (1 << 0) };

  struct MYSQLWBBACKEND_PUBLIC_FUNC WBFrontendCallbacks {
    // Args: type, title, file extensions
    std::function<std::string(std::string, std::string, std::string)> show_file_dialog;

    // Show some text in the application's status bar: must be thread-safe
    std::function<void(std::string)> show_status_text;

    // Open an editor
    // Args: grtmanager, module containing plugin, editor dll, editor class, edited object
    std::function<NativeHandle(grt::Module *, std::string, std::string, grt::BaseListRef, bec::GUIPluginFlags)>
      open_editor;
    // Show/Hide an editor
    // Args: editor handle (e.g: window handle)
    std::function<void(NativeHandle)> show_editor;
    std::function<void(NativeHandle)> hide_editor;

    // Execute a built-in command
    std::function<void(std::string)> perform_command;

    // Create a new diagram.
    std::function<mdc::CanvasView *(const model_DiagramRef &)> create_diagram;
    // Destroy a previously created canvas view
    std::function<void(mdc::CanvasView *)> destroy_view;
    // Signals the current view has been changed
    std::function<void(mdc::CanvasView *)> switched_view;

    // Open the named type of main view tab with the given form object. ownership is passed to frontend
    // Args: type (eg query), bec::UIForm*
    std::function<void(std::string, std::shared_ptr<bec::UIForm>)> create_main_form_view;
    std::function<void(bec::UIForm *)> destroy_main_form_view;

    // The tool for the view has been changed
    std::function<void(mdc::CanvasView *)> tool_changed;

    // Refresh interface
    std::function<void(RefreshType, std::string, NativeHandle)> refresh_gui;
    std::function<void(bool)> lock_gui;

    // Closes the application
    std::function<bool()> quit_application;
  };

  struct MYSQLWBBACKEND_PUBLIC_FUNC WBOptions {
    std::string basedir;
    std::string plugin_search_path;
    std::string struct_search_path;
    std::string module_search_path;
    std::string library_search_path;
    std::string cdbc_driver_search_path;
    std::string user_data_dir;
    std::string open_at_startup_type; // model, query, admin, script
    std::string open_at_startup;
    std::string open_connection;
    std::string run_at_startup; // script to be executed when started
    std::string run_language;   // language of the script in run_at_startup
    std::string binaryName;
    bool force_sw_rendering;
    bool force_opengl_rendering;
    bool verbose;
    bool quit_when_done;
    bool testing;         // True if we are currently running unit tests.
    bool init_python;     // True by default. Can be switched off for testing.
    bool full_init;       // True by default. Should be switched off when the options are created for an already running
                          // instance of WB.
    bool logLevelSet;
    WBOptions(const std::string &appBinaryName);
    ~WBOptions();
    auto analyzeCommandLineArguments() -> void;
    dataTypes::OptionsList *programOptions;
  };

#define FOREACH_COMPONENT(list, iter) \
  for (std::vector<WBComponent *>::iterator iter = list.begin(); iter != list.end(); ++iter)

  class MYSQLWBBACKEND_PUBLIC_FUNC WBContext : public base::trackable, base::Observer {
    friend class MySqlStudioImpl;
    friend class WBComponent;
    friend class WBContextUI;

  public:
    WBContext(bool verbose = false);
    virtual ~WBContext();

    auto software_rendering_enforced() -> bool;
    auto opengl_rendering_enforced() -> bool;

    auto init_(WBFrontendCallbacks *callbacks, WBOptions *options) -> bool;
    auto init_finish_(WBOptions *options) -> void;
    auto finalize() -> void;

    auto is_commercial() -> bool;

    auto get_active_form() -> bec::UIForm *;
    auto get_active_main_form() -> bec::UIForm *;

    auto get_model_context() -> WBContextModel * {
      return _model_context;
    }
    auto get_sqlide_context() -> WBContextSQLIDE * {
      return _sqlide_context;
    }

    // Document handling.
    auto new_document() -> void;
    auto can_close_document() -> bool; // returns false for cancelled
    auto close_document() -> bool;

    auto close_document_finish() -> void;
    auto new_model_finish() -> void;

    // save document

    auto save_as(const std::string &path) -> bool;

    auto get_filename() const -> std::string;

    auto report_bug(const std::string &errorInfo) -> void;

    // plugins
    auto execute_plugin(const std::string &plugin_name, const bec::ArgumentPool &argpool = bec::ArgumentPool()) -> void;

    auto update_plugin_arguments_pool(bec::ArgumentPool &args) -> void;

    // DB Querying
    auto add_new_query_window(const db_mgmt_ConnectionRef &target,
                                                        bool restore_session = true) -> std::shared_ptr<SqlEditorForm>;
    auto add_new_query_window() -> std::shared_ptr<SqlEditorForm>;

    // Admin
    auto add_new_admin_window(const db_mgmt_ConnectionRef &target) -> void;

    // Generic plugin tabs
    auto add_new_plugin_window(const std::string &plugin_id, const std::string &caption) -> void;

    // GUI Plugin
    auto register_builtin_plugins(grt::ListRef<app_Plugin> plugins) -> void;

    auto close_gui_plugin(NativeHandle handle) -> void;

    //
    auto request_refresh(RefreshType type, const std::string &str, NativeHandle ptr = (NativeHandle)0) -> void;

    auto get_user_datadir() const -> const std::string & {
      return _user_datadir;
    }
    // TODO: Temporary solution need to make ModelFile grt class
    auto openModelFile(const std::string &file) -> studio_DocumentRef;
    auto getTempDir() -> std::string;
    auto closeModelFile() -> int;
    auto getDbFilePath() -> std::string;

    auto open_document(const std::string &file) -> bool;
    auto open_script_file(const std::string &file) -> void;
    auto open_recent_document(int index) -> void;
    auto has_unsaved_changes() -> bool;
    auto save_changes() -> bool;

    auto open_file_by_extension(const std::string &path, bool interactive) -> bool;

    auto get_plugin_manager() -> bec::PluginManager * {
      return _plugin_manager;
    }
    template <class C>
    C *get_component() {
      return dynamic_cast<C *>(get_component_named(C::name()));
    }

    auto get_component_named(const std::string &name) -> WBComponent *;

    auto get_component_handling(const model_ObjectRef &object) -> WBComponent *;

    auto foreach_component(const std::function<void(WBComponent *)> &slot) -> void;

    auto get_studio() -> MySqlStudioImpl * {
      return _studio;
    };

    auto get_clipboard() const -> bec::Clipboard * {
      return _clipboard;
    }

    auto get_root() -> studio_MySqlStudioRef;
    auto get_document() -> studio_DocumentRef;
    auto get_wb_options() -> grt::DictRef;

    auto get_datadir() const -> std::string {
      return _datadir;
    }

    auto cancel_idle_tasks() -> bool;
    auto flush_idle_tasks(bool force) -> void;

    // utilities for error reporting
    auto show_exception(const std::string &operation, const std::exception &exc) -> void;
    auto show_exception(const std::string &operation, const grt::grt_runtime_error &exc) -> void;

    template <class R>
    R execute_in_main_thread(const std::string &name, const std::function<R()> &function) {
      return bec::GRTManager::get()->get_dispatcher()->call_from_main_thread /*<R>*/ (function, true, false);
    }
    auto execute_in_main_thread(const std::string &name, const std::function<void()> &function, bool wait) -> void;

    auto execute_in_grt_thread(const std::string &name, const std::function<grt::ValueRef()> &function) -> grt::ValueRef;

    auto execute_async_in_grt_thread(const std::string &name, const std::function<grt::ValueRef()> &function) -> void;

    auto activate_live_object(const GrtObjectRef &object) -> bool;

    auto create_attached_file(const std::string &group, const std::string &tmpl) -> std::string;
    auto save_attached_file_contents(const std::string &name, const char *data, size_t size) -> void;
    auto get_attached_file_contents(const std::string &name) -> std::string;
    auto get_attached_file_tmp_path(const std::string &name) -> std::string;
    auto delete_attached_file(const std::string &name) -> void;
    auto recreate_attached_file(const std::string &name, const std::string &data) -> std::string;
    auto export_attached_file_contents(const std::string &name, const std::string &export_to) -> int;

    auto block_user_interaction(bool flag) -> void;
    auto user_interaction_allowed() -> bool {
      return _user_interaction_blocked == 0;
    }

    // State handling.
    auto read_state(const std::string &name, const std::string &domain, const std::string &default_value) -> std::string;
    auto read_state(const std::string &name, const std::string &domain, const int &default_value) -> int;
    auto read_state(const std::string &name, const std::string &domain, const double &default_value) -> double;
    auto read_state(const std::string &name, const std::string &domain, const bool &default_value) -> bool;
    auto read_state(const std::string &name, const std::string &domain) -> grt::ValueRef;

    auto save_state(const std::string &name, const std::string &domain, const std::string &value) -> void;
    auto save_state(const std::string &name, const std::string &domain, const int &value) -> void;
    auto save_state(const std::string &name, const std::string &domain, const double &value) -> void;
    auto save_state(const std::string &name, const std::string &domain, const bool &value) -> void;
    auto save_state(const std::string &name, const std::string &domain, grt::ValueRef value) -> void;

  protected:
    friend class WBContextModel; // to access _components

    bec::PluginManager *_plugin_manager;

    int _user_interaction_blocked;
    bool _send_messages_to_shell;
    bool _asked_for_saving;
    bool _initialization_finished;
    bool _attachments_changed;

    std::string _datadir;
    std::string _user_datadir;

    struct RefreshRequest {
      RefreshType type;
      std::string str;
      NativeHandle ptr;
      double timestamp;
    };

    // Predicate for pending refresh removal on close.
    struct CancelRefreshCandidate {
      bool operator()(RefreshRequest request) {
        return (request.type == RefreshNewModel || request.type == RefreshNewDiagram ||
                request.type == RefreshOverviewNodeChildren || request.type == RefreshZoom ||
                request.type == RefreshDocument || request.type == RefreshOverviewNodeInfo);
      }
    };

    std::list<RefreshRequest> _pending_refreshes;
    base::Mutex _pending_refresh_mutex;

    base::RecMutex _block_user_interaction_mutex;

    WBContextModel *_model_context;
    WBContextSQLIDE *_sqlide_context;

    std::vector<WBComponent *> _components;

    MySqlStudioImpl *_studio;

    bec::Clipboard *_clipboard;

    ModelFile *_file;
    std::string _filename;
    // only used for comparing pointers
    grt::UndoAction *_save_point;

    TunnelManager *_tunnel_manager;

    ModelFile *_model_import_file;

    bool _force_sw_rendering;     // Command line switch.
    bool _force_opengl_rendering; // Command line switch.

    auto get_paper_types(std::shared_ptr<grt::internal::Unserializer> unserializer) -> grt::ListRef<app_PaperType>;

    std::vector<grt::SlotHolder*> _messageHandlerList;

    auto pushMessageHandler(grt::SlotHolder *slot) -> void;

    bool _other_connections_loaded;
    // setup
    auto init_templates() -> void;
    auto init_grt_tree(WBOptions *options, std::shared_ptr<grt::internal::Unserializer> unserializer) -> void;
    auto init_plugins_grt(WBOptions *options) -> void;
    auto init_plugin_groups_grt(WBOptions *options) -> void;
    auto init_object_listeners_grt() -> void;
    auto init_properties_grt(studio_DocumentRef &doc) -> void;
    auto init_rdbms_modules() -> void;

    auto do_close_document(bool destroying) -> void;

    auto setup_context_grt(WBOptions *options) -> grt::ValueRef;

    auto set_default_options(grt::DictRef options) -> void;

    auto load_app_options(bool update) -> void;

    auto auto_save_document() -> bool;
    auto get_auto_save_dir() -> std::string;

    auto cleanup_options() -> void;

  public:
    auto save_app_options() -> void;
    auto save_connections() -> void;
    auto save_instances() -> void;

  protected:
    auto add_recent_file(const std::string &file) -> void;

    auto load_app_state(std::shared_ptr<grt::internal::Unserializer> unserializer) -> void;
    auto save_app_state() -> void;

    auto save_grt() -> grt::ValueRef;

    auto execute_plugin_grt(const app_PluginRef &plugin, const grt::BaseListRef &args) -> grt::ValueRef;
    auto plugin_finished(const grt::ValueRef &result, const app_PluginRef &plugin) -> void;

    auto handle_message(const grt::Message &msg) -> bool;

    auto reset_document() -> void;
    auto reset_listeners() -> void;

    void option_dict_changed(grt::internal::OwnedDict *dict = 0, bool added = false, const std::string &key = "");

  private:
    // for base::Observer
    virtual auto handle_notification(const std::string &name, void *sender, std::map<std::string, std::string> &info) -> void;

  public:
    auto get_file() -> ModelFile * {
      return _file;
    }

    auto install_module_file(const std::string &path) -> bool;
    auto uninstall_module(grt::Module *module) -> bool;
    auto run_script_file(const std::string &path) -> void;

  private:
    auto find_connection_password(const db_mgmt_ConnectionRef &conn, std::string &password) -> bool;

    auto do_request_password(const std::string &title, const std::string &service, bool reset_password,
                              std::string *account, std::string *ret_password) -> void *;
    auto do_find_connection_password(const std::string &hostId, const std::string &username,
                                      std::string *ret_password) -> void *;

    auto load_other_connections() -> void;

    auto attempt_options_upgrade(xmlDocPtr xmldoc, const std::string &version) -> void;

    auto show_error(const std::string &title, const std::string &message) -> bool;

    auto setLogLevelFromGuiPreferences(const grt::DictRef &dict) -> void;

  public:
    auto request_connection_password(const db_mgmt_ConnectionRef &conn, bool force_asking) -> std::string;

  public: // front end callbacks
    WBFrontendCallbacks *_frontendCallbacks;

    // Internal, used for gui plugins
    std::function<void(std::string, void *)> show_gui_plugin;

  private:
    auto warnIfRunningOnUnsupportedOS() -> void;
  };

  struct GUILock {
    WBContext *_wb;

    GUILock(WBContext *wb, const std::string &message_title, const std::string &message) : _wb(wb) {
      mforms::Utilities::show_wait_message(message_title, message);
      _wb->block_user_interaction(true);
    }
    ~GUILock() {
      _wb->block_user_interaction(false);
      mforms::Utilities::hide_wait_message();
    }
  };
};
