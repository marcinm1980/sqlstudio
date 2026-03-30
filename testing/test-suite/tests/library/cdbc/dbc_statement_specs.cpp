/*
 * Copyright (c) 2012, 2019, Oracle and/or its affiliates. All rights reserved.
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
#include "cdbc/src/sql_batch_exec.h"
#include "grtsqlparser/sql_facade.h"

#include "wb_connection_helpers.h"
#include "wb_test_helpers.h"
#include "helpers.h"

#include "gtest/gtest.h"

#define DATABASE_TO_USE "USE test"

static bool populate_test_table(std::unique_ptr<sql::Statement> &stmt) {
  stmt->execute(DATABASE_TO_USE);
  stmt->execute("DROP TABLE IF EXISTS test_function");
  if (stmt->execute(
      "CREATE TABLE test_function (a integer, b integer, c integer default null)"))
    return false;

  if (stmt->execute("INSERT INTO test_function (a,b,c) VALUES(1, 111, NULL)")) {
    stmt->execute("DROP TABLE test_function");
    return false;
  }
  return true;
}

class DbcStatementTest : public ::testing::Test {
protected:
  std::unique_ptr<MySqlStudioTester> tester;
  SqlFacade::Ref sqlSplitter;
  sql::DriverManager *dm;
  db_mgmt_ConnectionRef connectionProperties;

  void SetUp() override {
    tester.reset(new MySqlStudioTester());
    sqlSplitter = SqlFacade::instance_for_rdbms_name("Mysql");
    ASSERT_TRUE(sqlSplitter != nullptr) << "failed to get sqlparser module";
    dm = sql::DriverManager::getDriverManager();
    connectionProperties = db_mgmt_ConnectionRef(grt::Initialized);
    setupConnectionEnvironment(connectionProperties);
    ASSERT_TRUE(dm != nullptr) << "Couldn't get a driver manager";
    sql::ConnectionWrapper connection = dm->getConnection(connectionProperties);
    ASSERT_TRUE(connection.get() != nullptr) << "Couldn't get a connection from driver";
    std::unique_ptr<sql::Statement> stmt(connection->createStatement());
    stmt->execute("CREATE SCHEMA IF NOT EXISTS test;");
  }

  void TearDown() override {
    auto connection = dm->getConnection(connectionProperties);
    std::unique_ptr<sql::Statement> stmt(connection->createStatement());
    stmt->execute("DROP SCHEMA IF EXISTS test;");
  }

  sql::ConnectionWrapper connection() {
    dm->set_testing();
    return dm->getConnection(connectionProperties);
  }

  void removeTestTable() {
    dm->set_testing();
    auto connection = dm->getConnection(connectionProperties);
    std::unique_ptr<sql::Statement> stmt2(connection->createStatement());
    stmt2->execute(DATABASE_TO_USE);
    stmt2->execute("DROP TABLE test_function");
  }
};

TEST_F(DbcStatementTest, SimpleUpdateStatement) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  ASSERT_TRUE(populate_test_table(stmt));
  ASSERT_FALSE(stmt->execute("UPDATE test_function SET a = 2, b = 222 where b = 111"));
  removeTestTable();
}

TEST_F(DbcStatementTest, SimpleQueryStatement) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  ASSERT_TRUE(populate_test_table(stmt));
  ASSERT_TRUE(stmt->execute("SELECT * FROM test_function"));
  removeTestTable();
}

TEST_F(DbcStatementTest, ExecuteQueryReturnsResultSet) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  ASSERT_TRUE(populate_test_table(stmt));
  std::unique_ptr<sql::ResultSet> rset(stmt->executeQuery("SELECT * FROM test_function"));
  ASSERT_TRUE(rset != nullptr);
  removeTestTable();
}

TEST_F(DbcStatementTest, ExecuteQueryReturnsEmptyResultSet) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  ASSERT_TRUE(populate_test_table(stmt));
  std::unique_ptr<sql::ResultSet> rset(stmt->executeQuery("SELECT * FROM test_function WHERE 1=2"));
  ASSERT_TRUE(rset != nullptr);
  ASSERT_FALSE(rset->next());
  removeTestTable();
}

TEST_F(DbcStatementTest, ExecuteQueryInsertShouldThrow) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  ASSERT_TRUE(populate_test_table(stmt));
  ASSERT_THROW(stmt->executeQuery("INSERT INTO test_function VALUES(2,200)"), sql::SQLException);
  removeTestTable();
}

TEST_F(DbcStatementTest, ExecuteUpdateReturnsRowCount) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  ASSERT_TRUE(populate_test_table(stmt));
  ASSERT_EQ(stmt->executeUpdate("UPDATE test_function SET a = 123"), 1);
  removeTestTable();
}

TEST_F(DbcStatementTest, ExecuteUpdateSelectShouldThrow) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  ASSERT_TRUE(populate_test_table(stmt));
  ASSERT_THROW(stmt->executeUpdate("SELECT * FROM test_function"), sql::SQLException);
  removeTestTable();
}

TEST_F(DbcStatementTest, GetFetchSizePending) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  GTEST_SKIP() << "needs implementation of getFetchSize and getFetchDirection";
}

TEST_F(DbcStatementTest, GetResultSetAfterExecuteQuery) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  ASSERT_TRUE(populate_test_table(stmt));
  ASSERT_TRUE(stmt->execute("SELECT * FROM test_function"));
  std::unique_ptr<sql::ResultSet> rset(stmt->getResultSet());
  ASSERT_TRUE(rset != nullptr);
  removeTestTable();
}

TEST_F(DbcStatementTest, GetResultSetAfterExecuteUpdate) {
  auto conn = connection();
  std::unique_ptr<sql::Statement> stmt(conn->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  ASSERT_TRUE(populate_test_table(stmt));
  ASSERT_FALSE(stmt->execute("UPDATE test_function SET a = 222"));
  std::unique_ptr<sql::ResultSet> rset(stmt->getResultSet());
  ASSERT_EQ(rset.get(), nullptr);
  removeTestTable();
}

TEST_F(DbcStatementTest, SetFetchSizePending) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_TRUE(dm != nullptr);
  sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
  ASSERT_TRUE(wrapper.get() != nullptr);
  sql::Connection *connection = wrapper.get();
  std::unique_ptr<sql::Statement> stmt(connection->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  GTEST_SKIP() << "setFetchSize not implemented";
}

TEST_F(DbcStatementTest, SetFetchSizeNegativePending) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_TRUE(dm != nullptr);
  sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
  ASSERT_TRUE(wrapper.get() != nullptr);
  sql::Connection *connection = wrapper.get();
  std::unique_ptr<sql::Statement> stmt(connection->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  GTEST_SKIP() << "setFetchSize not implemented";
}

TEST_F(DbcStatementTest, SetQueryTimeoutNegativePending) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_TRUE(dm != nullptr);
  sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
  ASSERT_TRUE(wrapper.get() != nullptr);
  sql::Connection *connection = wrapper.get();
  std::unique_ptr<sql::Statement> stmt(connection->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  GTEST_SKIP() << "setQueryTimeout not implemented";
}

TEST_F(DbcStatementTest, AddBatchExecuteBatch) {
  db_mgmt_ConnectionRef connectionProperties(grt::Initialized);
  setupConnectionEnvironment(connectionProperties);
  sql::DriverManager *dm = sql::DriverManager::getDriverManager();
  dm->set_testing();
  ASSERT_TRUE(dm != nullptr);
  sql::ConnectionWrapper wrapper = dm->getConnection(connectionProperties);
  ASSERT_TRUE(wrapper.get() != nullptr);
  sql::Connection *connection = wrapper.get();
  std::unique_ptr<sql::Statement> stmt(connection->createStatement());
  ASSERT_TRUE(stmt != nullptr);
  std::string sql_script =
      "DROP DATABASE IF EXISTS dbc_statement_test_15;"
      "CREATE DATABASE dbc_statement_test_15;"
      "CREATE TABLE dbc_statement_test_15.table1 (id int);"
      "SELECT 1;"
      "CREATE TABLE dbc_statement_test_15.table2 (id int);"
      "SELECT 1;"
      "CREATE TABLE dbc_statement_test_15.table3 (id int);"
      "SELECT 1;";
  std::list<std::string> statements;
  ASSERT_TRUE(sqlSplitter != nullptr) << "failed to get sqlparser module";
  sqlSplitter->splitSqlScript(sql_script, statements);
  sql::SqlBatchExec()(stmt.get(), statements);
  // Cleanup
  sql_script = "DROP DATABASE IF EXISTS dbc_statement_test_15;";
  sqlSplitter->splitSqlScript(sql_script, statements);
  sql::SqlBatchExec()(stmt.get(), statements);
}

