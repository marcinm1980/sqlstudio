
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

#include "gtk/lf_mforms.h"
#include <mforms/mforms.h>
#include <gtkmm.h>
#include "base/string_utilities.h"
#include "base/threading.h"
#include "base/log.h"
#include "ApplicationController.h"

#include <X11/Xlib.h>

//------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  studio::X::ApplicationController::get().init(argc, argv);

  try
  {
    studio::X::ApplicationController::get().run();
  }
  catch (const std::exception &exc)
  {
    g_warning("ERROR: unhandled exception %s", exc.what());
    Gtk::MessageDialog dlg(base::strfmt("<b>Unhandled Exception</b>\nAn unhandled exception has occurred (%s).\nInternal state may be inconsystent, please save your work to a temporary file and restart studio.\nPlease report this with details on how to repeat at http://bugs.mysql.com", exc.what()),
                           true, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
    dlg.set_title(_("Error"));
    dlg.set_transient_for(*studio::X::ApplicationController::get().getMainWindow());
    dlg.run();
  }
  catch (const Glib::Exception &exc)
  {
    g_warning("ERROR: unhandled exception %s", exc.what().c_str());
    Gtk::MessageDialog dlg(base::strfmt("<b>Unhandled Exception</b>\nAn unhandled exception has occurred (%s).\nInternal state may be inconsystent, please save your work to a temporary file and restart studio.\nPlease report this with details on how to repeat at http://bugs.mysql.com", exc.what().c_str()),
                           true, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
    dlg.set_title(_("Error"));
    dlg.set_transient_for(*studio::X::ApplicationController::get().getMainWindow());
    dlg.run();
  }
  catch (...)
  {
    g_warning("ERROR: unhandled exception");
    Gtk::MessageDialog dlg(base::strfmt("<b>Unhandled Exception</b>\nAn unhandled exception has occurred.\nInternal state may be inconsystent, please save your work to a temporary file and restart studio.\nPlease report this with details on how to repeat at http://bugs.mysql.com"),
                           true, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
    dlg.set_title(_("Error"));
    dlg.set_transient_for(*studio::X::ApplicationController::get().getMainWindow());
    dlg.run();
  }

  return 0;
}

//------------------------------------------------------------------------------------------------
