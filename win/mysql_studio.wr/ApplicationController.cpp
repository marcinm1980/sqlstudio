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

#pragma unmanaged
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "BackendFacade.h"
#include "base/drawing.h"

#pragma managed
#include "ConvUtils.h"
#include "ApplicationController.h"

using namespace System::Text;
using namespace MySqlStudio::X;

//------------------------------------------------------------------------------------------------

ApplicationController::ApplicationController() : _initialized(false)
{
  LoadLibraryEx(L"Scintilla.dll", NULL, 0);
  base::Color::set_active_scheme(base::ColorSchemeStandardWin8);
}

//------------------------------------------------------------------------------------------------

bool ApplicationController::isCommercial()
{
  // TODO: fix 
  return true;
}

void ApplicationController::init(String^ baseDir, String^ userDir)
{
  if (_initialized)
    throw std::runtime_error("ApplicationController was already initialized");

  BackendFacade::initialize(MySQL::NativeToCppStringRaw(userDir), MySQL::NativeToCppStringRaw(baseDir));
  _initialized = true;
}

//------------------------------------------------------------------------------------------------

void ApplicationController::parse(array<String^>^ args, String^ appPath)
{
  // Convert to UTF8 and keep the returned arrays as long as we are parsing that.
  // Add the application's path at the tip as the parse routine wants it so.
  array<array<unsigned char>^>^ managed_utf8 = gcnew array<array<unsigned char>^>(args->Length + 1);
  managed_utf8[0] = Encoding::UTF8->GetBytes(appPath);
  for (int i = 0; i < args->Length; i++)
    managed_utf8[i + 1] = Encoding::UTF8->GetBytes(args[i]);

  // Collect the managed string arrays into a c-like char* for parsing.
  char** arguments = new char*[managed_utf8->Length];
  try
  {
    for (int i = 0; i < managed_utf8->Length; i++)
    {
      pin_ptr<unsigned char>  chars = &managed_utf8[i][0];
      arguments[i] = (char*)chars;
    }

    int ret = 0;
    if (!BackendFacade::parseParams(managed_utf8->Length, arguments, &ret))
      return;
    BackendFacade::start();
  }
  finally
  {
    delete arguments;
  }
}

//------------------------------------------------------------------------------------------------