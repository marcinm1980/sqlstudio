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

#pragma once

#include "grt_wizard_form.h"

#include "mforms/imagebox.h"
#include "mforms/label.h"
#include "mforms/table.h"
#include "mforms/panel.h"
#include "mforms/textbox.h"
#include "mforms/progressbar.h"
#include "grt/grt_dispatcher.h"

namespace grtui {

  class WBPUBLICBACKEND_PUBLIC_FUNC WizardProgressPage : public WizardPage {
  public:
    WizardProgressPage(WizardForm *form, const std::string &id, bool has_progressbar);
    virtual ~WizardProgressPage();

    virtual auto get_advanced_panel() -> ::mforms::View * {
      return &_log_panel;
    }

    auto set_heading(const std::string &text) -> void;

  protected:
    enum TaskState { StateNormal, StateBusy, StateDone, StateWarning, StateError, StateDisabled };

    struct WBPUBLICBACKEND_PUBLIC_FUNC TaskRow {
      mforms::ImageBox icon;
      mforms::Label label;
      std::function<bool()> execute; //! return value indicates whether an asynchronous function was actually executed
      std::function<bool()> process_fail; //! return value indicates whether it can continue executing ok
      std::function<void(grt::ValueRef)> process_finish;
      std::string status_text;
      bool enabled;
      bool async;
      bool async_running;
      bool async_failed;
      int async_errors;

      TaskRow() : enabled(true), async(false), async_running(false), async_failed(false), async_errors(0) {
      }

      auto set_state(TaskState state) -> void;
      auto set_enabled(bool flag) -> void;
    };

    mforms::Label _heading;

    std::vector<TaskRow *> _tasks;
    std::map<bec::GRTTask *, bec::GRTTask::Ref> _task_list;

    std::string _finish_message;

    mforms::Label _status_text;

    mforms::Table _task_table;

    mforms::Box *_progress_bar_box;
    mforms::ProgressBar *_progress_bar;
    mforms::Label *_progress_label;

    mforms::Panel _log_panel;
    mforms::TextBox _log_text;

    int _current_task;
    bool _busy;
    bool _done;
    bool _got_warning_messages;
    bool _got_error_messages;

    auto add_async_task(const std::string &caption, const std::function<bool()> &execute,
                            const std::string &status_text) -> TaskRow *;

    auto add_task(const std::string &caption, const std::function<bool()> &execute, const std::string &status_text) -> TaskRow *;

    auto add_disabled_task(const std::string &caption) -> TaskRow *;

    auto current_task() -> TaskRow *;

    auto end_adding_tasks(const std::string &finish_message) -> void;

    auto clear_tasks() -> void;
    auto reset_tasks() -> void;

    auto start_tasks() -> void;

    auto set_status_text(const std::string &text, bool is_error = false) -> void;

    auto update_progress(float pct, const std::string &caption) -> void;

    auto add_log_text(const std::string &text) -> void;

    virtual auto extra_clicked() -> void;

  private:
    auto add_task(bool async, const std::string &caption, const std::function<bool()> &execute,
                      const std::string &status_text) -> TaskRow *;

  public:
    auto execute_grt_task(const std::function<grt::ValueRef()> &slot, bool sync) -> void;

    auto process_grt_task_message(const grt::Message &msg) -> void;
    auto process_grt_task_fail(const std::exception &error, bec::GRTTask *task) -> void;
    auto process_grt_task_finish(const grt::ValueRef &result, bec::GRTTask *task) -> void;

  protected:
    auto perform_tasks() -> void;

    virtual auto allow_cancel() -> bool;
    virtual auto allow_next() -> bool;
    virtual auto allow_back() -> bool;

    virtual auto tasks_finished(bool success) -> void {
    }

    virtual auto extra_button_caption() -> std::string;

    virtual auto enter(bool advancing) -> void;
  };
};
