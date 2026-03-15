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


class WBPUBLICBACKEND_PUBLIC_FUNC db_mgmt_SSHFile::ImplData {
public:
  ImplData();
  virtual ~ImplData();
  virtual auto getPath() -> grt::StringRef = 0;
  virtual auto read(const size_t length) -> grt::StringRef = 0;
  virtual auto readline() -> grt::StringRef = 0;
  virtual auto seek(const size_t offset) -> grt::IntegerRef = 0;
  virtual auto tell() -> grt::IntegerRef = 0;
};
