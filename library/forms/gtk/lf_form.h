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

#ifndef _LF_FORM_H_
#define _LF_FORM_H_

#include "mforms/mforms.h"
#include <boost/signals2.hpp>
#include <sigc++/sigc++.h>

#include "gtk_helpers.h"
#include "main_app.h"
#include "lf_button.h"
#include "lf_view.h"
#include "lf_mforms.h"

namespace mforms {
  namespace gtk {

    class FormImpl : public ViewImpl {
      runtime::loop _loop;
      Gtk::Window *_window;
      int _in_modal_loop;
      bool _result;
      virtual auto get_outer() const -> Gtk::Widget * {
        return _window;
      }
      boost::signals2::scoped_connection accept_c;
      boost::signals2::scoped_connection cancel_c;

      static auto create(::mforms::Form *self, ::mforms::Form *owner, mforms::FormFlag flag) -> bool;
      static auto set_title(::mforms::Form *self, const std::string &title) -> void;
      auto accept_clicked(bool *status, const bool is_run) -> void;
      auto cancel_clicked(bool *status, const bool is_run) -> void;
      auto on_widget_delete_event(GdkEventAny *event, Button *cancel) -> bool;
      auto can_delete_widget(GdkEventAny *event) -> bool;
      static auto show_modal(::mforms::Form *self, ::mforms::Button *accept, ::mforms::Button *cancel) -> void;
      static auto end_modal(::mforms::Form *self, bool result) -> void;
      auto on_key_release(GdkEventKey *event, bool *status, const bool is_run, ::mforms::Button *accept,
                          ::mforms::Button *cancel) -> bool;
      static auto run_modal(::mforms::Form *self, ::mforms::Button *accept, ::mforms::Button *cancel) -> bool;
      static auto close(::mforms::Form *self) -> void;
      static auto set_content(::mforms::Form *self, ::mforms::View *child) -> void;
      static auto flush_events(::mforms::Form *self) -> void;
      static auto center(Form *self) -> void;
      static auto set_menubar(mforms::Form *self, mforms::MenuBar *menu) -> void;
      auto realized(mforms::Form *owner, Gdk::WMDecoration flags) -> void;
      virtual auto set_name(const std::string &name) -> void;
      virtual auto show(bool show) -> void;
      auto on_focus_event(GdkEventFocus *ev, ::mforms::Form *form) -> bool;

    public:
      FormImpl(::mforms::Form *form, ::mforms::Form *owner, mforms::FormFlag form_flag);
      virtual auto set_title(const std::string &title) -> void;
      static auto init() -> void;
      static auto init_main_form(Gtk::Window *main) -> void;
      auto get_window() -> Gtk::Window * {
        return _window;
      }
    };
  };
};

#endif
