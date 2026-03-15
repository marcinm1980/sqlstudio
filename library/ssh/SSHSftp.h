/*
 * Copyright (c) 2017, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#else
#pragma warning(push)
#pragma warning(disable : 4267)
#endif
#include <libssh/libsshpp.hpp>
#include <libssh/sftp.h>
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#else
#pragma warning(pop)
#endif

#include "SSHCommon.h"
#include "SSHSession.h"
#include "base/any.h"
#include <vector>

#if defined(_MSC_VER)
#define __S_IREAD 0400  /* Read by owner.  */
#define __S_IWRITE  0200  /* Write by owner.  */
#define __S_IEXEC 0100  /* Execute by owner.  */

# define S_IRUSR  __S_IREAD       /* Read by owner.  */
# define S_IWUSR  __S_IWRITE      /* Write by owner.  */
# define S_IXUSR  __S_IEXEC       /* Execute by owner.  */
/* Read, write, and execute by owner.  */
# define S_IRWXU  (__S_IREAD|__S_IWRITE|__S_IEXEC)

# define S_IRGRP  (S_IRUSR >> 3)  /* Read by group.  */
# define S_IWGRP  (S_IWUSR >> 3)  /* Write by group.  */
# define S_IXGRP  (S_IXUSR >> 3)  /* Execute by group.  */
/* Read, write, and execute by group.  */
# define S_IRWXG  (S_IRWXU >> 3)

# define S_IROTH  (S_IRGRP >> 3)  /* Read by others.  */
# define S_IWOTH  (S_IWGRP >> 3)  /* Write by others.  */
# define S_IXOTH  (S_IXGRP >> 3)  /* Execute by others.  */
/* Read, write, and execute by others.  */
# define S_IRWXO  (S_IRWXG >> 3)
#else
#include <sys/stat.h>
#endif

namespace ssh {
  struct SftpStatAttrib {
    uint64_t size;
    uint32_t uid;
    uint32_t gid;
    uint64_t atime;
    uint64_t mtime;
    std::string name;
    bool isDir;
  };

  class WBSSHLIBRARY_PUBLIC_FUNC SSHSftp {
    std::shared_ptr<SSHSession> _session;
    sftp_session _sftp;
    std::size_t _maxFileLimit;
    std::vector<std::string> _path;
  public:
    SSHSftp(std::shared_ptr<SSHSession> session, std::size_t maxFileSize);
    virtual ~SSHSftp();

    auto open(const std::string &path) const -> sftp_file;
    auto mkdir(const std::string &dirname, unsigned int mode = S_IRWXU) -> void;
    auto rmdir(const std::string &dirname) -> void;
    auto unlink(const std::string &file) -> void;
    auto stat(const std::string &path) -> SftpStatAttrib;
    auto get(const std::string &src, const std::string &dest) const -> void;
    auto setContent(const std::string &path, const std::string &data) const -> void;
    auto put(const std::string &src, const std::string &dest) const -> void;
    auto getContent(const std::string &src) const -> std::string;
    auto setMaxFileLimit(std::size_t limit) -> void;
    auto cd(const std::string &dirname) -> int;
    auto ls(const std::string &dirname) const -> std::vector<SftpStatAttrib>;
    auto pwd() const -> std::string;
    auto fileExists(const std::string &path) const -> bool;

  protected:
    SSHSftp(const SSHSftp& ses) = delete;
    SSHSftp(const SSHSftp&& ses) = delete;
    SSHSftp &operator =(SSHSftp&) = delete;
    auto throwOnError(int rc) const -> void;
    auto createRemotePath(const std::string &path) const -> std::string;

  };

} /* namespace ssh */
