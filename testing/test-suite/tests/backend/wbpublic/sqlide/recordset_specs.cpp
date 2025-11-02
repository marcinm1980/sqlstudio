/*
 * Copyright (c) 2019, 2025, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2025, dev4fun. All rights reserved.
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

#include "sqlide/recordset_cdbc_storage.h"
#include "sqlide/recordset_be.h"
#include "cppdbc.h"

#include "gtest/gtest.h"
#include "wb_test_helpers.h"
#include "wb_connection_helpers.h"

namespace {

static void dummy() {
}

class RecordsetTest : public ::testing::Test {
protected:
  std::unique_ptr<MySqlStudioTester> tester;
  sql::Dbc_connection_handler::Ref connection;

  void SetUp() override {
    tester.reset(new MySqlStudioTester());
    tester->initializeRuntime();

    sql::DriverManager *manager = sql::DriverManager::getDriverManager();
    manager->set_testing();
    sql::Authentication::Ref auth;

    connection = sql::Dbc_connection_handler::Ref(new sql::Dbc_connection_handler());

    db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
    setupConnectionEnvironment(connectionProperties);
    connection->ref = manager->getConnection(connectionProperties, std::bind(dummy));

    EXPECT_NE(nullptr, connection->ref.get()) << "connection";
  }
};

TEST_F(RecordsetTest, RecordsetStorage) {
  Recordset_cdbc_storage::Ref data_storage(Recordset_cdbc_storage::create());

  base::RecMutex _connLock;
  data_storage->setUserConnectionGetter(
    [&](sql::Dbc_connection_handler::Ref &conn, bool LockOnly = false) -> base::RecMutexLock {
      base::RecMutexLock lock(_connLock, false);
      conn = connection;
      return lock;
    }
  );

  Recordset::Ref rs = Recordset::create();
  rs->data_storage(data_storage);

  std::shared_ptr<sql::Statement> dbc_statement(connection->ref->createStatement());
  dbc_statement->execute("select convert('', binary), convert(NULL, binary)");

  std::shared_ptr<sql::ResultSet> rset(dbc_statement->getResultSet());
  data_storage->dbc_resultset(rset);

  rs->reset(true);

  EXPECT_FALSE(rs->is_field_null(0, 0)) << "empty blob string is not NULL";
  EXPECT_TRUE(rs->is_field_null(0, 1)) << "NULL blob is NULL";
}

}

