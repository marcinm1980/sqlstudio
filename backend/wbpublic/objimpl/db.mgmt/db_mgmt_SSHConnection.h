/*
 * Copyright (c) 2017, 2018 Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2026 dev4fun. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#pragma once
#include <grts/structs.db.mgmt.h>

#include "wbpublic_public_interface.h"

//------------------------------------------------------------------------------------------------
class WBPUBLICBACKEND_PUBLIC_FUNC db_mgmt_SSHConnection::ImplData {
public:
  ImplData();
  virtual ~ImplData();
  virtual auto disconnect() -> void = 0;
  virtual auto isConnected() -> grt::IntegerRef = 0;
  virtual auto connect() -> grt::IntegerRef = 0;
  virtual auto executeCommand(const std::string &text) -> grt::DictRef = 0;
  virtual auto executeSudoCommand(const std::string &text, const std::string &user) -> grt::DictRef = 0;
  virtual auto cd(const std::string &directory) -> grt::IntegerRef = 0;
  virtual auto get(const std::string &src, const std::string &dest) -> void = 0;
  virtual auto getContent(const std::string &src) -> grt::StringRef = 0;
  virtual auto ls(const std::string &path) -> grt::DictListRef = 0;
  virtual auto mkdir(const std::string &directory) -> void = 0;
  virtual auto open(const std::string &path) -> db_mgmt_SSHFileRef = 0;
  virtual auto put(const std::string &src, const std::string &dest) -> void = 0;
  virtual auto pwd() -> grt::StringRef = 0;
  virtual auto rmdir(const std::string &directory) -> void = 0;
  virtual auto setContent(const std::string &path, const std::string &content) -> void = 0;
  virtual auto stat(const std::string &path) -> grt::DictRef = 0;
  virtual auto unlink(const std::string &file) -> void = 0;
  virtual auto fileExists(const std::string &path) -> grt::IntegerRef = 0;
};
