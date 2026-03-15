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
#include <errno.h>
#include <string.h>
#include <thread>
#include <atomic>
#include <deque>
#include <map>
#include "SSHCommon.h"
#include "SSHSession.h"
#include "SSHTunnelHandler.h"
#include "base/any.h"

namespace ssh {
  using sockInfo = struct {
    uint16_t port;
    int socketHandle;
  };

  class WBSSHLIBRARY_PUBLIC_FUNC SSHTunnelManager : public SSHThread {
  public:
    SSHTunnelManager();
    auto createTunnel(std::shared_ptr<SSHSession> &session) -> std::tuple<SSHReturnType, base::any>;
    auto lookupTunnel(const SSHConnectionConfig &config) -> int;
    virtual ~SSHTunnelManager();
    auto pokeWakeupSocket() -> void;
    auto setStop() -> void {
      _stop = true;
    }

    auto disconnect(const SSHConnectionConfig &config) -> void;

  protected:
    mutable base::RecMutex _socketMutex;
    auto lockSocketList() -> base::RecMutexLock;
    virtual void run() override;
    auto createSocket() -> sockInfo;
    auto localSocketHandler() -> void;
    auto getSocketList() -> std::vector<pollfd>;

    uint16_t _wakeupSocketPort;
    int _wakeupSocket;
    std::map<int, std::unique_ptr<SSHTunnelHandler>> _socketList;
  };

} /* namespace ssh */
