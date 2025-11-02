/*
 * Copyright (c) 2012, 2019, Oracle and/or its affiliates. All rights reserved.
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

static bool populate_test_table(std::unique_ptr<sql::Statement> &stmt) {
  stmt->execute("USE test");
  stmt->execute("DROP TABLE IF EXISTS test_function");
  if (true == stmt->execute("CREATE TABLE test_function (a integer, b integer, c integer default null)"))
    return false;

  if (true == stmt->execute("INSERT INTO test_function (a,b,c) VALUES(1, 111, NULL)")) {
    stmt->execute("DROP TABLE test_function");
    return false;
  }
  return true;
}

static bool populate_tx_test_table(std::unique_ptr<sql::Statement> &stmt) {
  stmt->execute("USE test");
  stmt->execute("DROP TABLE IF EXISTS test_function_tx");
  if (true ==
      stmt->execute("CREATE TABLE test_function_tx (a integer, b integer, c integer default null) engine = innodb"))
    return false;

  if (true == stmt->execute("INSERT INTO test_function_tx (a,b,c) VALUES(1, 111, NULL)")) {
    stmt->execute("DROP TABLE test_function_tx");
    return false;
  }
  stmt->getConnection()->commit();
  return true;
}

class DbcResultSetTest : public ::testing::Test {
protected:
  void SetUp() override {
    register_all_metaclasses();
    grt::GRT::get()->scan_metaclasses_in("../../res/grt/");
    grt::GRT::get()->end_loading_metaclasses();
    EXPECT_EQ((size_t)INT_METACLASS_COUNT, grt::GRT::get()->get_metaclasses().size());
  }

  void TearDown() override {
    db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
    setupConnectionEnvironment(connectionProperties);
    sql::DriverManager *dm = sql::DriverManager::getDriverManager();
    dm->set_testing();

    sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
    sql::Connection *connection = wrapper1.get();
    std::unique_ptr<sql::Statement> stmt(connection->createStatement());
    stmt->execute("DROP SCHEMA IF EXISTS test;");

    MySqlStudioTester::reinitGRT();
  }
};

TEST_F(DbcResultSetTest, TestConnection) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
  ASSERT_NE(nullptr, wrapper1.get());

  sql::Connection *connection = wrapper1.get();
  std::unique_ptr<sql::Statement> stmt(connection->createStatement());
  ASSERT_NE(nullptr, stmt.get());

  ASSERT_EQ(connection, stmt->getConnection());

  stmt->execute("DROP SCHEMA IF EXISTS test; CREATE SCHEMA test");
}

TEST_F(DbcResultSetTest, TestPreparation) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
  ASSERT_NE(nullptr, wrapper1.get());

  sql::Connection *connection = wrapper1.get();
  std::unique_ptr<sql::Statement> stmt1(connection->createStatement());
  ASSERT_NE(nullptr, stmt1.get());

  ASSERT_EQ(wrapper1.get(), stmt1->getConnection());
  ASSERT_TRUE(populate_tx_test_table(stmt1));

  std::unique_ptr<sql::PreparedStatement> ps1(wrapper1->prepareStatement("SELECT a, b, c FROM test_function_tx"));
  ASSERT_NE(nullptr, ps1.get());

  std::unique_ptr<sql::ResultSet> rset(ps1->executeQuery());
  ASSERT_NE(nullptr, rset.get());
  while (rset->next()) {}

  stmt1->execute("DROP TABLE test_function_tx");
}

TEST_F(DbcResultSetTest, TestExecuteQuerySameStatement) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
  ASSERT_NE(nullptr, wrapper1.get());

  sql::Connection *connection = wrapper1.get();
  std::unique_ptr<sql::Statement> stmt1(connection->createStatement());
  ASSERT_NE(nullptr, stmt1.get());

  std::unique_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT 1 FROM DUAL"));
  ASSERT_NE(nullptr, rset1.get());
  ASSERT_TRUE(rset1->next());
}

TEST_F(DbcResultSetTest, TestExecuteTwoQueriesSameStatement) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
  ASSERT_NE(nullptr, wrapper1.get());

  sql::Connection *connection = wrapper1.get();
  std::unique_ptr<sql::Statement> stmt1(connection->createStatement());
  ASSERT_NE(nullptr, stmt1.get());

  ASSERT_TRUE(populate_test_table(stmt1));

  std::unique_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT 1 FROM DUAL"));
  ASSERT_NE(nullptr, rset1.get());
  ASSERT_TRUE(rset1->next());
  ASSERT_FALSE(rset1->next());

  ASSERT_GT(stmt1->executeUpdate("UPDATE test_function SET a = 2"), 0);

  stmt1->execute("DROP TABLE test_function");
}

TEST_F(DbcResultSetTest, TestCommitRollbackAutocommitOff) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
  ASSERT_NE(nullptr, wrapper1.get());

  sql::Connection *connection = wrapper1.get();
  std::unique_ptr<sql::Statement> stmt1(connection->createStatement());
  ASSERT_NE(nullptr, stmt1.get());

  ASSERT_EQ(wrapper1.get(), stmt1->getConnection());

  bool old_commit_mode = wrapper1->getAutoCommit();
  wrapper1->setAutoCommit(0);
  ASSERT_TRUE(populate_tx_test_table(stmt1));

  std::unique_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
  ASSERT_NE(nullptr, rset1.get());
  ASSERT_TRUE(rset1->next());
  int count_full_before = rset1->getInt(1);
  ASSERT_FALSE(rset1->next());

  ASSERT_EQ(stmt1->executeUpdate("DELETE FROM test_function_tx WHERE 1"), count_full_before);

  std::unique_ptr<sql::ResultSet> rset2(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
  ASSERT_NE(nullptr, rset2.get());
  ASSERT_TRUE(rset2->next());
  ASSERT_EQ(rset2->getInt(1), 0);
  ASSERT_FALSE(rset2->next());

  stmt1->getConnection()->rollback();

  std::unique_ptr<sql::ResultSet> rset3(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
  ASSERT_NE(nullptr, rset3.get());
  ASSERT_TRUE(rset3->next());
  int count_full_after = rset3->getInt(1);
  ASSERT_FALSE(rset3->next());

  ASSERT_EQ(count_full_before, count_full_after);

  ASSERT_EQ(stmt1->executeUpdate("DELETE FROM test_function_tx WHERE 1"), count_full_before);
  stmt1->getConnection()->commit();

  std::unique_ptr<sql::ResultSet> rset4(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
  ASSERT_NE(nullptr, rset4.get());
  ASSERT_TRUE(rset4->next());
  ASSERT_EQ(rset4->getInt(1), 0);
  ASSERT_FALSE(rset4->next());

  stmt1->execute("DROP TABLE test_function_tx");
  wrapper1->setAutoCommit(old_commit_mode);
}

TEST_F(DbcResultSetTest, TestCommitRollbackAutocommitOn) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
  ASSERT_NE(nullptr, wrapper1.get());

  sql::Connection *connection = wrapper1.get();
  std::unique_ptr<sql::Statement> stmt1(connection->createStatement());
  ASSERT_NE(nullptr, stmt1.get());

  ASSERT_EQ(wrapper1.get(), stmt1->getConnection());

  bool old_commit_mode = wrapper1->getAutoCommit();
  wrapper1->setAutoCommit(1);
  ASSERT_TRUE(populate_tx_test_table(stmt1));

  std::unique_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
  ASSERT_NE(nullptr, rset1.get());
  ASSERT_TRUE(rset1->next());
  int count_full_before = rset1->getInt(1);
  ASSERT_FALSE(rset1->next());

  ASSERT_EQ(stmt1->executeUpdate("DELETE FROM test_function_tx WHERE 1"), count_full_before);

  std::unique_ptr<sql::ResultSet> rset2(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
  ASSERT_NE(nullptr, rset2.get());
  ASSERT_TRUE(rset2->next());
  ASSERT_EQ(rset2->getInt(1), 0);
  ASSERT_FALSE(rset2->next());

  stmt1->getConnection()->rollback();

  std::unique_ptr<sql::ResultSet> rset3(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
  ASSERT_NE(nullptr, rset3.get());
  ASSERT_TRUE(rset3->next());
  ASSERT_EQ(rset3->getInt(1), 0);
  ASSERT_FALSE(rset3->next());

  ASSERT_TRUE(populate_tx_test_table(stmt1));
  ASSERT_EQ(stmt1->executeUpdate("DELETE FROM test_function_tx WHERE 1"), count_full_before);
  stmt1->getConnection()->commit();

  std::unique_ptr<sql::ResultSet> rset4(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
  ASSERT_NE(nullptr, rset4.get());
  ASSERT_TRUE(rset4->next());
  ASSERT_EQ(rset4->getInt(1), 0);
  ASSERT_FALSE(rset4->next());

  stmt1->execute("DROP TABLE test_function_tx");
  wrapper1->setAutoCommit(old_commit_mode);
}

TEST_F(DbcResultSetTest, TestMultistatementOff) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
  ASSERT_NE(nullptr, wrapper1.get());

  sql::Connection *connection = wrapper1.get();
  std::unique_ptr<sql::Statement> stmt1(connection->createStatement());
  ASSERT_NE(nullptr, stmt1.get());

  try {
    std::unique_ptr<sql::ResultSet> rset1(
      stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx; DELETE FROM test_function_tx"));
    FAIL() << "ERR: Exception not thrown";
  } catch (sql::SQLException &) {
    // Expected exception
  }
}

TEST_F(DbcResultSetTest, TestOutOfBoundExtraction) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_NE(nullptr, dm);

  sql::ConnectionWrapper wrapper1 = dm->getConnection(connectionProperties);
  ASSERT_NE(nullptr, wrapper1.get());

  sql::Connection *connection = wrapper1.get();
  std::unique_ptr<sql::Statement> stmt1(connection->createStatement());
  ASSERT_NE(nullptr, stmt1.get());

  ASSERT_EQ(wrapper1.get(), stmt1->getConnection());
  ASSERT_TRUE(populate_tx_test_table(stmt1));

  std::unique_ptr<sql::ResultSet> rset1(
    stmt1->executeQuery("SELECT COUNT(*) AS 'count of rows' FROM test_function_tx"));
  ASSERT_NE(nullptr, rset1.get());
  ASSERT_TRUE(rset1->next());

  // Out of bounds and invalid column tests
  ASSERT_THROW(rset1->getInt(-123), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->getInt(123), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->getInt("no_such_column"), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->getString(-123), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->getString(123), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->getString("no_such_column"), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->getDouble(-123), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->getDouble(123), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->getDouble("no_such_column"), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->getInt(rset1->getInt(1) + 1), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->isNull(-123), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->isNull(123), sql::InvalidArgumentException);
  ASSERT_THROW(rset1->isNull("no_such_column"), sql::InvalidArgumentException);

  ASSERT_EQ(rset1->getInt(1), 1);
  ASSERT_EQ(rset1->getInt("count of rows"), 1);
  ASSERT_TRUE(rset1->getDouble(1) - 1 < 0.1);
  ASSERT_TRUE(rset1->getDouble("count of rows") - 1 < 0.1);
  ASSERT_EQ(rset1->getString(1), std::string("1"));
  ASSERT_EQ(rset1->getString("count of rows"), std::string("1"));
  ASSERT_FALSE(rset1->isNull(1));
  ASSERT_FALSE(rset1->next());

  stmt1->execute("DROP TABLE test_function_tx");
}
