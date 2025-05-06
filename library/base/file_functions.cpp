/*
 * Copyright (c) 2005, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "base/common.h"

#ifndef _MSC_VER
#include <errno.h>
#include <sys/file.h>
#endif

#include <filesystem>

#include "base/file_functions.h"
#include "base/string_utilities.h"

//#include <glib/gstdio.h>

using namespace base;

//--------------------------------------------------------------------------------------------------

/**
 * @brief Wrapper around fopen that expects a filename in UTF-8 encoding
 * @param filename name of file to open
 * @param mode second argument of fopen
 * @return If successful, base_fopen returns opened FILE*.
 *           Otherwise, it returns NULL.
 */
FILE *base_fopen(const char *filename, const char *mode) {
#ifdef _MSC_VER
  std::wstring wmode;
  while (*mode != '\0')
    wmode += *mode++;
  if (wmode.find_first_of(L"b") == std::wstring::npos && wmode.find_first_of(L"t") == std::wstring::npos)
    wmode += L"b"; // Always open in binary mode.
  return _wfsopen(string_to_wstring(filename).c_str(), wmode.c_str(), _SH_DENYWR);

#else

  FILE *file;
  char *local_filename;

  if (!(local_filename = g_filename_from_utf8(filename, -1, NULL, NULL, NULL)))
    return NULL;

  file = fopen(local_filename, mode);

  g_free(local_filename);

  return file;
#endif
}

//--------------------------------------------------------------------------------------------------

/**
 *	Similar to base_fopen but returns a file descriptor instead. The file is always opened in binary
 *	mode (only matters on Windows).
 *	Also here, the filename must be UTF-8 encoded.
 */
int base_open(const std::string &filename, int open_flag, int permissions) {
  int fd;

#ifdef _MSC_VER
  int result = _wsopen_s(&fd, string_to_wstring(filename).c_str(), open_flag | O_BINARY, _SH_DENYWR, permissions);
  if (result != 0)
    return -1;
#else
  char *local_filename = g_filename_from_utf8(filename.c_str(), -1, NULL, NULL, NULL);
  if (local_filename == NULL)
    return -1;

  fd = open(local_filename, open_flag, permissions);
  g_free(local_filename);

#endif

  return fd;
}

//--------------------------------------------------------------------------------------------------

int base_remove(const std::string &filename) {
#ifdef _MSC_VER
  return _wremove(string_to_wstring(filename).c_str());
#else
  char *local_filename;
  if (!(local_filename = g_filename_from_utf8(filename.c_str(), -1, NULL, NULL, NULL)))
    return -1;
  int res = remove(local_filename);
  g_free(local_filename);

  return res;
#endif
}

//--------------------------------------------------------------------------------------------------

int base_rename(const char *oldname, const char *newname) {
#ifdef _MSC_VER
  std::vector<WCHAR> converted_old_vec;
  std::vector<WCHAR> converted_new_vec;

  int required = MultiByteToWideChar(CP_UTF8, 0, oldname, -1, NULL, 0);
  if (required == 0)
    return -1;

  converted_old_vec.resize(required);
  MultiByteToWideChar(CP_UTF8, 0, oldname, -1, &converted_old_vec[0], required);

  required = MultiByteToWideChar(CP_UTF8, 0, newname, -1, NULL, 0);
  if (required == 0) {
    return -1;
  }

  MultiByteToWideChar(CP_UTF8, 0, newname, -1, &converted_new_vec[0], required);

  return _wrename(&converted_old_vec[0], &converted_new_vec[0]);

#else

  char *local_oldname;
  char *local_newname;

  if (!(local_oldname = g_filename_from_utf8(oldname, -1, NULL, NULL, NULL)) ||
      !(local_newname = g_filename_from_utf8(newname, -1, NULL, NULL, NULL)))
    return EINVAL;

  const int file = rename(local_oldname, local_newname);

  g_free(local_oldname);
  g_free(local_newname);

  return file;
#endif
}

//--------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
int base_stat(const char *filename, struct _stat *stbuf) {
  // Convert filename from UTF-8 to UTF-16.
  std::vector<WCHAR> converted_vec;
  int required = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
  if (required == 0)
    return -1;
  // Required contains the length for the result string including the terminating 0.
  converted_vec.resize(required);
  MultiByteToWideChar(CP_UTF8, 0, filename, -1, &converted_vec[0], required);

  return _wstat(&converted_vec[0], stbuf);
}
#else
int base_stat(const char *filename, struct stat *stbuf) {
  return g_stat(filename, stbuf);
}
#endif

//--------------------------------------------------------------------------------------------------

int base_rmdir_recursively(const char* path) {
  try {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
      if (entry.is_directory()) {
        base_rmdir_recursively(entry.path().string().c_str());
      }
      else {
        std::filesystem::remove(entry.path());
      }
    }
    std::filesystem::remove(path);
    return 0;
  }
  catch (const std::filesystem::filesystem_error& /*e*/) {
    // @@FIXMEE error handling
    return -1;
  }
}

//--------------------------------------------------------------------------------------------------

/**
 * Returns the size of the specified file (if it exists and can be accessed, otherwise 0).
 */
long base_get_file_size(const char *filename) {
  long result = 0;

#ifdef _MSC_VER
  struct _stat file_stat;
  if (base_stat(filename, &file_stat) == 0)
    result = file_stat.st_size;
#else
  struct stat file_stat;
  if (base_stat(filename, &file_stat) == 0)
    result = file_stat.st_size;
#endif

  return result;
}

//--------------------------------------------------------------------------------------------------
