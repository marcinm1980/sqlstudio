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

#include "mforms/code_editor.h"
#include "stub/stub_mforms.h"
#include "wb_test_helpers.h"
#include "gtest/gtest.h"

using namespace mforms;

namespace {

class CodeEditorTest : public ::testing::Test {
protected:
    std::unique_ptr<MySqlStudioTester> tester;

    void SetUp() override {
        tester.reset(new MySqlStudioTester());
    }
};

TEST_F(CodeEditorTest, EditorConfigLoading_MySQLConfigValues) {
    CodeEditorConfig config(mforms::LanguageMySQL);
    ASSERT_GT(config.get_languages().size(), 0U) << "No language nodes found";

    // Keywords.
    std::map<std::string, std::string> keywords = config.get_keywords();
    ASSERT_GT(keywords.size(), 0U) << "Couldn't read keywords";
    ASSERT_NE(keywords.find("Keywords"), keywords.end()) << "Keyword list missing";
    ASSERT_FALSE(keywords["Keywords"].empty()) << "Keyword list empty";
    ASSERT_NE(keywords.find("Procedure keywords"), keywords.end()) << "Procedure keyword list missing";
    ASSERT_FALSE(keywords["Procedure keywords"].empty()) << "Procedure keyword list empty";
    ASSERT_NE(keywords.find("User Keywords 1"), keywords.end()) << "User keyword list 1 missing";
    ASSERT_FALSE(keywords["User Keywords 1"].empty()) << "User keyword list 1 empty";

    // Properties.
    std::map<std::string, std::string> properties = config.get_properties();
    ASSERT_FALSE(properties.empty()) << "Couldn't read properties";

    // Settings.
    std::map<std::string, std::string> settings = config.get_settings();
    ASSERT_FALSE(settings.empty()) << "Couldn't read settings";

    // Styles.
    std::map<int, std::map<std::string, std::string> > styles = config.get_styles();
    ASSERT_FALSE(styles.empty()) << "Couldn't read styles";

    // Pick some entries, just to check sub map.
    std::map<std::string, std::string> &values = styles[22]; // SCE_MYSQL_KEYWORD
    ASSERT_FALSE(values.empty()) << "Wrong number of style values found";
    ASSERT_TRUE(values["fore-color"].empty()) << "Old style color entry found";
    ASSERT_FALSE(values["bold"].empty()) << "Missing bold style for MySQL keywords";

    values = styles[22]; // SCE_MYSQL_PLACEHOLDER
    ASSERT_GE(values.size(), 5U) << "Wrong number of style values found";
    ASSERT_TRUE(values["fore-color"].empty()) << "Old style color entry found";
    ASSERT_FALSE(values["fore-color-light"].empty()) << "Missing fore-color-light";
    ASSERT_FALSE(values["fore-color-dark"].empty()) << "Missing fore-color-dark";
    ASSERT_FALSE(values["back-color-light"].empty()) << "Missing back-color-light";
    ASSERT_FALSE(values["back-color-dark"].empty()) << "Missing back-color-dark";
    ASSERT_FALSE(values["bold"].empty()) << "Missing bold style for MySQL keywords";
}

TEST_F(CodeEditorTest, PythonConfigValues) {
    CodeEditorConfig config(mforms::LanguagePython);
    ASSERT_FALSE(config.get_languages().empty()) << "No language nodes found";

    // Keywords.
    std::map<std::string, std::string> keywords = config.get_keywords();
    ASSERT_FALSE(keywords.empty()) << "Couldn't read keywords";
    ASSERT_NE(keywords.find("Keywords"), keywords.end()) << "Keyword list missing";
    ASSERT_FALSE(keywords["Keywords"].empty()) << "Keyword list empty";
    ASSERT_NE(keywords["Keywords"].find("continue"), std::string::npos) << "Python keyword \"continue\" not in keyword list";

    // Properties.
    std::map<std::string, std::string> properties = config.get_properties();
    ASSERT_FALSE(properties.empty()) << "Couldn't read properties";

    // Settings.
    std::map<std::string, std::string> settings = config.get_settings();
    ASSERT_FALSE(settings.empty()) << "Couldn't read settings";

    // Styles.
    std::map<int, std::map<std::string, std::string> > styles = config.get_styles();
    ASSERT_FALSE(styles.empty()) << "Couldn't read styles";

    // Pick some entries, just to check sub map.
    std::map<std::string, std::string> values = styles[8]; // Python class name.
    ASSERT_GT(values.size(), 1U) << "Invalid style set for Python class names";
    ASSERT_TRUE(values["fore-color"].empty()) << "Old style color entry found";
    ASSERT_FALSE(values["fore-color-light"].empty()) << "Missing fore color style for Python class names";
    ASSERT_FALSE(values["bold"].empty()) << "Missing bold style for Python class names";
}

} // namespace

