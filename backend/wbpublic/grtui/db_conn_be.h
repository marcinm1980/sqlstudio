/*
 * Copyright (c) 2007, 2022, Oracle and/or its affiliates. All rights reserved.
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

#include "grts/structs.db.mgmt.h"
#include "cppdbc.h"
#include <vector>
#include <map>

#include "wbpublic_public_interface.h"
#include "base/geometry.h"

class DbDriverParam;
class DbDriverParams;
class DbConnection;

enum ControlType {
  ctUnknown,
  ctLabel,
  ctDescriptionLabel,
  ctTextBox,
  ctKeychainPassword,
  ctCheckBox,
  ctNumericUpDown,
  ctButton,
  ctDirSelector,
  ctFileSelector,
  ctEnumSelector,
  ctEnumOption,
  ctText
};

class WBPUBLICBACKEND_PUBLIC_FUNC DbDriverParam {
public:
  enum ParamType {
    ptUnknown,
    ptInt,
    ptString,
    ptPassword,
    ptKeychainPassword,
    ptBoolean,
    ptTristate,
    ptDir,
    ptFile,
    ptEnum,
    ptIntEnum,
    ptIntOption,
    ptText,
    ptButton
  };

private:
  static auto decode_param_type(std::string type_name, std::string real_type) -> ParamType;

  db_mgmt_DriverParameterRef _inner;
  ParamType _type;
  grt::ValueRef _value;

  DbDriverParam(const DbDriverParam &) {
  }
  DbDriverParam(const db_mgmt_DriverParameterRef &driver_param, const db_mgmt_ConnectionRef &stored_conn);
  DbDriverParam(const db_mgmt_DriverParameterRef &driver_param, const grt::ValueRef &value);

  auto get_control_type() const -> ControlType;

  friend class DbDriverParams;

public:
  auto object() const -> const db_mgmt_DriverParameterRef & {
    return _inner;
  }

  auto get_type() const -> ParamType {
    return _type;
  }
  auto get_control_name() const -> grt::StringRef;
  auto get_accessibility_name() const -> grt::StringRef;
  auto get_value() const -> const grt::ValueRef & {
    return _value;
  }
  auto get_value_repr() const -> const grt::StringRef {
    return _value.toString();
  }
  auto set_value(const grt::ValueRef &value) -> void;
  auto get_enum_options() -> std::vector<std::pair<std::string, std::string> >;
  auto getValue() -> grt::StringRef;
};

class WBPUBLICBACKEND_PUBLIC_FUNC DbDriverParams {
private:
  using Collection = std::vector<DbDriverParam *>;
  using String_index = std::map<std::string, DbDriverParam *>;

  Collection _collection;
  String_index _control_name_index;
  db_mgmt_DriverRef _driver;

  DbDriverParams(const DbDriverParams &) {
  }
  auto free_dyn_mem() -> void;

  auto parameter_not_valid(const db_mgmt_DriverRef &driver, const std::string &param) -> bool;

public:
  DbDriverParams() {
  }
  ~DbDriverParams() {
    free_dyn_mem();
  }

  auto init(const db_mgmt_DriverRef &driver, const db_mgmt_ConnectionRef &stored_conn,
            const std::function<void(bool)> &suspend_layout, const std::function<void()> &begin_layout,
            const std::function<void(DbDriverParam *, ControlType, const base::ControlBounds &, const std::string &)>
              &create_control,
            const std::function<void()> &end_layout, bool skip_schema = false, int first_row_label_width = 100,
            int hmargin = 10, int vmargin = 10) -> void;
  auto get_params() const -> grt::DictRef;
  auto validate() const -> std::string;

  auto count() const -> size_t {
    return _collection.size();
  }
  auto get(std::string control_name) -> DbDriverParam *;
};

class WBPUBLICBACKEND_PUBLIC_FUNC DbConnection {
private:
  db_mgmt_ManagementRef _mgmt;
  DbDriverParams _db_driver_param_handles;
  db_mgmt_DriverRef _active_driver;
  db_mgmt_ConnectionRef _connection;
  bool _skip_schema;

  std::function<void()> _begin_layout;
  std::function<void()> _end_layout;
  std::function<void(bool)> _suspend_layout;
  std::function<void(DbDriverParam *, ControlType, const base::ControlBounds &, const std::string &)> _create_control;

  auto init_dbc_connection(sql::Connection *dbc_conn, const db_mgmt_ConnectionRef &connectionProperties) -> void;

public:
  DbConnection(const db_mgmt_ManagementRef &mgmt, const db_mgmt_DriverRef &driver, bool skip_schema);

  ~DbConnection();

  auto set_control_callbacks(const std::function<void(bool)> &suspend_layout, const std::function<void()> &begin_layout,
                             const std::function<void(DbDriverParam *, ControlType, const base::ControlBounds &,
                                                      const std::string &)> &create_control,
                             const std::function<void()> &end_layout) -> void;

  auto get_db_driver_param_handles() -> DbDriverParams * {
    return &_db_driver_param_handles;
  }

  auto update() -> void;
  auto set_connection_and_update(const db_mgmt_ConnectionRef &connection) -> void;
  auto set_connection_keeping_parameters(const db_mgmt_ConnectionRef &connection) -> void;
  auto get_connection() -> db_mgmt_ConnectionRef;

  auto save_changes() -> void;

  auto get_dbc_connection() -> sql::ConnectionWrapper;
  auto get_db_mgmt() -> db_mgmt_ManagementRef {
    return _mgmt;
  }
  auto driver() -> db_mgmt_DriverRef {
    return _active_driver;
  }

  void set_driver_and_update(db_mgmt_DriverRef);

  auto test_connection() -> bool;
  auto validate_driver_params() const -> std::string;
};
