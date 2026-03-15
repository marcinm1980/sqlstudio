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

#include "gtest/gtest.h"

#include "base/file_utilities.h"
#include "grt.h"

#ifdef _MSC_VER
#define FILE_SEPARATOR "\\"
#define INVALID_NAME "__test_file01*?"
#define RESERVED_NAME "com1"
#define MKDIR "mkdir "
#define READONLY "attrib +R "
#define READWRITE "attrib -R "
#else
#define FILE_SEPARATOR "/"
#define INVALID_NAME "__test_file01/?"
#define RESERVED_NAME "."
#define MKDIR "mkdir -p "
#define READONLY "chmod 0444 "
#define READWRITE "chmod 0777 "
#endif

#define TEST_DIR_NAME01 "__test_dir01"
#define TEST_DIR_NAME02 "__test_dir02"
#define TEST_FILE_NAME01 "__test_file01.txt"
#define TEST_FILE_NAME02 "__test_file02.txt"
#define TEST_FILE_NAME03 "__test_file03.txt"
#define TEST_FILE_NAME04 "__test_file04.txt"
#define TEST_FILE_STRIPPED_NAME02 "__test_file02"

#define FILE_PATTERN "__test_"
#define EMPTY_NAME ""

namespace {

// File utilities test class
class FileUtilitiesTest : public ::testing::Test {
 protected:
  std::string too_long_name;
  std::string too_long_basename;
  std::string dir_unicode_name;
  std::string file_unicode_name;
  std::string file_unicode_basename;

  void SetUp() override {
    unsigned int i;

    dir_unicode_name.clear();
    dir_unicode_name += "__test_dir_";
    dir_unicode_name += "\xE3\x8A\xA8"; // Valid Unicode character.

    file_unicode_basename.clear();
    file_unicode_basename += "__test_file_";
    file_unicode_basename += "\xE3\x8F\xA3"; // Valid Unicode character.

    file_unicode_name.clear();
    file_unicode_name = file_unicode_basename;
    file_unicode_name += ".txt";

    too_long_basename.clear();
    for (i = 0; i < 1000; i++)
      too_long_basename.append("x");

    too_long_name.clear();
    too_long_name = too_long_basename;
    too_long_name += ".txt";

    // Clean up any existing test directories/files
    base::remove_recursive(TEST_DIR_NAME01);
    base::remove_recursive(TEST_DIR_NAME02);
    base::remove(TEST_FILE_NAME01);
    base::remove(TEST_FILE_NAME02);
    base::remove(TEST_FILE_NAME03);
    base::remove(TEST_FILE_NAME04);
    base::remove(dir_unicode_name);
    base::remove(file_unicode_name);
  }

  void TearDown() override {
    // Clean up any remaining test directories/files
    base::remove_recursive(TEST_DIR_NAME01);
    base::remove_recursive(TEST_DIR_NAME02);
    base::remove(TEST_FILE_NAME01);
    base::remove(TEST_FILE_NAME02);
    base::remove(TEST_FILE_NAME03);
    base::remove(TEST_FILE_NAME04);
    base::remove(dir_unicode_name);
    base::remove(file_unicode_name);
  }
};

// Testing public API's
// - create_directory(const std::string &path, int mode)
// - remove(const std::string &path)
TEST_F(FileUtilitiesTest, RemoveNonExistingDirectory) {
  EXPECT_NO_THROW({
    EXPECT_FALSE(base::remove(TEST_DIR_NAME01));
  });

  // Create a non existing directory
  EXPECT_NO_THROW({
    EXPECT_TRUE(base::create_directory(TEST_DIR_NAME01, 0700));
  });

  // Create an already existing directory
  EXPECT_NO_THROW({
    EXPECT_FALSE(base::create_directory(TEST_DIR_NAME01, 0700));
  });

  // Remove an existing directory
  EXPECT_NO_THROW({
    EXPECT_TRUE(base::remove(TEST_DIR_NAME01));
  });

  // Remove a non existing file
  EXPECT_NO_THROW({
    EXPECT_FALSE(base::remove(TEST_FILE_NAME01));
  });

  // Remove an existing file
  EXPECT_NO_THROW({
    base::FileHandle test_file_scoped(TEST_FILE_NAME01, "w+"); // Create file
    test_file_scoped.dispose();                                // Close file

    EXPECT_TRUE(base::remove(TEST_FILE_NAME01));
  });
}

// Testing public API's
// - create_directory(const std::string &path, int mode)
// - remove(const std::string &path)
// -- Corner/Limit Values --
TEST_F(FileUtilitiesTest, CreateDirectory) {
  // Create a directory -- Invalid name
  EXPECT_THROW(base::create_directory(INVALID_NAME, 0700), base::file_error);

  // Create a directory -- Empty name
  EXPECT_THROW(base::create_directory(EMPTY_NAME, 0700), base::file_error);

  // Create a directory -- Too long name
  EXPECT_THROW(base::create_directory(too_long_name, 0700), base::file_error);

  // Create a directory -- Unicode name
  EXPECT_NO_THROW({
    EXPECT_TRUE(base::create_directory(dir_unicode_name, 0700));
  });

#ifdef _MSC_VER
  EXPECT_THROW(base::create_directory(RESERVED_NAME, 0700), base::file_error);
#endif

  // Create a file -- Unicode name
  EXPECT_NO_THROW({
    base::FileHandle test_file_scoped(file_unicode_name, "w+"); // Create file
    EXPECT_TRUE(base::file_exists(file_unicode_name));
  });
#ifdef _MSC_VER
  // Remove a file/directory -- Invalid name
  EXPECT_THROW(base::remove(INVALID_NAME), base::file_error);

  // Remove a file/directory -- Empty name
  EXPECT_THROW(base::remove(EMPTY_NAME), base::file_error);
#endif

  // Remove a file/directory -- Too long name
  EXPECT_THROW(base::remove(too_long_name), base::file_error);

  // Remove an existing directory -- Unicode name
  EXPECT_NO_THROW({
    EXPECT_TRUE(base::remove(dir_unicode_name));
  });

  // Remove an existing file -- Unicode name
  EXPECT_NO_THROW({
    EXPECT_TRUE(base::remove(file_unicode_name));
  });

    // Remove a file/directory -- Reserved name
    // try
    //{
    //  if (base::remove(RESERVED_NAME))
    //  {
    //      // return true means dir exists
    //      fail(strfmt("TEST 10.12: Directory \"%s\" exists",dir_unicode_name));
    //  }
    //}
    // catch (base::file_error &exc)
    //{
    //  throw grt::os_error(strfmt("Cannot remove directory for document: %s", exc.what()));
    //}

  // Clean leftover test files
  base::remove(dir_unicode_name);
  base::remove(file_unicode_name);
}

// Testing file_error public API's
// - c-tor
// - code()
// - sys_code()
TEST_F(FileUtilitiesTest, MiscellaneousVariables) {
  // Miscellaneous variables
  base::error_code test_result;
  base::error_code expected_result;
  // int int_test_result;
  // int int_expected_result;

  EXPECT_NO_THROW({
    // Testing c-tor
    base::file_error c_tor_file_error("Error Test", 0);

    // Testing code()
    test_result = c_tor_file_error.code();
    expected_result = base::success;

    EXPECT_EQ((int)test_result, expected_result);

    // Testing sys_code()
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // NOTE: THIS FUNCTION IS NOT IMPLEMENTED YET!!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // int_test_result = c_tor_file_error.sys_code();
    // int_expected_result = 0;

    // if(int_test_result != int_expected_result)
    //{
    //  fail("TEST 15.2: Unexpected result calling function sys_code()");
    //}
  });
}

// Testing file_error public API's
// - c-tor
// -- Corner/Limit Values --
TEST_F(FileUtilitiesTest, FileErrorConstructor) {
  EXPECT_NO_THROW({
    // Testing c-tor -- Empty string
    base::file_error c_tor_file_error_empty(EMPTY_NAME, 0);

    // Testing c-tor -- Unicode string
    base::file_error c_tor_file_error_unicode(file_unicode_name, 0);
  });
}

// Testing FileHandle public API's (operators)
// - operator FILE *()
//  - operator bool()
// - operator =(FileHandle &fh)
// - operator ->()
TEST_F(FileUtilitiesTest, FileHandleOperators) {
  EXPECT_NO_THROW({
      // Miscellaneous variables
      base::FileHandle test_file01(TEST_FILE_NAME01, "w+");
      base::FileHandle test_file03(TEST_FILE_NAME03, "w+");
      base::FileHandle test_file04;

      FILE *test_result = NULL;
      FILE *expected_result = NULL;

    // Testing overridden operator bool()
    EXPECT_FALSE(test_file01.operator bool());

    test_file01.dispose();

    EXPECT_TRUE(test_file01.operator bool());

      // Testing overridden operator =(FileHandle &fh)
      expected_result = test_file03.file();
      test_file04 = test_file03;
      test_result = test_file04.file();

    EXPECT_EQ(test_result, expected_result);
  });

  // Clean leftover test files
  base::remove(TEST_FILE_NAME01);
  base::remove(TEST_FILE_NAME03);
}

// Testing FileHandle public API's
// - swap(FileHandle &fh)
// - file()
// - dispose()
TEST_F(FileUtilitiesTest, FileHandleSwapFileAndDispose) {
  // Miscellaneous variables
  base::FileHandle test_file01(TEST_FILE_NAME01, "w+");
  base::FileHandle test_file02(TEST_FILE_NAME02, "w+");
  base::FileHandle test_file03;

  FILE *test_result = nullptr;
  FILE *expected_result = nullptr;

  // Testing swap(FileHandle &fh)
  expected_result = test_file02.file();
  test_file01.swap(test_file02);
  test_result = test_file01.file();

  EXPECT_EQ(test_result, expected_result);

  // Testing file()
  EXPECT_NE(test_file01.file(), nullptr);

  // Testing dispose()
  test_file01.dispose();
  test_result = test_file01.file();
  expected_result = nullptr;

  EXPECT_EQ(test_result, expected_result);

  // Testing open_file(const char *filename, const char *mode, bool throw_on_fail= true)

  // test case for 'throw_on_fail' default value (i.e. TRUE)
  EXPECT_THROW(base::FileHandle test_file_scoped(TEST_FILE_NAME03, "r"), std::exception);

  // Testing FileHandle c-tor
  EXPECT_NO_THROW({
    base::FileHandle test_file03(TEST_FILE_NAME03, "r", false);
    EXPECT_EQ(test_file03.file(), nullptr);
  });

  EXPECT_NO_THROW({
    base::FileHandle test_file04(TEST_FILE_NAME04, "w+");
    EXPECT_NE(test_file04.file(), nullptr);
  });

  // Clean leftover test files
  test_file01.dispose();
  test_file02.dispose();
  base::remove(TEST_FILE_NAME01);
  base::remove(TEST_FILE_NAME02);
  base::remove(TEST_FILE_NAME04);
}

//  Testing FileHandle public API's
// - swap(FileHandle &fh)
// - file()
// - dispose()
// - open_file(const char *filename, const char *mode, bool throw_on_fail= true)
// -- Read-only permissions --
TEST_F(FileUtilitiesTest, FileHandleSwapFileDisposeOpenFileOnReadOnlyPermissions) {
  // Miscellaneous variables
  FILE *test_result = NULL;
  FILE *expected_result = NULL;
  base::FileHandle test_file01(TEST_FILE_NAME01, "w+");
  base::FileHandle test_file02(TEST_FILE_NAME02, "w+");
  base::FileHandle test_file;

  std::string command_line;

  // Change file permissions to read-only
  command_line.clear();
  command_line.assign(READONLY);
  command_line.append(TEST_FILE_NAME01);

  system(command_line.c_str());

  command_line.clear();
  command_line.assign(READONLY);
  command_line.append(TEST_FILE_NAME02);

  system(command_line.c_str());

  // Testing swap(FileHandle &fh)
  expected_result = test_file02.file();
  test_file01.swap(test_file02);
  test_result = test_file01.file();

  EXPECT_EQ(test_result, expected_result);

  // Testing file()
  EXPECT_NE(test_file01.file(), nullptr);

  // Testing dispose()
  test_file01.dispose();
  test_result = test_file01.file();

  EXPECT_EQ(test_result, nullptr);

  // Testing open_file(const char *filename, const char *mode, bool throw_on_fail= true)

  // test case for 'throw_on_fail' default value (i.e. TRUE)
  EXPECT_THROW(base::FileHandle test_file_scoped(TEST_FILE_NAME01, "w"), std::exception);

  // test case for 'throw_on_fail' FALSE value
  EXPECT_NO_THROW({
    base::FileHandle test_file_scoped(TEST_FILE_NAME01, "w", false);
    EXPECT_EQ(test_file.file(), nullptr);
  });

  // Change back permissions to read-write
  command_line.clear();
  command_line.assign(READWRITE);
  command_line.append(TEST_FILE_NAME01);

  system(command_line.c_str());

  command_line.clear();
  command_line.assign(READWRITE);
  command_line.append(TEST_FILE_NAME02);

  system(command_line.c_str());

  // Clean leftover test files
  test_file01.dispose();
  test_file02.dispose();
  base::remove(TEST_FILE_NAME01);
  base::remove(TEST_FILE_NAME02);
}

// Testing FileHandle public API's
// - open_file(const char *filename, const char *mode, bool throw_on_fail= true)
// -- Corner/Limit Values --
TEST_F(FileUtilitiesTest, OpenFile) {
  // Miscellaneous variables
  base::FileHandle test_file;

  // Testing open_file(const char *filename, const char *mode, bool throw_on_fail= true)

  // test case for 'throw_on_fail' default value (i.e. TRUE)
  // -- Invalid name
  EXPECT_THROW(base::FileHandle test_file_scoped(INVALID_NAME, "r"), std::exception);

  // test case for 'throw_on_fail' default value (i.e. TRUE)
  // -- Empty name
  EXPECT_THROW(base::FileHandle test_file_scoped(EMPTY_NAME, "r"), std::exception);

  // test case for 'throw_on_fail' default value (i.e. TRUE)
  // -- Too long name
  EXPECT_THROW(base::FileHandle test_file_scoped(too_long_name.c_str(), "r"), std::exception);

#ifdef _MSC_VER
  // test case for 'throw_on_fail' default value (i.e. TRUE)
  // -- Unicode name (file doesn't exist)
  EXPECT_THROW(base::FileHandle test_file_scoped(file_unicode_name.c_str(), "r"), std::exception);
#endif

  // test case for 'throw_on_fail' FALSE value
  // -- Invalid name
  EXPECT_NO_THROW(base::FileHandle test_file_scoped(INVALID_NAME, "r", false));

  // test case for 'throw_on_fail' FALSE value
  // -- Empty name
  EXPECT_NO_THROW(base::FileHandle test_file_scoped(EMPTY_NAME, "r", false));

  // test case for 'throw_on_fail' FALSE value
  // -- Too long name
  EXPECT_NO_THROW(base::FileHandle test_file_scoped(too_long_name.c_str(), "r", false));

  // test case for 'throw_on_fail' FALSE value
  // -- Unicode name
  EXPECT_NO_THROW(base::FileHandle test_file_scoped(file_unicode_name.c_str(), "r", false));

  // test case for 'throw_on_fail' FALSE value
  // -- Reserved name
  EXPECT_NO_THROW(base::FileHandle test_file_scoped(RESERVED_NAME, "r", false));

  // Clean leftover test files
  base::remove(file_unicode_name);
}

// Testing FileHandle c-tors & d-tors
TEST_F(FileUtilitiesTest, FileHandleCtorsAndDtors) {
  // constructors & miscellaneous variables
  base::FileHandle c_tor_no_name;
  base::FileHandle c_tor_test_filename(TEST_FILE_NAME01, "w+");
  base::FileHandle c_tor_temp(TEST_FILE_NAME02, "w+");
  FILE *original_c_tor_temp_ptr = c_tor_temp.file();
  base::FileHandle *c_tor_FileHandle_ref = new base::FileHandle(c_tor_temp);

  // test case for empty name string
  FILE *test_result = c_tor_no_name.file();

  EXPECT_EQ(test_result, nullptr);

  // test case for 'throw_on_fail' default value (i.e. TRUE)
  EXPECT_THROW(base::FileHandle c_tor_throw_on_fail(TEST_FILE_NAME03, "r"), std::exception);

  // test case for 'throw_on_fail' FALSE value
  EXPECT_NO_THROW({
    // doesn't throw error (even if file does not exist)
    base::FileHandle c_tor_throw_on_fail(TEST_FILE_NAME03, "r", false);
    EXPECT_EQ(c_tor_throw_on_fail.file(), nullptr);
  });

  // test case for FileHandle& value
  EXPECT_EQ(c_tor_FileHandle_ref->file(), original_c_tor_temp_ptr);

  // test case for d-tor
  c_tor_FileHandle_ref->~FileHandle();
  EXPECT_EQ(c_tor_FileHandle_ref->file(), nullptr);

  // Clean leftover test files
  c_tor_test_filename.dispose();
  c_tor_temp.dispose();
  base::remove(TEST_FILE_NAME01);
  base::remove(TEST_FILE_NAME02);
}

// Testing FileHandle c-tors
// -- Corner/Limit Values --
TEST_F(FileUtilitiesTest, FileHandleCtorsAndDtorsCornerValues) {
  // Testing FileHandle(const char *filename, const char *mode, bool throw_on_fail= true)

  // test case for 'throw_on_fail' default value (i.e. TRUE)
  // -- Invalid name
  EXPECT_THROW(base::FileHandle c_tor(INVALID_NAME, "r"), std::exception);

  // test case for 'throw_on_fail' default value (i.e. TRUE)
  // -- Empty name
  EXPECT_THROW(base::FileHandle c_tor(EMPTY_NAME, "r"), std::exception);

  // test case for 'throw_on_fail' default value (i.e. TRUE)
  // -- Too long name
  EXPECT_THROW(base::FileHandle c_tor(too_long_name.c_str(), "r"), std::exception);
#ifdef _MSC_VER
  // test case for 'throw_on_fail' default value (i.e. TRUE)
  // -- Unicode name (file doesn't exist)
  EXPECT_THROW(base::FileHandle c_tor(file_unicode_name.c_str(), "r"), std::exception);
#endif
  // test case for 'throw_on_fail' FALSE value
  // -- Invalid name
  EXPECT_NO_THROW(base::FileHandle c_tor(INVALID_NAME, "r", false));

  // test case for 'throw_on_fail' FALSE value
  // -- Empty name
  EXPECT_NO_THROW(base::FileHandle c_tor(EMPTY_NAME, "r", false));

  // test case for 'throw_on_fail' FALSE value
  // -- Too long name
  EXPECT_NO_THROW(base::FileHandle c_tor(too_long_name.c_str(), "r", false));

  // test case for 'throw_on_fail' FALSE value
  // -- Unicode name
  EXPECT_NO_THROW(base::FileHandle c_tor(file_unicode_name.c_str(), "r", false));

  // test case for 'throw_on_fail' FALSE value
  // -- Reserved name
  EXPECT_NO_THROW(base::FileHandle c_tor(RESERVED_NAME, "r", false));

  // Clean leftover test files
  base::remove(file_unicode_name);
}

// Testing public API's
// - file_exists(const std::string &path)
// - is_directory(const std::string &path)
// - dirname(const std::string &path)
// - rename(const std::string &from, const std::string &to)
// - extension(const std::string &path)
// - basename(const std::string &path)
// - strip_extension(const std::string &path)
// - remove_recursive(const std::string &path)
TEST_F(FileUtilitiesTest, FileHandleOperatingMethods) {
  std::string command_line;
  std::string test_result;

  // Create subdirectory structure
  command_line.clear();
  command_line.assign(MKDIR);
  command_line.append(TEST_DIR_NAME01);
  command_line.append(FILE_SEPARATOR);
  command_line.append(TEST_DIR_NAME02);

  system(command_line.c_str());

  // Create directory
  command_line.clear();
  command_line.assign(MKDIR);
  command_line.append(TEST_DIR_NAME02);

  system(command_line.c_str());

  EXPECT_NO_THROW(base::FileHandle test_file_scoped(TEST_FILE_NAME01, "w+"));

  // Testing file_exists(const std::string &path) with a directory
  EXPECT_TRUE(base::file_exists(TEST_DIR_NAME01));

  // Testing file_exists(const std::string &path) with a file
  EXPECT_TRUE(base::file_exists(TEST_FILE_NAME01));

  // Testing is_directory(const std::string &path)
  EXPECT_TRUE(base::is_directory(TEST_DIR_NAME01));

  // Testing dirname(const std::string &path)
  test_result.clear();
  test_result = base::dirname(TEST_FILE_NAME01);
  EXPECT_EQ(test_result, ".");

  test_result.clear();
  test_result = base::dirname(TEST_DIR_NAME01 FILE_SEPARATOR TEST_DIR_NAME02);
  EXPECT_EQ(test_result, TEST_DIR_NAME01);

  // Testing rename(const std::string &from, const std::string &to)
  base::rename(TEST_FILE_NAME01, TEST_FILE_NAME02);

  EXPECT_FALSE(base::file_exists(TEST_FILE_NAME01));

  EXPECT_TRUE(base::file_exists(TEST_FILE_NAME02));

  // Testing extension(const std::string &path)
  test_result.clear();
  test_result = base::extension(TEST_FILE_NAME02);
  EXPECT_EQ(test_result, ".txt");

  // Testing basename(const std::string &path)
  test_result.clear();
  test_result = base::basename("." FILE_SEPARATOR TEST_FILE_NAME02);
  EXPECT_EQ(test_result, TEST_FILE_NAME02);

  // Testing strip_extension(const std::string &path)
  test_result.clear();
  test_result = base::strip_extension(TEST_FILE_NAME02);
  EXPECT_EQ(test_result, TEST_FILE_STRIPPED_NAME02);

  // Testing remove_recursive(const std::string &path)
  EXPECT_TRUE(base::remove_recursive(TEST_DIR_NAME01));

  // Clean leftover test files
  base::remove_recursive(TEST_DIR_NAME01);
  base::remove_recursive(TEST_DIR_NAME02);
  base::remove(TEST_FILE_NAME01);
  base::remove(TEST_FILE_NAME02);
}

// Testing public API's
// - file_exists(const std::string &path)
// - is_directory(const std::string &path)
// - dirname(const std::string &path)
// - rename(const std::string &from, const std::string &to)
// - extension(const std::string &path)
// - basename(const std::string &path)
// - strip_extension(const std::string &path)
// - remove_recursive(const std::string &path)
// -- Corner/Limit Values --
TEST_F(FileUtilitiesTest, FileHandleOperatingMethodsCornerTests) {
  std::string command_line;
  std::string test_result;

  // Create subdirectory structure
  command_line.clear();
  command_line.assign(MKDIR);
  command_line.append(TEST_DIR_NAME01);
  command_line.append(FILE_SEPARATOR);
  command_line.append(TEST_DIR_NAME02);

  system(command_line.c_str());

  EXPECT_NO_THROW(base::FileHandle test_file_scoped(TEST_FILE_NAME01, "w+"));

  // Testing file_exists(const std::string &path)
  EXPECT_FALSE(base::file_exists(INVALID_NAME));
  EXPECT_FALSE(base::file_exists(EMPTY_NAME));
  EXPECT_FALSE(base::file_exists(too_long_name.c_str()));
  EXPECT_FALSE(base::file_exists(file_unicode_name.c_str()));

  // Testing is_directory(const std::string &path)
  EXPECT_FALSE(base::is_directory(INVALID_NAME));
  EXPECT_FALSE(base::is_directory(EMPTY_NAME));
  EXPECT_FALSE(base::is_directory(too_long_name.c_str()));
  EXPECT_FALSE(base::is_directory(file_unicode_name.c_str()));

#ifdef _MSC_VER
  EXPECT_FALSE(base::is_directory(RESERVED_NAME));

  // Testing dirname(const std::string &path)
  EXPECT_EQ(base::dirname(INVALID_NAME), ".");
#endif

  EXPECT_EQ(base::dirname(EMPTY_NAME), ".");
  EXPECT_EQ(base::dirname(too_long_name.c_str()), ".");
  EXPECT_EQ(base::dirname(file_unicode_name.c_str()), ".");
  EXPECT_EQ(base::dirname(RESERVED_NAME), ".");

  // Testing rename(const std::string &from, const std::string &to)
  EXPECT_THROW(base::rename(INVALID_NAME, TEST_FILE_NAME02), std::exception);
  EXPECT_THROW(base::rename(TEST_FILE_NAME01, INVALID_NAME), std::exception);
  EXPECT_THROW(base::rename(INVALID_NAME, INVALID_NAME), std::exception);
  EXPECT_THROW(base::rename(EMPTY_NAME, TEST_FILE_NAME02), std::exception);
  EXPECT_THROW(base::rename(TEST_FILE_NAME01, EMPTY_NAME), std::exception);
  EXPECT_THROW(base::rename(EMPTY_NAME, EMPTY_NAME), std::exception);
  EXPECT_THROW(base::rename(too_long_name.c_str(), TEST_FILE_NAME02), std::exception);
  EXPECT_THROW(base::rename(TEST_FILE_NAME01, too_long_name.c_str()), std::exception);
  EXPECT_THROW(base::rename(too_long_name.c_str(), too_long_name.c_str()), std::exception);
  EXPECT_THROW(base::rename(RESERVED_NAME, TEST_FILE_NAME02), std::exception);
  EXPECT_THROW(base::rename(TEST_FILE_NAME01, RESERVED_NAME), std::exception);
  EXPECT_THROW(base::rename(RESERVED_NAME, RESERVED_NAME), std::exception);

  base::rename(TEST_FILE_NAME01, file_unicode_name);

  EXPECT_FALSE(base::file_exists(TEST_FILE_NAME01));
  EXPECT_TRUE(base::file_exists(file_unicode_name));

  // -- Unicode name (change back from Unicode to ASCII)
  base::rename(file_unicode_name.c_str(), TEST_FILE_NAME01);

  EXPECT_FALSE(base::file_exists(file_unicode_name));
  EXPECT_TRUE(base::file_exists(TEST_FILE_NAME01));

  // -- Source File does not exist
  EXPECT_THROW(base::rename(TEST_FILE_NAME03, TEST_FILE_NAME02), std::exception);
#ifdef _MSC_VER
  // -- Target File already exists
  EXPECT_THROW({
    // Create file
    base::FileHandle test_file_scoped(TEST_FILE_NAME03, "w+");
    base::rename(TEST_FILE_NAME01, TEST_FILE_NAME03);
  }, std::exception);
#endif
  // -- Source & Target files are the same, and non-existing
  EXPECT_THROW(base::rename(TEST_FILE_NAME02, TEST_FILE_NAME02), std::exception);

  // -- Source & Target files are the same, and both exist
  base::rename(TEST_FILE_NAME01, TEST_FILE_NAME01);

  EXPECT_TRUE(base::file_exists(TEST_FILE_NAME01));

  // Testing extension(const std::string &path)
  EXPECT_EQ(base::extension(INVALID_NAME), "");
  EXPECT_EQ(base::extension(EMPTY_NAME), "");
  EXPECT_EQ(base::extension(too_long_name.c_str()), ".txt");
  EXPECT_EQ(base::extension(file_unicode_name.c_str()), ".txt");

#ifdef _MSC_VER
  EXPECT_EQ(base::extension(RESERVED_NAME), "");
#endif

  EXPECT_EQ(base::extension("filename_no_ext"), "");
  EXPECT_EQ(base::extension("filename_with_no_extension."), ".");
  EXPECT_EQ(base::extension(".txt"), ".txt");
  EXPECT_EQ(base::extension("basename.xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"), ".xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

  // Testing strip_extension(const std::string &path)

  EXPECT_EQ(base::strip_extension(INVALID_NAME), INVALID_NAME);
  EXPECT_EQ(base::strip_extension(EMPTY_NAME), EMPTY_NAME);
  EXPECT_EQ(base::strip_extension(too_long_name), too_long_basename);
  EXPECT_EQ(base::strip_extension(file_unicode_name), file_unicode_basename);

#ifdef _MSC_VER
  EXPECT_EQ(base::strip_extension(RESERVED_NAME), RESERVED_NAME);
#endif

  EXPECT_EQ(base::strip_extension("filename_with_no_extension"), "filename_with_no_extension");
  EXPECT_EQ(base::strip_extension("filename_with_no_extension."), "filename_with_no_extension");
  EXPECT_EQ(base::strip_extension(".txt"), "");
  EXPECT_EQ(base::strip_extension("basename.xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"), "basename");

  // Testing remove_recursive(const std::string &path)

  EXPECT_FALSE(base::remove_recursive(INVALID_NAME));
  EXPECT_FALSE(base::remove_recursive(EMPTY_NAME));
  EXPECT_FALSE(base::remove_recursive(too_long_name));

  base::rename(TEST_DIR_NAME01, dir_unicode_name);

  EXPECT_TRUE(base::remove_recursive(dir_unicode_name.c_str()));
#ifdef _MSC_VER
  // -- Reserved name
  EXPECT_FALSE(base::remove_recursive(RESERVED_NAME));
#endif

  // Clean leftover test files
  base::remove_recursive(TEST_DIR_NAME01);
  base::remove_recursive(dir_unicode_name);
  base::remove(TEST_FILE_NAME01);
  base::remove(TEST_FILE_NAME02);
  base::remove(TEST_FILE_NAME03);
}

#if 0
  // Testing public API's
  // - rename(const std::string &from, const std::string &to)
  // - remove(const std::string &path)
  // - remove_recursive(const std::string &path)
  // -- Read-only permissions --
#ifdef _MSC_VER
  TEST_FUNCTION(54) {
    std::string command_line;

    try {
      // Create subdirectory structure
      command_line.clear();
      command_line.assign(MKDIR);
      command_line.append(TEST_DIR_NAME01);
      command_line.append(FILE_SEPARATOR);
      command_line.append(TEST_DIR_NAME02);

      system(command_line.c_str());

      // Create directory
      command_line.clear();
      command_line.assign(MKDIR);
      command_line.append(TEST_DIR_NAME02);

      system(command_line.c_str());
    } catch (std::runtime_error &exc) {
      throw grt::os_error(strfmt("Runtime error: %s", exc.what()));
    }

    try {
      // Create file
      base::FileHandle test_file_scoped(TEST_FILE_NAME01, "w+");

      // Change file permission to read-only
      command_line.clear();
      command_line.assign(READONLY);
      command_line.append(TEST_FILE_NAME01);

      system(command_line.c_str());

      // Change directories permission to read-only
      command_line.clear();
      command_line.assign(READONLY);
      command_line.append(TEST_DIR_NAME01);

      system(command_line.c_str());

      command_line.clear();
      command_line.assign(READONLY);
      command_line.append(TEST_DIR_NAME02);

      system(command_line.c_str());
    } catch (base::file_error &exc) {
      throw grt::os_error(strfmt("File error: %s", exc.what()));
    }

    try {
      // Rename a read-only file
      base::rename(TEST_FILE_NAME01, TEST_FILE_NAME02);

      if (base::file_exists(TEST_FILE_NAME01)) {
        // return true means old file still exists
        fail(strfmt("TEST 54.1: File \"%s\" still exists", TEST_FILE_NAME01));
      }

      if (!base::file_exists(TEST_FILE_NAME02)) {
        // return false means new file does not exist
        fail(strfmt("TEST 54.1: File \"%s\" does not exist", TEST_FILE_NAME02));
      }

      // Remove a read-only directory
      try {
        bool result = base::remove(TEST_DIR_NAME02);
        fail(strfmt("TEST 54.2: Read-only directory \"%s\" did not throw an error", TEST_DIR_NAME02));
      } catch (const base::file_error &exc) {
        if (0 != std::string(exc.what()).find("Could not delete file ")) {
          fail(
               strfmt("TEST 54.2: Read-only directory \"%s\" threw an unexpected error: %s", TEST_DIR_NAME02, exc.what()));
        }
      } catch (std::exception &exc) {
        fail(strfmt("TEST 54.2: Read-only directory \"%s\" threw an unexpected error: %s", TEST_DIR_NAME02, exc.what()));
      }

      // Remove a read-only file
      try {
        base::remove(TEST_FILE_NAME02);
        fail(strfmt("TEST 54.3: Read-only file \"%s\" did not throw an error", TEST_FILE_NAME02));
      } catch (const base::file_error &exc) {
        if (0 != std::string(exc.what()).find("Could not delete file ")) {
          fail(strfmt("TEST 54.3: Read-only file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME02, exc.what()));
        }
      } catch (std::exception &exc) {
        fail(strfmt("TEST 54.3: Read-only file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME02, exc.what()));
      }

      // Recursively remove a read-only directory structure
      if (!base::remove_recursive(TEST_DIR_NAME01)) {
        // return false means dir does not exist
        fail(strfmt("TEST 54.4: Directory \"%s\" does not exist", TEST_DIR_NAME01));
      }
    } catch (base::file_error &exc) {
      throw grt::os_error(strfmt("File error: %s", exc.what()));
    }

    // Change back permissions to read-write
    try {
      // Change file permission to read-write
      command_line.clear();
      command_line.assign(READWRITE);
      command_line.append(TEST_FILE_NAME02);

      system(command_line.c_str());

      // Change directories permission to read-only
      command_line.clear();
      command_line.assign(READWRITE);
      command_line.append(TEST_DIR_NAME01);

      system(command_line.c_str());

      command_line.clear();
      command_line.assign(READWRITE);
      command_line.append(TEST_DIR_NAME02);

      system(command_line.c_str());
    } catch (base::file_error &exc) {
      throw grt::os_error(strfmt("File error: %s", exc.what()));
    }

    // Clean leftover test files
    base::remove_recursive(TEST_DIR_NAME01);
    base::remove_recursive(TEST_DIR_NAME02);
    base::remove(TEST_FILE_NAME01);
    base::remove(TEST_FILE_NAME02);
  }
#endif
#endif

//  Testing public API
// - scan_for_files_matching(const std::string &pattern,
TEST_F(FileUtilitiesTest, ScanForFilesMatching) {
  std::string command_line;

  // Create subdirectory structure
  command_line.clear();
  command_line.assign(MKDIR);
  command_line.append(TEST_DIR_NAME01);
  command_line.append(FILE_SEPARATOR);
  command_line.append(TEST_DIR_NAME02);

  system(command_line.c_str());

  // Create directory
  command_line.clear();
  command_line.assign(MKDIR);
  command_line.append(TEST_DIR_NAME02);

  system(command_line.c_str());

  // Create files
  base::FileHandle test_file01(TEST_FILE_NAME01, "w+");
  base::FileHandle test_file02(TEST_FILE_NAME02, "w+");

  // test case for 'recursive' default value (i.e. FALSE)
  {
    std::string search_pattern = "." FILE_SEPARATOR FILE_PATTERN "*";
    std::list<std::string> test_result = base::scan_for_files_matching(search_pattern);

    EXPECT_FALSE(test_result.empty());
    EXPECT_EQ(test_result.size(), 4U);

    while (!test_result.empty()) {
      EXPECT_NE(test_result.front().find(FILE_PATTERN), std::string::npos);
      test_result.pop_front();
    }
  }

  // test case for 'recursive' TRUE value
  {
    std::string search_pattern = "." FILE_SEPARATOR FILE_PATTERN "*";
    std::list<std::string> test_result = base::scan_for_files_matching(search_pattern, true);

    EXPECT_FALSE(test_result.empty());
    EXPECT_EQ(test_result.size(), 5U);

    while (!test_result.empty()) {
      EXPECT_NE(test_result.front().find(FILE_PATTERN), std::string::npos);
      test_result.pop_front();
    }
  }

  // Clean leftover test files
  base::remove_recursive(TEST_DIR_NAME01);
  base::remove_recursive(TEST_DIR_NAME02);
  test_file01.dispose();
  test_file02.dispose();
  base::remove(TEST_FILE_NAME01);
  base::remove(TEST_FILE_NAME02);
}

// Testing file_locked_error public API's
// - c-tors
TEST_F(FileUtilitiesTest, FileLockedErrorCtors) {
  // ml: very questionable what is tested here.
  // test cases for constructors
  EXPECT_THROW(throw base::file_locked_error("File Locked Error Message"), base::file_locked_error);

  EXPECT_THROW({
    base::file_locked_error first_error("File Locked Error Message");
    throw base::file_locked_error(first_error);
  }, base::file_locked_error);
}

// Testing LockFile public API's
// - c-tor
// - d-tor
// - check(const std::string &path)
TEST_F(FileUtilitiesTest, LockFileCtorsDtorsAndCheck) {
  base::remove(TEST_FILE_NAME01);
  base::remove(TEST_FILE_NAME02);

  // test cases for constructor, check(const std::string &path)
  // with Status == LockedSelf and Status == NotLocked
  // and destructor
  {
    base::LockFile lock_file01(TEST_FILE_NAME01);
    base::FileHandle test_file02(TEST_FILE_NAME02, "w+");

    EXPECT_EQ(base::LockFile::check(TEST_FILE_NAME01), base::LockFile::LockedSelf);
#ifndef _MSC_VER
    // Semantic issue with NotLocked for a plain file without content.
    // TODO: rework lock detection with other than base::LockFile instances.
    EXPECT_EQ(base::LockFile::check(TEST_FILE_NAME02), base::LockFile::NotLocked);
#endif
  }
  EXPECT_FALSE(base::file_exists(TEST_FILE_NAME01));

  // Clean leftover test files
  base::remove(TEST_FILE_NAME01);
  base::remove(TEST_FILE_NAME02);
}

#if 0
#ifdef _MSC_VER
  // Child thread function
  gpointer _child_thread_func(gpointer data) {
    // Miscellaneous variables
    FILE *test_result = NULL;
    FILE *expected_result = NULL;
    base::FileHandle test_file;

    //  If the tread thows an exception it will SIGABRT the process. So we'll catch the exception
    //  and return gracefully.
    try {
      // Testing rename(const std::string &from, const std::string &to)
      try {
        base::rename(TEST_FILE_NAME01, TEST_FILE_NAME02);
        fail(strfmt("TEST 70.1: Locked file \"%s\" did not throw an error", TEST_FILE_NAME01));
      } catch (const base::file_error &exc) {
        if (0 != std::string(exc.what()).find("Could not rename file ")) {
          fail(strfmt("TEST 70.1: Locked file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
        }
      } catch (std::exception &exc) {
        fail(strfmt("TEST 70.1: Locked file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
      }

      // Testing remove(const std::string &path)
      try {
        base::remove(TEST_FILE_NAME01);
        fail(strfmt("TEST 70.2: Locked file \"%s\" did not throw an error", TEST_FILE_NAME01));
      } catch (const base::file_error &exc) {
        if (0 != std::string(exc.what()).find("Could not delete file ")) {
          fail(strfmt("TEST 70.2: Locked file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
        }
      } catch (std::exception &exc) {
        fail(strfmt("TEST 70.2: Locked file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
      }

      // Testing open_file(const char *filename, const char *mode, bool throw_on_fail= true)

      // test case for 'throw_on_fail' default value (i.e. TRUE)
      try {
        // throw error (file is locked)
        base::FileHandle test_file_scoped(TEST_FILE_NAME01, "w");
        fail(strfmt("TEST 70.3: Locked file \"%s\" did not throw an error", TEST_FILE_NAME01));
      } catch (const base::file_error &exc) {
        if (0 != std::string(exc.what()).find("Failed to open file \"")) {
          fail(strfmt("TEST 70.3: Locked file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
        }
      } catch (std::exception &exc) {
        fail(strfmt("TEST 70.3: Locked file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
      }

      // test case for 'throw_on_fail' FALSE value
      try {
        // doesn't throw error (even if file is locked)
        base::FileHandle test_file_scoped(TEST_FILE_NAME01, "w", false);
        test_result = test_file.file();
        expected_result = NULL;

        ensure_equals("TEST 70.4: Unexpected result calling FileHandle c-tor", test_result, expected_result);
      } catch (std::exception &exc) {
        fail(strfmt("TEST 70.4: Read-only file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
      }

      // Testing FileHandle c-tor
      try {
        base::FileHandle c_tor_test_filename(TEST_FILE_NAME01, "w+");
        fail(strfmt("TEST 70.5: Locked file \"%s\" did not throw an error", TEST_FILE_NAME01));
      } catch (const base::file_error &exc) {
        if (0 != std::string(exc.what()).find("Failed to open file \"")) {
          fail(strfmt("TEST 70.5: Locked file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
        }
      } catch (std::exception &exc) {
        fail(strfmt("TEST 70.5: Locked file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
      }

      // Testing LockFile c-tor
      try {
        base::LockFile lock_file(TEST_FILE_NAME01);
        fail(strfmt("TEST 70.6: Locked file \"%s\" did not throw an error", TEST_FILE_NAME01));
      } catch (const base::file_locked_error) {
        // Nothing to do, just catch the error and continue
      } catch (std::exception &exc) {
        fail(strfmt("TEST 70.6: Locked file \"%s\" threw an unexpected error: %s", TEST_FILE_NAME01, exc.what()));
      }

      // Testing LockFile::check in child thread
      try {
        if (LockFile::check(TEST_FILE_NAME01) != LockFile::LockedSelf)
          fail(strfmt("TEST 70.7: File \"%s\" not locked", TEST_FILE_NAME01));
      } catch (base::file_error &exc) {
        throw grt::os_error(strfmt("File error: %s", exc.what()));
      } catch (std::invalid_argument &exc) {
        throw grt::os_error(strfmt("Invalid argument error: %s", exc.what()));
      } catch (std::runtime_error &exc) {
        throw grt::os_error(strfmt("Runtime/file-locked error: %s", exc.what()));
      }
    } catch (std::exception &exc) {
      fail(exc.what());
    }
    return NULL;
  }

  // Testing public API's
  // - rename(const std::string &from, const std::string &to)
  // - remove(const std::string &path)
  // Testing FileHandle public API's
  // - open_file(const char *filename, const char *mode, bool throw_on_fail= true)
  // Testing FileHandle c-tor
  // -- Multi-threading locking --
  TEST_FUNCTION(70) {
    // Miscellaneous variables
    GThread *_child_thread;

    try {
      // Create file
      base::FileHandle test_file01(TEST_FILE_NAME01, "w+");
    } catch (base::file_error &exc) {
      throw grt::os_error(strfmt("File error: %s", exc.what()));
    }

    // Lock the file
    base::LockFile lock_file(TEST_FILE_NAME01);

    // Testing LockFile::check in main thread
    try {
      if (LockFile::check(TEST_FILE_NAME01) != LockFile::LockedSelf)
        fail(strfmt("TEST 70.8: File \"%s\" not locked", TEST_FILE_NAME01));
    } catch (base::file_error &exc) {
      throw grt::os_error(strfmt("File error: %s", exc.what()));
    } catch (std::invalid_argument &exc) {
      throw grt::os_error(strfmt("Invalid argument error: %s", exc.what()));
    } catch (std::runtime_error &exc) {
      throw grt::os_error(strfmt("Runtime/file-locked error: %s", exc.what()));
    }

    // Kick off the child thread
    _child_thread = base::create_thread(_child_thread_func, this);
    // Wait for _main_thread to finish
    g_thread_join(_child_thread);
  }
#endif
#endif

TEST_F(FileUtilitiesTest, TestsForRelativePath) {
  // .
  EXPECT_EQ(base::relativePath("", ""), "");
  EXPECT_EQ(base::relativePath("/", ""), "");
  EXPECT_EQ(base::relativePath("", "/"), "/");
  EXPECT_EQ(base::relativePath("", "\\"), "\\");
  EXPECT_EQ(base::relativePath("/////////////////////////", "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"), "../../../../../../../../../../");
  EXPECT_EQ(base::relativePath("/abc/def", "\\abc\\def"), "");
  EXPECT_EQ(base::relativePath("/abc/def", "/abc/def/ghi"), "ghi");
  EXPECT_EQ(base::relativePath("/abc/def/ghi", "/abc/def/"), "../");

  // Long names without sub paths.
  std::string basePath(50000, 'x');
  std::string pathToMakeRelative = "y";

  EXPECT_EQ(base::relativePath(basePath, pathToMakeRelative), pathToMakeRelative);
  EXPECT_EQ(base::relativePath(pathToMakeRelative, basePath), basePath);

  // Many (short) subpaths.
  basePath = "";
  for (size_t i = 0; i < 30000; ++i)
    basePath += "/abc";
  EXPECT_EQ(base::relativePath(basePath, ""), "");
  EXPECT_EQ(base::relativePath(basePath, "abc"), "abc");
  EXPECT_EQ(base::relativePath(basePath, basePath), "");

  pathToMakeRelative = "";
  for (size_t i = 0; i < 29999; ++i)
    pathToMakeRelative += "../";
  EXPECT_EQ(base::relativePath(basePath, "/abc"), pathToMakeRelative);

  EXPECT_EQ(base::relativePath("🍏🍎🍐/ЀЁЂ/ᚋᚌᚍ/last", "/last"), "/last");
  EXPECT_EQ(base::relativePath("/🍏🍎🍐/ЀЁЂ\\ᚋᚌᚍ\\last", "\\last"), "../../../../last");
  EXPECT_EQ(base::relativePath("🍏🍎🍐/ЀЁЂ\\ᚋᚌᚍ/last", "🍏🍎🍐"), "../../../");

  // Case sensitivity.
#ifdef _MSC_VER
  EXPECT_EQ(base::relativePath("/🍏🍎🍐/ЀЁЂ\\ᚋᚌᚍ\\last", "\\Last"), "../../../../Last");
  EXPECT_EQ(base::relativePath("/XYZ/🍏🍎🍐/ЀЁЂ\\ᚋᚌᚍ\\last", "\\xyz\\Last"), "../../../../Last");
#else
  EXPECT_EQ(base::relativePath("/🍏🍎🍐/ЀЁЂ\\ᚋᚌᚍ\\last", "\\Last"), "../../../../Last");
  EXPECT_EQ(base::relativePath("/XYZ/🍏🍎🍐/ЀЁЂ\\ᚋᚌᚍ\\last", "\\xyz\\Last"), "../../../../../xyz/Last");
#endif

  // Win specific, but nonetheless working on any platform.
  EXPECT_EQ(base::relativePath("C:\\abc/def/ghi", "C:/abc/def/"), "../");
  EXPECT_EQ(base::relativePath("C:\\abc/def/ghi", "D:/abc/def/"), "D:/abc/def/");
}
}
