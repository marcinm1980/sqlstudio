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

#include "grt.h"

#include "base/trackable.h"
#include "base/threading.h"
#include "common.h"
#include "grt_dispatcher.h"
#include "grt_shell.h"
#include "grt_value_inspector.h"
#include "grt_message_list.h"

#include "wbpublic_public_interface.h"

#include "plugin_manager.h"
#include <boost/signals2/connection.hpp>

namespace bec {

  class Clipboard;

  // Manages a GRT context and other associated objects useful for a GRT shell and other apps.
  class WBPUBLICBACKEND_PUBLIC_FUNC GRTManager : public base::trackable {
  public:
    using Ref = std::shared_ptr<GRTManager>;

    struct Timer {
      std::function<bool()> slot;
      gint64 next_trigger_us;
      double interval;

      Timer(const std::function<bool()> &slot, double interval);

      auto trigger() -> bool;

      auto delay_for_next_trigger(gint64 now_us) -> double;
    };

  protected: // Set those c-tors to protected as we need to have different GRTManager in TUT.
    GRTManager(bool threaded);
    GRTManager(const GRTManager &) = delete;
    auto operator=(GRTManager &) -> GRTManager & = delete;

  public:
    static auto get() -> GRTManager::Ref;
    virtual ~GRTManager();

    void setVerbose(bool verbose);

    void set_basedir(const std::string &path);
    auto get_basedir() -> std::string {
      return _basedir;
    }

    void set_datadir(const std::string &path);
    auto get_data_file_path(const std::string &file) -> std::string;

    void set_user_datadir(const std::string &path);
    auto get_user_datadir() -> std::string {
      return _user_datadir;
    }

    auto get_tmp_dir() -> std::string;
    auto get_unique_tmp_subdir() -> std::string;
    void cleanup_tmp_dir();

    void set_module_extensions(const std::list<std::string> &extensions);

    void rescan_modules();
    auto do_scan_modules(const std::string &path, const std::list<std::string> &exts, bool refresh) -> int;
    void scan_modules_grt(const std::list<std::string> &extensions, bool refresh);

    void set_clipboard(Clipboard *clipb);

    auto get_clipboard() -> Clipboard * {
      return _clipboard;
    }

    void set_search_paths(const std::string &module_sp, const std::string &struct_sp, const std::string &libraries_sp);

    void set_user_extension_paths(const std::string &user_module_path, const std::string &user_library_path,
                                  const std::string &user_script_path);

    auto get_user_module_path() const -> std::string {
      return _user_module_path;
    }
    auto get_user_library_path() const -> std::string {
      return _user_library_path;
    }
    auto get_user_script_path() const -> std::string {
      return _user_script_path;
    }

    // main window statusbar text
    void push_status_text(const std::string &message);
    void replace_status_text(const std::string &message);
    void pop_status_text();
    void set_status_slot(const std::function<void(std::string)> &slot);

  public:
    auto get_dispatcher() const -> GRTDispatcher::Ref {
      return _dispatcher;
    };

    void cleanUpAndReinitialize();

    void initialize(bool init_python, const std::string &loader_module_path = "");
    auto initialize_shell(const std::string &shell_type) -> bool;

    auto cancel_idle_tasks() -> bool;
    void perform_idle_tasks();

    auto get_plugin_manager() const -> PluginManager * {
      return _plugin_manager;
    }

    auto is_threaded() -> bool {
      return _threaded;
    }
    auto in_main_thread() -> bool;

    // shell
    auto get_shell() -> ShellBE *;

    void execute_grt_task(const std::string &title, const std::function<grt::ValueRef()> &function,
                          const std::function<void(grt::ValueRef)> &finished_cb);

    // message displaying (as dialogs)
    void show_error(const std::string &message, const std::string &detail, bool important = true);
    void show_warning(const std::string &title, const std::string &message, bool important = false);
    void show_message(const std::string &title, const std::string &message, bool important = false);

    auto get_messages_list() -> MessageListStorage *;

    //
    void set_app_option_slots(const std::function<grt::ValueRef(std::string)> &slot,
                              const std::function<void(std::string, grt::ValueRef)> &set_slot);
    auto get_app_option(const std::string &name) -> grt::ValueRef;
    auto get_app_option_string(const std::string &name, std::string default_ = "") -> std::string;
    auto get_app_option_int(const std::string &name, long default_ = 0) -> long;
    void set_app_option(const std::string &name, const grt::ValueRef &value);

    auto run_once_when_idle(const std::function<void()> &func) -> boost::signals2::connection;
    auto run_once_when_idle(base::trackable *owner, const std::function<void()> &func) -> boost::signals2::connection;

    void block_idle_tasks();
    void unblock_idle_tasks();

    auto run_every(const std::function<bool()> &slot, double seconds) -> Timer *;
    void cancel_timer(Timer *timer);
    auto delay_for_next_timeout() -> double;

    void set_timeout_request_slot(const std::function<void()> &slot);

    void flush_timers();

    void terminate() {
      _terminated = true;
    };
    auto terminated() -> bool {
      return _terminated;
    };
    void reset_termination() {
      _terminated = false;
    };

    void set_db_file_path(const std::string &db_file_path) {
      _db_file_path = db_file_path;
    }
    auto get_db_file_path() -> std::string {
      return _db_file_path;
    }

    auto has_unsaved_changes() -> bool {
      return _has_unsaved_changes;
    }
    void has_unsaved_changes(bool has_unsaved_changes) {
      _has_unsaved_changes = has_unsaved_changes;
    }

    // use for advisory locks on grt globals tree
    // ex: UI should not refresh layer and catalog trees while a plugin is running
    auto try_soft_lock_globals_tree() -> bool;
    void soft_lock_globals_tree();
    void soft_unlock_globals_tree();
    auto is_globals_tree_locked() -> bool;

  public:
    std::function<void(bec::ArgumentPool &)> update_plugin_arguments_pool; // set by WBContext

    auto get_plugin_context_menu_items(const std::list<std::string> &groups, const bec::ArgumentPool &argument_pool)
      -> bec::MenuItemList;
    auto check_plugin_runnable(const app_PluginRef &plugin, const bec::ArgumentPool &argpool, bool debug_output = false)
      -> bool;

    void open_object_editor(const GrtObjectRef &object, bec::GUIPluginFlags flags = bec::NoFlags);

  protected:
    bool _has_unsaved_changes;
    GRTDispatcher::Ref _dispatcher;
    base::Mutex _idle_mutex;
    base::Mutex _idle_task_blocker_mutex;
    base::Mutex _timer_mutex;

  public:
    void add_dispatcher(const bec::GRTDispatcher::Ref disp);
    void remove_dispatcher(const bec::GRTDispatcher::Ref disp);

  protected:
    using DispatcherMap = std::map<GRTDispatcher::Ref, void *>;
    DispatcherMap _disp_map;
    base::Mutex _disp_map_mutex;

    PluginManager *_plugin_manager;

    Clipboard *_clipboard;

    ShellBE *_shell;

    MessageListStorage *_messages_list;

    std::function<void(std::string)> _status_text_slot;

    std::list<Timer *> _timers;
    std::set<Timer *> _cancelled_timers;
    std::function<void()> _timeout_request;

    // Using two signals to manage the idle tasks
    boost::signals2::signal<void()> _idle_signals[2];
    int _current_idle_signal;

    int _idle_blocked;

    std::list<std::string> _module_extensions;

    std::string _basedir;
    std::string _datadir;
    std::string _user_datadir;
    std::string _module_pathlist;
    std::string _struct_pathlist;
    std::string _libraries_pathlist;
    std::string _db_file_path;

    std::string _user_module_path;
    std::string _user_library_path;
    std::string _user_script_path;

    std::function<grt::ValueRef(std::string)> _get_app_option_slot;
    std::function<void(std::string, grt::ValueRef)> _set_app_option_slot;

    bool _threaded;
    bool _verbose;

    int _globals_tree_soft_lock_count;

    virtual auto load_structs() -> bool;
    virtual auto load_modules() -> bool;
    virtual auto load_libraries() -> bool;
    virtual auto init_module_loaders(const std::string &loader_module_path, bool init_python) -> bool;

    auto init_loaders(const std::string &loader_module_path, bool init_python) -> bool;

    void flush_shell_output();

  private:
    bool _terminated; // true if application termination was requested by the BE or a plugin.

    std::shared_ptr<grt::GRT> _grt;

    auto setup_grt() -> grt::ValueRef;
    void shell_write(const std::string &text);
    void task_error_cb(const std::exception &error, const std::string &title);
  };
}; // namespace bec
