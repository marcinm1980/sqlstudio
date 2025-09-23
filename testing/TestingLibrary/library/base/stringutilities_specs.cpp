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

#include "base/sqlstring.h"

#include "gtest/gtest.h"

namespace {

// String utilities test class
class StringUtilitiesTest : public ::testing::Test {
 protected:
  std::string long_random_string; // Content doesn't matter. There must be no crash using it.

  void SetUp() override {
    for (int i = 0; i < 1000; i++) {
      long_random_string += ' ' + rand() % 94; // Visible characters after space

      if (rand() > RAND_MAX / 2)
        long_random_string += "\xE3\x8A\xA8"; // Valid Unicode character.
      if (i == 500)
        long_random_string += "\xE3\x8A\xA8"; // Ensure it is there at least once.
    }
    long_random_string.erase(std::remove(long_random_string.begin(), long_random_string.end(), 0x7f),
                             long_random_string.end()); // 0x7F is a special character that we use for tests
  }
};

TEST_F(StringUtilitiesTest, QuoteIdentifier) {
  EXPECT_EQ(base::quote_identifier("first_test", '`'), "`first_test`");
  EXPECT_EQ(base::quote_identifier("second_test", '\"'), "\"second_test\"");
  EXPECT_EQ(base::quote_identifier("", '\"'), "\"\"");
  // UTF-8 encoded: CIRCLED IDEOGRAPH RIGHT
  EXPECT_EQ(base::quote_identifier("Unicode \xE3\x8A\xA8", '%'), "%Unicode \xE3\x8A\xA8%");
}

TEST_F(StringUtilitiesTest, QuoteIdentifierIfNeeded) {
  EXPECT_EQ(base::quoteIdentifierIfNeeded("first_test", '`', base::MySQLVersion::MySQL80), "first_test");
  EXPECT_EQ(base::quoteIdentifierIfNeeded("second_test", '\"', base::MySQLVersion::MySQL80), "second_test");

  // UTF-8 encoded: CIRCLED IDEOGRAPH RIGHT
  EXPECT_EQ(base::quoteIdentifierIfNeeded("Unicode\xE3\x8A\xA8", '%', base::MySQLVersion::MySQL80),
    "Unicode\xE3\x8A\xA8");
  EXPECT_EQ(base::quoteIdentifierIfNeeded("test.case", '$', base::MySQLVersion::MySQL80), "$test.case$");

  // Note: currently there is no support to check if the given string contains the quote char already.
  EXPECT_EQ(base::quoteIdentifierIfNeeded("test$case", '$', base::MySQLVersion::MySQL80), "test$case");
  EXPECT_EQ(base::quoteIdentifierIfNeeded(".test$case", '$', base::MySQLVersion::MySQL80), "$.test$case$");
  EXPECT_EQ(base::quoteIdentifierIfNeeded("test-case", '`', base::MySQLVersion::MySQL80), "`test-case`");

  // Identifiers consisting only of digits cannot be distinguished from normal numbers
  // so they must be quoted.
  EXPECT_EQ(base::quoteIdentifierIfNeeded("12345", '`', base::MySQLVersion::MySQL80), "`12345`");
}

TEST_F(StringUtilitiesTest, UnquoteIdentifier) {
  std::string test = "\"first_test\"";
  std::string test_result = base::unquote_identifier(test);
  EXPECT_EQ(test_result, "first_test");

  test = "`second_test`";
  test_result = base::unquote_identifier(test);
  EXPECT_EQ(test_result, "second_test");
}

TEST_F(StringUtilitiesTest, SplitTokenList) {
  std::vector<std::string> empty = { "" };
  std::vector<std::string> empty2 = { "", "" };
  std::vector<std::string> empty3 = { "", "", "" };
  std::vector<std::string> null_empty = { "NULL", "" };
  std::vector<std::string> a = { "a" };
  std::vector<std::string> a_empty1 = { "a", "" };
  std::vector<std::string> a_empty2 = { "a", "", "" };
  std::vector<std::string> ab_empty1 = { "a", "b", "" };
  std::vector<std::string> ab_empty2 = { "a", "", "b" };
  std::vector<std::string> ab_empty3 = { "", "a", "b" };
  std::vector<std::string> null_null = { "NULL", "NULL" };
  std::vector<std::string> emptys_null = { "''", "NULL" };
  std::vector<std::string> ab_null = { "'a,b'", "NULL" };
  std::vector<std::string> ab_xxx = { "'a,b'", "\"x\\xx\"", "'fo''bar'" };

  EXPECT_EQ(base::split_token_list("", ','), empty);
  EXPECT_EQ(base::split_token_list(",", ','), empty2);
  EXPECT_EQ(base::split_token_list(" ,", ','), empty2);
  EXPECT_EQ(base::split_token_list(", ", ','), empty2);
  EXPECT_EQ(base::split_token_list(",,", ','), empty3);
  EXPECT_EQ(base::split_token_list("NULL,", ','), null_empty);
  EXPECT_EQ(base::split_token_list("a", ','), a);
  EXPECT_EQ(base::split_token_list("a,", ','), a_empty1);
  EXPECT_EQ(base::split_token_list("a,,", ','), a_empty2);
  EXPECT_EQ(base::split_token_list("a,b,", ','), ab_empty1);
  EXPECT_EQ(base::split_token_list("a,,b", ','), ab_empty2);
  EXPECT_EQ(base::split_token_list(",a,b", ','), ab_empty3);
  EXPECT_EQ(base::split_token_list("NULL,", ','), null_empty);
  EXPECT_EQ(base::split_token_list("NULL,NULL", ','), null_null);
  EXPECT_EQ(base::split_token_list("'',NULL", ','), emptys_null);
  EXPECT_EQ(base::split_token_list("'a,b',NULL", ','), ab_null);
  EXPECT_EQ(base::split_token_list("'a,b' , \"x\\xx\",'fo''bar'   ", ','), ab_xxx);
}

TEST_F(StringUtilitiesTest, SplitBySet) {
  std::vector<std::string> input;
  EXPECT_EQ(base::split_by_set("", ""), input);
  EXPECT_EQ(base::split_by_set("", " "), input);
  EXPECT_EQ(base::split_by_set("", long_random_string), input);

  std::vector<std::string> input2 = { long_random_string.c_str() };
  EXPECT_EQ(base::split_by_set(long_random_string, ""), input2);
  EXPECT_GT(base::split_by_set(long_random_string, "\xA8").size(), 1U); // Works only because our implementation is not utf-8 aware.

  std::vector<std::string> input3 = { "Lorem", "ipsum", "dolor", "sit", "amet." };
  EXPECT_EQ(base::split_by_set("Lorem ipsum dolor sit amet.", " "), input3);
  std::vector<std::string> input4 = { "Lorem", "ipsum", "dolor sit amet." };
  EXPECT_EQ(base::split_by_set("Lorem ipsum dolor sit amet.", " ", 2), input4);

  std::vector<std::string> input5 = { "\"Lorem\"", "\"ipsum\"", "\"dolor\"", "\"sit\"", "\"amet\"" };
  EXPECT_EQ(base::split_by_set("\"Lorem\"\t\"ipsum\"\t\"dolor\"\t\"sit\"\t\"amet\"", "\t"), input5);

  std::vector<std::string> input6 = { "\"Lorem\"", "\"ipsum\"", "\"dolor\"", "\"sit\"", "\"amet\"" };
  EXPECT_EQ(base::split_by_set("\"Lorem\"\t\"ipsum\"\n\"dolor\"\t\"sit\"\n\"amet\"", " \t\n"), input6);

  std::vector<std::string> input7 = { "\"Lorem\"", "\"ip", "sum\"", "\"dolor\"", "\"sit\"", "\"amet\"" };
  EXPECT_EQ(base::split_by_set("\"Lorem\"\t\"ip sum\"\n\"dolor\"\t\"sit\"\n\"amet\"", " \t\n"), input7);

  std::vector<std::string> input8 = { "", "Lorem", "", " ", "ipsum", "", " ", "dolor", "", " ", "sit", "", " ", "amet", "" };
  EXPECT_EQ(base::split_by_set("\"Lorem\", \"ipsum\", \"dolor\", \"sit\", \"amet\"", ",\""), input8);
  EXPECT_EQ(base::split_by_set("\"Lorem\", \"ipsum\", \"dolor\", \"sit\", \"amet\"", ",\"", 100), input8);
  std::vector<std::string> input9 = { "", "Lorem", "", " ", "ipsum", "", " ", "dolor\", \"sit\", \"amet\"" };
  EXPECT_EQ(base::split_by_set("\"Lorem\", \"ipsum\", \"dolor\", \"sit\", \"amet\"", ",\"", 7), input9);
}

TEST_F(StringUtilitiesTest, TrimRightTrimLeftTrim) {
  EXPECT_EQ(base::trim_left(""), "");
  EXPECT_EQ(base::trim_left("                                       "), "");
  EXPECT_EQ(base::trim_left("           \n\t\t\t\t      a"), "a");
  EXPECT_EQ(base::trim_left("a           \n\t\t\t\t      "), "a           \n\t\t\t\t      ");
  EXPECT_EQ(base::trim_left("", long_random_string), "");

  EXPECT_EQ(base::trim_left("\xE3\x8A\xA8\xE3\x8A\xA8\x7F", long_random_string), "\x7F");
  EXPECT_EQ(base::trim_left("\t\t\tLorem ipsum dolor sit amet\n\n\n"), "Lorem ipsum dolor sit amet\n\n\n");
  EXPECT_EQ(base::trim_left("\t\t\tLorem ipsum dolor sit amet\n\n\n", "L"), "\t\t\tLorem ipsum dolor sit amet\n\n\n");
  EXPECT_EQ(base::trim_left("\t\t\tLorem ipsum dolor sit amet\n\n\n", "\t\t\tL"), "orem ipsum dolor sit amet\n\n\n");

  EXPECT_EQ(base::trim_right(""), "");
  EXPECT_EQ(base::trim_right("                                       "), "");
  EXPECT_EQ(base::trim_right("           \n\t\t\t\t      a"), "           \n\t\t\t\t      a");
  EXPECT_EQ(base::trim_right("a           \n\t\t\t\t      "), "a");
  EXPECT_EQ(base::trim_right("", long_random_string), "");

  EXPECT_EQ(base::trim_right("\x7F\xE3\x8A\xA8\xE3\x8A\xA8", long_random_string), "\x7F");
  EXPECT_EQ(base::trim_right("\t\t\tLorem ipsum dolor sit amet\n\n\n"), "\t\t\tLorem ipsum dolor sit amet");
  EXPECT_EQ(base::trim_right("\t\t\tLorem ipsum dolor sit amet\n\n\n", "L"), "\t\t\tLorem ipsum dolor sit amet\n\n\n");
  EXPECT_EQ(base::trim_right("\t\t\tLorem ipsum dolor sit amet\n\n\n", "\n\n\nt"), "\t\t\tLorem ipsum dolor sit ame");

  EXPECT_EQ(base::trim(""), "");
  EXPECT_EQ(base::trim("                                       "), "");
  EXPECT_EQ(base::trim("           \n\t\t\t\t      a"), "a");
  EXPECT_EQ(base::trim("a           \n\t\t\t\t      "), "a");
  EXPECT_EQ(base::trim("", long_random_string), "");

  EXPECT_EQ(base::trim("\xE3\x8A\xA8\xE3\x8A\xA8\x7F\xE3\x8A\xA8\xE3\x8A\xA8", long_random_string), "\x7F");
  EXPECT_EQ(base::trim("\t\t\tLorem ipsum dolor sit amet\n\n\n"), "Lorem ipsum dolor sit amet");
  EXPECT_EQ(base::trim("\t\t\tLorem ipsum dolor sit amet\n\n\n", "L"), "\t\t\tLorem ipsum dolor sit amet\n\n\n");
  EXPECT_EQ(base::trim("\t\t\tLorem ipsum dolor sit amet\n\n\n", "\n\n\t\t\ttL"), "orem ipsum dolor sit ame");
}

TEST_F(StringUtilitiesTest, UnescapeEscapeSequenceHandling) {
  std::string test_result = base::unescape_sql_string("", '`');
  EXPECT_EQ(test_result, "");

  test_result = base::unescape_sql_string("lorem ipsum dolor sit amet", '"');
  EXPECT_EQ(test_result, "lorem ipsum dolor sit amet");

  test_result = base::unescape_sql_string("lorem ipsum dolor`` sit amet", '"');
  EXPECT_EQ(test_result, "lorem ipsum dolor`` sit amet");

  test_result = base::unescape_sql_string("lorem ipsum \"\"dolor sit amet", '"');
  EXPECT_EQ(test_result, "lorem ipsum \"dolor sit amet");

  test_result = base::unescape_sql_string("lorem \"\"\"\"ipsum \"\"dolor\"\" sit amet", '"');
  EXPECT_EQ(test_result, "lorem \"\"ipsum \"dolor\" sit amet");

  test_result = base::unescape_sql_string("lorem \\\"\\\"ipsum\"\" \\\\dolor sit amet", '"');
  EXPECT_EQ(test_result, "lorem \"\"ipsum\" \\dolor sit amet");

  test_result = base::unescape_sql_string("lorem \\\"\\\"ipsum\"\" \\\\dolor sit amet", '\'');
  EXPECT_EQ(test_result, "lorem \"\"ipsum\"\" \\dolor sit amet");

  // Embedded 0 is more difficult to test due to limitations when comparing strings. So we do this
  // in a separate test.
  test_result = base::unescape_sql_string("lorem\\n ip\\t\\rsum dolor\\b sit \\Zamet", '"');
  EXPECT_EQ(test_result, "lorem\n ip\t\rsum dolor\b sit \032amet");

  test_result = base::unescape_sql_string("lorem ipsum \\zd\\a\\olor sit amet", '"');
  EXPECT_EQ(test_result, "lorem ipsum zdaolor sit amet");

  test_result = base::unescape_sql_string("\\0\\n\\t\\r\\b\\Z", '"');
  EXPECT_TRUE(test_result[0] == 0 && test_result[1] == 10 && test_result[2] == 9 &&
         test_result[3] == 13 && test_result[4] == '\b' &&
         test_result[5] == 26);

  std::string long_string;
  long_string.resize(2000);
  std::fill(long_string.begin(), long_string.end(), '`');
  test_result = base::unescape_sql_string(long_string, '`');
  EXPECT_EQ(test_result, long_string.substr(0, 1000));
}

TEST_F(StringUtilitiesTest, TextReflow) {
    std::string content1 = "11111111 22222 3333 444444 555555555 666666 77777777 88888 999999999 00000000";
    std::string content2 =
      "\xC3\x81\xC3\x81\xC3\x81\xC3\x81\xC3\x81\xC3\x81\xC3\x81\xC3\x81 "
      "\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89 "
      "\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D "
      "\xC3\x93\xC3\x93\xC3\x93\xC3\x93\xC3\x93\xC3\x93\xC3\x93\xC3\x93 "
      "\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A";
    std::string content3 =
      "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
      "11111111111111111111111111";
    std::string content4 = "aaaaa bbbbb cccc dddd \xAA\xBB\xCC\xDD";
    std::string content5 = "aaaa bbbb cccc dddd eeee";
    //   模板名，使用英文，保证唯一性。格式建议：“类型_动作”，如“blog_add”或“credit_blog_add”
    //   Template name, in English, to ensure uniqueness. Format advice: "type _ action," such as "blog_add" or
    //   "credit_blog_add"
    unsigned char content6[] = {
      0x20, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x3a, 0x20, 0xe6, 0xa8, 0xa1, 0xe6, 0x9d, 0xbf, // 10
      0xe5, 0x90, 0x8d, 0xef, 0xbc, 0x8c, 0xe4, 0xbd, 0xbf, 0xe7, 0x94, 0xa8, 0xe8, 0x8b, 0xb1, 0xe6, 0x96, 0x87, 0xef,
      0xbc, 0x8c, 0xe4, 0xbf, 0x9d, 0xe8, 0xaf, 0x81, 0xe5, 0x94, 0xaf, // 20
      0xe4, 0xb8, 0x80, 0xe6, 0x80, 0xa7, 0xe3, 0x80, 0x82, 0xe6, 0xa0, 0xbc, 0xe5, 0xbc, 0x8f, 0xe5, 0xbb, 0xba, 0xe8,
      0xae, 0xae, 0xef, 0xbc, 0x9a, 0xe2, 0x80, 0x9c, 0xe7, 0xb1, 0xbb, // 30
      0xe5, 0x9e, 0x8b, 0x5f, 0xe5, 0x8a, 0xa8, 0xe4, 0xbd, 0x9c, 0xe2, 0x80, 0x9d, 0xef, 0xbc, 0x8c, 0xe5, 0xa6, 0x82,
      0xe2, 0x80, 0x9c, 0x62, 0x6c, // 40
      0x6f, 0x67, 0x5f, 0x61, 0x64, 0x64, 0xe2, 0x80, 0x9d, 0xe6, 0x88, 0x96, 0xe2, 0x80, 0x9c, 0x63, // 50
      0x72, 0x65, 0x64, 0x69, 0x74, 0x5f, 0x62, 0x6c, 0x6f, 0x67, // 60
      0x5f, 0x61, 0x64, 0x64, 0xe2, 0x80, 0x9d, // 65
      0x00
    };

    std::string expected1 =
      "  11111111 22222 \n  3333 444444 \n  555555555 666666 \n  77777777 88888 \n  999999999 \n  00000000";
    std::string expected2 =
      "11111111 22222 \n  3333 444444 \n  555555555 666666 \n  77777777 88888 \n  999999999 \n  00000000";
    std::string expected3 =
      "  \xC3\x81\xC3\x81\xC3\x81\xC3\x81\xC3\x81\xC3\x81\xC3\x81\xC3\x81 "
      "\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89 \n  "
      "\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D\xC3\x8D \n  "
      "\xC3\x93\xC3\x93\xC3\x93\xC3\x93\xC3\x93\xC3\x93\xC3\x93\xC3\x93 "
      "\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A\xC3\x9A";
    std::string expected4 = "  11111111 22222 \n  3333 444444 \n(...)";
    std::string expected5 =
      "  111111111111111111\n  111111111111111111\n  111111111111111111\n  111111111111111111\n  111111111111111111\n  "
      "111111111111111111\n  111111111111111111\n  11111111111111";
    std::string expected6 = "";
    std::string expected7 = "aaaa \nbbbb \ncccc \ndddd \neeee";

  std::string result;

  EXPECT_THROW(base::reflow_text(content4, 20, "  "), std::invalid_argument);
  EXPECT_NO_THROW(result = base::reflow_text(content1, 20, "  ", true, 10));

  EXPECT_EQ(result, expected1);

  //  Do not indent first line
  EXPECT_NO_THROW(result = base::reflow_text(content1, 20, "  ", false, 10));
  EXPECT_EQ(result, expected2);

  //  String with multi-byte characters
  EXPECT_NO_THROW(result = base::reflow_text(content2, 20, "  ", true, 10));
  EXPECT_EQ(result, expected3);

  //  Line limit reached
  EXPECT_NO_THROW(result = base::reflow_text(content1, 20, "  ", true, 2));
  EXPECT_EQ(result, expected4);

  //  Big word
  EXPECT_NO_THROW(result = base::reflow_text(content3, 20, "  ", true, 10));
  EXPECT_EQ(result, expected5);

  //  Empty string
  EXPECT_NO_THROW(result = base::reflow_text("", 20, "  ", true, 10));
  EXPECT_EQ(result, expected6);

  //  Left fill automatic removal
  EXPECT_NO_THROW(result = base::reflow_text(content5, 6, "    ", true, 10));
  EXPECT_EQ(result, expected7);

  //  Invalid line length
  EXPECT_NO_THROW(result = base::reflow_text(content5, 4, "    ", true, 10));
  EXPECT_EQ(result, "");

  //  This test is to ensure that a big string won't mess up algorithm
  std::string dictionary[] = { "one", "big", "speech", "made", "words", "a", "of", "out" };
  std::string long_text;

  while (long_text.size() < SHRT_MAX) {
    int index = rand() % 8; //  8 is the size of the dictionary
    long_text += dictionary[index] + " ";
  }

  //  Short string, long line
  EXPECT_NO_THROW(result = base::reflow_text(long_text, 100, "  ", true, 1000));

  //  Short string, long line
  EXPECT_NO_THROW(result = base::reflow_text(std::string((char *)content6), 10, "  ", false));

  //  Remove the line feed and fill to verify coherence
  std::size_t position = 0;
  while ((position = result.find("\n  ", position)) != std::string::npos)
    result.replace(position, 3, "");

  EXPECT_EQ(result, std::string((char *)content6));

  //  This test was a specific case of a bug
  //  Short string, long line
  EXPECT_NO_THROW(result = base::reflow_text(std::string((char *)content6), 60, "    "));
}

TEST_F(StringUtilitiesTest, Atoi) {
  EXPECT_EQ(base::atoi<int>("10G", 0), 10);
  EXPECT_EQ(base::atoi<int>("10", 0), 10);
  EXPECT_EQ(base::atoi<int>("G", -1), -1);
  EXPECT_THROW(base::atoi<int>("G"), std::exception);
}

TEST_F(StringUtilitiesTest, PrefixSuffix) {
  std::string test_string = "This is a test string...to test.";
  std::string test_string_unicode =
    "\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89...Now that is a unicode string...\xE3\x8A\xA8";

  //  Base tests
  EXPECT_TRUE(base::hasPrefix(test_string, "This"));
  EXPECT_FALSE(base::hasPrefix(test_string, "is"));
  EXPECT_FALSE(base::hasPrefix(test_string, "test."));
  EXPECT_FALSE(base::hasPrefix(test_string, "blablabla"));
  EXPECT_TRUE(base::hasPrefix(test_string, ""));
  EXPECT_FALSE(base::hasPrefix(test_string, "his"));
  EXPECT_TRUE(base::hasPrefix(test_string, test_string));
  EXPECT_FALSE(base::hasPrefix(test_string, test_string + " Second part..."));
  EXPECT_FALSE(base::hasPrefix("", "blablabla"));
  EXPECT_TRUE(base::hasPrefix("", ""));

  EXPECT_TRUE(base::hasSuffix(test_string, "test."));
  EXPECT_FALSE(base::hasSuffix(test_string, "to "));
  EXPECT_FALSE(base::hasSuffix(test_string, "This"));
  EXPECT_FALSE(base::hasSuffix(test_string, "blablabla"));
  EXPECT_TRUE(base::hasSuffix(test_string, ""));
  EXPECT_FALSE(base::hasSuffix(test_string, "test"));
  EXPECT_TRUE(base::hasSuffix(test_string, test_string));
  EXPECT_FALSE(base::hasSuffix(test_string, test_string + " Second part..."));
  EXPECT_FALSE(base::hasSuffix("", "blablabla"));
  EXPECT_TRUE(base::hasSuffix("", ""));

  // Unicode tests
  EXPECT_TRUE(base::hasPrefix(test_string_unicode, "\xC3\x89\xC3\x89"));
  EXPECT_FALSE(base::hasPrefix(test_string_unicode, "is"));
  EXPECT_FALSE(base::hasPrefix(test_string_unicode, "\xE3\x8A\xA8"));
  EXPECT_FALSE(base::hasPrefix(test_string_unicode, "blablabla"));
  EXPECT_TRUE(base::hasPrefix(test_string_unicode, ""));
  EXPECT_FALSE(base::hasPrefix(test_string_unicode, "\x89\xC3\x89\xC3"));
  EXPECT_TRUE(base::hasPrefix(test_string_unicode, test_string_unicode));
  EXPECT_FALSE(base::hasPrefix(test_string_unicode, test_string_unicode + ". Second part..."));

  EXPECT_TRUE(base::hasSuffix(test_string_unicode, ".\xE3\x8A\xA8"));
  EXPECT_FALSE(base::hasSuffix(test_string_unicode, "to "));
  EXPECT_FALSE(base::hasSuffix(test_string_unicode, "\xC3\x89\xC3\x89"));
  EXPECT_FALSE(base::hasSuffix(test_string_unicode, "blablabla"));
  EXPECT_TRUE(base::hasSuffix(test_string_unicode, ""));
  EXPECT_FALSE(base::hasSuffix(test_string_unicode, ".\xE3\x8A"));
  EXPECT_TRUE(base::hasSuffix(test_string_unicode, test_string_unicode));
  EXPECT_FALSE(base::hasSuffix(test_string_unicode, test_string_unicode + ". Second part..."));
}

TEST_F(StringUtilitiesTest, TextExtraction) {
  std::string test_string = "This is a test string...to test.";
  std::string test_string_unicode =
    "\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89\xC3\x89...Now that is a unicode string...\xE3\x8A\xA8";

  // Base tests
  EXPECT_EQ(base::left(test_string, 5), "This ");
  EXPECT_EQ(base::left(test_string, 1), "T");
  EXPECT_EQ(base::left(test_string, 0), "");
  EXPECT_EQ(base::left("", 5), "");
  EXPECT_EQ(base::left(test_string, test_string.length()), test_string);
  EXPECT_EQ(base::left(test_string, 50), test_string);

  EXPECT_EQ(base::right(test_string, 5), "test.");
  EXPECT_EQ(base::right(test_string, 1), ".");
  EXPECT_EQ(base::right(test_string, 0), "");
  EXPECT_EQ(base::right("", 5), "");
  EXPECT_EQ(base::right(test_string, test_string.length()), test_string);
  EXPECT_EQ(base::right(test_string, 50), test_string);

  //  Unicode tests
  EXPECT_EQ(base::left(test_string_unicode, 5), "\xC3\x89\xC3\x89\xC3");
  EXPECT_EQ(base::left(test_string_unicode, 1), "\xC3");
  EXPECT_EQ(base::left(test_string_unicode, 0), "");
  EXPECT_EQ(base::left("", 5), "");
  EXPECT_EQ(base::left(test_string_unicode, test_string_unicode.length()), test_string_unicode);
  EXPECT_EQ(base::left(test_string_unicode, 500), test_string_unicode);

  EXPECT_EQ(base::right(test_string_unicode, 5), "..\xE3\x8A\xA8");
  EXPECT_EQ(base::right(test_string_unicode, 1), "\xA8");
  EXPECT_EQ(base::right(test_string_unicode, 0), "");
  EXPECT_EQ(base::right("", 5), "");
  EXPECT_EQ(base::right(test_string_unicode, test_string_unicode.length()), test_string_unicode);
  EXPECT_EQ(base::right(test_string_unicode, 500), test_string_unicode);
}

TEST_F(StringUtilitiesTest, FontDescriptionParsing) {
  std::string font_description;
  std::string font;
  float size = 0;
  bool bold = false;
  bool italic = false;

  font_description = "Sans 10";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
  EXPECT_EQ(font, "Sans");
  EXPECT_EQ(size, 10U);
  EXPECT_FALSE(bold);
  EXPECT_FALSE(italic);

  font_description = "Sans 12";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
    EXPECT_EQ(font, "Sans");
    EXPECT_EQ(size, 12);
    EXPECT_FALSE(bold);
    EXPECT_FALSE(italic);

  font_description = "Sans 10 bold";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
    EXPECT_EQ(font, "Sans");
    EXPECT_EQ(size, 10U);
    EXPECT_TRUE(bold);
    EXPECT_FALSE(italic);

  font_description = "Sans 10 BOLD";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
    EXPECT_EQ(font, "Sans");
    EXPECT_EQ(size, 10U);
    EXPECT_TRUE(bold);
    EXPECT_FALSE(italic);

  font_description = "Sans 10 italic";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
    EXPECT_EQ(font, "Sans");
    EXPECT_EQ(size, 10U);
    EXPECT_FALSE(bold);
    EXPECT_TRUE(italic);

  font_description = "Sans 10 ITALIC";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
    EXPECT_EQ(font, "Sans");
    EXPECT_EQ(size, 10U);
    EXPECT_FALSE(bold);
    EXPECT_TRUE(italic);

  font_description = "Sans 10 bold italic";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
    EXPECT_EQ(font, "Sans");
    EXPECT_EQ(size, 10U);
    EXPECT_TRUE(bold);
    EXPECT_TRUE(italic);

  font_description = "Sans 10 BOLD ITALIC";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
    EXPECT_EQ(font, "Sans");
    EXPECT_EQ(size, 10U);
    EXPECT_TRUE(bold);
    EXPECT_TRUE(italic);

  font_description = "My Font 10 BOLD ITALIC";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
    EXPECT_EQ(font, "My Font");
    EXPECT_EQ(size, 10U);
    EXPECT_TRUE(bold);
    EXPECT_TRUE(italic);

  font_description = "Helvetica Bold 12";
  EXPECT_TRUE(base::parse_font_description(font_description, font, size, bold, italic));
  EXPECT_EQ(font, "Helvetica");
  EXPECT_EQ(size, 12);
  EXPECT_TRUE(bold);
  EXPECT_FALSE(italic);
}

TEST_F(StringUtilitiesTest, PathNormalization) {
  std::string separator(1, G_DIR_SEPARATOR);

  EXPECT_EQ(base::normalize_path(""), "");
  EXPECT_EQ(base::normalize_path("/"), separator);
  EXPECT_EQ(base::normalize_path("\\"), separator);
  EXPECT_EQ(base::normalize_path("/////////"), separator);
  EXPECT_EQ(base::normalize_path("../../../"), "");
  EXPECT_EQ(base::normalize_path("abc/././../def"), "def");
  EXPECT_EQ(base::normalize_path("a/./b/.././d/./.."), "a");
  EXPECT_EQ(base::normalize_path("a/b/c/../d/../"), base::replaceString("a/b/", "/", separator));
  EXPECT_EQ(base::normalize_path("/path///to/my//////dir"), base::replaceString("/path/to/my/dir", "/", separator));
  EXPECT_EQ(base::normalize_path("D:\\files\\to//scan"), base::replaceString("D:/files/to/scan", "/", separator));
}
}


