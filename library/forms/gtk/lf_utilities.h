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

#ifndef _LF_UTILITIES_H_
#define _LF_UTILITIES_H_

#include "mforms/mforms.h"
#include <sigc++/sigc++.h>

#include "gtk_helpers.h"

#include "lf_mforms.h"

namespace mforms {
  namespace gtk {

    class UtilitiesImpl {
      static auto show_message(const std::string &title, const std::string &text, const std::string &ok,
                              const std::string &cancel, const std::string &other) -> int;
      static auto show_error(const std::string &title, const std::string &text, const std::string &ok,
                            const std::string &cancel, const std::string &other) -> int;
      static auto show_warning(const std::string &title, const std::string &text, const std::string &ok,
                              const std::string &cancel, const std::string &other) -> int;
      static int show_message_with_checkbox(
        const std::string &title, const std::string &text, const std::string &ok, const std::string &cancel,
        const std::string &other,
        const std::string &checkbox_text, // empty text = default "Don't show this message again" text
        bool &remember_checked);

      static auto show_wait_message(const std::string &title, const std::string &text) -> void;
      static auto hide_wait_message() -> bool;
      static auto run_cancelable_wait_message(const std::string &title, const std::string &text,
                                              const std::function<void()> &start_task,
                                              const std::function<bool()> &cancel_task) -> bool;
      static auto stop_cancelable_wait_message() -> void;

      static auto set_clipboard_text(const std::string &text) -> void;
      static auto get_clipboard_text() -> std::string;
      static auto open_url(const std::string &url) -> void;
      static auto get_special_folder(mforms::FolderType type) -> std::string;
      static auto add_timeout(float interval, const std::function<bool()> &slot) -> TimeoutHandle;
      static auto cancel_timeout(TimeoutHandle h) -> void;

      static auto store_password(const std::string &service, const std::string &account, const std::string &password) -> void;
      static auto find_password(const std::string &service, const std::string &account, std::string &password) -> bool;
      static auto forget_password(const std::string &service, const std::string &account) -> void;

      static auto move_to_trash(const std::string &path) -> bool;
      static auto reveal_file(const std::string &path) -> void;

      static auto set_thread_name(const std::string &name) -> void;
      static auto beep() -> void;

      static auto get_text_width(const std::string &text, const std::string &font_desc) -> double;

    public:
      static auto init() -> void;

      static auto get_cached_icon(const std::string &icon) -> Glib::RefPtr<Gdk::Pixbuf>;
    };

    class MainThreadRequestQueue {
      struct Request {
        std::function<void *()> slot;
        void *result;
        Glib::Mutex mutex;
        Glib::Cond cond;
        bool done;
      };

      Glib::Dispatcher _disp;
      Glib::Mutex _mutex;
      std::list<std::shared_ptr<Request> > _queue;

      auto from_main_thread() -> void;

    public:
      MainThreadRequestQueue();
      static auto get() -> MainThreadRequestQueue *;
      static auto perform(const std::function<void *()> &slot, bool wait) -> void *;
    };
  };
};

#endif
