/*
 * Copyright (c) 2018, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include <gtest/gtest.h>

#include "data_types.h"

namespace {

// Command line parser test class
class CommandLineParserTest : public ::testing::Test {
 protected:
  bool callbackTriggered = false;
  
  void SetUp() override {
    callbackTriggered = false;
  }
};

TEST_F(CommandLineParserTest, GeneralArgumentHandling) {
    std::vector<std::string> args({"--test-argument-value-space", "argument value", "--test-argument-value-equals",
      "=sample", "some/file/path", "--test-boolean"});
    dataTypes::OptionsList opts;
    opts.addEntry(dataTypes::OptionEntry(dataTypes::OptionArgumentType::OptionArgumentFilename, 0, "test-argument-value-space",
                                         "Test passing argument for the value with space", nullptr, "name"));
    opts.addEntry(dataTypes::OptionEntry(dataTypes::OptionArgumentType::OptionArgumentFilename, 0, "test-argument-value-equals",
                                         "Test passing argument for the value with equals", nullptr, "name"));
    opts.addEntry(
      dataTypes::OptionEntry(dataTypes::OptionArgumentType::OptionArgumentLogical, 0, "test-boolean", "Test boolean value")
    );

    int retVal = 0;
    EXPECT_TRUE(opts.parse(args, retVal));
    EXPECT_TRUE(opts.getEntry("test-boolean")->value.logicalValue);
    EXPECT_EQ(opts.getEntry("test-argument-value-equals")->value.textValue, "=sample");
    EXPECT_EQ(opts.getEntry("test-argument-value-space")->value.textValue, "argument value");
    EXPECT_EQ(opts.pathArgs.size(), 1U);
    EXPECT_EQ(opts.pathArgs[0], "some/file/path");
}

TEST_F(CommandLineParserTest, UnknownArgumentHandling) {
    std::vector<std::string> args({"--test-return-value", "--test-callback", "--test-argument-value-space",
      "argument value", "--test-argument-value-equals", "=sample", "some/file/path",
      "--test-boolean"});
    dataTypes::OptionsList opts;
    opts.addEntry(
      dataTypes::OptionEntry(dataTypes::OptionArgumentType::OptionArgumentLogical, 0, "test-boolean", "Test boolean value")
    );

    EXPECT_THROW({
      int retVal = 0;
      opts.parse(args, retVal);
    }, std::runtime_error);
}

TEST_F(CommandLineParserTest, ArgumentCallbackTrigger) {
    std::vector<std::string> args({"--test-callback"});
    dataTypes::OptionsList opts;
    opts.addEntry(dataTypes::OptionEntry(dataTypes::OptionArgumentType::OptionArgumentLogical, 0, "test-callback",
                                         "Test callback trigger", [&](const dataTypes::OptionEntry &entry, int *retval) {
                                           callbackTriggered = true;
                                           *retval = 10;
                                           return false;
                                         }));

    int retVal = 0;
    EXPECT_FALSE(opts.parse(args, retVal));
    EXPECT_TRUE(callbackTriggered);
    EXPECT_EQ(retVal, 10);
}

}
