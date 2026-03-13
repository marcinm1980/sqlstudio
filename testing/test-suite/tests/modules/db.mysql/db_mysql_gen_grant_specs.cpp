/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "grt_test_helpers.h"
#include "db_mysql_diffsqlgen_grant.h"

#include "gtest/gtest.h"
#include "model_mockup.h"
#include "wb_test_helpers.h"

namespace {
template <class _InIt1, class _InIt2>
  inline void expectContainersEqual(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2) {
    EXPECT_EQ(std::distance(_First1, _Last1), std::distance(_First2, _Last2));

    _InIt1 iter1 = _First1;
    _InIt2 iter2 = _First2;
    for (; iter1 != _Last1; iter1++, iter2++)
      EXPECT_EQ(*iter1, *iter2);
  }

struct DbMysqlGenGrantData {
  std::unique_ptr<MySqlStudioTester> tester;
};


class DB_MySQL_gen_grantTest : public ::testing::Test {
protected:
  static std::unique_ptr<DbMysqlGenGrantData> data;

  static void SetUpTestSuite() {
    data = std::make_unique<DbMysqlGenGrantData>();
    data->tester.reset(new MySqlStudioTester()); 
  }

  static void TearDownTestSuite() {

    data.reset();
  }

};

std::unique_ptr<DbMysqlGenGrantData> DB_MySQL_gen_grantTest::data;

TEST_F(DB_MySQL_gen_grantTest, Grant_select_role) {
  testing::SyntheticMySQLModel model;
  model.catalog->users().remove_all();
  model.catalog->roles().remove_all();

  testing::xRole role("Admin", model);
  testing::xUser user("monty", model);

  testing::addPrivilege(model, role, model.table, "SELECT");

  testing::assignRole(user, role);

  std::list<std::string> actual;
  gen_grant_sql((db_CatalogRef)model.catalog, actual);
  std::string expect[] = {"GRANT SELECT ON TABLE `test_schema`.`t1` TO 'monty'"};

  expectContainersEqual(actual.begin(), actual.end(), expect, expect + UPPER_BOUND(expect));
}

TEST_F(DB_MySQL_gen_grantTest, Grant_insert) {
  testing::SyntheticMySQLModel model;
  model.catalog->users().remove_all();
  model.catalog->roles().remove_all();

  testing::xRole adminRole("Admin", model);
  testing::xRole userRole("User", model);

  testing::addPrivilege(model, adminRole, model.table, "INSERT");
  testing::addPrivilege(model, userRole, model.table, "SELECT");

  testing::xUser user1("monty", model);
  testing::xUser user2("scott", model);

  testing::assignRole(user1, adminRole);
  testing::assignRole(user1, userRole);

  testing::assignRole(user2, userRole);

  std::list<std::string> actual;
  gen_grant_sql((db_CatalogRef)model.catalog, actual);
  std::string expect[] = {
    "GRANT INSERT ON TABLE `test_schema`.`t1` TO 'monty'", "GRANT SELECT ON TABLE `test_schema`.`t1` TO 'monty'",
    "GRANT SELECT ON TABLE `test_schema`.`t1` TO 'scott'",
  };

  expectContainersEqual(actual.begin(), actual.end(), expect, expect + UPPER_BOUND(expect));
}

TEST_F(DB_MySQL_gen_grantTest, Test_when_no_databaseObject_assigned_use_databaseObjectName_instead) {
  testing::SyntheticMySQLModel model;
  model.catalog->users().remove_all();
  model.catalog->roles().remove_all();

  testing::xRole role("Admin", model);
  testing::xUser user("monty", model);

  testing::addPrivilege(model, role, "TABLE", "dummy_obj", "SELECT");

  testing::assignRole(user, role);

  std::list<std::string> actual;
  gen_grant_sql((db_CatalogRef)model.catalog, actual);
  std::string expect[] = {"GRANT SELECT ON TABLE dummy_obj TO 'monty'"};

  expectContainersEqual(actual.begin(), actual.end(), expect, expect + UPPER_BOUND(expect));
}

TEST_F(DB_MySQL_gen_grantTest, Test_parent_role) {
  testing::SyntheticMySQLModel model;
  model.catalog->users().remove_all();
  model.catalog->roles().remove_all();

  testing::xRole role("Admin", model);
  testing::xRole roleBase("Deleter", model);

  role->parentRole(roleBase);
  roleBase->childRoles().insert(role);

  testing::xUser user("monty", model);

  testing::addPrivilege(model, role, model.table, "SELECT");
  testing::addPrivilege(model, roleBase, model.table, "DELETE");

  // note: only one role (and one privilege) assigned here but should derive one more (see expect[])
  testing::assignRole(user, role);

  std::list<std::string> actual;
  gen_grant_sql((db_CatalogRef)model.catalog, actual);
  std::string expect[] = {
    "GRANT DELETE ON TABLE `test_schema`.`t1` TO 'monty'", "GRANT SELECT ON TABLE `test_schema`.`t1` TO 'monty'",
  };

  expectContainersEqual(actual.begin(), actual.end(), expect, expect + UPPER_BOUND(expect));
}

}
