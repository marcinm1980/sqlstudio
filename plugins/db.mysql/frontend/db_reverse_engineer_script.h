/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

#pragma once

#include "grtui/grt_wizard_plugin.h"
#include "grtui/wizard_progress_page.h"
#include "grtui/wizard_finished_page.h"

#include "db_rev_eng_be.h"

#include "mforms/fs_object_selector.h"
#include "mforms/table.h"
#include "mforms/label.h"
#include "mforms/selector.h"
#include "mforms/checkbox.h"

using namespace grtui;
using namespace mforms;

namespace ScriptImport {

  /**
   * Wizard page for setting up the import.
   */
  class ImportInputPage : public WizardPage {
  private:
    Table _table;
    Label _heading;
    Label _caption;
    FsObjectSelector _file_selector;
    Label _file_codeset_caption;
    Selector _file_codeset_sel;

    CheckBox _autoplaceCheck;
    CheckBox _ansiQuotesCheck;

    auto fill_encodings_list() -> void;

  public:
    ImportInputPage(WizardPlugin *form);
    auto file_changed() -> void;
    virtual auto allow_next() -> bool;
    virtual auto next_button_caption() -> std::string;
    auto gather_options(bool advancing) -> void;
  };

  /**
   * Wizard page that shows the progress of the current import operation.
   */
  class ImportProgressPage : public WizardProgressPage {
  private:
    Sql_import _import_be;
    TaskRow *_auto_place_task;
    std::function<void(bool, std::string)> _finished_cb;
    bool _auto_place;
    bool _done;

  public:
    ImportProgressPage(WizardForm *form, const std::function<void(bool, std::string)> &finished_cb);
    auto import_objects_finished(grt::ValueRef value) -> void;
    auto import_objects() -> bool;
    auto verify_results() -> bool;
    auto place_objects() -> bool;
    virtual auto allow_back() -> bool;
    virtual auto enter(bool advancing) -> void;
    virtual auto tasks_finished(bool success) -> void;
    auto get_summary() -> std::string;
  };

  /**
   * The actual import wizard comprising the pages declared above and some additional stuff.
   */

  class WbPluginSQLImport : public WizardPlugin {
  private:
    ImportInputPage *_input_page;
    ImportProgressPage *_progress_page;
    WizardFinishedPage *_finish_page;

  public:
    WbPluginSQLImport(grt::Module *module);
    auto update_summary(bool success, const std::string &summary) -> void;
  };

}; // namespace ScriptImport

auto createImportScriptWizard(grt::Module *module, db_CatalogRef catalog) -> grtui::WizardPlugin *;

