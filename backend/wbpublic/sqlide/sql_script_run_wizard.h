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

#ifndef _SQL_SCRIPT_RUN_WIZARD_H_
#define _SQL_SCRIPT_RUN_WIZARD_H_

#include "wbpublic_public_interface.h"
#include "grtui/grt_wizard_plugin.h"
#include "grtui/wizard_progress_page.h"
#include "grtui/wizard_finished_page.h"

namespace mforms {
  class CodeEditor;
  class Selector;
}

class WBPUBLICBACKEND_PUBLIC_FUNC SqlScriptReviewPage : public grtui::WizardPage {
public:
  SqlScriptReviewPage(grtui::WizardForm *form, GrtVersionRef version, std::string algorithm, std::string lock);
  virtual ~SqlScriptReviewPage();

protected:
  virtual auto enter(bool advancing) -> void;
  virtual auto advance() -> bool;
  virtual auto next_button_caption() -> std::string;

  auto option_changed() -> void;

private:
  mforms::Box _box;
  mforms::Label _page_heading;
  mforms::CodeEditor *_sql_editor;
  mforms::Selector *_algorithm_selector;
  mforms::Selector *_lock_selector;
};

class WBPUBLICBACKEND_PUBLIC_FUNC SqlScriptApplyPage : public grtui::WizardProgressPage {
private:
  std::string _log;
  long _err_count;
  mforms::Button *_abort_btn;

  auto abort_exec() -> void;

  auto do_execute_sql_script(const std::string &sql_script) -> grt::ValueRef;

public:
  SqlScriptApplyPage(grtui::WizardForm *form);
  auto on_error(long long err_code, const std::string &err_msg, const std::string &err_sql) -> int;
  auto on_exec_progress(float progress) -> int;
  auto on_exec_stat(long success_count, long err_count) -> int;
  std::function<void(const std::string &)> apply_sql_script;
  auto execute_sql_script() -> bool;
  virtual auto next_button_caption() -> std::string;
  virtual auto allow_back() -> bool;
  virtual auto allow_next() -> bool;
  virtual auto allow_cancel() -> bool;
  virtual auto enter(bool advancing) -> void;
};

class WBPUBLICBACKEND_PUBLIC_FUNC SqlScriptRunWizard : public grtui::WizardForm {
public:
  SqlScriptReviewPage *review_page;
  SqlScriptApplyPage *apply_page;

public:
  SqlScriptRunWizard(GrtVersionRef version, std::string algorithm, std::string lock);

  auto has_errors() -> bool;
  auto applied() -> bool;

  std::function<void()> abort_apply;

  // Used by the wizard if an option changed.
  // Parameters: online DDL algorithm and lock.
  std::function<std::string(const std::string &, const std::string &)> regenerate_script;
};

#endif /* _SQL_SCRIPT_RUN_WIZARD_H_ */
