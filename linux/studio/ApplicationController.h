/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once
namespace Gtk { class Widget;}

#include "mforms/view.h"
#include "backend/ApplicationBackend.h"
#include "MainForm.h"
#include <gtkmm/window.h>
#include <gtkmm/main.h>
#include "main_app.h"

namespace studio { namespace X {

class ApplicationController
{
protected:
  bool _initialized;
  MainForm *_mainForm;
  sigc::connection _sigFinalizeInitialization;
  dataTypes::AppOptions _opts;
  ApplicationController();
  ApplicationController(const ApplicationController&) = delete;
  ApplicationController& operator= (ApplicationController&) = delete;

public:
  static ApplicationController& get();
  virtual ~ApplicationController();
  void init(int argc, char **argv);
  void finalizeInit();

  Gtk::Window* getMainWindow();
  int run();
  void quit();
};

}}
