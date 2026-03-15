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

#include "base/trackable.h"
#include "wbpublic_public_interface.h"
#include "grt/grt_manager.h"

class WBPUBLICBACKEND_PUBLIC_FUNC GrtThreadedTask : public base::trackable {
public:
  using Ref = std::shared_ptr<GrtThreadedTask>;

public:
  static auto create() -> Ref {
    return Ref(new GrtThreadedTask());
  }
  static auto create(const GrtThreadedTask::Ref parent_task) -> Ref {
    return Ref(new GrtThreadedTask(parent_task));
  }

public:
  virtual ~GrtThreadedTask();
  auto disconnect_callbacks() -> void;

protected:
  GrtThreadedTask();
  GrtThreadedTask(const GrtThreadedTask::Ref parent_task);

public:
  auto is_busy() -> bool {
    return _dispatcher && _dispatcher->get_busy();
  }

private:
  auto dispatcher() -> const bec::GRTDispatcher::Ref &;

private:
  bec::GRTDispatcher::Ref _dispatcher;

private:
  bec::GRTTask::Ref _task;
  GrtThreadedTask::Ref _parent_task;

public:
  auto parent_task() const -> const GrtThreadedTask::Ref {
    return _parent_task;
  }
  auto parent_task(const GrtThreadedTask::Ref val) -> void;

  auto task() -> const bec::GRTTask::Ref; // Returns the underlying grt task.

private:
  auto on_starting(const bec::GRTTaskBase::Ref task) -> void;

public:
  auto desc() -> std::string {
    return _desc;
  }
  auto desc(const std::string &desc) -> void {
    _desc = desc;
  }

private:
  std::string _desc;

public:
  auto send_task_res_msg(bool value) -> void {
    _send_task_res_msg = value;
  }

private:
  bool _send_task_res_msg;

public:
  using Proc_cb = std::function<grt::StringRef()>;
  using Msg_cb = std::function<int(int, const std::string &, const std::string &)>;
  using Progress_cb = std::function<int(float, const std::string &)>;
  using Finish_cb = std::function<void()>;
  using Fail_cb = std::function<void(const std::string &)>;

public:
  auto exec(bool sync = false, Proc_cb proc_cb = Proc_cb()) -> void;
  void send_msg(int msg_type, const std::string &msg, const std::string &detail = "");
  void send_progress(float percentage, const std::string &msg, const std::string &detail = "");

public:
  auto msg_cb(Msg_cb cb) -> void {
    _msg_cb = cb;
  }
  auto msg_cb() -> const Msg_cb & {
    return _msg_cb;
  }

  auto progress_cb(Progress_cb cb) -> void {
    _progress_cb = cb;
  }
  auto finish_cb(Finish_cb cb, bool onetime = false) -> void {
    _finish_cb = cb;
    _onetime_finish_cb = onetime;
  }
  auto fail_cb(Fail_cb cb, bool onetime = false) -> void {
    _fail_cb = cb;
    _onetime_fail_cb = onetime;
  }
  auto proc_cb(Proc_cb cb) -> void {
    _proc_cb = cb;
  }

private:
  auto process_msg(const grt::Message &msgs) -> void;
  auto process_finish(grt::ValueRef res) -> void;
  auto process_fail(const std::exception &error) -> void;

private:
  Proc_cb _proc_cb;
  Msg_cb _msg_cb;
  Progress_cb _progress_cb;
  Finish_cb _finish_cb;
  bool _onetime_finish_cb;
  Fail_cb _fail_cb;
  bool _onetime_fail_cb;

public:
  auto execute_in_main_thread(const std::function<void()> &function, bool wait, bool force_queue) -> void;
};
