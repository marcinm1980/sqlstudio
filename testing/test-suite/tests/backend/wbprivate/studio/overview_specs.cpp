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

#include "grt.h"
#include "wb_test_helpers.h"
#include "grt_test_helpers.h"

#include "studio/wb_overview.h"
#include "grts/structs.studio.h"
#include "grts/structs.studio.logical.h"
#include "grts/structs.studio.physical.h"

#include "gtest/gtest.h"

#include <fstream>

namespace {

using namespace grt;
using namespace wb;
using namespace bec;

static auto ensure_files_equal(const std::string &test, const char *file, const char *reffile) -> void {
  std::string line, refline;
  std::ifstream ref(reffile);
  std::ifstream f(file);

  EXPECT_TRUE(ref.is_open());
  EXPECT_TRUE(f.is_open());

  while (!ref.eof() && !f.eof()) {
    getline(ref, refline);
    getline(f, line);

    EXPECT_EQ(refline, line);
  }

  EXPECT_TRUE(f.eof() && ref.eof());
}

struct TestData {
  std::unique_ptr<MySqlStudioTester> tester;
};

class WBOverviewTest : public ::testing::Test {
protected:
  TestData *data = new TestData();

  void SetUp() override {
    data->tester.reset(new MySqlStudioTester());
    data->tester->initializeRuntime();
  }

  void TearDown() override {
    delete data;
  }
};

TEST_F(WBOverviewTest, OpenDocument) {
  bool flag = data->tester->wb->open_document("data/studio/test_model1.mwb");
  EXPECT_TRUE(flag);
}

  TEST_F(WBOverviewTest, DumpTreeModel) {
    std::vector<ssize_t> columns;

    columns.push_back(wb::OverviewBE::Label);
    columns.push_back(wb::OverviewBE::NodeType);
    columns.push_back(wb::OverviewBE::Expanded);
    columns.push_back(wb::OverviewBE::Height);
    columns.push_back(wb::OverviewBE::DisplayMode);
    testing::dumpTreeModel("output/overview_test2.txt", (TreeModel *)wb::WBContextUI::get()->get_physical_overview(),
                           columns);

    ensure_files_equal("initial overview state ", "output/overview_test2.txt", "data/be/overview_test2.txt");
  }
}
