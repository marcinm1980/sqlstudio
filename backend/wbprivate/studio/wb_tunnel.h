/*
 * Copyright (c) 2008, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "SSHTunnelManager.h"
#include "wb_backend_public_interface.h"
#include "driver_manager.h"
#include "base/threading.h"

namespace wb {
  class TunnelManager {
  public:
    TunnelManager();
    ~TunnelManager();

    auto start() -> void;
    auto shutdown() -> void;
    auto portUsageIncrement(const ssh::SSHConnectionConfig &config) -> void;
    auto portUsageDecrement(const ssh::SSHConnectionConfig &config) -> void;
    auto createTunnel(db_mgmt_ConnectionRef connectionProperties) -> std::shared_ptr<SSHTunnel>;

  private:
    ssh::SSHTunnelManager *_manager;

    std::map<int, std::pair<ssh::SSHConnectionConfig, base::refcount_t>> _portUsage;
    base::Mutex _usageMapMtx;
  };

  class SSHTunnel {
  private:
    TunnelManager *_tm;
    ssh::SSHConnectionConfig _config;

  public:
    SSHTunnel(TunnelManager *tm, const ssh::SSHConnectionConfig &config) : _tm(tm), _config(config) {
      _tm->portUsageIncrement(_config);
    }

    virtual ~SSHTunnel() {
      disconnect();
    }

    auto connect(db_mgmt_ConnectionRef connectionProperties) -> void {
      if (_config.localport == 0)
        throw std::runtime_error("Could not connect SSH tunnel");
    }

    auto disconnect() -> void {
      _tm->portUsageDecrement(_config);
    }

    auto getConfig() const -> const ssh::SSHConnectionConfig { return _config; }
  };

};
