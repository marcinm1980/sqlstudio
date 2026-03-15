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

#pragma once

#include "grt/common.h"
#include "grtui/grt_wizard_form.h"
#include "grtui/wizard_finished_page.h"
#include "grtui/wizard_progress_page.h"
#include "grtui/grtdb_connect_panel.h"

#include "studio/wb_context.h"
#include "studio/wb_module.h"

#include "cppdbc.h"

#include "mforms/fs_object_selector.h"
#include "mforms/radiobutton.h"
#include "mforms/checkbox.h"

/**
 * Implementation of a wizard to set up remote management for a given connection.
 */

using namespace grtui;
using namespace mforms;

class NewServerInstanceWizard;

class NewServerInstancePage : public WizardPage {
public:
  NewServerInstancePage(WizardForm* form, const std::string& pageid);

protected:
  auto wizard() -> NewServerInstanceWizard*;
};

class IntroductionPage : public WizardPage {
public:
  IntroductionPage(WizardForm* form);
};

class TestDatabaseSettingsPage : public WizardProgressPage {
public:
  TestDatabaseSettingsPage(WizardForm* host);
  virtual auto enter(bool advancing) -> void;

protected:
  sql::ConnectionWrapper _dbc_conn;
  std::string _message;
  auto open_connection() -> bool;
  virtual auto tasks_finished(bool success) -> void;
  auto get_server_version() -> bool;
  auto get_server_platform() -> bool;

  auto wizard() -> NewServerInstanceWizard*;
};

class HostAndRemoteTypePage : public NewServerInstancePage {
public:
  HostAndRemoteTypePage(WizardForm* host);

protected:
  virtual auto enter(bool advancing) -> void;
  virtual auto advance() -> bool;
  virtual auto skip_page() -> bool;

  auto refresh_profile_list() -> void;
  auto toggle_remote_admin() -> void;

private:
  Panel _management_type_panel; // Border.
  Box _management_type_box;     // Content.
  Panel _os_panel;              // Border.
  Box _os_box;                  // Content.

  Label _os_description;

  Table _params;
  Label _os_label;
  Selector _os_selector;

  Label _type_label;
  Selector _type_selector;

  mforms::RadioButton _win_remote_admin;
  mforms::RadioButton _ssh_remote_admin;

  std::map<std::string, std::vector<std::pair<std::string, std::string> > > _presets;
};

class SSHConfigurationPage : public NewServerInstancePage {
public:
  SSHConfigurationPage(WizardForm* host);

protected:
  auto use_ssh_key_changed() -> void;

  virtual auto enter(bool advancing) -> void;
  virtual auto advance() -> bool;
  virtual auto leave(bool advancing) -> void;
  virtual auto skip_page() -> bool;

private:
  Label _main_description1;
  Label _main_description2;

  Table _ssh_settings_table;

  Box _indent;
  Label _host_name_label;
  TextEntry _host_name;
  Label _port_label;
  TextEntry _port;

  Label _username_label;
  TextEntry _username;

  CheckBox _use_ssh_key;
  Label _ssh_path_label;
  TextEntry _ssh_key_path;
  Button _ssh_key_browse_button;
  FsObjectSelector* _file_selector;
};

class WindowsManagementPage : public NewServerInstancePage {
public:
  WindowsManagementPage(WizardForm* host, wb::WBContext* context);

protected:
  auto refresh_config_path() -> void;

  virtual auto enter(bool advancing) -> void;
  virtual auto leave(bool advancing) -> void;

  virtual auto advance() -> bool;
  virtual auto skip_page() -> bool;

private:
  wb::WBContext* _context;
  std::vector<std::string> _config_paths;
  std::vector<std::string> _service_names;

  Table _layout_table;
  Box _indent;

  Label _main_description1;
  Label _main_description2;

  Label _service_label;
  TextEntry _service_name;
  Selector _service_selector;
  Label _progress_label;

  Label _config_path_label;
  TextEntry _config_path;
  Button _browse_button;
  FsObjectSelector* _file_selector;
};

class TestHostMachineSettingsPage : public WizardProgressPage {
public:
  TestHostMachineSettingsPage(WizardForm* host);

  virtual auto enter(bool advance) -> void;
  virtual auto leave(bool advancing) -> void;

protected:
  auto connect_to_host() -> bool;
  auto find_config_file() -> bool;
  auto find_error_files() -> bool;
  auto check_admin_commands() -> bool;
  virtual auto tasks_finished(bool success) -> void;
  virtual auto skip_page() -> bool;

  auto wizard() -> NewServerInstanceWizard*;

private:
  TaskRow* _connect_task;
  TaskRow* _commands_task;
};

class ReviewPage : public NewServerInstancePage {
public:
  ReviewPage(WizardForm* host);

protected:
  virtual auto enter(bool advancing) -> void;
  virtual auto leave(bool advancing) -> void;
  virtual auto skip_page() -> bool;
  virtual auto next_closes_wizard() -> bool;
  virtual auto close_caption() const -> std::string {
    return finish_caption();
  }

  auto customize_changed() -> void;

private:
  Label _description;
  Table _content;
  Label _label;

  TextBox _text;

  CheckBox _customize_check;
};

class PathsPage : public NewServerInstancePage {
public:
  PathsPage(WizardForm* host, wb::WBContext* context);

protected:
  virtual auto enter(bool advancing) -> void;
  virtual auto advance() -> bool;
  virtual auto skip_page() -> bool;
  auto browse_remote_config_file() -> void;
  auto test_path() -> void;
  auto test_section() -> void;

private:
  wb::WBContext* _context;

  Label _description;
  Table _content;

  Label _version_label;
  TextEntry _version;

  Label _config_path_label;
  TextEntry _config_path;
  Button _browse_button;
  FsObjectSelector* _file_selector;
  Button _test_config_path_button;
  Label _test_config_path_description;

  Label _section_name_label;
  TextEntry _section_name;
  Button _test_section_button;
  Label _test_section_description;
};

class CommandsPage : public NewServerInstancePage {
public:
  CommandsPage(WizardForm* host);

protected:
  virtual auto enter(bool advancing) -> void;
  virtual auto leave(bool advancing) -> void;
  virtual auto advance() -> bool;
  virtual auto skip_page() -> bool;
  virtual auto next_closes_wizard() -> bool {
    return true;
  }
  virtual auto close_caption() const -> std::string {
    return finish_caption();
  }

private:
  Label _description;
  Table _content;

  Label _start_label;
  TextEntry _start_command;
  Label _stop_label;
  TextEntry _stop_command;

  CheckBox _use_sudo;
};

class NewServerInstanceWizard : public WizardForm {
public:
  NewServerInstanceWizard(wb::WBContext* context, db_mgmt_ConnectionRef connection);
  ~NewServerInstanceWizard();

  auto assemble_server_instance() -> db_mgmt_ServerInstanceRef;
  auto test_setting_grt(const std::string& name) -> grt::ValueRef;

  auto load_defaults() -> void;
  auto get_server_info(const std::string& key) -> std::string;

  auto wb() -> wb::WBContext* {
    return _context;
  }

  auto is_admin_enabled() -> bool;
  auto is_local() -> bool;
  auto test_setting(const std::string& name, std::string& detail) -> bool;

  auto create_instance() -> void;

protected:
  wb::WBContext* _context;

  db_mgmt_ConnectionRef _connection;   // The connection for which we are configuring the server instance.
  db_mgmt_ServerInstanceRef _instance; // The server instance we are working on.

private:
  IntroductionPage* _introduction_page;
  TestDatabaseSettingsPage* _test_database_settings_page;
  HostAndRemoteTypePage* _os_page;
  SSHConfigurationPage* _ssh_configuration_page;
  WindowsManagementPage* _windows_connection_page;
  TestHostMachineSettingsPage* _test_host_machine_settings_page;
  ReviewPage* _review_page;
  PathsPage* _paths_page;
  CommandsPage* _commands_page;
};
