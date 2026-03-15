/*
 * Copyright (c) 2011, 2019, Oracle and/or its affiliates. All rights reserved.
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

#pragma once

#include "common.h"
#include <list>
#include <string>
#include <stdexcept>

namespace base {
  enum error_code { success = 0, file_not_found = -1, already_exists = -2, access_denied = -3, other_error = -1000 };

#ifdef _MSC_VER
#pragma warning(disable : 4275) // non dll-interface class used as base dll-interface class.
#endif

  class BASELIBRARY_PUBLIC_FUNC file_error : public std::runtime_error {
    int sys_error_code;

  public:
    file_error(const std::string &text, int err);

    auto code() -> error_code;
    auto sys_code() -> int;
  };

  BASELIBRARY_PUBLIC_FUNC auto scan_for_files_matching(const std::string &pattern,
                                                                         bool recursive = false) -> std::list<std::string>;

  class BASELIBRARY_PUBLIC_FUNC file_locked_error : public std::runtime_error {
  public:
    file_locked_error(const std::string &msg) : std::runtime_error(msg) {
    }
  };

  struct BASELIBRARY_PUBLIC_FUNC LockFile {
#ifdef _MSC_VER
#pragma warning(disable : 4251) // DLL interface required for std::string member.
#pragma warning(disable : 4290) // C++ exception specification ignored.
    HANDLE handle;
#else
    int fd;
#endif

    std::string path;
    enum LockStatus {
      LockedSelf,  // lock file exists and is the process itself
      LockedOther, // lock file exists and its owner is running
      NotLocked,
    };

    LockFile(const std::string &path);
    ~LockFile();
#undef check // there's a #define check in osx
    static auto check(const std::string &path) -> LockStatus;
  };

  class BASELIBRARY_PUBLIC_FUNC FileHandle {
    FILE *_file;
    std::string _path;
  public:
    FileHandle() : _file(NULL) {
    }
    FileHandle(const std::string &filename, const char* mode, bool throwOnFail = true);
    FileHandle(FileHandle &fh) : _file(NULL) {
      swap(fh);
    }

    FileHandle(FileHandle &&fh) : _file(nullptr) {
      swap(fh);
    }
    ~FileHandle() {
      dispose();
    }

    auto getPath() const -> std::string;
    auto swap(FileHandle &fh) -> void;
    operator bool() const {
      return (!_file);
    }
    auto operator=(FileHandle &fh) -> FileHandle &; // will pass ownership of FILE from assigned obj to this
    auto operator=(FileHandle &&fh) -> FileHandle &;
    //  NOTE: Never close this handle, because it's managed by the FileHandle class.
    auto file() -> FILE * {
      return _file;
    }
    auto dispose() -> void;
  };

  // creates the directory, returns false if the directory exists.. throws exception on error
  BASELIBRARY_PUBLIC_FUNC auto create_directory(const std::string &path, int mode, bool with_parents = false) -> bool;
  BASELIBRARY_PUBLIC_FUNC auto copyDirectoryRecursive(const std::string &src, const std::string &dest,
                                                      bool includeFiles = true) -> bool; // Obsolete with C++17

  BASELIBRARY_PUBLIC_FUNC auto openTextInputStream(const std::string &fileName) -> std::wifstream;
  BASELIBRARY_PUBLIC_FUNC auto openTextOutputStream(const std::string &fileName) -> std::wofstream;

  BASELIBRARY_PUBLIC_FUNC auto openBinaryInputStream(const std::string &fileName) -> std::ifstream;
  BASELIBRARY_PUBLIC_FUNC auto openBinaryOutputStream(const std::string &fileName) -> std::ofstream;

  BASELIBRARY_PUBLIC_FUNC auto copyFile(const std::string &src, const std::string &dest) -> bool; // Obsolete with C++17

  BASELIBRARY_PUBLIC_FUNC auto remove(const std::string &path) -> bool;
  BASELIBRARY_PUBLIC_FUNC auto tryRemove(const std::string &path) -> bool;

  BASELIBRARY_PUBLIC_FUNC auto remove_recursive(const std::string &path) -> bool;

  BASELIBRARY_PUBLIC_FUNC auto rename(const std::string &from, const std::string &to) -> void;

  BASELIBRARY_PUBLIC_FUNC auto file_exists(const std::string &path) -> bool;
  BASELIBRARY_PUBLIC_FUNC auto is_directory(const std::string &path) -> bool;

  // file.ext -> .ext
  BASELIBRARY_PUBLIC_FUNC auto extension(const std::string &path) -> std::string;
  // returns path.ext (if path has no ext, it will add it)
  BASELIBRARY_PUBLIC_FUNC auto appendExtensionIfNeeded(const std::string &path, const std::string &ext) -> std::string;
  // returns . if no dirname in path
  BASELIBRARY_PUBLIC_FUNC auto dirname(const std::string &path) -> std::string;
  // returns . if no filename in path
  BASELIBRARY_PUBLIC_FUNC auto basename(const std::string &path) -> std::string;

  // file.ext -> file
  BASELIBRARY_PUBLIC_FUNC auto strip_extension(const std::string &path) -> std::string;
  BASELIBRARY_PUBLIC_FUNC auto file_mtime(const std::string &path, time_t &mtime) -> bool;

  BASELIBRARY_PUBLIC_FUNC auto joinPath(const char *prefix, ...) -> std::string;
  BASELIBRARY_PUBLIC_FUNC auto makePath(const std::string &prefix, const std::string &file) -> std::string;
  BASELIBRARY_PUBLIC_FUNC auto relativePath(const std::string &basePath, const std::string &pathToMakeRelative) -> std::string;
  BASELIBRARY_PUBLIC_FUNC auto makeTmpFile(const std::string &prefix) -> FileHandle;

  BASELIBRARY_PUBLIC_FUNC auto pathlistAppend(const std::string &l, const std::string &s) -> std::string;
  BASELIBRARY_PUBLIC_FUNC auto pathlistPrepend(const std::string &l, const std::string &s) -> std::string;

  BASELIBRARY_PUBLIC_FUNC auto cwd() -> std::string;
};
