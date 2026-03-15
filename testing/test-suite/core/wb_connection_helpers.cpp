/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "wb_connection_helpers.h"

#include "grt.h"
#include "grtpp_util.h"
#include "cppdbc.h"

#include <cstdlib>
#include <string>

// Helper to get env var or default
static auto getEnvOrDefault(const char* var, const char* def) -> std::string {
  const char* val = std::getenv(var);
  return val ? std::string(val) : std::string(def);
}

//----------------------------------------------------------------------------------------------------------------------

auto setupConnectionEnvironment(const db_mgmt_ConnectionRef &connectionProperties, db_mgmt_DriverRef driver) -> void {
  grt::DictRef conn_params(true);
  conn_params.set("hostName", grt::StringRef(getEnvOrDefault("DB_HOST", "localhost")));
  conn_params.set("port", grt::IntegerRef(std::stoi(getEnvOrDefault("DB_PORT", "3306"))));
  conn_params.set("userName", grt::StringRef(getEnvOrDefault("DB_USER", "root")));
  conn_params.set("password", grt::StringRef(getEnvOrDefault("DB_PASSWORD", "")));
  grt::replace_contents(connectionProperties->parameterValues(), conn_params);

  if (driver.is_valid()) {
    connectionProperties->driver(driver);
  } else {
    db_mgmt_DriverRef driverProperties(grt::Initialized);
    driverProperties->driverLibraryName(grt::StringRef("mysqlcppconn"));
    connectionProperties->driver(driverProperties);
  }
  connectionProperties->name("Test_conn");
}

//----------------------------------------------------------------------------------------------------------------------

auto createConnectionForImport() -> sql::ConnectionWrapper {
  db_mgmt_ConnectionRef properties(grt::Initialized);
  setupConnectionEnvironment(properties);

  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  return dm->getConnection(properties);
}

//----------------------------------------------------------------------------------------------------------------------
