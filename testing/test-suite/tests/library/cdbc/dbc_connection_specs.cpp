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
#include "cdbc/src/driver_manager.h"
#include "wb_connection_helpers.h"
#include "wb_test_helpers.h"
#include "helpers.h"
#include "gtest/gtest.h"

extern auto register_all_metaclasses() -> void;

namespace {


  class DbcConnectionTest : public ::testing::Test {
  protected:
    std::unique_ptr<MySqlStudioTester> tester;
    db_mgmt_ConnectionRef connectionProperties;

    void SetUp() override {
      tester.reset(new MySqlStudioTester);
      register_all_metaclasses();
      grt::GRT::get()->scan_metaclasses_in("../../res/grt/");
      grt::GRT::get()->end_loading_metaclasses();
      EXPECT_EQ((size_t)INT_METACLASS_COUNT, grt::GRT::get()->get_metaclasses().size());

      connectionProperties = db_mgmt_ConnectionRef(grt::Initialized);
      setupConnectionEnvironment(connectionProperties);
    }

    void TearDown() override {
      MySqlStudioTester::reinitGRT();
    }
  };

TEST_F(DbcConnectionTest, TestInitializationOfConnectionAndDestruction) {
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  EXPECT_NE(nullptr, dm);
}

TEST_F(DbcConnectionTest, TestInitializationOfStatementAndDestruction) {
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  EXPECT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
  EXPECT_NE(nullptr, wrapper.get());
  sql::Connection *connection = wrapper.get();
  {
    std::unique_ptr<sql::Statement> stmt(connection->createStatement());
    EXPECT_NE(nullptr, stmt.get());
  }
}

TEST_F(DbcConnectionTest, TestConstructionOfMetadataObject) {
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  EXPECT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
  EXPECT_NE(nullptr, wrapper.get());
  sql::Connection *connection = wrapper.get();
  {
    sql::DatabaseMetaData *meta(connection->getMetaData());
    EXPECT_NE(nullptr, meta);
  }
}

TEST_F(DbcConnectionTest, TestAutocommit) {
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  EXPECT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
  EXPECT_NE(nullptr, wrapper.get());

  sql::Connection *connection = wrapper.get();
  try {
    connection->commit();
    connection->rollback();

    bool hadAutoCommit = connection->getAutoCommit();
    connection->setAutoCommit(true);
    EXPECT_TRUE(connection->getAutoCommit());

    connection->commit();
    connection->setAutoCommit(false);
    EXPECT_FALSE(connection->getAutoCommit());

    connection->commit();
    /* Try to set an invalid mode */
    // try {
    //  conn->setAutoCommit(-1);
    //  ensure("sql::InvalidArgumentException expected but not thrown", false);
    //} catch (sql::InvalidArgumentException &e) {
    //  /* Correctly thrown exception */
    //}
    /* Last valid was 0, we should leave it 0 */
    EXPECT_FALSE(connection->getAutoCommit());

    /* Leave the connection in the same state */
    connection->setAutoCommit(hadAutoCommit);
    EXPECT_EQ(hadAutoCommit, connection->getAutoCommit());

  } catch (sql::SQLException &e) {
    printf("ERR: Caught sql::SQLException: %s\n", e.what());
    throw;
  }
}

TEST_F(DbcConnectionTest, TestClearWarnings) {
  // db_mgmt_ConnectionRef connectionProperties;
  // setupConnectionEnvironment(connectionProperties);

  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  EXPECT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
  EXPECT_NE(nullptr, wrapper.get());
  sql::Connection *connection = wrapper.get();

  /* Clear tripple times */ // WHY? ml
  connection->clearWarnings();
  connection->clearWarnings();
  connection->clearWarnings();
}

TEST_F(DbcConnectionTest, Test2Connections) {
  try {
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm);

    sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper1.get());
    sql::Connection *connection1 = wrapper1.get();

    sql::ConnectionWrapper wrapper2 = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper2.get());
    sql::Connection *connection2 = wrapper2.get();

    std::unique_ptr<sql::Statement> stmt1(connection1->createStatement());
    EXPECT_NE(nullptr, stmt1.get());

    std::unique_ptr<sql::Statement> stmt2(connection2->createStatement());
    EXPECT_NE(nullptr, stmt2.get());

    std::unique_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT CONNECTION_ID()"));
    EXPECT_NE(nullptr, rset1.get());

    std::unique_ptr<sql::ResultSet> rset2(stmt2->executeQuery("SELECT CONNECTION_ID()"));
    EXPECT_NE(nullptr, rset2.get());

    EXPECT_TRUE(rset1->next());
    EXPECT_TRUE(rset2->next());

    EXPECT_NE(rset2->getInt(1), rset1->getInt(1));
  } catch (sql::SQLException &e) {
    printf("ERR: Caught sql::SQLException: %s\n", e.what());
    throw;
  }
}

TEST_F(DbcConnectionTest, TestKillOurselves1) {
  try {
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm);

    sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper1.get());
    sql::Connection *connection = wrapper1.get();

    std::unique_ptr<sql::Statement> stmt1(connection->createStatement());
    EXPECT_NE(nullptr, stmt1.get());

    std::unique_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT CONNECTION_ID()"));
    EXPECT_NE(nullptr, rset1.get());

    EXPECT_TRUE(rset1->next());
    // DBC is not supposed to check that, instead DBC user has to check validity of connection when needed
    // snprintf(buff, sizeof(buff), "KILL %d", rset1->getInt(1));
    // try
    //{
    //  stmt1->execute(buff);
    //  fail("An exception should have shown up.");
    //}
    // catch (sql::SQLException &e) {
    //  // Expected.
    //  ensure_equals("Unexpected exception", e.what(), "Commands out of sync; you can't run this command now");
    //}
  } catch (sql::SQLException &e) {
    printf("ERR: Caught sql::SQLException: %s\n", e.what());
    throw;
  }
}

TEST_F(DbcConnectionTest, TestKillOurselves2KillAndQueryThereafter) {
  try {
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();
    EXPECT_NE(nullptr, dm);

    sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
    EXPECT_NE(nullptr, wrapper1.get());
    sql::Connection *connection = wrapper1.get();

    std::unique_ptr<sql::Statement> stmt1(connection->createStatement());
    EXPECT_NE(nullptr, stmt1.get());

    std::unique_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT CONNECTION_ID()"));
    EXPECT_NE(nullptr, rset1.get());

    EXPECT_TRUE(rset1->next());

    // DBC is not supposed to check that, instead DBC user has to check validity of connection when needed
    // snprintf(buff, sizeof(buff), "KILL %d", rset1->getInt(1));
    // try
    //{
    //  // Kill the connection. This will give us an exception.
    //  stmt1->execute(buff);
    //  fail("An exception should have shown up.");
    //}
    // catch (sql::SQLException &e) {
    //  // Expected.
    //  ensure_equals("Unexpected exception", e.what(), "Commands out of sync; you can't run this command now");
    //}

    // Try another statement. This should give us another exception
    try {
      std::unique_ptr<sql::ResultSet> rset2(stmt1->executeQuery("SELECT CONNECTION_ID()"));
    } catch (sql::SQLException &e) {
      // Expected.
      EXPECT_STREQ("Commands out of sync; you can't run this command now", e.what());
    }
  } catch (sql::SQLException &e) {
    printf("ERR: Caught sql::SQLException: %s\n", e.what());
    throw;
  }
}
}

