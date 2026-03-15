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

#include "grtsqlparser/sql_facade.h"

#include "gtest/gtest.h"
#include "wb_test_helpers.h"

namespace {
struct MysqlSqlFacadeData {
  std::unique_ptr<MySqlStudioTester> tester;
  SqlFacade::Ref facade;
  db_mgmt_RdbmsRef rdbms;
  grt::DictRef options;
};

class SQL_Parser_FE_MySQLTest : public ::testing::Test {
protected:
  static std::unique_ptr<MysqlSqlFacadeData> data;

  static auto SetUpTestSuite() -> void {
    data = std::make_unique<MysqlSqlFacadeData>();
    data->tester.reset(new MySqlStudioTester());
    data->facade = nullptr;
    data->tester->createNewDocument();

    EXPECT_EQ(data->tester->wb->get_document()->physicalModels().count(), 1U) << "loaded physycal model count";

    data->options = grt::DictRef(true);
    data->options.set("gen_fk_names_when_empty", grt::IntegerRef(0));

    data->rdbms = data->tester->wb->get_document()->physicalModels().get(0)->rdbms();

    data->facade = SqlFacade::instance_for_rdbms(data->rdbms);
    EXPECT_NE(data->facade, nullptr) << "Failed to get sqlparser module";
  }

  static auto TearDownTestSuite() -> void {
    data.reset();
  }

};

std::unique_ptr<MysqlSqlFacadeData> SQL_Parser_FE_MySQLTest::data;

TEST_F(SQL_Parser_FE_MySQLTest, Pretty_simple_parsing_sample) {
  std::string schema_name;
  std::string table_name;
  SqlFacade::String_tuple_list columns;

  std::string query = "select first_name, last_name from sakila.customer;";

  EXPECT_TRUE(data->facade->parseSelectStatementForEdit(query, schema_name, table_name, columns)) << "Unexpexted failure parsing test";
  EXPECT_EQ(schema_name, "sakila") << "Unexpected Schema Name";
  EXPECT_EQ(table_name, "customer") << "Unexpected Table Name";
  EXPECT_EQ(columns.size(), 2U) << "Unexpected Column Count";
  EXPECT_EQ(columns.front().first, "first_name") << "Unexpected Column Name";
  EXPECT_EQ(columns.front().second, "first_name") << "Unexpected Column Alias";
  columns.pop_front();
  EXPECT_EQ(columns.front().first, "last_name") << "Unexpected Column Name";
  EXPECT_EQ(columns.front().second, "last_name") << "Unexpected Column Alias";
  columns.pop_front();
}

TEST_F(SQL_Parser_FE_MySQLTest, Simple_parsing_sample_using_aliases_for_the_columns) {
  std::string schema_name;
  std::string table_name;
  SqlFacade::String_tuple_list columns;

  std::string query = "select first_name as 'First Name', last_name as 'Last Name' from sakila.customer;";

  EXPECT_TRUE(data->facade->parseSelectStatementForEdit(query, schema_name, table_name, columns)) << "Unexpexted failure parsing test";
  EXPECT_EQ(schema_name, "sakila") << "Unexpected Schema Name";
  EXPECT_EQ(table_name, "customer") << "Unexpected Table Name";
  EXPECT_EQ(columns.size(), 2U) << "Unexpected Column Count";
  EXPECT_EQ(columns.front().first, "first_name") << "Unexpected Column Name";
  EXPECT_EQ(columns.front().second, "First Name") << "Unexpected Column Alias";
  columns.pop_front();
  EXPECT_EQ(columns.front().first, "last_name") << "Unexpected Column Name";
  EXPECT_EQ(columns.front().second, "Last Name") << "Unexpected Column Alias";
  columns.pop_front();
}

TEST_F(SQL_Parser_FE_MySQLTest, Numeric_literals_as_columns) {
  std::string schema_name;
  std::string table_name;
  SqlFacade::String_tuple_list columns;

  std::string query = "select customer_id, 10 as 'years' from sakila.customer;";

  EXPECT_FALSE(data->facade->parseSelectStatementForEdit(query, schema_name, table_name, columns)) << "Unexpexted success parsing test";
  EXPECT_TRUE(schema_name.empty()) << "Unexpected Schema Name";
  EXPECT_TRUE(table_name.empty()) << "Unexpected Table Name";
  EXPECT_EQ(columns.size(), 0U) << "Unexpected Column Count";
}

TEST_F(SQL_Parser_FE_MySQLTest, Using_text_literals_as_columns) {
  std::string schema_name;
  std::string table_name;
  SqlFacade::String_tuple_list columns;

  std::string query = "select 'Dear' as Greeting, first_name, last_name from sakila.customer;";

  EXPECT_FALSE(data->facade->parseSelectStatementForEdit(query, schema_name, table_name, columns)) << "Unexpexted success parsing test";
  EXPECT_TRUE(schema_name.empty()) << "Unexpected Schema Name";
  EXPECT_TRUE(table_name.empty()) << "Unexpected Table Name";
  EXPECT_TRUE(columns.empty()) << "Unexpected Column Count";
}

TEST_F(SQL_Parser_FE_MySQLTest, Using_SELECT) {
  std::string schema_name;
  std::string table_name;
  SqlFacade::String_tuple_list columns;

  std::string query = "select * from `sakila`.`address`";

  EXPECT_TRUE(data->facade->parseSelectStatementForEdit(query, schema_name, table_name, columns)) << "Unexpected failure parsing test";
  EXPECT_EQ(schema_name, "sakila") << "Unexpected Schema Name";
  EXPECT_EQ(table_name, "address") << "Unexpected Table Name";
  EXPECT_EQ(columns.size(), 1U) << "Unexpected Column Count";
  EXPECT_EQ(columns.front().first, "*") << "Unexpected Column Name";
  EXPECT_EQ(columns.front().second, "*") << "Unexpected Column Alias";
  columns.pop_front();
}

TEST_F(SQL_Parser_FE_MySQLTest, Using_WHERE) {
  // Using the WHERE clause doesn't impact the parsing as long as the information is being
  // retrieved from a single table
  std::string schema_name;
  std::string table_name;
  SqlFacade::String_tuple_list columns;

  std::string query = "select address, phone as Phone from `sakila`.`address` where district = 'Adana'";

  EXPECT_TRUE(data->facade->parseSelectStatementForEdit(query, schema_name, table_name, columns)) << "Unexpexted failure parsing test";
  EXPECT_EQ(schema_name, "sakila") << "Unexpected Schema Name";
  EXPECT_EQ(table_name, "address") << "Unexpected Table Name";
  EXPECT_EQ(columns.size(), 2U) << "Unexpected Column Count";
  EXPECT_EQ(columns.front().first, "address") << "Unexpected Column Name";
  EXPECT_EQ(columns.front().second, "address") << "Unexpected Column Alias";
  columns.pop_front();
  EXPECT_EQ(columns.front().first, "phone") << "Unexpected Column Name";
  EXPECT_EQ(columns.front().second, "Phone") << "Unexpected Column Alias";
  columns.pop_front();
}

TEST_F(SQL_Parser_FE_MySQLTest, Using_many_tables_to_pull_the_information) {
  std::string schema_name;
  std::string table_name;
  SqlFacade::String_tuple_list columns;

  std::string query =
  "SELECT customer.first_name, customer.last_name, address.address FROM sakila.customer, sakila.address where "
  "customer.address_id = address.address_id;";

  EXPECT_FALSE(data->facade->parseSelectStatementForEdit(query, schema_name, table_name, columns)) << "Unexpexted success parsing test";
  EXPECT_EQ(schema_name, "");
  EXPECT_EQ(table_name, "");
  EXPECT_EQ(columns.size(), 0U) << "Unexpected Column Count";
}

TEST_F(SQL_Parser_FE_MySQLTest, Multiple_select_statements) {
  std::string schema_name;
  std::string table_name;
  SqlFacade::String_tuple_list columns;

  std::string query = "SELECT * FROM sakila.customer; SELECT * FROM sakila.address;";

  EXPECT_FALSE(data->facade->parseSelectStatementForEdit(query, schema_name, table_name, columns)) << "Unexpexted success parsing test";
  EXPECT_EQ(schema_name, "");
  EXPECT_EQ(table_name, "");
  EXPECT_TRUE(columns.empty()) << "Unexpected Column Count";
}

TEST_F(SQL_Parser_FE_MySQLTest, Multiple_functions_as_columns) {
  std::string schema_name;
  std::string table_name;
  SqlFacade::String_tuple_list columns;

  std::string query = "SELECT count(*) FROM sakila.customer";

  EXPECT_FALSE(data->facade->parseSelectStatementForEdit(query, schema_name, table_name, columns)) << "Unexpexted success parsing test";
  EXPECT_EQ(schema_name, "");
  EXPECT_EQ(table_name, "");
  EXPECT_TRUE(columns.empty()) << "Unexpected Column Count";
}


}
