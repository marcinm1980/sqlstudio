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

#include <gtkmm/applicationwindow.h>
#include "mforms/view.h"
#include "mforms/dockingpoint.h"
#include "mforms/appview.h"

namespace studio {
namespace X {

class MainForm : public mforms::DockingPointDelegate
{
protected:
  Gtk::ApplicationWindow *_wnd;
public:
  MainForm();
  void initialize();
  virtual ~MainForm();
  void showStatusTextBecb(const std::string& text);
  void setupMFormsApp();
  void insert(mforms::View *v);
  Gtk::Window* getMainWindow();
  void setVisible(bool flag = true);


  // DockingPointDelegate requirements
  virtual std::string get_type() { return "MainWindow"; }
  void dock_view(mforms::AppView *view, const std::string &position, int) { /*No longer needed */ }
  virtual bool select_view(mforms::AppView *view)  { /*No longer needed */ return false;}
  virtual void undock_view(mforms::AppView *view)  { /*No longer needed */ }
  virtual void set_view_title(mforms::AppView *view, const std::string &title);
  virtual std::pair<int, int> get_size();

  virtual mforms::AppView *selected_view() { return nullptr; }
  virtual int view_count() { return 0; };
  virtual mforms::AppView *view_at_index(int index) { return nullptr; };
};

} }
