/*
 * Copyright (c) 2019, 2021, Oracle and/or its affiliates.
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

#include "base/file_utilities.h"

#include "code-completion/mysql-code-completion.h"
#include "mysql/MySQLRecognizerCommon.h"
#include "mysql/MySQLLexer.h"
#include "mysql/MySQLParser.h"
#include "mysql/MySQLParserBaseListener.h"

#include "grtdb/db_helpers.h"
#include "grtdb/db_object_helpers.h"
#include "sqlide/wb_sql_editor_form.h"
#include "sqlide/sql_editor_be.h"

using namespace std::string_literals;

#include "grtsqlparser/mysql_parser_services.h"

using namespace bec;
using namespace wb;
using namespace parsers;
using namespace antlr4;

namespace ph = std::placeholders;

namespace {

  struct TestData {
    std::unique_ptr<MySqlStudioTester> tester;
    MySQLEditor::Ref sql_editor;

    parsers::MySQLParserContext::Ref autocompleteContext;
    SymbolTable mainSymbols;
    SymbolTable dbObjects;
    SymbolTable runtimeFunctions;

    long version;
  };

struct ac_test_entry {
  int version_first; //  First supported version for this entry
  int version_last;  //  Last supported version for this entry

  std::string query;
  std::string typed_part;
  int line;
  int offset;

  bool check_entries;
  std::string entries;
  int parts;
};

class ErrorListener : public BaseErrorListener {
public:
  size_t errorCount = 0;
  virtual void syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line, size_t charPositionInLine,
                           const std::string &msg, std::exception_ptr e) override {
    ++errorCount;
  }
};

  // Create a mockup symbol table with all database objects we support.
void createDBObjects(SymbolTable &symbolTable) {
  auto sakila = symbolTable.addNewSymbol<SchemaSymbol>(nullptr, "sakila");
  symbolTable.addNewSymbol<SchemaSymbol>(nullptr, "sakila_test");
  symbolTable.addNewSymbol<SchemaSymbol>(nullptr, "mysql");
  symbolTable.addNewSymbol<SchemaSymbol>(nullptr, "information_schema");
  symbolTable.addNewSymbol<SchemaSymbol>(nullptr, "sys");
  symbolTable.addNewSymbol<SchemaSymbol>(nullptr, "test");

  symbolTable.addNewSymbol<TableSymbol>(sakila, "actor");
  symbolTable.addNewSymbol<TableSymbol>(sakila, "address");
  symbolTable.addNewSymbol<TableSymbol>(sakila, "category");
  symbolTable.addNewSymbol<TableSymbol>(sakila, "city");
  symbolTable.addNewSymbol<TableSymbol>(sakila, "country");
  symbolTable.addNewSymbol<TableSymbol>(sakila, "custom");
  auto film = symbolTable.addNewSymbol<TableSymbol>(sakila, "film");
  symbolTable.addNewSymbol<ColumnSymbol>(film, "film_id", FundamentalType::INTEGER_TYPE);
  symbolTable.addNewSymbol<ColumnSymbol>(film, "title", FundamentalType::STRING_TYPE);
  symbolTable.addNewSymbol<ColumnSymbol>(film, "description", FundamentalType::STRING_TYPE);
  symbolTable.addNewSymbol<ColumnSymbol>(film, "release_year", FundamentalType::DATE_TYPE);
  symbolTable.addNewSymbol<ColumnSymbol>(film, "language_id", FundamentalType::INTEGER_TYPE);
  symbolTable.addNewSymbol<ColumnSymbol>(film, "original_language_id", FundamentalType::INTEGER_TYPE);
  symbolTable.addNewSymbol<IndexSymbol>(film, "PRIMARY");
  symbolTable.addNewSymbol<IndexSymbol>(film, "idx_title");
  symbolTable.addNewSymbol<ForeignKeySymbol>(film, "fk_film_language");
  symbolTable.addNewSymbol<ForeignKeySymbol>(film, "fk_film_language_original");

  symbolTable.addNewSymbol<ViewSymbol>(sakila, "actor_info");
  symbolTable.addNewSymbol<ViewSymbol>(sakila, "custom_list");
  symbolTable.addNewSymbol<ViewSymbol>(sakila, "nicer_but_slower_film_list");
  symbolTable.addNewSymbol<ViewSymbol>(sakila, "sales_by_film_category");
  symbolTable.addNewSymbol<ViewSymbol>(sakila, "sales_by_store");

  symbolTable.addNewSymbol<EventSymbol>(sakila, "event1");

  symbolTable.addNewSymbol<StoredRoutineSymbol>(sakila, "film_in_stock", nullptr);
  symbolTable.addNewSymbol<StoredRoutineSymbol>(sakila, "film_not_in_stock", nullptr);
  symbolTable.addNewSymbol<StoredRoutineSymbol>(sakila, "rewards_report", nullptr);

  symbolTable.addNewSymbol<StoredRoutineSymbol>(sakila, "get_customer_balance", FundamentalType::FLOAT_TYPE);
  symbolTable.addNewSymbol<StoredRoutineSymbol>(sakila, "inventory_held_by_customer", FundamentalType::INTEGER_TYPE);
  symbolTable.addNewSymbol<StoredRoutineSymbol>(sakila, "inventory_in_stock", FundamentalType::INTEGER_TYPE);

  // Triggers are schema local objects, even though bound to a specific table.
  symbolTable.addNewSymbol<TriggerSymbol>(sakila, "ins_film");
  symbolTable.addNewSymbol<TriggerSymbol>(sakila, "del_film");

  // Other schema or global entities.
  symbolTable.addNewSymbol<UdfSymbol>(sakila, "udf1");
  symbolTable.addNewSymbol<UdfSymbol>(sakila, "udf2");
  symbolTable.addNewSymbol<UdfSymbol>(sakila, "udf3");

  symbolTable.addNewSymbol<EngineSymbol>(nullptr, "innodb");
  symbolTable.addNewSymbol<EngineSymbol>(nullptr, "myisam");
  symbolTable.addNewSymbol<EngineSymbol>(nullptr, "blackhole");

  symbolTable.addNewSymbol<LogfileGroupSymbol>(nullptr, "group1");
  symbolTable.addNewSymbol<LogfileGroupSymbol>(nullptr, "group2");
  symbolTable.addNewSymbol<LogfileGroupSymbol>(nullptr, "group3");

  symbolTable.addNewSymbol<TableSpaceSymbol>(nullptr, "space1");
  symbolTable.addNewSymbol<TableSpaceSymbol>(nullptr, "space2");
  symbolTable.addNewSymbol<TableSpaceSymbol>(nullptr, "space3");

  symbolTable.addNewSymbol<SystemVariableSymbol>(nullptr, "@@var1");
  symbolTable.addNewSymbol<SystemVariableSymbol>(nullptr, "@@var2");
  symbolTable.addNewSymbol<SystemVariableSymbol>(nullptr, "@@var3");

  symbolTable.addNewSymbol<UserVariableSymbol>(nullptr, "@var11", FundamentalType::INTEGER_TYPE);
  symbolTable.addNewSymbol<UserVariableSymbol>(nullptr, "@var22", FundamentalType::INTEGER_TYPE);
  symbolTable.addNewSymbol<UserVariableSymbol>(nullptr, "@var33", FundamentalType::INTEGER_TYPE);

  symbolTable.addNewSymbol<CharsetSymbol>(nullptr, "utf8");
  symbolTable.addNewSymbol<CharsetSymbol>(nullptr, "latin1");
  symbolTable.addNewSymbol<CharsetSymbol>(nullptr, "big5");

  symbolTable.addNewSymbol<CollationSymbol>(nullptr, "utf8_bin");
  symbolTable.addNewSymbol<CollationSymbol>(nullptr, "latin1_general_cs");
  symbolTable.addNewSymbol<CollationSymbol>(nullptr, "big5_chinese_ci");
}

class SqlCodeCompletionTest : public ::testing::Test {
protected:
  TestData *data = new TestData();

  void SetUp() override {
    data->tester.reset(new MySqlStudioTester());
    data->tester->initializeRuntime();

    GrtVersionRef version = data->tester->getRdbms()->version();

    // Copy a current version of the code editor configuration file to the test data folder.
    base::copyFile("../../res/wbdata/code_editor.xml", "data/code_editor.xml");

    parsers::MySQLParserServices::Ref services = parsers::MySQLParserServices::get();
    parsers::MySQLParserContext::Ref context = services->createParserContext(
      data->tester->getRdbms()->characterSets(), version, "", false);

    data->autocompleteContext = services->createParserContext(data->tester->getRdbms()->characterSets(), version, "", false);

    data->sql_editor = MySQLEditor::create(context, data->autocompleteContext, { &data->mainSymbols });
    data->sql_editor->set_current_schema("sakila");

    createDBObjects(data->dbObjects);
    data->mainSymbols.addDependencies({ &data->dbObjects, functionSymbolsForVersion(base::MySQLVersion::MySQL57) });
  }

  void TearDown() override {
    data->sql_editor.reset();
    data->autocompleteContext.reset();
    delete data;
  }
};

TEST_F(SqlCodeCompletionTest, TestingProperSymbolRetrievalInSymbolTables) {
  auto tables = data->mainSymbols.getSymbolsOfType<TableSymbol>();
  EXPECT_EQ(0U, tables.size()) << "Test 10.1";

  auto schemas = data->mainSymbols.getSymbolsOfType<SchemaSymbol>();
  EXPECT_EQ(6U, schemas.size()) << "Test 10.2";

  auto iterator = std::find_if(schemas.begin(), schemas.end(), [](Symbol *symbol) { return symbol->name == "sakila"; });

  // TODO: need matcher/translator for iterators.
  //EXPECT_NE(iterator, schemas.end()) << "Test 10.3";
  EXPECT_TRUE(iterator != schemas.end()) << "Test 10.3";

  SchemaSymbol *schema = *iterator;
  EXPECT_NE(nullptr, schema) << "Test 10.4";

  tables = data->mainSymbols.getSymbolsOfType<TableSymbol>(schema);
  EXPECT_EQ(7U, tables.size()) << "Test 10.5";
  auto views = data->mainSymbols.getSymbolsOfType<ViewSymbol>(schema);
  EXPECT_EQ(5U, views.size()) << "Test 10.6";
  auto routines = data->mainSymbols.getSymbolsOfType<RoutineSymbol>(schema);
  EXPECT_EQ(6U, routines.size()) << "Test 10.7";
  auto udfs = data->mainSymbols.getSymbolsOfType<UdfSymbol>(schema);
  EXPECT_EQ(3U, udfs.size()) << "Test 10.8";

  auto systemFunctions = data->mainSymbols.getSymbolsOfType<RoutineSymbol>(); // System functions.
  EXPECT_EQ(293U, systemFunctions.size());
}

  TEST_F(SqlCodeCompletionTest, CodeCompletionForCorrectCandidateCollections) {
    ANTLRInputStream input(
      "CREATE TABLE `partition_test` (\n"
      "`id` int(10) NOT NULL AUTO_INCREMENT,\n"
      "`team` int(10) NOT NULL DEFAULT '0',\n"
      "`created` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',\n"
      "PRIMARY KEY (`id`,`team`,`created`)\n"
      ") ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8\n"
      "/*!50100 PARTITION BY RANGE (team)\n"
      "(PARTITION t1 VALUES LESS THAN (1) ENGINE = MyISAM,\n"
      "PARTITION t2 VALUES LESS THAN (2) ENGINE = MyISAM,\n"
      "PARTITION t3 VALUES LESS THAN (3) ENGINE = MyISAM,\n"
      "PARTITION t4 VALUES LESS THAN (4) ENGINE = MyISAM,\n"
      "PARTITION t5 VALUES LESS THAN (5) ENGINE = MyISAM,\n"
      "PARTITION t6 VALUES LESS THAN (6) ENGINE = MyISAM,\n"
      "PARTITION t7 VALUES LESS THAN (7) ENGINE = MyISAM,\n"
      "PARTITION t8 VALUES LESS THAN (8) ENGINE = MyISAM,\n"
      "PARTITION t9 VALUES LESS THAN (9) ENGINE = MyISAM,\n"
      "PARTITION t10 VALUES LESS THAN (10) ENGINE = MyISAM,\n"
      "PARTITION t11 VALUES LESS THAN (11) ENGINE = MyISAM,\n"
      "PARTITION t12 VALUES LESS THAN (12) ENGINE = MyISAM,\n"
      "PARTITION t13 VALUES LESS THAN (13) ENGINE = MyISAM,\n"
      "PARTITION t14 VALUES LESS THAN (14) ENGINE = MyISAM,\n"
      "PARTITION t15 VALUES LESS THAN (15) ENGINE = MyISAM,\n"
      "PARTITION t16 VALUES LESS THAN MAXVALUE ENGINE = MyISAM) */;"s);

    MySQLLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    MySQLParser parser(&tokens);

    lexer.serverVersion = 50717;
    lexer.sqlMode = MySQLBaseLexer::SqlMode::AnsiQuotes;
    parser.serverVersion = 50717;
    parser.sqlMode = MySQLBaseLexer::SqlMode::AnsiQuotes;
    parser.setBuildParseTree(true);

    ErrorListener errorListener;
    parser.removeErrorListeners();
    parser.addErrorListener(&errorListener);
    parser.query();
    EXPECT_EQ(0U, errorListener.errorCount) << "Test 20.1";

    auto candidates = getCodeCompletionList(7, 34, "sakila", false, &parser, data->mainSymbols);
    EXPECT_EQ(9U, candidates.size()) << "Test 20.2";
    EXPECT_EQ("comment", candidates[0].second) << "Test 20.3";
    EXPECT_EQ("data directory", candidates[1].second) << "Test 20.4";
    EXPECT_EQ("engine", candidates[2].second) << "Test 20.5";
    EXPECT_EQ("index directory", candidates[3].second) << "Test 20.6";
    EXPECT_EQ("max_rows", candidates[4].second) << "Test 20.7";
    EXPECT_EQ("min_rows", candidates[5].second) << "Test 20.8";
    EXPECT_EQ("nodegroup", candidates[6].second) << "Test 20.9";
    EXPECT_EQ("storage", candidates[7].second) << "Test 20.10";
    EXPECT_EQ("tablespace", candidates[8].second) << "Test 20.11";

    candidates = getCodeCompletionList(7, 44, "sakila", false, &parser, data->mainSymbols);
    EXPECT_EQ(3U, candidates.size()) << "Test 20.12";
    EXPECT_EQ("blackhole", candidates[0].second) << "Test 20.13";
    EXPECT_EQ("innodb", candidates[1].second) << "Test 20.14";
    EXPECT_EQ("myisam", candidates[2].second) << "Test 20.15";
  }
}
