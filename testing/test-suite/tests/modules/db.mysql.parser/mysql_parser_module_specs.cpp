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

#include "gtest/gtest.h"
#include "wb_test_helpers.h"

#include "grt.h"
#include "grtsqlparser/mysql_parser_services.h"

using namespace parsers;

namespace {
struct MysqlParserModuleData {
  std::unique_ptr<MySqlStudioTester> tester;
  MySQLParserServices::Ref services;
  MySQLParserContext::Ref context;
};

// Contains tests for the parser module implementing the ANTLR based parser services.
// Many of the APIs are also used in other tests.
class Parser_moduleTest : public ::testing::Test {
protected:
  static std::unique_ptr<MysqlParserModuleData> data;

  static auto SetUpTestSuite() -> void {
    data = std::make_unique<MysqlParserModuleData>();
    data->tester.reset(new MySqlStudioTester(false));
    data->tester->initializeRuntime();

    data->services = MySQLParserServices::get();
    GrtVersionRef version(grt::Initialized);
    version->majorNumber(5);
    version->minorNumber(7);
    version->releaseNumber(10);
    data->context = MySQLParserServices::get()->createParserContext(data->tester->getRdbms()->characterSets(), version, "", true);
  }

  static auto TearDownTestSuite() -> void {
    data->context.reset();
    data.reset();
  }

};

std::unique_ptr<MysqlParserModuleData> Parser_moduleTest::data;

TEST_F(Parser_moduleTest, alterStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, createStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, dropStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, renameTableStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, truncateTableStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, callStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, deleteStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, doStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, handlerStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, insertStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, loadStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, replaceStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, selectStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, updateStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, partitioning) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, transactionOrLockingStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, replicationStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, preparedStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, accountManagementStatement) {
  // ----- Grant statements.
  grt::DictRef result = data->services->parseStatement(data->context, "grant all privileges on table a to current_user");
  grt::StringListRef privileges = grt::StringListRef::cast_from(result["privileges"]);
  EXPECT_TRUE(privileges.is_valid()) << "95.1";
  EXPECT_EQ(privileges.get_index("all privileges"), 0U) << "95.2";
  EXPECT_EQ(*grt::StringRef::cast_from(result["target"]), "table a") << "95.3";

  grt::DictRef users = grt::DictRef::cast_from(result["users"]);
  EXPECT_TRUE(users.is_valid()) << "95.4";
  EXPECT_TRUE(users.has_key("current_user")) << "95.5";
  grt::DictRef user = grt::DictRef::cast_from(users["current_user"]);
  EXPECT_TRUE(user.has_key("user")) << "95.6";
  EXPECT_EQ(*grt::StringRef::cast_from(user["user"]), "current_user") << "95.7";

  result = data->services->parseStatement(data->context, "grant all privileges on table *.* to CURRENT_USER() identified by password 'blah'");
  privileges = grt::StringListRef::cast_from(result["privileges"]);
  EXPECT_TRUE(privileges.is_valid()) << "95.8";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[0]), "all privileges") << "95.9";
  EXPECT_EQ(*grt::StringRef::cast_from(result["target"]), "table *.*") << "95.10";

  users = grt::DictRef::cast_from(result["users"]);
  EXPECT_TRUE(users.is_valid()) << "95.11";
  user = grt::DictRef::cast_from(users["CURRENT_USER"]);
  EXPECT_EQ(*grt::StringRef::cast_from(user["user"]), "CURRENT_USER") << "95.12";
  EXPECT_EQ(*grt::StringRef::cast_from(user["id_method"]), "PASSWORD") << "95.13";
  EXPECT_EQ(*grt::StringRef::cast_from(user["id_string"]), "blah") << "95.14";

  result = data->services->parseStatement(data->context, "grant all privileges on x.* to mike identified with 'blah' by 'blubb'");
  privileges = grt::StringListRef::cast_from(result["privileges"]);
  EXPECT_TRUE(privileges.is_valid()) << "95.15";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[0]), "all privileges") << "95.1";
  EXPECT_EQ(*grt::StringRef::cast_from(result["target"]), "x.*") << "95.17";

  users = grt::DictRef::cast_from(result["users"]);
  EXPECT_TRUE(users.is_valid()) << "95.18";
  user = grt::DictRef::cast_from(users["mike"]);
  EXPECT_TRUE(user.is_valid()) << "95.1";
  EXPECT_EQ(*grt::StringRef::cast_from(user["user"]), "mike") << "95.20";
  EXPECT_EQ(*grt::StringRef::cast_from(user["id_method"]), "blah") << "95.21";
  EXPECT_EQ(*grt::StringRef::cast_from(user["id_string"]), "blubb") << "95.22";

  result = data->services->parseStatement(data->context, "grant all privileges on function x.y to mike\t@\nhome");
  privileges = grt::StringListRef::cast_from(result["privileges"]);
  EXPECT_TRUE(privileges.is_valid()) << "95.23";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[0]), "all privileges") << "95.24";
  EXPECT_EQ(*grt::StringRef::cast_from(result["target"]), "function x.y") << "95.25";

  users = grt::DictRef::cast_from(result["users"]);
  EXPECT_TRUE(users.is_valid()) << "95.26";
  user = grt::DictRef::cast_from(users["mike"]);
  EXPECT_TRUE(user.is_valid()) << "95.27";
  EXPECT_EQ(*grt::StringRef::cast_from(user["user"]), "mike") << "95.28";
  EXPECT_FALSE(user["id_method"].is_valid()) << "95.29";
  EXPECT_FALSE(user["id_string"].is_valid()) << "95.30";
  EXPECT_EQ(*grt::StringRef::cast_from(user["host"]), "home") << "95.31";

  result = data->services->parseStatement(data->context, "grant select on pizza to me require X509 with grant option");
  privileges = grt::StringListRef::cast_from(result["privileges"]);
  EXPECT_TRUE(privileges.is_valid()) << "95.32";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[0]), "select") << "95.33";
  EXPECT_EQ(*grt::StringRef::cast_from(result["target"]), "pizza") << "95.34";

  users = grt::DictRef::cast_from(result["users"]);
  EXPECT_TRUE(users.is_valid()) << "95.35";
  user = grt::DictRef::cast_from(users["me"]);
  EXPECT_TRUE(user.is_valid()) << "95.36";
  EXPECT_EQ(*grt::StringRef::cast_from(user["user"]), "me") << "95.37";
  EXPECT_EQ(user.count(), 1U) << "95.38";

  // Everything possible in a grant statements.
  std::string sql = base::wstring_to_string(
    L"grant insert (a), insert (b), insert(c), update(a), "
    L"alter routine, create routine, "
    L"create tablespace, create\t\t\t temporary      tables, create user, create view, delete, drop, event, "
    L"execute, file, grant option, index, insert, insert (a, b, c, d, ⌚️, ♨️), lock tables, process, proxy, "
    L"references (a, b, c, d, ⌚️, ♨️), reload, replication client, select, select (a, b, c, d, ⌚️, ♨️), "
    L"show databases, show view, shutdown, super, trigger, update, update (a, b, c, d, ⌚️, ♨️), usage "
    L"on *.* to current_user, CURRENT_USER() identified by password 'blah', mike identified with 'blah' by 'blubb', "
    L"mike@home require cipher 'abc' and cipher 'xyz' issuer 'a' subject 'b' and issuer '⌚️' with "
    L"grant option max_queries_per_hour 1 max_updates_per_hour 2 max_connections_per_hour 3 "
    L"max_user_connections 4 max_queries_per_hour 111 max_queries_per_hour 111 max_queries_per_hour 222");
  result = data->services->parseStatement(data->context, sql);
  privileges = grt::StringListRef::cast_from(result["privileges"]);
  EXPECT_TRUE(privileges.is_valid()) << "95.39";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[0]), "insert (a)") << "95.40";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[7]), "create\t\t\t temporary      tables") << "95.41";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[11]), "drop") << "95.42";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[15]), "grant option") << "95.43";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[18]), base::wstring_to_string(L"insert (a, b, c, d, ⌚️, ♨️)")) << "95.44";
  EXPECT_EQ(*grt::StringRef::cast_from(privileges[27]), "show databases") << "95.45";

  EXPECT_EQ(*grt::StringRef::cast_from(result["target"]), "*.*") << "95.46";

  users = grt::DictRef::cast_from(result["users"]);
  EXPECT_TRUE(users.is_valid()) << "95.48";
  EXPECT_EQ(users.count(), 3U) << "95.49";
  EXPECT_TRUE(users["current_user"].is_valid()) << "95.40";
  EXPECT_TRUE(users["CURRENT_USER"].is_valid()) << "95.50";
  EXPECT_TRUE(users["mike"].is_valid()) << "95.51";

  user = grt::DictRef::cast_from(users["CURRENT_USER"]);
  EXPECT_EQ(*grt::StringRef::cast_from(user["user"]), "CURRENT_USER") << "95.52";
  EXPECT_EQ(user.count(), 3U) << "95.53";

  grt::DictRef options = grt::DictRef::cast_from(result["options"]);
  EXPECT_TRUE(options.is_valid()) << "95.54";
  EXPECT_EQ(options.count(), 5U) << "95.55";
  EXPECT_EQ(*grt::StringRef::cast_from(options["grant"]), "") << "95.56";
  EXPECT_EQ(*grt::StringRef::cast_from(options["max_queries_per_hour"]), "222") << "95.57";

  grt::DictRef requirements = grt::DictRef::cast_from(result["requirements"]);
  EXPECT_TRUE(requirements.is_valid()) << "95.58";
  EXPECT_EQ(requirements.count(), 3U) << "95.59";
  EXPECT_EQ(*grt::StringRef::cast_from(requirements["cipher"]), "xyz") << "95.60";
  EXPECT_EQ(*grt::StringRef::cast_from(requirements["issuer"]), base::wstring_to_string(L"⌚️")) << "95.61";
}

TEST_F(Parser_moduleTest, table_administrationStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, install_uninstall_statment) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, setStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, showStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, other_administrativeStatement) {
  GTEST_SKIP() << "requires implementation";
}

TEST_F(Parser_moduleTest, utilityStatement) {
  GTEST_SKIP() << "requires implementation";
}


}
