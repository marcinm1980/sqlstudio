/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "gtest/gtest.h"

#include "base/config_file.h"

#include "studio/wb_overview.h"
#include "grts/structs.studio.h"
#include "grts/structs.studio.logical.h"
#include "grts/structs.studio.physical.h"

namespace {

using namespace base;

// Base library config file handling tests
class ConfigFileTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(ConfigFileTest, NoConfigFile_CreatesDefaultSection) {
  ConfigurationFile file("", AutoCreateNothing);
  EXPECT_EQ(file.section_count(), 1);
  EXPECT_TRUE(file.has_section(""));
  EXPECT_EQ(file.key_count(), 0);
}

TEST_F(ConfigFileTest, EmptyConfigFile_NoKeysNoFun) {
  ConfigurationFile file("../data/base/my-1.ini", AutoCreateNothing);
  EXPECT_EQ(file.section_count(), 1);
  EXPECT_TRUE(file.has_section(""));
  EXPECT_EQ(file.key_count(), 0);
}

TEST_F(ConfigFileTest, StandardIniFile_WindowsNoIncludesOrSpecialties) {
    ConfigurationFile file("../data/base/my-2.ini", AutoCreateNothing);
    EXPECT_EQ(file.section_count(), 4);
    EXPECT_TRUE(file.has_section("client"));
    EXPECT_TRUE(file.has_section("MYSQLd"));
    EXPECT_TRUE(file.has_section("mYsQl"));
    EXPECT_FALSE(file.has_section("mysqlx"));
    EXPECT_EQ(file.key_count(), 32);

    file.clear();
    EXPECT_EQ(file.section_count(), 1);
    EXPECT_TRUE(file.has_section(""));
    EXPECT_EQ(file.key_count(), 0);

    file.create_section("mine", "A manually added section");
    EXPECT_EQ(file.section_count(), 2);
    EXPECT_TRUE(file.has_section("mine"));

    file.load("../data/base/my-2.ini");
    EXPECT_EQ(file.section_count(), 5);
    EXPECT_TRUE(file.has_section("client"));
    EXPECT_TRUE(file.has_section("MYSQLd"));
    EXPECT_TRUE(file.has_section("mYsQl"));
    EXPECT_TRUE(file.has_section("mine")); // Must still be there.
    EXPECT_FALSE(file.has_section("mysqlx"));
    EXPECT_EQ(file.key_count(), 32);

    // Save and reload. We cannot directly compare the original file and its generated copy
    // as there are differences in whitespaces. But we can compare content.
    file.save("../data/base/my-2-copy.ini");
    ConfigurationFile file_copy("../data/base/my-2-copy.ini", AutoCreateNothing);
    EXPECT_EQ(file_copy.section_count(), 5);
    EXPECT_TRUE(file_copy.has_section("client"));
    EXPECT_TRUE(file_copy.has_section("MYSQLd"));
    EXPECT_TRUE(file_copy.has_section("mYsQl"));
    EXPECT_EQ(file_copy.key_count(), 32);
}

TEST_F(ConfigFileTest, DetailedCheckOfLoadedContent) {
    ConfigurationFile file("../data/base/my-2.ini", AutoCreateNothing);

    EXPECT_EQ(file.get_int("port", "client"), 3306);

    EXPECT_EQ(file.get_value("default-character-set", "mysql"), "utf8");

    EXPECT_EQ(file.get_value("basedir", "mysqld"), "\"C:/Program Files/MySQL/MySQL Server 5.5/\"");
    EXPECT_EQ(file.get_value("datadir", "mysqld"), "\"C:/ProgramData/MySQL/MySQL Server 5.5/Data/\"");
    EXPECT_EQ(file.get_value("character-set-server", "mysqld"), "utf8");
    EXPECT_EQ(file.get_value("default-storage-engine", "mysqld"), "INNODB");
    EXPECT_EQ(file.get_value("sql-mode", "mysqld"),
      "\"STRICT_TRANS_TABLES,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION\"");
    EXPECT_EQ(file.get_int("max_connections", "mysqld"), 100);

    EXPECT_EQ(file.get_int("query_cache_size", "mysqld"), 0);
    EXPECT_FALSE(file.get_bool("query_cache_size", "mysqld"));

    EXPECT_EQ(file.get_int("table_cache", "mysqld"), 256);

    EXPECT_EQ(file.get_value("tmp_table_size", "mysqld"), "35M");
    EXPECT_EQ(file.get_int("tmp_table_size", "mysqld"), 35 * 1024 * 1024);

    EXPECT_EQ(file.get_int("thread_cache_size", "mysqld"), 8);

    EXPECT_EQ(file.get_value("myisam_max_sort_file_size", "mysqld"), "100G");
    EXPECT_EQ(file.get_float("myisam_max_sort_file_size", "mysqld"), 100.0 * 1024 * 1024 * 1024);

    EXPECT_EQ(file.get_value("myisam_sort_buffer_size", "mysqld"), "69M");
    EXPECT_EQ(file.get_float("myisam_sort_buffer_size", "mysqld"), 69.0 * 1024 * 1024);

    EXPECT_EQ(file.get_value("key_buffer_size", "mysqld"), "55M");
    EXPECT_EQ(file.get_int("key_buffer_size", "mysqld"), 55 * 1024 * 1024);

    EXPECT_EQ(file.get_value("read_buffer_size", "mysqld"), "64K");
    EXPECT_EQ(file.get_float("read_buffer_size", "mysqld"), 64.0 * 1024);

    EXPECT_EQ(file.get_value("read_rnd_buffer_size", "mysqld"), "256K");
    EXPECT_EQ(file.get_float("read_rnd_buffer_size", "mysqld"), 256.0 * 1024);
    EXPECT_TRUE(file.get_bool("read_rnd_buffer_size", "mysqld"));

    EXPECT_EQ(file.get_value("sort_buffer_size", "mysqld"), "256K");
    EXPECT_EQ(file.get_int("sort_buffer_size", "mysqld"), 256 * 1024);

    EXPECT_EQ(file.get_value("innodb_additional_mem_pool_size", "mysqld"), "3M");
    EXPECT_EQ(file.get_int("innodb_additional_mem_pool_size", "mysqld"), 3 * 1024 * 1024);

    EXPECT_EQ(file.get_int("innodb_flush_log_at_trx_commit", "mysqld"), 1);

    EXPECT_EQ(file.get_value("innodb_log_buffer_size", "mysqld"), "2M");
    EXPECT_EQ(file.get_int("innodb_log_buffer_size", "mysqld"), 2 * 1024 * 1024);

    EXPECT_EQ(file.get_value("innodb_buffer_pool_size", "mysqld"), "107M");
    EXPECT_EQ(file.get_int("innodb_buffer_pool_size", "mysqld"), 107 * 1024 * 1024);

    EXPECT_EQ(file.get_value("innodb_log_file_size", "mysqld"), "54M");
    EXPECT_EQ(file.get_int("innodb_log_file_size", "mysqld"), 54 * 1024 * 1024);

    EXPECT_EQ(file.get_int("innodb_thread_concurrency", "mysqld"), 8);

    EXPECT_TRUE(file.has_key("slow-query-log", "mysqld"));
    EXPECT_EQ(file.get_value("slow-query-log", "mysqld"), ""); // No value for this key.

    EXPECT_EQ(file.get_value("log-output", "mysqld"), "TABLE");
    EXPECT_TRUE(file.has_key("log-slow-admin-statements", "mysqld"));
    EXPECT_EQ(file.get_int("long_query_time", "mysqld"), 10);
    EXPECT_TRUE(file.has_key("log-slow-slave-statements", "mysqld"));

    EXPECT_TRUE(file.get_bool("dummy-with-bool", "mysqld"));
    EXPECT_TRUE(file.has_key("general-log", "mysqld"));
}

TEST_F(ConfigFileTest, SpecificTestsForIncludeAndIncludedirEntries) {
    ConfigurationFile file("../data/base/my-3.ini", AutoCreateNothing);
    std::vector<std::string> includes = file.get_includes("");
    EXPECT_EQ(includes.size(), 1U);
    EXPECT_EQ(includes[0], "C:\\\\test.cnf");

    includes = file.get_includes("mysqld");
    EXPECT_EQ(includes.size(), 2U);
    EXPECT_EQ(includes[0], "C:\\\\test.cnf");
    EXPECT_EQ(includes[1], "C:/config-files/");

    file.clear_includes("xxx");
    includes = file.get_includes("mysqld");
    EXPECT_EQ(includes.size(), 2U);

    file.clear_includes("mysqld");
    includes = file.get_includes("mysqld");
    EXPECT_EQ(includes.size(), 0U);

    file.add_include("mysqld", "abc");
    file.add_include_dir("mysqld", "def");
    includes = file.get_includes("mysqld");
    EXPECT_EQ(includes.size(), 2U);
    EXPECT_EQ(includes[0], "abc");
    EXPECT_EQ(includes[1], "def");
}

//----------------------------------------------------------------------------------------------------------------------

}
