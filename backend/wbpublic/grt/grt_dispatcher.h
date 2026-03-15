/*
 * Copyright (c) 2007, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "base/threading.h"

#include "grt.h"
#include "grtpp_util.h"
#include "grtpp_shell.h"

#include "common.h"

#include "wbpublic_public_interface.h"

namespace bec {

  class GRTManager;
  class WBPUBLICBACKEND_PUBLIC_FUNC GRTDispatcher;

  // Mechanism for allowing queuing of callbacks to be executed
  // in the main thread by the GRT worked thread.
  // The target object, method and arguments are all encapsulated
  // in the callback object.

  class WBPUBLICBACKEND_PUBLIC_FUNC DispatcherCallbackBase {
  private:
    base::Semaphore _semaphore;

  protected:
    DispatcherCallbackBase();

  public:
    using Ref = std::shared_ptr<DispatcherCallbackBase>;

    virtual ~DispatcherCallbackBase();
    virtual auto execute() -> void = 0;
    auto wait() -> void;
    auto signal() -> void;
  };

  //------------------------------------------------------------------------------------------------

  template <class R>
  class DispatcherCallback : public DispatcherCallbackBase {
  public:
    using slot_type = std::function<R()>;
    using Ref = std::shared_ptr<DispatcherCallback<R>>;

    static auto create_callback(const slot_type &slot) -> Ref {
      return Ref(new DispatcherCallback<R>(slot));
    }

    auto execute() -> void {
      if (_slot)
        _return_value = _slot();
    }

    auto get_result() -> R {
      return _return_value;
    }

  private:
    slot_type _slot;
    R _return_value;

    DispatcherCallback(const slot_type &slot) : DispatcherCallbackBase(), _slot(slot) {};
  };

  template <>
  class DispatcherCallback<void> : public DispatcherCallbackBase {
  public:
    using slot_type = std::function<void()>;
    using Ref = std::shared_ptr<DispatcherCallback<void>>;

    static auto create_callback(const slot_type &slot = slot_type()) -> Ref {
      return Ref(new DispatcherCallback<void>(slot));
    }

    auto execute() -> void {
      if (_slot)
        _slot();
    }

  private:
    slot_type _slot;

    DispatcherCallback(const slot_type &slot) : DispatcherCallbackBase(), _slot(slot) {};
  };

  //------------------------------------------------------------------------------------------------

  class WBPUBLICBACKEND_PUBLIC_FUNC GRTTaskBase {
  public:
    using Ref = std::shared_ptr<GRTTaskBase>;

    virtual ~GRTTaskBase();

    inline auto is_finished() -> bool {
      return _finished;
    }

    virtual auto execute() -> grt::ValueRef = 0;

    auto cancel() -> void;
    inline auto is_cancelled() -> bool {
      return _cancelled;
    }

    auto name() -> std::string {
      return _name;
    }
    auto result() -> grt::ValueRef {
      return _result;
    };

    auto set_handle_messages_from_thread() -> void {
      _messages_to_main_thread = false;
    }

    // _m suffix methods are called in the main thread
    // the other ones are called in the grt thread and
    // schedule the call of their _m counterparts

    virtual auto started() -> void;
    virtual auto started_m() -> void;

    virtual auto finished(const grt::ValueRef &result) -> void;
    virtual auto finished_m(const grt::ValueRef &result) -> void;

    virtual auto failed(const std::exception &exc) -> void;
    virtual auto failed_m(const std::exception &exc) -> void;

    virtual auto process_message(const grt::Message &msg) -> bool;
    virtual auto process_message_m(const grt::Message &msg) -> void;

    auto get_error() -> grt::grt_runtime_error * {
      return _exception;
    };

    // Signals.
    using StartingTaskSignal = boost::signals2::signal<void()>;
    StartingTaskSignal signal_starting_task;

    using FinishingTaskSignal = boost::signals2::signal<void()>;
    FinishingTaskSignal signal_finishing_task;

    using FailingTaskSignal = boost::signals2::signal<void()>;
    FailingTaskSignal signal_failing_task;

  protected:
    std::shared_ptr<GRTDispatcher> _dispatcher;
    grt::grt_runtime_error *_exception;
    grt::ValueRef _result;

    GRTTaskBase(const std::string &name, const std::shared_ptr<GRTDispatcher> dispatcher)
      : _dispatcher(dispatcher),
        _exception(0),
        _name(name),
        _cancelled(false),
        _finished(false),
        _messages_to_main_thread(true) {
    }

    auto set_finished() -> void;

  private:
    std::string _name;
    bool _cancelled;
    bool _finished;
    bool _messages_to_main_thread;

    // Should never be defined and called.
    GRTTaskBase(GRTTaskBase &);
    auto operator=(GRTTaskBase &) -> GRTTaskBase &;
  };

  //------------------------------------------------------------------------------------------------

  class WBPUBLICBACKEND_PUBLIC_FUNC GRTTask : public GRTTaskBase {
    using StartedSignal = boost::signals2::signal<void()>;
    using FinishedSignal = boost::signals2::signal<void(grt::ValueRef)>;
    using FailedSignal = boost::signals2::signal<void(const std::exception &)>;
    using ProcessMessageSignal = boost::signals2::signal<void(const grt::Message &)>;

  public:
    using Ref = std::shared_ptr<GRTTask>;

    static auto create_task(const std::string &name, const std::shared_ptr<GRTDispatcher> dispatcher,
                            const std::function<grt::ValueRef()> &function) -> Ref;

    // XXX replace with direct slots?
    auto signal_started() -> StartedSignal * {
      return &_sigStarted;
    }
    auto signal_finished() -> FinishedSignal * {
      return &_sigFinished;
    }
    auto signal_failed() -> FailedSignal * {
      return &_sigFailed;
    }
    auto signal_message() -> ProcessMessageSignal * {
      return &_message;
    }

  protected:
    std::function<grt::ValueRef()> _function;

    StartedSignal _sigStarted;
    FinishedSignal _sigFinished;
    FailedSignal _sigFailed;
    ProcessMessageSignal _message;

    virtual auto execute() -> grt::ValueRef;

    GRTTask(const std::string &name, const std::shared_ptr<GRTDispatcher> dispatcher,
            const std::function<grt::ValueRef()> &function);
    virtual auto started_m() -> void;
    virtual auto finished_m(const grt::ValueRef &result) -> void;
    virtual auto failed_m(const std::exception &error) -> void;

    virtual auto process_message(const grt::Message &msg) -> bool;
    virtual auto process_message_m(const grt::Message &msg) -> void;
  };

  //------------------------------------------------------------------------------------------------

  class GRTShellTask : public GRTTaskBase {
    using FinishedSignal = boost::signals2::signal<void(grt::ShellCommand, std::string)>;
    using ProcessMessageSignal = boost::signals2::signal<void(const grt::Message &)>;

  public:
    using Ref = std::shared_ptr<GRTShellTask>;

    static auto create_task(const std::string &name, const std::shared_ptr<GRTDispatcher> dispatcher,
                            const std::string &command) -> Ref;

    auto signal_finished() -> FinishedSignal & {
      return _finished_signal;
    }
    auto signal_message() -> ProcessMessageSignal & {
      return _message;
    }

    inline auto get_prompt() const -> std::string {
      return _prompt;
    }
    inline auto get_result() const -> grt::ShellCommand {
      return _result;
    }

  protected:
    GRTShellTask(const std::string &name, const std::shared_ptr<GRTDispatcher> dispatcher, const std::string &command);

    virtual auto execute() -> grt::ValueRef;
    virtual auto finished_m(const grt::ValueRef &result) -> void;

    virtual auto process_message(const grt::Message &msg) -> bool;
    virtual auto process_message_m(const grt::Message &msg) -> void;

    FinishedSignal _finished_signal;
    ProcessMessageSignal _message;

    std::string _command;

    std::string _prompt;
    grt::ShellCommand _result;
  };

  //------------------------------------------------------------------------------------------------

  class WBPUBLICBACKEND_PUBLIC_FUNC GRTDispatcher : public std::enable_shared_from_this<GRTDispatcher> {
  public:
    using FlushAndWaitCallback = void (*)();
    using Ref = std::shared_ptr<GRTDispatcher>;

  private:
    GAsyncQueue *_task_queue;
    FlushAndWaitCallback _flush_main_thread_and_wait;
    std::weak_ptr<bec::GRTManager> _grtm;

    volatile base::refcount_t _busy;

    bool _threading_disabled;
    base::Semaphore _w_runing;
    volatile bool _shutdown_callback;
    bool _is_main_dispatcher;
    bool _shut_down;
    bool _started;

    GAsyncQueue *_callback_queue;
    GThread *_thread;

    static auto worker_thread(gpointer data) -> gpointer;

    GRTTaskBase::Ref _current_task;

    GRTDispatcher(bool threaded, bool is_main_dispatcher);

    auto prepare_task(const GRTTaskBase::Ref task) -> void;
    auto execute_task(const GRTTaskBase::Ref task) -> void;

    auto worker_thread_init() -> void;
    auto worker_thread_release() -> void;
    auto worker_thread_iteration() -> void;

    auto restore_callbacks(const GRTTaskBase::Ref task) -> void;

    auto message_callback(const grt::Message &msg, void *sender) -> bool;

  public:
    static auto create_dispatcher(bool threaded, bool is_main_dispatcher) -> Ref;

    virtual ~GRTDispatcher();

    auto execute_now(const GRTTaskBase::Ref task) -> void;

    auto add_task(const GRTTaskBase::Ref task) -> void;
    auto add_task_and_wait(const GRTTaskBase::Ref task) -> grt::ValueRef;

    auto execute_sync_function(const std::string &name, const std::function<grt::ValueRef()> &function)
      -> grt::ValueRef;

    auto execute_async_function(const std::string &name, const std::function<grt::ValueRef()> &function) -> void;

    auto wait_task(const GRTTaskBase::Ref task) -> void;

    template <class R>
    auto call_from_main_thread(const std::function<R()> &callback, bool wait, bool force_queue) -> R {
      typename DispatcherCallback<R>::Ref cb = DispatcherCallback<R>::create_callback(callback);
      call_from_main_thread(cb, wait, force_queue);
      return cb->get_result();
    }

    auto call_from_main_thread(const DispatcherCallbackBase::Ref callback, bool wait, bool force_queue) -> void;

    auto set_main_thread_flush_and_wait(FlushAndWaitCallback callback) -> void;
    auto get_main_thread_flush_and_wait() -> FlushAndWaitCallback {
      return _flush_main_thread_and_wait;
    }

    auto start() -> void;
    auto shutdown() -> void;

    auto get_busy() -> bool;

    auto cancel_task(const GRTTaskBase::Ref task) -> void;

    auto flush_pending_callbacks() -> void;

    auto get_thread() const -> GThread * {
      return _thread;
    }
  };

  template <>
  inline void GRTDispatcher::call_from_main_thread<void>(const std::function<void()> &callback, bool wait,
                                                         bool force_queue) {
    DispatcherCallback<void>::Ref cb = DispatcherCallback<void>::create_callback(callback);
    call_from_main_thread(cb, wait, force_queue);
  }
}; // namespace bec
