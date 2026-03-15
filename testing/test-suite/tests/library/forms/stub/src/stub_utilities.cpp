/*
 * Copyright (c) 2008, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "../stub_utilities.h"

#include "base/log.h"
#include <cerrno>

using namespace mforms;
using namespace stub;

std::function<mforms::DialogResult(void)> UtilitiesWrapper::message_callback;
// the only reason why we're wrapping this map is so our unit tests don't segfault.  We're running into static
// initialisation order fiasco,
// because the unit tests are created before main() runs, at which point they call different things.  Feel free to
// remove the wrapper once the problem is amended.
/*static*/ std::map<const std::string, std::string> &UtilitiesWrapper::passwords() {
  static std::map<const std::string, std::string> passwords_;
  return passwords_;
}

DEFAULT_LOG_DOMAIN("mforms");

auto UtilitiesWrapper::show_message(const std::string &title, const std::string &text, const std::string &ok,
                                   const std::string &cancel, const std::string &other) -> int {
  logInfo("DIALOG: %s: %s\n", title.c_str(), text.c_str());
  if (message_callback)
    return message_callback();

  if (other == "Don't Save")
    return mforms::ResultOther;
  return mforms::ResultOk;
}

auto UtilitiesWrapper::show_error(const std::string &title, const std::string &text, const std::string &ok,
                                 const std::string &cancel, const std::string &other) -> int {
  logInfo("DIALOG: %s: %s\n", title.c_str(), text.c_str());
  if (message_callback)
    return message_callback();

  return mforms::ResultOk;
}

auto UtilitiesWrapper::show_warning(const std::string &title, const std::string &text, const std::string &ok,
                                   const std::string &cancel, const std::string &other) -> int {
  logInfo("DIALOG: %s: %s\n", title.c_str(), text.c_str());
  if (message_callback)
    return message_callback();

  return mforms::ResultOk;
}

int UtilitiesWrapper::show_message_with_checkbox(
  const std::string &title, const std::string &text, const std::string &ok, const std::string &cancel,
  const std::string &other,
  const std::string &checkbox_text, // empty text = default "Don't show this message again" text
  bool &remember_checked) {
  logInfo("DIALOG: %s: %s\n", title.c_str(), text.c_str());
  if (message_callback)
    return message_callback();

  return mforms::ResultOk;
}

auto UtilitiesWrapper::show_wait_message(const std::string &title, const std::string &text) -> void {
}

auto UtilitiesWrapper::hide_wait_message() -> bool {
  return true;
}

auto UtilitiesWrapper::run_cancelable_wait_message(const std::string &title, const std::string &text,
                                                   const std::function<void()> &start_task,
                                                   const std::function<bool()> &cancel_task) -> bool {
  return true;
}

auto UtilitiesWrapper::stop_cancelable_wait_message() -> void {
}

auto UtilitiesWrapper::set_clipboard_text(const std::string &text) -> void {
}

auto UtilitiesWrapper::get_clipboard_text() -> std::string {
  return "";
}

auto UtilitiesWrapper::open_url(const std::string &url) -> void {
}

auto UtilitiesWrapper::get_special_folder(mforms::FolderType type) -> std::string {
  return "./";
}

auto UtilitiesWrapper::add_timeout(float interval, const std::function<bool()> &slot) -> mforms::TimeoutHandle {
  return 0;
}

auto UtilitiesWrapper::cancel_timeout(mforms::TimeoutHandle) -> void {
}

auto UtilitiesWrapper::store_password(const std::string &service, const std::string &account,
                                      const std::string &password) -> void {
  passwords()[service + ":" + account] = password;
}

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::find_password(const std::string &service, const std::string &account, std::string &password) -> bool {
  static bool loaded_passwords = false;
  bool ret_val = false;

  if (!loaded_passwords) {
    // FIXME: Load passwords from configuration
    /*const auto &tutPasswords = testing::Context::get().configuration["tutPasswords"];
    try {
      for (auto &entry: tutPasswords.GetArray()) {
        if (entry.HasMember("service")) {
          passwords()[entry["service"].GetString()] = entry["password"].GetString();
          if (std::get<bool>(testing::Context::get().settings["verbose"])) {
            g_message("%s=%s", entry["service"].GetString(), entry["password"].GetString());
          }
        }
      }
    } catch (std::out_of_range &) {
      g_message("Config file is missing service credentials.\n");
    }*/
    loaded_passwords = true;
  }

  if (passwords().count(service + ":" + account)) {
    password = passwords()[service + ":" + account];
    ret_val = true;
  } else {
    if (passwords().count(":" + account)) {
      password = passwords()[":" + account];
      ret_val = true;
    } else
      logError("Unknown password for %s:%s\n", service.c_str(), account.c_str());
  }
  return ret_val;
}

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::forget_password(const std::string &service, const std::string &account) -> void {
}

//--------------------------------------------------------------------------------------------------

enum { Gnome_keyring_results_size = 10 };

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::perform_from_main_thread(const std::function<void *()> &slot, bool wait) -> void * {
  return slot();
};

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::beep() -> void {
}

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::revealFile(const std::string &url) -> void {
}

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::moveToTrash(const std::string &path) -> bool {
  return false;
}

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::setThreadName(const std::string &name) -> void {
}

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::getTextWidth(const std::string &text, const std::string &font) -> double {
  return 0.0;
}

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::init() -> void {
  ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

  f->_utilities_impl.show_message = &UtilitiesWrapper::show_message;
  f->_utilities_impl.show_error = &UtilitiesWrapper::show_error;
  f->_utilities_impl.show_warning = &UtilitiesWrapper::show_warning;
  f->_utilities_impl.set_clipboard_text = &UtilitiesWrapper::set_clipboard_text;
  f->_utilities_impl.get_clipboard_text = &UtilitiesWrapper::get_clipboard_text;
  f->_utilities_impl.open_url = &UtilitiesWrapper::open_url;
  f->_utilities_impl.add_timeout = &UtilitiesWrapper::add_timeout;
  f->_utilities_impl.cancel_timeout = &UtilitiesWrapper::cancel_timeout;
  f->_utilities_impl.get_special_folder = &UtilitiesWrapper::get_special_folder;
  f->_utilities_impl.store_password = &UtilitiesWrapper::store_password;
  f->_utilities_impl.find_password = &UtilitiesWrapper::find_password;
  f->_utilities_impl.forget_password = &UtilitiesWrapper::forget_password;

  f->_utilities_impl.hide_wait_message = &UtilitiesWrapper::hide_wait_message;
  f->_utilities_impl.run_cancelable_wait_message = &UtilitiesWrapper::run_cancelable_wait_message;
  f->_utilities_impl.show_message_with_checkbox = &UtilitiesWrapper::show_message_with_checkbox;
  f->_utilities_impl.show_wait_message = &UtilitiesWrapper::show_wait_message;
  f->_utilities_impl.stop_cancelable_wait_message = &UtilitiesWrapper::stop_cancelable_wait_message;
  f->_utilities_impl.perform_from_main_thread = &UtilitiesWrapper::perform_from_main_thread;

  f->_utilities_impl.beep = &UtilitiesWrapper::beep;
  f->_utilities_impl.reveal_file = &UtilitiesWrapper::revealFile;
  f->_utilities_impl.move_to_trash = &UtilitiesWrapper::moveToTrash;
  f->_utilities_impl.set_thread_name = &UtilitiesWrapper::setThreadName;
  f->_utilities_impl.get_text_width = &UtilitiesWrapper::getTextWidth;
}

//--------------------------------------------------------------------------------------------------

auto UtilitiesWrapper::set_message_callback(std::function<mforms::DialogResult(void)> callback) -> void {
  message_callback = callback;
}

//--------------------------------------------------------------------------------------------------
