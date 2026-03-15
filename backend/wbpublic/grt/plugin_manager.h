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

#pragma once

#include "grt.h"
#include "grtpp_module_cpp.h"

#include "grts/structs.h"
#include "grts/structs.app.h"
#include "wbpublic_public_interface.h"

#ifdef _MSC_VER
typedef uintptr_t NativeHandle;
#else
// Don't make this "id" on OSX or we risk a strong reference cycle.
using NativeHandle = void *;
#endif

// GUI plugins with native code that are loaded and managed by the MySqlStudio process
#define GUI_PLUGIN_TYPE "gui"
// GUI plugins that are standalone and can be called directly by the module system (eg mforms)
#define STANDALONE_GUI_PLUGIN_TYPE "standalone"
// normal plugins with no UI
#define NORMAL_PLUGIN_TYPE "normal"
// internal plugins without dedicated assemblies (usually is a part of backend)
#define INTERNAL_PLUGIN_TYPE "internal"
// custom plugins (validation on load is not applied)
#define CUSTOM_PLUGIN_TYPE "custom"

namespace bec {

#define PluginManager_VERSION "1.0.0"

  class BaseEditor;
  class GRTManager;

  enum GUIPluginFlags { NoFlags = 0, ForceNewWindowFlag = (1 << 0), StandaloneWindowFlag = (1 << 1) };

  class WBPUBLICBACKEND_PUBLIC_FUNC ArgumentPool : public std::map<std::string, grt::ValueRef> {
  public:
    auto build_argument_list(const app_PluginRef &plugin) -> grt::BaseListRef;

    static auto needs_file_input(const app_PluginRef &plugin) -> app_PluginFileInputRef;
    static auto needs_simple_input(const app_PluginRef &plugin, const std::string &name) -> bool;

    auto find_match(const app_PluginInputDefinitionRef &pdef, std::string &searched_key_name_ret,
                    bool strict = true) const -> grt::ValueRef;

    void dump_keys(const std::function<void(std::string)> &dump_function) const;

    void add_file_input(const app_PluginFileInputRef &pdef, const std::string &value);

    void add_simple_value(const std::string &name, const grt::ValueRef &value);

    void add_list_for_selection(const std::string &source_name, const grt::ObjectListRef &list);

    void add_entries_for_object(const std::string &name, const grt::ObjectRef &object,
                                const std::string &topmost_class_name = "");
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC PluginManagerImpl : public grt::CPPModule {
    using superclass = grt::CPPModule;

  public:
    using OpenGUIPluginSlot =
      std::function<NativeHandle(grt::Module *, std::string, std::string, grt::BaseListRef, GUIPluginFlags)>;
    using ShowGUIPluginSlot = std::function<void(NativeHandle)>;
    using CloseGUIPluginSlot = std::function<void(NativeHandle)>;

  public:
    // NOTE: not to be directly instantiated
    PluginManagerImpl(grt::CPPModuleLoader *loader);
    virtual ~PluginManagerImpl() {
    }

    void set_registry_paths(const std::string &plugins_path, const std::string &groups_path);

    void rescan_plugins();

    DEFINE_INIT_MODULE(PluginManager_VERSION, "Oracle and/or its affiliates", grt::ModuleImplBase,
                       DECLARE_MODULE_FUNCTION(PluginManagerImpl::show_plugin),
                       DECLARE_MODULE_FUNCTION(PluginManagerImpl::close_plugin));

    // generic plugin interface

    // ordered by rating
    std::vector<app_PluginRef> get_plugins_for_objects(const grt::ObjectListRef &objects,
                                                       const std::string &group = "");

    std::vector<app_PluginRef> get_plugins_for_group(const std::string &group);

    auto get_plugin(const std::string &name) -> app_PluginRef;

    auto open_plugin(const app_PluginRef &plugin, const grt::BaseListRef &args) -> std::string;
    auto open_gui_plugin(const app_PluginRef &plugin, const grt::BaseListRef &args, GUIPluginFlags flags)
      -> std::string;

    auto open_plugin_with_object(const app_PluginRef &plugin, GrtObjectRef object) -> std::string;
    auto open_plugin_with_selection(const app_PluginRef &plugin, const grt::ObjectListRef &selection) -> std::string;
    auto open_plugin_with_file(const app_PluginRef &plugin, const std::string &filename) -> std::string;

    auto execute_plugin_function(const app_PluginRef &plugin, const grt::BaseListRef &args) -> grt::ValueRef;

    auto show_plugin(const std::string &handle) -> int;
    auto close_plugin(const std::string &handle) -> int;

    auto select_plugin_for_input(const std::string &group, const grt::BaseListRef &args) -> app_PluginRef;

    auto check_plugin_input(const app_PluginInputDefinitionRef &def, const grt::ValueRef &value) -> bool;

    void set_plugin_enabled(const app_PluginRef &plugin, bool flag);
    auto plugin_enabled(const std::string &plugin_name) -> bool;

  public: // for frontends
    void register_plugins(grt::ListRef<app_Plugin> plugins);

    void set_gui_plugin_callbacks(const OpenGUIPluginSlot &open, const ShowGUIPluginSlot &show,
                                  const CloseGUIPluginSlot &close);

    void forget_gui_plugin_handle(NativeHandle handle);
    void close_and_forget_gui_plugin(NativeHandle handle);

    std::vector<NativeHandle> get_similar_open_plugins(grt::Module *, const std::string &class_name, grt::BaseListRef);

    auto get_plugin_list(const std::string &group = "") -> grt::ListRef<app_Plugin>;

  protected:
    std::string _registry_path;
    std::string _group_registry_path;

    OpenGUIPluginSlot _open_gui_plugin_slot;
    ShowGUIPluginSlot _show_gui_plugin_slot;
    CloseGUIPluginSlot _close_gui_plugin_slot;

    // plugin-name+oid -> editor handle
    std::map<std::string, NativeHandle> _open_gui_plugins;

    // plugin name -> module name
    std::map<std::string, std::string> _plugin_source_module;

  private:
    auto get_plugin_groups() -> grt::ListRef<app_PluginGroup>;

    auto check_input_for_plugin(const app_PluginRef &plugin, const grt::BaseListRef &args) -> bool;

    auto open_normal_plugin_grt(const app_PluginRef &plugin, const grt::BaseListRef &args) -> grt::ValueRef;

    auto open_gui_plugin_main(const app_PluginRef &plugin, const grt::BaseListRef &args, GUIPluginFlags flags)
      -> std::string;
    auto show_gui_plugin_main(const std::string &handle) -> int;
    auto close_gui_plugin_main(const std::string &handle) -> int;

    void open_standalone_plugin_main(const app_PluginRef &plugin, const grt::BaseListRef &args);

    auto get_group(const std::string &path) -> app_PluginGroupRef;
    void add_plugin_to_group(const app_PluginRef &plugin, const std::string &path);

    auto check_plugin_validity(const app_PluginRef &plugin, grt::Module *module) -> bool;

    auto get_disabled_plugin_names() -> grt::StringListRef;
  };

  using PluginManager = ::bec::PluginManagerImpl;
}; // namespace bec
