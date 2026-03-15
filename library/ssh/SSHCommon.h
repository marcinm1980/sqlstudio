/*
 * Copyright (c) 2017, 2019, Oracle and/or its affiliates. All rights reserved.
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

#ifdef _WIN32
#pragma warning(disable : 4251) // class needs to have dll-interface
#pragma warning(disable : 4275) // non dll-interface class used as base dll-interface class.
#ifdef WBSSHLIBRARY_EXPORTS
#define WBSSHLIBRARY_PUBLIC_FUNC __declspec(dllexport)
#else
#define WBSSHLIBRARY_PUBLIC_FUNC __declspec(dllimport)
#endif
#else
#define WBSSHLIBRARY_PUBLIC_FUNC
#endif

#include <errno.h>
#include <string>
#include <cstring>
#include <exception>
#include <thread>
#include <atomic>
#include <mutex>

#ifndef _MSC_VER
  #include <poll.h>
#endif

#include <libssh/callbacks.h>

#include "base/threading.h"

#ifndef NOEXCEPT
  #if defined(_MSC_VER)
    #define NOEXCEPT noexcept
  #elif defined(__APPLE__)
    #define NOEXCEPT _NOEXCEPT
  #else
    #ifndef _GLIBCXX_USE_NOEXCEPT
      #define NOEXCEPT throw()
    #else
      #define NOEXCEPT _GLIBCXX_USE_NOEXCEPT
    #endif
  #endif
#endif

#ifdef _MSC_VER
  typedef int socklen_t;
#endif

struct ssh_threads_callbacks_struct * ssh_threads_get_std_threads(void);
auto sshLogCallback(int priority, const char *function, const char *buffer, void *userdata) -> void;

namespace ssh {

  inline auto wbCloseSocket(int socket) -> void {
#if _MSC_VER
    closesocket(socket);
#else
    close(socket);
#endif
  }

  inline auto wbPoll(pollfd *data, size_t size) -> int {
#if _MSC_VER
    return WSAPoll(data, static_cast<ULONG>(size), -1);
#else
    return poll(data, static_cast<nfds_t>(size), -1);
#endif
  }

  const std::size_t LOG_SIZE_100MB = 104857600;
  static std::once_flag sshInitOnce;
  auto getError() -> std::string;
  auto getSftpErrorDescription(int rc) -> std::string;
  auto setSocketNonBlocking(int sock) -> void;
  auto initLibSSH() -> void;

  class WBSSHLIBRARY_PUBLIC_FUNC SSHConnectionConfig {
  public:
    std::string localhost;
    int localport;
    ssize_t bufferSize;
    std::string remoteSSHhost;
    std::size_t remoteSSHport;
    std::string remotehost;
    int remoteport;
    bool strictHostKeyCheck;
    int compressionLevel;
    std::string fingerprint;
    std::string configFile;
    std::string knownHostsFile;
    std::string optionsDir;
    std::size_t connectTimeout;
    std::size_t readWriteTimeout;
    std::size_t commandTimeout;
    std::size_t commandRetryCount;

    SSHConnectionConfig();

    auto getServer() -> std::string {
      return remoteSSHhost + ":" + std::to_string(remoteSSHport);
    }

    auto dumpConfig() const -> void;
    friend bool operator==(const SSHConnectionConfig &tun1, const SSHConnectionConfig &tun2);
    friend bool operator!=(const SSHConnectionConfig &tun1, const SSHConnectionConfig &tun2);
  };

  enum class SSHFingerprint {
    STORE,
    REJECT
  };

  enum class SSHReturnType {
    CONNECTION_FAILURE,
    CONNECTED,
    INVALID_AUTH_DATA,
    FINGERPRINT_MISMATCH,
    FINGERPRINT_CHANGED,
    FINGERPRINT_UNKNOWN_AUTH_FILE_MISSING,
    FINGERPRINT_UNKNOWN
  };
  enum class SSHAuthtype {
    PASSWORD,
    KEYFILE,
    AUTOPUBKEY
  };

  class SSHConnectionCredentials {
  public:
    std::string username;
    std::string password;
    std::string keyfile;
    std::string keypassword;
    SSHFingerprint fingerprint;
    SSHAuthtype auth;
  };

  class WBSSHLIBRARY_PUBLIC_FUNC SSHTunnelException : public std::exception {
  public:
    explicit SSHTunnelException(const std::string &message)
        : _msgText(message) {
    }
    explicit SSHTunnelException(const char *message)
        : _msgText(message) {
    }
    virtual ~SSHTunnelException() NOEXCEPT {
    }
    virtual auto what() const NOEXCEPT -> const char * {
      return _msgText.c_str();
    }
  protected:
    std::string _msgText;
  };

  class WBSSHLIBRARY_PUBLIC_FUNC SSHSftpException : public std::exception {
  public:
    explicit SSHSftpException(const std::string &message)
        : _msgText(message) {
    }
    explicit SSHSftpException(const char *message)
        : _msgText(message) {
    }
    virtual ~SSHSftpException() NOEXCEPT {
    }
    virtual auto what() const NOEXCEPT -> const char * {
      return _msgText.c_str();
    }
  protected:
    std::string _msgText;
  };

  class WBSSHLIBRARY_PUBLIC_FUNC SSHAuthException : public std::exception {
  public:
    explicit SSHAuthException(const std::string &message)
        : _msgText(message) {
    }
    explicit SSHAuthException(const char *message)
        : _msgText(message) {
    }
    virtual ~SSHAuthException() NOEXCEPT {
    }
    virtual auto what() const NOEXCEPT -> const char * {
      return _msgText.c_str();
    }
  protected:
    std::string _msgText;
  };

  class WBSSHLIBRARY_PUBLIC_FUNC SSHThread {
  public:
    SSHThread();
    virtual ~SSHThread();
    virtual auto stop() -> void;
    auto isRunning() -> bool;
    auto start() -> void;
    auto join() -> void;

  protected:
    std::atomic<bool> _stop;
    std::atomic<bool> _finished;
    base::Semaphore _initializationSem;
    virtual auto run() -> void = 0;
    auto _run() -> void;

  private:
    std::thread _thread;
  };

} /* namespace ssh */
