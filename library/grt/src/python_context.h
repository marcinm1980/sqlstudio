/*
 * Copyright (c) 2009, 2020, Oracle and/or its affiliates.
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

#include "base/python_utils.h"

#include "grt.h"
#include "grtpp_notifications.h"

namespace grt {
  const std::string LanguagePython = "python";

  class AutoPyObject {
  private:
    PyObject *object;
    bool autorelease;

  public:
    AutoPyObject() : object(0), autorelease(false) {
    }

    // Assigning another auto object always makes this one ref-counting as they share
    // now the same object. Same for the assignment operator.
    AutoPyObject(const AutoPyObject &other) : object(other.object), autorelease(false) {
      Py_XINCREF(object);
    }

    AutoPyObject(PyObject *py, bool retain = true) : object(py) {
      autorelease = retain;
      if (autorelease) {
        // Leave the braces in place, even though this is a one liner. They will silence LLVM.
        Py_XINCREF(object);
      }
    }

    ~AutoPyObject() {
      if (autorelease) {
        Py_XDECREF(object);
      }
    }

    AutoPyObject &operator=(PyObject *other) {
      if (object == other) // Ignore assignments of the same object.
        return *this;

      // Auto release only if we actually have increased its ref count.
      // Always make this auto object auto-releasing after that as we get an object that might
      // be shared by another instance.
      if (autorelease) {
        Py_XDECREF(object);
      }

      object = other;
      autorelease = false;
      Py_XINCREF(object);

      return *this;
    }

    operator bool() {
      return object != 0;
    }

    operator PyObject *() {
      return object;
    }

    PyObject *operator->() {
      return object;
    }
  };

  // Helper class to allow cleaning up the python context after its instance vars are finished.
  class MYSQLGRT_PUBLIC PythonContextHelper {
  private:
    PyThreadState *_main_thread_state;
    PyConfig _config;

  protected:
    PythonContextHelper(const std::string &module_path);
    auto InitPython() -> void;

  public:
    virtual ~PythonContextHelper();
  };

  class MYSQLGRT_PUBLIC PythonContext : private PythonContextHelper, public GRTObserver {
  public:
    PythonContext(const std::string &module_path);
    virtual ~PythonContext();

    static auto get() -> PythonContext *;
    static auto get_and_check() -> PythonContext *;

    static auto internal_cobject_from_value(const ValueRef &value) -> PyObject *;
    static auto value_from_internal_cobject(PyObject *value) -> ValueRef;

    static auto set_wrap_pyobject_func(PyObject *(*func)(PyObject *, PyObject *)) -> void;
    static auto set_unwrap_pyobject_func(PyObject *(*func)(PyObject *, PyObject *)) -> void;

    auto add_module_path(const std::string &path, bool prepend = false) -> void;
    auto import_module(const std::string &name) -> PyObject *;

    auto from_grt(const ValueRef &value) -> PyObject *;
    auto from_pyobject(PyObject *object) -> grt::ValueRef;
    auto from_pyobject(PyObject *object, const grt::TypeSpec &expected_type) -> grt::ValueRef;
    auto pystring_to_string(PyObject *str, std::string &ret_string, bool convert = false) -> bool;

    auto run_file(const std::string &file, bool interactive) -> int;
    auto run_buffer(const std::string &buffer, std::string *line_buffer = 0) -> int;

    auto call_grt_function(const std::string &module, const std::string &function, const BaseListRef &args) -> int;

    auto eval_string(const std::string &expression) -> PyObject *;

    auto get_grt_module() -> PyObject *;

    auto get_global(const std::string &value) -> PyObject *;
    auto set_global(const std::string &name, PyObject *value) -> bool;

    auto refresh() -> int;

    auto set_cwd(const std::string &path) -> bool;
    auto get_cwd() const -> std::string {
      return _cwd;
    }

    std::function<std::string()> stdin_readline_slot;

    static auto set_user_interrupted(const grt::user_cancelled &exc) -> void;
    static auto set_db_access_denied(const grt::db_access_denied &exc) -> void;
    static auto set_db_login_error(const grt::db_login_error &exc) -> void;
    static auto set_db_not_conected(const grt::db_not_connected &exc) -> void;
    static auto set_db_error(const grt::db_error &exc) -> void;
    static void set_python_error(const grt::type_error &exc, const std::string &location = "");
    static void set_python_error(const grt::bad_item &exc, const std::string &location = "");
    static void set_python_error(const std::exception &exc, const std::string &location = "");

    static auto log_python_error(const char *message) -> void;

    auto user_interrupted_error() -> PyObject * {
      return _grt_user_interrupt_error;
    }
    auto db_access_denied_error() -> PyObject * {
      return _grt_db_access_denied_error;
    }
    auto db_login_error() -> PyObject * {
      return _grt_db_login_error;
    }
    auto db_error() -> PyObject * {
      return _grt_db_error;
    }
    auto db_not_connected() -> PyObject * {
      return _grt_db_not_connected;
    }

    auto set_grt_observer_callable(PyObject *obj) -> void;
    auto setEventlogCallback(PyObject *obj) -> void;
    auto printResult(std::map<std::string, std::string> &output) -> void;

    static auto grt_module_create() -> PyObject *;
//     static PyObject *grt_modules_module_create();

  protected:
    std::string _cwd;
    AutoPyObject _grt_module;
    AutoPyObject _grt_classes_module;
    AutoPyObject _grt_modules_module;

    AutoPyObject _grt_module_class;
    AutoPyObject _grt_function_class;

    AutoPyObject _grt_list_class;
    AutoPyObject _grt_dict_class;
    AutoPyObject _grt_object_class;
    AutoPyObject _grt_method_class;

    AutoPyObject _grt_user_interrupt_error;
    AutoPyObject _grt_db_access_denied_error;
    AutoPyObject _grt_db_login_error;
    AutoPyObject _grt_db_error;
    AutoPyObject _grt_db_not_connected;

    AutoPyObject _grt_notification_observer;

    AutoPyObject _grtEventLogNotification;

    std::map<std::string, AutoPyObject> _grt_class_wrappers;

  private:
    auto simple_type_from_pyobject(PyObject *object, const grt::SimpleTypeSpec &type) -> ValueRef;

    auto register_grt_module(PyObject *module) -> void;
    auto register_grt_functions() -> void;
    auto redirect_python_output() -> void;

    auto init_grt_module_type() -> void;
    auto init_grt_list_type() -> void;
    auto init_grt_dict_type() -> void;
    auto init_grt_object_type() -> void;

    auto run_post_init_script() -> void;

    virtual auto handle_grt_notification(const std::string &name, ObjectRef sender, DictRef info) -> void;
    virtual auto handle_notification(const std::string &name, void *sender, base::NotificationInfo &info) -> void;
  };

  class python_error : public std::runtime_error {
  public:
    python_error(const std::string &what) : std::runtime_error(what) {
    }
  };
};
