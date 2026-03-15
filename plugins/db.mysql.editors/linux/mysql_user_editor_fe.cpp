/*
 * Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2026 dev4fun. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA 
 */

#include "plugin_editor_base.h"
#include "grtdb/editor_user.h"
#include "grtdb/db_object_helpers.h"
#include "treemodel_wrapper.h"
#include "text_list_columns_model.h"
#include <gtkmm/notebook.h>
#include <gtkmm/image.h>
#include <gtkmm/stock.h>
#include <gtkmm/treeview.h>
#include <gtkmm/textview.h>
#include <gtkmm/togglebutton.h>

//==============================================================================
//
//==============================================================================
class DbMySQLUserEditor : public PluginEditorBase {
  bec::UserEditorBE *_be;
  Glib::RefPtr<Gtk::ListStore> _assigned_roles_model;
  TextListColumnsModel *_assigned_roles_columns;
  Glib::RefPtr<ListModelWrapper> _roles_model;
  Gtk::TreeView *_all_roles_tv;
  Gtk::TreeView *_user_roles_tv;
  Gtk::Entry *_user_pw;
  Gtk::ToggleButton *_show_pw;

  virtual auto get_be() -> bec::BaseEditor *;

  auto show_password() -> void;
  auto add_role() -> void;
  auto remove_role() -> void;

  auto add_role_by_iter(const Gtk::TreeModel::iterator &iter) -> void;
  auto remove_role_by_iter(const Gtk::TreeModel::iterator &iter) -> void;

  virtual ~DbMySQLUserEditor() {
    delete _be;
    _be = 0;
  }

  auto set_name(const std::string &name) -> void {
    _be->set_name(name);
    _signal_title_changed.emit(_be->get_title());
  }

  auto set_password(const std::string &password) -> void {
    _be->set_password(password);
  }

  auto set_comment(const std::string &comm) -> void {
    _be->set_comment(comm);
  }

public:
  DbMySQLUserEditor(grt::Module *m, const grt::BaseListRef &args);

  virtual auto do_refresh_form_data() -> void;

  virtual auto switch_edited_object(const grt::BaseListRef &args) -> bool;
};

DbMySQLUserEditor::DbMySQLUserEditor(grt::Module *m, const grt::BaseListRef &args)
  : PluginEditorBase(m, args, "modules/data/editor_user.glade"),
    _be(new bec::UserEditorBE(db_UserRef::cast_from(args[0]))) {
  xml()->get_widget("mysql_user_editor_notebook", _editor_notebook);

  Gtk::Image *image;
  xml()->get_widget("user_editor_image", image);
  image->set(ImageCache::get_instance()->image_from_filename("db.User.editor.48x48.png", false));

  _be->set_refresh_ui_slot(std::bind(&DbMySQLUserEditor::refresh_form_data, this));

  bind_entry_and_be_setter("user_name", this, &DbMySQLUserEditor::set_name);
  bind_entry_and_be_setter("user_password", this, &DbMySQLUserEditor::set_password);

  Gtk::TextView *textview(0);
  xml()->get_widget("user_comment", textview);
  add_text_change_timer(textview, sigc::mem_fun(this, &DbMySQLUserEditor::set_comment));

  xml()->get_widget("all_roles", _all_roles_tv);
  xml()->get_widget("user_roles", _user_roles_tv);

  xml()->get_widget("user_password", _user_pw);
  xml()->get_widget("show_pw_toggle_btn", _show_pw);

  Gtk::Image *img = Gtk::manage(new Gtk::Image(Gtk::Stock::DIALOG_AUTHENTICATION, Gtk::ICON_SIZE_MENU));
  _show_pw->set_image(*img);
  _show_pw->signal_clicked().connect(sigc::mem_fun(this, &DbMySQLUserEditor::show_password));

  _assigned_roles_model = model_from_string_list(_be->get_roles(), &_assigned_roles_columns);
  _roles_model = ListModelWrapper::create(_be->get_role_tree(), _all_roles_tv, "AllRoles");

  _all_roles_tv->set_model(_roles_model);
  _roles_model->model().append_string_column(bec::RoleTreeBE::Name, "Role", RO, NO_ICON);
  _all_roles_tv->set_headers_visible(false);

  _user_roles_tv->set_model(_assigned_roles_model);
  _user_roles_tv->append_column("Assigned role", _assigned_roles_columns->item);
  _user_roles_tv->set_headers_visible(false);

  Gtk::Button *btn(0);
  xml()->get_widget("add_role_btn", btn);
  btn->signal_clicked().connect(sigc::mem_fun(this, &DbMySQLUserEditor::add_role));

  btn = 0;
  xml()->get_widget("remove_role_btn", btn);
  btn->signal_clicked().connect(sigc::mem_fun(this, &DbMySQLUserEditor::remove_role));

  _editor_notebook->reparent(*this);
  _editor_notebook->show();

  show_all();

  refresh_form_data();
}

//------------------------------------------------------------------------------
auto DbMySQLUserEditor::switch_edited_object(const grt::BaseListRef &args) -> bool {
  bec::UserEditorBE *old_be = _be;

  _be = new bec::UserEditorBE(db_UserRef::cast_from(args[0]));
  _be->set_refresh_ui_slot(std::bind(&DbMySQLUserEditor::refresh_form_data, this));

  _assigned_roles_model = model_from_string_list(_be->get_roles(), &_assigned_roles_columns);
  _roles_model = ListModelWrapper::create(_be->get_role_tree(), _all_roles_tv, "AllRoles");

  _all_roles_tv->remove_all_columns();
  _all_roles_tv->set_model(_roles_model);
  _roles_model->model().append_string_column(bec::RoleTreeBE::Name, "Role", RO, NO_ICON);
  _all_roles_tv->set_headers_visible(false);

  _user_roles_tv->remove_all_columns();
  _user_roles_tv->set_model(_assigned_roles_model);
  _user_roles_tv->append_column("Assigned role", _assigned_roles_columns->item);
  _user_roles_tv->set_headers_visible(false);

  refresh_form_data();

  delete old_be;

  return true;
}

//------------------------------------------------------------------------------
auto DbMySQLUserEditor::get_be() -> bec::BaseEditor * {
  return _be;
}

//------------------------------------------------------------------------------
auto DbMySQLUserEditor::do_refresh_form_data() -> void {
  Gtk::Entry *entry(0);
  xml()->get_widget("user_name", entry);
  entry->set_text(_be->get_name());
  _signal_title_changed.emit(_be->get_title());

  entry = 0;
  xml()->get_widget("user_password", entry);
  entry->set_text(_be->get_password());

  Gtk::TextView *textview(0);
  xml()->get_widget("user_comment", textview);
  textview->get_buffer()->set_text(_be->get_comment());

  recreate_model_from_string_list(_assigned_roles_model, _be->get_roles());
  _be->get_role_tree()->refresh();
  _roles_model->refresh();
  _all_roles_tv->set_model(_roles_model);
}

//------------------------------------------------------------------------------
auto DbMySQLUserEditor::add_role_by_iter(const Gtk::TreeModel::iterator &iter) -> void {
  // Get node, get role name from be
  // Add role to be
  bec::NodeId node = _roles_model->node_for_iter(iter);
  std::string role_name;
  _be->get_role_tree()->get_field(node, bec::RoleTreeBE::Name, role_name);
  g_log("UserEditorFE", G_LOG_LEVEL_DEBUG, "adding role '%s'", role_name.c_str());
  _be->add_role(role_name);
}
//------------------------------------------------------------------------------
auto DbMySQLUserEditor::show_password() -> void {
  if (_show_pw)
    _user_pw->set_visibility(!_user_pw->get_visibility());
}
//------------------------------------------------------------------------------
auto DbMySQLUserEditor::add_role() -> void {
  // Get selection from all_roles
  // add roles to be
  Glib::RefPtr<Gtk::TreeSelection> selection = _all_roles_tv->get_selection();
  selection->selected_foreach_iter(sigc::mem_fun(this, &DbMySQLUserEditor::add_role_by_iter));
  do_refresh_form_data();
}

//------------------------------------------------------------------------------
auto DbMySQLUserEditor::remove_role_by_iter(const Gtk::TreeModel::iterator &iter) -> void {
  // Get row, get role name from row
  // Remove role from be
  Gtk::TreeModel::Row row = *iter;
  const std::string role_name = row[_assigned_roles_columns->item];
  g_log("UserEditorFE", G_LOG_LEVEL_DEBUG, "removing role '%s'", role_name.c_str());
  _be->remove_role(role_name);
}

//------------------------------------------------------------------------------
auto DbMySQLUserEditor::remove_role() -> void {
  // Get selection from user_roles
  // remove roles from be
  Glib::RefPtr<Gtk::TreeSelection> selection = _user_roles_tv->get_selection();
  selection->selected_foreach_iter(sigc::mem_fun(this, &DbMySQLUserEditor::remove_role_by_iter));
  do_refresh_form_data();
}

//------------------------------------------------------------------------------
extern "C" {
auto createDbMysqlUserEditor(grt::Module *m, const grt::BaseListRef &args) -> GUIPluginBase * {
  return Gtk::manage(new DbMySQLUserEditor(m, args));
}
};
