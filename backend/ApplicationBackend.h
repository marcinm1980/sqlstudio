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

#include <string>
#include "base/data_types.h"
#include "mforms/view.h"
#include "backend/ng_ide.h"
#include "grts/structs.studio.h"
#include "mforms/home_screen.h"
#include "mforms/home_screen_x_connections.h"

namespace studio {
namespace X {

class ApplicationBackend {
protected:
  ApplicationBackend();
  ApplicationBackend(const ApplicationBackend&) = delete;
  ApplicationBackend& operator= (ApplicationBackend&) = delete;

  dataTypes::XProject _initialProject;
  char *_argv0;
  ng::NgIDE *_ide;
  dataTypes::AppOptions _options;
  bool _registeredMetaclasses;
  studio_studioRef _wb_root;
  mforms::HomeScreen *_homeScreen;
  mforms::XConnectionsSection *_ngConnectionsSection;
  std::vector<mforms::Form*> _forms;
  base::Mutex _formListProtector;

  void openNgProject(const dataTypes::XProject &project);

  void handleHomeContextMenu(const base::any &object, const std::string &action);
  void homeActionCallback(mforms::HomeScreenAction action, const base::any &object);

public:
  static ApplicationBackend& get();
  virtual ~ApplicationBackend();

  void initialize(const dataTypes::AppOptions &opts);

  bool checkArgWithValue(char **argv, int &argi, const char *arg, char *&value);
  bool parseParams(int argc, char **argv, int *retval);
  void showHelp(const char* arg0);
  bool parseLoglevel(const std::string& line);
  mforms::View *getHomeScreen();
  bool showHomeScreen();
  void start();
  void formCloseHandler(mforms::Form *frm);

  std::function<void()> quitCallback;

};

} /* namespace X */
} /* namespace studio */

