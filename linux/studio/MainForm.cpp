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

#include "MainForm.h"
#include "mforms/mforms.h"
#include "mforms/../gtk/lf_view.h"
#include "grt/grt_manager.h"
#include "base/file_utilities.h"
#include "ApplicationController.h"
#include <gtkmm.h>

using namespace studio::X;

MainForm::MainForm() : _wnd(nullptr)
{
}

static bool onMainFormDelete(GdkEventAny* evt)
{
  ApplicationController::get().quit();
  return true;
}

void MainForm::initialize()
{
  _wnd = new Gtk::ApplicationWindow();
  _wnd->set_visible(false);
  _wnd->set_default_geometry(960, 600);
  _wnd->signal_delete_event().connect(&onMainFormDelete);
}

//------------------------------------------------------------------------------

MainForm::~MainForm()
{
  if (_wnd != nullptr)
    delete _wnd;
  _wnd = nullptr;
}

//------------------------------------------------------------------------------

void MainForm::showStatusTextBecb(const std::string& text)
{
//  Gtk::Statusbar *status = 0;
//
//  _ui->get_widget("statusbar1", status);
//
//  if (bec::GRTManager::get().in_main_thread())
//    change_status(status, text);
//  else
//    // execute when idle in case we're being called from the worker thread
//    _sig_change_status = Glib::signal_idle().connect(sigc::bind<Gtk::Statusbar*,std::string>(sigc::ptr_fun(change_status), status, text));
}

//------------------------------------------------------------------------------

static std::string getResourcePath(mforms::App* app, const std::string& file)
{
  if (file.empty()) return bec::GRTManager::get().get_data_file_path("");
  if (file[0] == '/') return file;

  if (g_str_has_suffix(file.c_str(), ".png") || g_str_has_suffix(file.c_str(), ".xpm"))
    return bec::IconManager::get_instance()->get_icon_path(file);
  else if (g_str_has_suffix(file.c_str(), ".vbs"))
  {
    return bec::GRTManager::get().get_data_file_path(file);
  }

  return bec::GRTManager::get().get_data_file_path(file);
}

//------------------------------------------------------------------------------

static std::string getExecutablePath(mforms::App* app, const std::string& file)
{
  std::string path = bec::GRTManager::get().get_data_file_path(file);
  if (!path.empty() && base::file_exists(path))
    return path;

  path = base::dirname(std::string(getenv("MWB_MODULE_DIR"))) + "/" + file;
  if (base::file_exists(path))
    return path;

  const char *basedir = getenv("MWB_BASE_DIR");
  if (basedir)
  {
    char *p = g_strdup_printf("%s/libexec/mysql-studio/%s", basedir, file.c_str());
    path = p;
    g_free(p);
    if (g_file_test(path.c_str(), G_FILE_TEST_EXISTS))
      return path;

    p = g_strdup_printf("%s/bin/%s", basedir, file.c_str());
    path = p;
    g_free(p);
    if (g_file_test(path.c_str(), G_FILE_TEST_EXISTS))
      return path;

    p = g_strdup_printf("%s/libexec/%s", basedir, file.c_str());
    path = p;
    g_free(p);
    if (g_file_test(path.c_str(), G_FILE_TEST_EXISTS))
      return path;
  }

  return "";
}

//------------------------------------------------------------------------------

static base::Rect getMainWindowBounds(mforms::App* app)
{
  Gtk::Window *w = ApplicationController::get().getMainWindow();
  int x, y;

  w->get_window()->get_position(x, y);

  return base::Rect(x, y, w->get_width(), w->get_height());
}

//------------------------------------------------------------------------------

static void setStatusText(mforms::App* app, const std::string &text)
{
  MainForm *self= reinterpret_cast<MainForm*>(app->get_data_ptr());

  self->showStatusTextBecb(text);
}

//------------------------------------------------------------------------------

void MainForm::setupMFormsApp()
{
  mforms::ControlFactory *cf = mforms::ControlFactory::get_instance();
   g_assert(cf);

   mforms::App::instantiate(this, false);
   mforms::App::get()->set_data(this);

   cf->_app_impl.get_resource_path = &getResourcePath;
   cf->_app_impl.get_executable_path = &getExecutablePath;
   cf->_app_impl.set_status_text = &setStatusText;
   cf->_app_impl.get_application_bounds = &getMainWindowBounds;
   // Those two events are used only for python debugger, we don't need those here.
   cf->_app_impl.enter_event_loop = [](mforms::App*,float timeout)->int{ return 0; };
   cf->_app_impl.exit_event_loop = [](mforms::App *,int rc){};
}

//------------------------------------------------------------------------------

void MainForm::insert(mforms::View *v)
{
  if (_wnd != nullptr)
  _wnd->add(*mforms::widget_for_view(v));
}

//------------------------------------------------------------------------------

Gtk::Window* MainForm::getMainWindow()
{
  return _wnd;
}

void MainForm::setVisible(bool flag)
{
  if (_wnd != nullptr)
    _wnd->set_visible(flag);
}

void MainForm::set_view_title(mforms::AppView *view, const std::string &title) {
  if (_wnd != nullptr)
    _wnd->set_title(title);
}

std::pair<int, int> MainForm::get_size()
{
  if (_wnd != nullptr)
    return std::pair<int, int>(_wnd->get_width(), _wnd->get_height());
  else
    return std::pair<int, int>(0, 0);
}
