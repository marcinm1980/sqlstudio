/*
 * Copyright (c) 2011, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/metadata.h>

#include "grt.h"
#include "cdbc/src/driver_manager.h"
#include "wb_connection_helpers.h"
#include "wb_test_helpers.h"

#include "helpers.h"
#include "gtest/gtest.h"

extern void register_all_metaclasses();

namespace {

  class DbcMetadataTest : public ::testing::Test {
  protected:
    void SetUp() override {
      register_all_metaclasses();
      grt::GRT::get()->scan_metaclasses_in("../../res/grt/");
      grt::GRT::get()->end_loading_metaclasses();
      EXPECT_EQ((size_t)INT_METACLASS_COUNT, grt::GRT::get()->get_metaclasses().size());
    }

    void TearDown() override {
      MySqlStudioTester::reinitGRT();
    }
  };

TEST_F(DbcMetadataTest, TestDatabaseMetaDataGetCatalogs) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);

  setupConnectionEnvironment(connectionProperties);

  try {
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm);

    sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper.get());

    sql::Connection *connection = wrapper.get();
    sql::DatabaseMetaData *meta(connection->getMetaData());
    EXPECT_NE(nullptr, meta);

    // TODO: getCatalogs() is not implemented.
    // std::auto_ptr<sql::Statement> rset(meta->getCatalogs());
    // ensure("rset is not NULL", rset.get() != NULL);
    // while (rset->next());
  } catch (sql::SQLException &) {
    printf("ERR: Caught sql::SQLException\n");
    throw;
  } catch (...) {
    printf("ERR: Caught unknown exception\n");
    throw;
  }
}

TEST_F(DbcMetadataTest, TestDatabaseMetaDataGetDatabaseProductName) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);

  setupConnectionEnvironment(connectionProperties);

  try {
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm);

    sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper.get());

    sql::Connection *connection = wrapper.get();
    sql::DatabaseMetaData *meta(connection->getMetaData());
    EXPECT_NE(nullptr, meta);

    meta->getDatabaseProductName();
  } catch (sql::SQLException &) {
    printf("ERR: Caught sql::SQLException\n");
  }
}

TEST_F(DbcMetadataTest, TestDatabaseMetaDataGetDatabaseProductVersion) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);

  setupConnectionEnvironment(connectionProperties);

  try {
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm);

    sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper.get());

    sql::Connection *connection = wrapper.get();
    sql::DatabaseMetaData *meta(connection->getMetaData());
    EXPECT_NE(nullptr, meta);

    meta->getDatabaseProductVersion();
  } catch (sql::SQLException &) {
    printf("ERR: Caught sql::SQLException\n");
    throw;
  }
}

TEST_F(DbcMetadataTest, TestDatabaseMetaDataGetDriverMajorVersion) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);

  setupConnectionEnvironment(connectionProperties);

  try {
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm);

    sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper.get());

    sql::Connection *connection = wrapper.get();
    sql::DatabaseMetaData *meta(connection->getMetaData());
    EXPECT_NE(nullptr, meta);

    meta->getDriverMajorVersion();
  } catch (sql::SQLException &) {
    printf("ERR: Caught sql::SQLException\n");
    throw;
  }
}

TEST_F(DbcMetadataTest, TestDatabaseMetaDataGetDriverMinorVersion) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);

  setupConnectionEnvironment(connectionProperties);

  try {
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm);

    sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper.get());

    sql::Connection *connection = wrapper.get();
    sql::DatabaseMetaData *meta(connection->getMetaData());
    EXPECT_NE(nullptr, meta);

    meta->getDriverMinorVersion();
  } catch (sql::SQLException &) {
    printf("ERR: Caught sql::SQLException\n");
    throw;
  }
}

TEST_F(DbcMetadataTest, TestDatabaseMetaDataGetDriverName) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);

  setupConnectionEnvironment(connectionProperties);

  try {
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm);

    sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper.get());

    sql::Connection *connection = wrapper.get();
    sql::DatabaseMetaData *meta(connection->getMetaData());
    EXPECT_NE(nullptr, meta);

    meta->getDriverName();
  } catch (sql::SQLException &) {
    printf("ERR: Caught sql::SQLException\n");
    throw;
  }
}
}

