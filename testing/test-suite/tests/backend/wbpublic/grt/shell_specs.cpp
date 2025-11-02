/*
 * Copyright (c) 2011, 2019, 2025, Oracle and/or its affiliates. All rights reserved.
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

#include "grt/grt_shell.h"
#include "grt/grt_dispatcher.h"
#include "grt/grt_manager.h"
#include "wb_test_helpers.h"
#include "gtest/gtest.h"
#include "casmine.h"

using namespace grt;
using namespace bec;

extern void register_all_metaclasses();

namespace {

class GrtShellBackendTest : public ::testing::Test {
protected:
  GRTDispatcher::Ref dispatcher;

  void SetUp() override {
    register_all_metaclasses();
    grt::GRT::get()->scan_metaclasses_in("../../res/grt/");
    grt::GRT::get()->end_loading_metaclasses();
    dispatcher = GRTDispatcher::create_dispatcher(false, true);
  }

  void TearDown() override {
    dispatcher->shutdown();
    dispatcher.reset();
    MySqlStudioTester::reinitGRT();
  }
};

TEST_F(GrtShellBackendTest, TestHistoryNavigation) {
  bool flag;
  std::string line;

  ShellBE *shell = new ShellBE(dispatcher);
  shell->set_saves_history(10);
  shell->save_history_line("line1");
  flag = shell->previous_history_line("newline", line);
  EXPECT_TRUE(flag) << "previous line";
  EXPECT_EQ("line1", line) << "previous line value";


  flag = shell->next_history_line(line);
  EXPECT_TRUE(flag) << "next line";
  EXPECT_EQ("newline", line) << "next line value";

  shell->save_history_line("line2");
  shell->save_history_line("line3");

  flag = shell->next_history_line(line);
  EXPECT_FALSE(flag) << "next";

  flag = shell->previous_history_line("newline", line);
  EXPECT_TRUE(flag) << "previous line";
  EXPECT_EQ("line3", line) << "previous line value";

  flag = shell->previous_history_line("line3", line);
  EXPECT_TRUE(flag) << "previous line";
  EXPECT_EQ("line2", line) << "previous line value";


  flag = shell->previous_history_line("line2", line);
  EXPECT_TRUE(flag) << "previous line";
  EXPECT_EQ("line1", line) << "previous line value";

  flag = shell->previous_history_line("line1", line);
  EXPECT_FALSE(flag) << "prevous line";

  flag = shell->next_history_line(line);
  EXPECT_TRUE(flag) << "next line";
  EXPECT_EQ("line2", line) << "next line value";

  flag = shell->next_history_line(line);
  EXPECT_TRUE(flag) << "next line";
  EXPECT_EQ("line3", line) << "next line value";

  flag = shell->next_history_line(line);
  EXPECT_TRUE(flag);
  EXPECT_EQ("newline", line) << "next line value";

  flag = shell->next_history_line(line);
  EXPECT_FALSE(flag) << "previous line";

  flag = shell->previous_history_line("newline", line);
  EXPECT_TRUE(flag) << "previous line";
  EXPECT_EQ("line3", line) << "previous line value";

  delete shell;
}

TEST_F(GrtShellBackendTest, AdditionalHistoryLinesTest) {
  bool flag;
  ShellBE *shell = new ShellBE(dispatcher);

  shell->set_saves_history(10);
  shell->set_save_directory(casmine::CasmineContext::get()->outputDir());

  shell->save_history_line("line1");
  shell->save_history_line("line2");
  shell->save_history_line("line3.1\nline3.2\n\nline3.3");
  shell->save_history_line("line4");
  shell->save_history_line("line5");

  shell->set_snippet_data("hello world\nsnippet line this");
  shell->store_state();

  delete shell;

  shell = new ShellBE(dispatcher);
  shell->set_saves_history(10);
  shell->set_save_directory(casmine::CasmineContext::get()->outputDir());
  shell->restore_state();

  std::string line;

  flag = shell->previous_history_line("newline", line);
  EXPECT_TRUE(flag) << "get restored line";

  EXPECT_EQ("line5", line) << "last line";

  flag = shell->previous_history_line(line, line);
  EXPECT_TRUE(flag) << "prev after save";
  EXPECT_EQ("line4", line) << "last line ";

  flag = shell->previous_history_line(line, line);
  EXPECT_TRUE(flag) << "prev after save";
  EXPECT_EQ("line3.1\nline3.2\n\nline3.3", line) << "last line";

  line = shell->get_snippet_data();
  EXPECT_EQ("hello world\nsnippet line this", line) << "snippet";
  delete shell;
}

}

