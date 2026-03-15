/*
 * Copyright (c) 2019, 2025, Oracle and/or its affiliates. All rights reserved.
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

#include "grt/grt_dispatcher.h"
#include "grt/grt_manager.h"
#include "wb_test_helpers.h"

#include "gtest/gtest.h"

namespace {

using namespace grt;
using namespace bec;

static auto finished(grt::ValueRef result, bool *flag) -> void {
  *flag = true;
}

static auto finished_with_wait(grt::ValueRef result, bool *flag) -> void {
  g_usleep(2000000);
  *flag = true;
}

static auto normal_test_function() -> grt::ValueRef {
  return grt::IntegerRef(123);
}

class GRTRequestDispatcherTest : public ::testing::Test {
protected:
  GRTDispatcher::Ref dispatcher;

  void SetUp() override {
    // No need to initialize Python for this test. No need for a module path either.
    grt::GRT::get(); // make sure grt is initialized before Dispatcher is created
    dispatcher = GRTDispatcher::create_dispatcher(false, true);
    dispatcher->start();
  }

  void TearDown() override {
    dispatcher->shutdown();
    dispatcher.reset();
  }
};

TEST_F(GRTRequestDispatcherTest, TestingCallbacks) {
  grt::ValueRef result;
  bool finish_called = false;

  bec::GRTTask::Ref task = GRTTask::create_task("test", dispatcher, std::bind(normal_test_function));
  task->signal_finished()->connect(std::bind(&finished, std::placeholders::_1, &finish_called));

  result = dispatcher->add_task_and_wait(task);

  EXPECT_TRUE(result.is_valid());
  EXPECT_EQ(grt::IntegerType, result.type());
  EXPECT_EQ(123, *grt::IntegerRef::cast_from(result));

  EXPECT_TRUE(finish_called);

  finish_called = false;
  task = GRTTask::create_task("test", dispatcher, std::bind(normal_test_function));
  task->signal_finished()->connect(std::bind(&finished_with_wait, std::placeholders::_1, &finish_called));

  result = dispatcher->add_task_and_wait(task);

  EXPECT_TRUE(finish_called);
}

}

