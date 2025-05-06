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

#import "ApplicationController.h"

#include "base/log.h"
#include "mforms/box.h"
#include "gtk/lf_view.h"
#include "gtk/lf_wizard.h"
#include "gtk/lf_utilities.h"
#include <mforms/mforms.h>

#include "base/string_utilities.h"
#include <iostream>
#include "backend/endless_control.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <gtkmm.h>
#pragma GCC diagnostic pop

DEFAULT_LOG_DOMAIN("ApplicationController")

using namespace studio::X;

extern  void lf_record_grid_init();

#if defined(HAVE_GNOME_KEYRING) || defined(HAVE_OLD_GNOME_KEYRING)
extern "C" {
// gnome-keyring has been deprecated in favor of libsecret
// More informations can be found here  https://mail.gnome.org/archives/commits-list/2013-October/msg08876.html
// Below defines will turn off deprecations and allow build with never Gnome until we will not move to libsecret.
  #define GNOME_KEYRING_DEPRECATED
  #define GNOME_KEYRING_DEPRECATED_FOR(x)
  #include <gnome-keyring.h>
};
#endif

ApplicationController::ApplicationController() : _initialized(false), _mainForm(nullptr)
{
//  be sure app is created before ApplicationController
  runtime::app::get();
}

ApplicationController& ApplicationController::get()
{
  static ApplicationController app;
  return app;
}

ApplicationController::~ApplicationController()
{
  if (_mainForm != nullptr)
    delete _mainForm;
}

void ApplicationController::init(int argc, char** argv)
{
  if (_initialized)
    throw std::runtime_error("ApplicationController was already initialized");

  std::string exeName = argv[0];


  _opts.userDataDir = std::string(g_get_home_dir()).append("/.mysql/studio");
  _opts.basedir = getenv("MWB_DATA_DIR");
  _opts.pluginSearchPath = getenv("MWB_PLUGIN_DIR");
  _opts.structSearchPath = _opts.basedir + "/grt";
  _opts.moduleSearchPath = getenv("MWB_MODULE_DIR");

  base::Logger log(_opts.userDataDir, getenv("MWB_LOG_TO_STDERR") != nullptr, "studio");

  runtime::app::get().parseParams = [](int argc, char **argv, int *retval)->bool{
    return ApplicationBackend::get().parseParams(argc, argv, retval);
  };

  runtime::app::get().onActivate = [](){
    ApplicationController::get().finalizeInit();
  };

  runtime::app::get().onBeforeActivate = [&]()->bool{

    #if defined(HAVE_GNOME_KEYRING) || defined(HAVE_OLD_GNOME_KEYRING)
    if (getenv("WB_NO_GNOME_KEYRING"))
      logInfo("WB_NO_GNOME_KEYRING environment variable has been set. Stored passwords will be lost once quit.\n");
    else
    {
      if (!gnome_keyring_is_available())
      {
        setenv("WB_NO_GNOME_KEYRING", "1", 1);
        logError("Can't communicate with gnome-keyring, it's probably not running. Stored passwords will be lost once quit.\n");
      }
    }
    #endif

    base::threading_init();
    mforms::gtk::init(getenv("WB_FORCE_SYSTEM_COLORS") != NULL);
    mforms::gtk::WizardImpl::set_icon_path(_opts.basedir + "/images");
    {
      lf_record_grid_init();
    }
    mforms::gtk::check();

    _mainForm = new MainForm();
    _mainForm->setupMFormsApp();

    ApplicationBackend::get().initialize(_opts);
    return true;
  };

  ApplicationBackend::get().quitCallback = [](){
    ApplicationController::get().quit();
  };

  g_set_application_name("MySqlStudio.X");
  gtk_widget_set_default_direction(GTK_TEXT_DIR_LTR);

  if (!getenv("MWB_DATA_DIR"))
   {
     std::string termination = "-bin";
     std::string name;
     if (base::hasSuffix(exeName, termination))
       name = base::left(exeName, exeName.length() - termination.length());
     std::cout << "To start MySQL studio, use " << name << " instead of " << exeName << std::endl;
     exit(1);
   }

  runtime::app::get().init("com.mysql.studio.x", argc, argv);



#ifdef ENABLE_DEBUG
  if ( !getenv("MWB_DATA_DIR") )
  {
    const char *path = "../share/mysql-studio";
    g_message("MWB_DATA_DIR is unset! Setting MWB_DATA_DIR to predifined value '%s'", path);
    setenv("MWB_DATA_DIR", path, 1);
  }

  if ( !getenv("MWB_MODULE_DIR") )
  {
    const char *path = "../lib/mysql-studio/modules";
    g_message("MWB_MODULE_DIR is unset! Setting MWB_MODULE_DIR to predifined value '%s'", path);
    setenv("MWB_MODULE_DIR", path, 1);
  }

  if ( !getenv("MWB_LIBRARY_DIR") )
  {
    const char *path = "../share/mysql-studio/libraries";
    g_message("MWB_LIBRARY_DIR is unset! Setting MWB_LIBRARY_DIR to predifined value '%s'", path);
    setenv("MWB_LIBRARY_DIR", path, 1);
  }

  if ( !getenv("MWB_PLUGIN_DIR") )
  {
    const char *path = "../lib/mysql-studio";
    g_message("MWB_PLUGIN_DIR is unset! Setting MWB_PLUGIN_DIR to predifined value '%s'", path);
    setenv("MWB_PLUGIN_DIR", path, 1);
  }
#endif
  if ( !getenv("MWB_DATA_DIR") || (!getenv("MWB_MODULE_DIR")))
  {
    g_print("Please start studio through studio.x instead of calling studio.x-bin directly\n");
    exit(1);
  }
}

//------------------------------------------------------------------------------

void ApplicationController::finalizeInit()
{
  try {
    ApplicationBackend::get().start();
  } catch (grt::user_cancelled &g)
  {
    mforms::Utilities::show_message("Aborted", "User cancel operation", "Ok", "", "");
    quit();
  }
}

//------------------------------------------------------------------------------

Gtk::Window* ApplicationController::getMainWindow()
{
  return _mainForm->getMainWindow();
}

//------------------------------------------------------------------------------

int ApplicationController::run()
{
  return runtime::app::get().run();
}

void ApplicationController::quit()
{
  runtime::app::get().quit();
}

//------------------------------------------------------------------------------

// Declared in gtk_helpers.h
void* get_mainwindow_impl()
{
  return ApplicationController::get().getMainWindow();
}
