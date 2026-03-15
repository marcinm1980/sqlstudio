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

// This file contains unit tests for the yacc based invalid sql parser.

namespace {
struct MysqlInvalidSqlParserData {
  std::unique_ptr<MySqlStudioTester> tester;
  SqlFacade::Ref facade;
  std::string specificsDelimiter;
  std::string userDelimiter;
};

class MySQL_invalid_sql_parser_test_suite_yaccTest : public ::testing::Test {
protected:
  static std::unique_ptr<MysqlInvalidSqlParserData> data;

  static auto SetUpTestSuite() -> void {
    data = std::make_unique<MysqlInvalidSqlParserData>();
    data->tester.reset(new MySqlStudioTester());
    data->tester->initializeRuntime();

    data->facade = SqlFacade::instance_for_rdbms_name("Mysql");
    Sql_specifics::Ref sql_specifics = data->facade->sqlSpecifics();
    data->specificsDelimiter = sql_specifics->non_std_sql_delimiter();
    if (data->specificsDelimiter == ";;")
      data->userDelimiter = "%%";
    else
      data->userDelimiter = ";;";
  }

  static auto TearDownTestSuite() -> void {
    data.reset();
  }

};

std::unique_ptr<MysqlInvalidSqlParserData> MySQL_invalid_sql_parser_test_suite_yaccTest::data;

TEST_F(MySQL_invalid_sql_parser_test_suite_yaccTest, Trigger_parsing) {
  std::string trigger_sql =
  "CREATE TRIGGER `ins_film` AFTER INSERT ON `film` FOR EACH ROW BEGIN\n"
  "  INSERT INTO film_text (film_id, title, description)\n"
  "  VALUES (new.film_id, new.title, new.description);\n"
  "END";

  Invalid_sql_parser::Ref parser = data->facade->invalidSqlParser();

  db_mysql_CatalogRef catalog(grt::Initialized);
  db_mysql_SchemaRef schema(grt::Initialized);
  schema->name("sakila");
  catalog->schemata().insert(schema);
  schema->owner(catalog);

  db_mysql_TableRef table(grt::Initialized);
  table->name("film");
  schema->tables().insert(table);
  table->owner(schema);

  db_mysql_TriggerRef trigger(grt::Initialized);
  table->triggers().insert(trigger);
  trigger->owner(table);

  parser->parse_trigger(trigger, trigger_sql);

  // The parsing process returns one line break before the actual sql definition, even if no delimiter
  // etc. was given. This is due to the way query separation works there.
  // So we exclude the first char from the result in our comparisons.
  std::string result = (*trigger->sqlDefinition()).substr(1);
  EXPECT_EQ(result, trigger_sql) << "Trigger SQL differs";

  std::string sql = "use test;\n" + trigger_sql;
  parser->parse_trigger(trigger, trigger_sql);
  result = (*trigger->sqlDefinition()).substr(1);
  EXPECT_EQ(result, trigger_sql) << "Trigger SQL differs";

  sql = "DELIMITER ;\n" + trigger_sql;
  parser->parse_trigger(trigger, trigger_sql);
  result = (*trigger->sqlDefinition()).substr(1);
  EXPECT_EQ(result, trigger_sql) << "Trigger SQL differs";

  sql = "DELIMITER " + data->specificsDelimiter + "\n" + trigger_sql;
  parser->parse_trigger(trigger, trigger_sql);
  result = (*trigger->sqlDefinition()).substr(1);
  EXPECT_EQ(result, trigger_sql) << "5.4 Trigger SQL differs";

  sql = "DELIMITER " + data->specificsDelimiter + "\nuse test" + data->specificsDelimiter + "\n" + trigger_sql;
  parser->parse_trigger(trigger, trigger_sql);
  result = (*trigger->sqlDefinition()).substr(1);
  EXPECT_EQ(result, trigger_sql) << "Trigger SQL differs";

  sql = "DELIMITER " + data->specificsDelimiter + "\nDELIMITER " + data->userDelimiter + "\nDELIMITER ;\nDELIMITER " +
  data->userDelimiter + "\nDELIMITER " + data->specificsDelimiter + "\nDELIMITER " + data->specificsDelimiter +
  "\nuse test" + data->specificsDelimiter + "\n" + trigger_sql;
  parser->parse_trigger(trigger, trigger_sql);
  result = (*trigger->sqlDefinition()).substr(1);
  EXPECT_EQ(result, trigger_sql) << "Trigger SQL differs";

  sql = "DELIMITER " + data->userDelimiter + "\nuse test" + data->specificsDelimiter + "\n\n\n\n" + trigger_sql;
  parser->parse_trigger(trigger, trigger_sql);
  result = (*trigger->sqlDefinition()).substr(1);
  EXPECT_EQ(result, trigger_sql) << "Trigger SQL differs";
}


}
