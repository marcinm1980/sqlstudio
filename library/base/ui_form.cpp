/*
 * Copyright (c) 2007, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "base/string_utilities.h"
#include "base/ui_form.h"
#include "base/log.h"
#include "base/notifications.h"

using namespace bec;

static std::map<std::string, UIForm *> ui_form_instances;

UIForm::UIForm() : _owner_data(0), _frontend_data(0) {
  ui_form_instances[form_id()] = this;

  base::NotificationInfo info;
  info["form"] = form_id();
  base::NotificationCenter::get()->send(GNUIFormCreated, 0, info);
}

UIForm::~UIForm() {
  base::NotificationInfo info;
  info["form"] = form_id();
  base::NotificationCenter::get()->send(GNUIFormDestroyed, 0, info);

  ui_form_instances.erase(ui_form_instances.find(form_id()));
}

auto UIForm::form_with_id(const std::string &id) -> bec::UIForm * {
  if (ui_form_instances.find(id) != ui_form_instances.end())
    return ui_form_instances[id];
  return 0;
}

auto UIForm::form_id() -> std::string {
  return base::strfmt("<UIForm %p>", this);
}

auto UIForm::set_frontend_data(void *data) -> void {
  _frontend_data = data;
}

auto UIForm::get_frontend_data() -> void * {
  return _frontend_data;
}

auto UIForm::set_owner_data(void *data) -> void {
  _owner_data = data;
}
auto UIForm::get_owner_data() -> void * {
  return _owner_data;
}

auto UIForm::is_main_form() -> bool {
  return false;
}

// Target description for cut/copy/delete menu items and for paste, after a copy is made.
auto UIForm::get_edit_target_name() -> std::string {
  return "";
}

auto UIForm::can_undo() -> bool {
  return false;
}
auto UIForm::can_redo() -> bool {
  return false;
}
auto UIForm::can_cut() -> bool {
  return can_copy() && can_delete();
}
auto UIForm::can_copy() -> bool {
  return false;
}
auto UIForm::can_paste() -> bool {
  return false;
}
auto UIForm::can_delete() -> bool {
  return false;
}
auto UIForm::can_select_all() -> bool {
  return false;
}

auto UIForm::undo() -> void {
}
auto UIForm::redo() -> void {
}
auto UIForm::cut() -> void {
}
auto UIForm::copy() -> void {
}
auto UIForm::paste() -> void {
}
auto UIForm::delete_selection() -> void {
}
auto UIForm::select_all() -> void {
}

//--------------------------------------------------------------------------------------------------

static struct RegisterNotifDocs_ui_form {
  RegisterNotifDocs_ui_form() {
    base::NotificationCenter::get()->register_notification(
      GNUIFormCreated, "application",
      "Sent when a new form object is created.\n"
      "Note: at the time this notification is sent, the form object is not yet fully initialized.",
      "", "form - identifier of the form");

    base::NotificationCenter::get()->register_notification(GNUIFormDestroyed, "application",
                                                           "Sent when a new form object is destroyed.", "",
                                                           "form - identifier of the form");

    base::NotificationCenter::get()->register_notification("GNFormTitleDidChange", "application",
                                                           "Sent when a form's title change.", "",
                                                           "form - identifier of the form\n"
                                                           "title - the new title of the form");
    base::NotificationCenter::get()->register_notification("GNTextSelectionChanged", "application",
                                                           "Sent when the selection or the caret position\n"
                                                           " in an editor changed.",
                                                           "", "");
    base::NotificationCenter::get()->register_notification("GNFocusChanged", "application",
                                                           "Sent when a control gets the input focus.", "", "");
    base::NotificationCenter::get()->register_notification(
      "GNColorsChanged", "application",
      "Sent when colors or the theme changed in the preferences or when the system's appearance changed.", "", "");
    base::NotificationCenter::get()->register_notification(
      "GNBackingScaleChanged", "application",
      "Sent when a window moved to a monitor with a different resolution (DPI).", "", "");
  }
} initdocs_ui_form;
