/*
 * Copyright (c) 2016 Oracle and/or its affiliates. All rights reserved.
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

#include "BackendFacade.h"
#include "ApplicationBackend.h"
#include "mforms/view.h"

using namespace MySqlStudio::X;
using namespace studio::X;
  
//------------------------------------------------------------------------------------------------

bool BackendFacade::parseParams(int argc, char **argv, int *retval)
{
  int ret = 0;
  if(ApplicationBackend::get().parseParams(argc, argv, &ret))
    return true;
  return false;
}

//------------------------------------------------------------------------------------------------

void BackendFacade::initialize(const std::string &userDir, const std::string &baseDir)
{
  dataTypes::AppOptions opts;
  opts.userDataDir = userDir;
  opts.basedir = baseDir;
  opts.structSearchPath = opts.basedir + "/grt";
  ApplicationBackend::get().initialize(opts);
}

//------------------------------------------------------------------------------------------------

void BackendFacade::start()
{
  ApplicationBackend::get().start();
}

//------------------------------------------------------------------------------------------------
