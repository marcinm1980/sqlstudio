#include "base/string_utilities.h"
#include "base/file_utilities.h"
#include "base/file_functions.h"

#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#endif
#include <algorithm>
#include <filesystem>
#include <regex>

//--------------------------------------------------------------------------------------------------

namespace base {

  std::string format_file_error(const std::string &text, int err) {
#ifdef _MSC_VER
    return strfmt("%s: error code %i", text.c_str(), err);
#else
    return strfmt("%s: %s", text.c_str(), strerror(err));
#endif
  }

  file_error::file_error(const std::string &text, int err)
    : std::runtime_error(format_file_error(text, err)), sys_error_code(err) {
  }

  error_code file_error::code() {
#ifdef _MSC_VER
    switch (sys_error_code) {
      case 0:
        return success;
      case ERROR_FILE_NOT_FOUND:
      case ERROR_PATH_NOT_FOUND:
        return file_not_found;
      case ERROR_ALREADY_EXISTS:
        return already_exists;
      case ERROR_ACCESS_DENIED:
        return access_denied;
      default:
        return other_error;
    }
#else
    switch (sys_error_code) {
      case 0:
        return success;
      case ENOENT:
        return file_not_found;
      case EEXIST:
        return already_exists;
      case EACCES:
        return access_denied;
      default:
        return other_error;
    }
#endif
  }

  //--------------------------------------------------------------------------------------------------

  std::list<std::string> scan_for_files_matching(const std::string &pattern, bool recursive) {
    std::list<std::string> matches;

    std::filesystem::path path = std::filesystem::path(pattern).parent_path();
    if (!std::filesystem::exists(path)) {
      return matches;
    }

    std::string pure_pattern = std::filesystem::path(pattern).filename().string();
    // Convert the pattern to a regex
    std::string regex_pattern = std::regex_replace(pure_pattern, std::regex(R"(\*)"), ".*");
    regex_pattern = std::regex_replace(regex_pattern, std::regex(R"(\?)"), ".");
    std::regex file_regex(regex_pattern);

    try {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
        const auto& filename = entry.path().filename().string();
        if (std::regex_match(filename, file_regex)) {
          matches.push_back(entry.path().string());
        }

        if (recursive && entry.is_directory()) {
          std::string subpattern = (entry.path() / pure_pattern).string();
          std::list<std::string> submatches = scan_for_files_matching(subpattern, true);
          matches.insert(matches.end(), submatches.begin(), submatches.end());
        }
      }
    }
    catch (const std::filesystem::filesystem_error& e) {
      throw std::runtime_error("Error accessing directory: " + std::string(e.what()));
    }

    return matches;
  }

//--------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  LockFile::LockFile(const std::string &path)
    : path(path), handle(0) {
    std::wstring wpath(string_to_wstring(path));
    if (path.empty())
      throw std::invalid_argument("invalid path");

    handle = CreateFileW(wpath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
      if (GetLastError() == ERROR_SHARING_VIOLATION)
        throw file_locked_error("File already locked");
      throw std::runtime_error(strfmt("Error creating lock file (%i)", GetLastError()));
    }

    char buffer[32];
    sprintf_s(buffer, "%i", GetCurrentProcessId());
    DWORD bytes_written;

    if (!WriteFile(handle, buffer, (DWORD)strlen(buffer), &bytes_written, NULL) || bytes_written != strlen(buffer)) {
      CloseHandle(handle);
      DeleteFileW(wpath.c_str());
      throw std::runtime_error("Could not write to lock file");
    }
  }

  LockFile::~LockFile() {
    if (handle)
      CloseHandle(handle);
    DeleteFileW(string_to_wstring(path).c_str());
  }

  LockFile::LockStatus LockFile::check(const std::string &path) {
    // Can we open the file in exclusive mode?
    std::wstring wpath = string_to_wstring(path);
    HANDLE h = CreateFile(wpath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (h != INVALID_HANDLE_VALUE) {
      CloseHandle(h);
      return NotLocked;
    }

    switch (GetLastError()) {
      case ERROR_SHARING_VIOLATION:
        // If file cannot be opened for writing it's open somewhere else.
        // Try to open it and read the first bytes to see if that corresponds to our process id.
        h = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) {
          char buffer[32];
          DWORD bytes_read;
          if (ReadFile(h, buffer, sizeof(buffer), &bytes_read, NULL)) {
            CloseHandle(h);
            buffer[bytes_read] = 0;
            if (base::atoi<int>(buffer, -1) != GetCurrentProcessId())
              return LockedOther; // TODO: this unreliable. Any non-lock file would qualify here (which is wrong).
            return LockedSelf;
          }
          CloseHandle(h);
          return LockedOther;
        }

        // If the file cannot be read assume its locked by another process.
        return LockedOther;

      case ERROR_FILE_NOT_FOUND:
        return NotLocked;

      case ERROR_PATH_NOT_FOUND:
        throw std::invalid_argument("Invalid path");

      default:
        throw std::runtime_error(
          strfmt("Unknown error while checking file %s for locks (%i)", path.c_str(), GetLastError()));
    }
  }
#else
  LockFile::LockFile(const std::string &apath) : path(apath) {
    if (path.empty())
      throw std::invalid_argument("invalid path");

    fd = open(path.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
      // this could mean lock exists, that it's a dangling file or that it's currently being locked by some other
      // process/thread
      // we just go on and try to lock the file if the file already exists
      if (errno == ENOENT || errno == ENOTDIR)
        throw std::invalid_argument("invalid path");

      throw std::runtime_error(strfmt("%s creating lock file", g_strerror(errno)));
    }
    if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
      close(fd);
      fd = -1;
      if (errno == EWOULDBLOCK)
        throw file_locked_error("file already locked");

      throw std::runtime_error(strfmt("%s while locking file", g_strerror(errno)));
    }

    if (ftruncate(fd, 0)) {
      close(fd);
      fd = -1;
      throw std::runtime_error(strfmt("%s while truncating file", g_strerror(errno)));
    }

    char pid[32];
    snprintf(pid, sizeof(pid), "%i", getpid());
    if (write(fd, pid, strlen(pid) + 1) < 0) {
      close(fd);
      throw std::runtime_error(strfmt("%s while locking file", g_strerror(errno)));
    }
  }

  LockFile::~LockFile() {
    if (fd >= 0)
      close(fd);
    unlink(path.c_str());
  }

  LockFile::LockStatus LockFile::check(const std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0)
      return NotLocked;

    if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
      char pid[32];
      // couldn't lock file, check if we own it ourselves
      ssize_t c = read(fd, pid, sizeof(pid) - 1);
      close(fd);
      if (c < 0)
        return LockedOther;
      pid[c] = 0;
      if (base::atoi<int>(pid, -1) != getpid())
        return LockedOther;
      return LockedSelf;
    } else // nobody holds a lock on the file, so this is a leftover
    {
      flock(fd, LOCK_UN);
      close(fd);
      return NotLocked;
    }
  }
#endif

  bool create_directory(const std::string &path, int mode, bool with_parents) {
#ifdef _MSC_VER
    SetLastError(0);
    if (!CreateDirectoryW(path_from_utf8(path).c_str(), NULL)) {
      DWORD error = GetLastError();
      if (error == ERROR_ALREADY_EXISTS)
        return false;

      if (with_parents) {
        if (error == ERROR_PATH_NOT_FOUND) {
          // create parents too
          std::string tmp = path;
          std::list<std::string> stack;
          while (!tmp.empty() && !file_exists(tmp)) {
            stack.push_back(tmp);
            tmp = dirname(tmp);
          }
          if (tmp.empty()) // path is invalid
            throw file_error(strfmt("Could not create directory %s", path.c_str()), ERROR_PATH_NOT_FOUND);

          while (!stack.empty()) {
            if (!CreateDirectoryW(path_from_utf8(stack.back()).c_str(), NULL))
              throw file_error(strfmt("Could not create directory %s", path.c_str()), GetLastError());
            stack.pop_back();
          }
          return true;
        }
      }

      throw file_error(strfmt("Could not create directory %s", path.c_str()), GetLastError());
    }
#else
    if (with_parents) {
      if (g_mkdir_with_parents(path_from_utf8(path).c_str(), mode) < 0)
        throw file_error(strfmt("Could not create directory %s", path.c_str()), errno);
    } else {
      // do not use g_mkdir_with_parents here, as we rely on the specific behaviour of g_mkdir (g_mkdir_with_parents
      // doesnt fail if dir exists)
      if (g_mkdir(path_from_utf8(path).c_str(), mode) < 0) {
        if (errno == EEXIST)
          return false;
        throw file_error(strfmt("Could not create directory %s", path.c_str()), errno);
      }
    }
#endif
    return true;
  }

  //--------------------------------------------------------------------------------------------------------------------

  bool copyDirectoryRecursive(const std::string& src, const std::string& dst, bool includeFiles) {
    namespace fs = std::filesystem;

    try {
      // Check if the source directory exists
      if (!fs::exists(src) || !fs::is_directory(src)) {
        throw std::runtime_error("Source directory does not exist or is not a directory: " + src);
      }

      // Create the destination directory if it doesn't exist
      if (!fs::exists(dst)) {
        fs::create_directories(dst);
      }

      // Iterate through the source directory
      for (const auto& entry : fs::directory_iterator(src)) {
        const auto& entryPath = entry.path();
        auto relativePath = fs::relative(entryPath, src);
        auto destinationPath = fs::path(dst) / relativePath;

        if (entry.is_directory()) {
          // Recursively copy subdirectories
          copyDirectoryRecursive(entryPath.string(), destinationPath.string(), includeFiles);
        }
        else if (includeFiles && entry.is_regular_file()) {
          // Copy files if includeFiles is true
          fs::copy(entryPath, destinationPath, fs::copy_options::overwrite_existing);
        }
      }
    }
    catch (const fs::filesystem_error& e) {
      throw std::runtime_error("Error copying directory: " + std::string(e.what()));
    }

    return true;
  }

  //--------------------------------------------------------------------------------------------------------------------

  std::wifstream openTextInputStream(const std::string &fileName) {
    std::wifstream result;

#ifdef _MSC_VER
    result.open(base::string_to_wstring(fileName));
#else
    result.open(fileName);
#endif

    return result;
  }

  //--------------------------------------------------------------------------------------------------------------------

  std::wofstream openTextOutputStream(const std::string &fileName) {
    std::wofstream result;

#ifdef _MSC_VER
    result.open(base::string_to_wstring(fileName));
#else
    result.open(fileName);
#endif

    return result;
  }

  //--------------------------------------------------------------------------------------------------------------------

  std::ifstream openBinaryInputStream(const std::string &fileName) {
    std::ifstream result;

#ifdef _MSC_VER
    result.open(base::string_to_wstring(fileName), std::ios_base::in | std::ios_base::binary);
#else
    result.open(fileName, std::ios_base::in | std::ios_base::binary);
#endif

    return result;
  }

  //--------------------------------------------------------------------------------------------------------------------

  std::ofstream openBinaryOutputStream(const std::string &fileName) {
    std::ofstream result;

#ifdef _MSC_VER
    result.open(base::string_to_wstring(fileName), std::ios_base::out | std::ios_base::binary);
#else
    result.open(fileName, std::ios_base::out | std::ios_base::binary);
#endif

    return result;
  }

  //--------------------------------------------------------------------------------------------------------------------

  bool copyFile(const std::string &source, const std::string &target) {
    std::ifstream src = openBinaryInputStream(source);
    if (src.bad())
      return false;
    std::ofstream dst = openBinaryOutputStream(target);
    if (dst.bad())
      return false;
    dst << src.rdbuf();

    return true;
  }

  //--------------------------------------------------------------------------------------------------------------------

  void rename(const std::string &from, const std::string &to) {
#ifdef _MSC_VER
    if (!MoveFile(path_from_utf8(from).c_str(), path_from_utf8(to).c_str()))
      throw file_error(strfmt("Could not rename file %s to %s", from.c_str(), to.c_str()), GetLastError());
#else
    if (::g_rename(path_from_utf8(from).c_str(), path_from_utf8(to).c_str()) < 0)
      throw file_error(strfmt("Could not rename file %s to %s", from.c_str(), to.c_str()), errno);
#endif
  }

  //--------------------------------------------------------------------------------------------------------------------

  bool remove_recursive(const std::string& path) {
    namespace fs = std::filesystem;

    try {
      // Check if the path exists
      if (!fs::exists(path)) {
        return false; // Path does not exist
      }

      // Remove the directory or file recursively
      fs::remove_all(path);
      return true;
    }
    catch (const fs::filesystem_error& e) {
      throw std::runtime_error("Error removing path: " + std::string(e.what()));
    }
  }

  //--------------------------------------------------------------------------------------------------------------------

  /**
   * Deletes file or folder.
   * Returns false if the object doesn't exist and throws an exception on error.
   */
  bool remove(const std::string &path) {
#ifdef _MSC_VER
    if (is_directory(path)) {
      if (!RemoveDirectoryW(path_from_utf8(path).c_str())) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND)
          return false;
        throw file_error(strfmt("Could not delete file %s", path.c_str()), GetLastError());
      }
    } else {
      if (!DeleteFileW(path_from_utf8(path).c_str())) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
          return false;
        throw file_error(strfmt("Could not delete file %s", path.c_str()), GetLastError());
      }
    }
#else
    if (::g_remove(path_from_utf8(path).c_str()) < 0) {
      if (errno == ENOENT)
        return false;
      throw file_error(strfmt("Could not delete file %s", path.c_str()), errno);
    }
#endif
    return true;
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Tries to delete a file or folder.
   * No exception is thrown if that fails. Returns true on success, otherwise false.
   */
  bool tryRemove(const std::string &path) {
#ifdef _MSC_VER
    if (is_directory(path))
      return RemoveDirectory(path_from_utf8(path).c_str()) == TRUE;
    else
      return DeleteFile(path_from_utf8(path).c_str()) == TRUE;
#else
    return ::g_remove(path_from_utf8(path).c_str()) == 0;
#endif
  }

  //--------------------------------------------------------------------------------------------------

  bool file_exists(const std::string& path) {
    namespace fs = std::filesystem;
    return fs::exists(path);
  }

  //--------------------------------------------------------------------------------------------------

  bool is_directory(const std::string& path) {
    namespace fs = std::filesystem;
    return fs::is_directory(path);
  }

  //--------------------------------------------------------------------------------------------------

  std::string extension(const std::string &path) {
    std::string::size_type p = path.rfind('.');
    if (p != std::string::npos) {
      std::string ext(path.substr(p));
      if (ext.find('/') != std::string::npos || ext.find('\\') != std::string::npos)
        return "";
      return ext;
    }
    return "";
  }

  //--------------------------------------------------------------------------------------------------

  std::string appendExtensionIfNeeded(const std::string &path, const std::string &ext) {
    if (!base::hasSuffix(path, ext))
      return path + ext;
    return path;
  }

  //--------------------------------------------------------------------------------------------------

  std::string dirname(const std::string& path) {
    namespace fs = std::filesystem;
    return fs::path(path).parent_path().string();
  }

  //--------------------------------------------------------------------------------------------------

  std::string basename(const std::string& path) {
    namespace fs = std::filesystem;
    return fs::path(path).filename().string();
  }

  //--------------------------------------------------------------------------------------------------

  std::string strip_extension(const std::string &path) {
    std::string ext;
    if (!(ext = extension(path)).empty()) {
      return path.substr(0, path.size() - ext.size());
    }
    return path;
  }

  //--------------------------------------------------------------------------------------------------

  FileHandle::FileHandle(const std::string &filename, const char* mode, bool throwOnFail) : _file(nullptr) {
    _file = base_fopen(filename.c_str(), mode);
    if (!_file && throwOnFail)
      throw file_error(std::string("Failed to open file \"").append(filename).append("\""), errno);
    _path = filename;
  }

  //--------------------------------------------------------------------------------------------------

  FileHandle &FileHandle::operator=(FileHandle &fh) {
    dispose();
    swap(fh);
    return *this;
  }

  //--------------------------------------------------------------------------------------------------

  FileHandle &FileHandle::operator=(FileHandle &&fh) {
    dispose();
    swap(fh);
    return *this;
  }

  //--------------------------------------------------------------------------------------------------

  std::string FileHandle::getPath() const {
    return _path;
  }

  //--------------------------------------------------------------------------------------------------

  void FileHandle::swap(FileHandle &fh) {
    std::swap(_file, fh._file);
    _path = std::move(fh._path);
  }

  //--------------------------------------------------------------------------------------------------

  void FileHandle::dispose() {
    if (_file) {
      ::fclose(_file);
      _file = NULL;
      _path = "";
    }
  }

  //--------------------------------------------------------------------------------------------------

  /**
   * Returns the last modification time of the given file.
   */
  bool file_mtime(const std::string &path, time_t &mtime) {
#ifdef _MSC_VER
    struct _stat stbuf;
#else
    struct stat stbuf;
#endif

    if (base_stat(path.c_str(), &stbuf) == 0) {
#ifdef __APPLE__
      mtime = (time_t)stbuf.st_mtimespec.tv_sec;
#else
      mtime = stbuf.st_mtime;
#endif
      return true;
    }
    return false;
  }

  std::string makePath(const std::string& prefix, const std::string& file) {
    if (prefix.empty())
      return file;

    if (prefix[prefix.size() - 1] == '/' || prefix[prefix.size() - 1] == '\\')
      return prefix + file;
    return (std::filesystem::path(prefix) / file).string();
  }

  //----------------------------------------------------------------------------------------------------

  std::string joinPath(const char* prefix, ...) {
    std::filesystem::path result = prefix;

    va_list ap;
    va_start(ap, prefix);
    const char* arg = nullptr;

    while ((arg = va_arg(ap, const char*)) != nullptr) {
      result /= arg; // Use the `/` operator to append paths
    }
    va_end(ap);

    return result.string(); // Convert the resulting path back to a string
  }

  //----------------------------------------------------------------------------------------------------

  /**
   * Returns the second path relative to the given base path, provided both have a common ancestor.
   * If not then the second path is return unchanged.
   * Paths can contain both forward and backward slash separators. The result only uses backslashes.
   * Folder names are compared case insensitively on Windows, otherwise case matters.
   */
  std::string relativePath(const std::string &basePath, const std::string &pathToMakeRelative) {
    std::vector<std::string> basePathList = split_by_set(basePath, "/\\");
    std::vector<std::string> otherPathList = split_by_set(pathToMakeRelative, "/\\");

#ifdef _MSC_VER
    bool caseSensitive = false;
#else
    bool caseSensitive = true;
#endif

    size_t totalDepth = std::min(basePathList.size(), otherPathList.size());
    size_t commonDepth = 0;
    for (size_t i = 0; i < totalDepth; ++i, ++commonDepth) {
      if (!same_string(basePathList[i], otherPathList[i], caseSensitive))
        break;
    }

    if (commonDepth == 0)
      return pathToMakeRelative;

    std::string result;
    for (size_t i = 0; i < basePathList.size() - commonDepth; ++i)
      result += "../";

    for (size_t i = commonDepth; i < otherPathList.size(); ++i) {
      result += otherPathList[i];
      if (i < otherPathList.size() - 1)
        result += "/";
    }

    return result;
  }

  //--------------------------------------------------------------------------------------------------------------------

  FileHandle makeTmpFile(const std::string& prefix) {
    std::string tmp = prefix;

#ifdef _MSC_VER
    wchar_t tempPathBuffer[MAX_PATH] = { 0 };
    if (GetTempPath(MAX_PATH, tempPathBuffer) == 0) {
      throw std::runtime_error("Failed to retrieve temporary path.");
    }

    wchar_t tempFileName[MAX_PATH] = { 0 };
    if (GetTempFileName(tempPathBuffer, L"wb_", 0, tempFileName) == 0) {
      throw std::runtime_error("Failed to create temporary file name.");
    }

    tmp = base::wstring_to_string(tempFileName);
#else
    tmp.append("XXXXXX");
    int fd = mkstemp(&tmp[0]);
    if (fd == -1) {
      throw std::runtime_error("Failed to create temporary file.");
    }
    close(fd);
#endif

    return FileHandle(tmp, "w+");
  }

  //--------------------------------------------------------------------------------------------------------------------

  const char* getPreferredSeparatorAsCString() {
    static const char separator[] = { std::filesystem::path::preferred_separator, '\0' };
    return separator;
  }

  //--------------------------------------------------------------------------------------------------------------------

  std::string pathlistAppend(const std::string &l, const std::string &s) {
    if (l.empty())
      return s;
    return l + getPreferredSeparatorAsCString() + s;
  }

  //--------------------------------------------------------------------------------------------------------------------

  std::string pathlistPrepend(const std::string &l, const std::string &s) {
    if (l.empty())
      return s;
    return s + getPreferredSeparatorAsCString() + l;
  }

  //--------------------------------------------------------------------------------------------------------------------

  std::string cwd() {
#ifdef _MSC_VER
    wchar_t widePath[FILENAME_MAX + 1];
    ::_wgetcwd(widePath, FILENAME_MAX);

    return normalize_path(wstring_to_string(widePath));

#else
    char currentPath[FILENAME_MAX + 1];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    ::getcwd(currentPath, FILENAME_MAX);

    return currentPath;
#pragma GCC diagnostic pop
#endif
  }

  //--------------------------------------------------------------------------------------------------------------------

};
