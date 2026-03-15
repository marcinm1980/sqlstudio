/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _LF_PROGRESSBAR_H_
#define _LF_PROGRESSBAR_H_

#include "mforms/progressbar.h"

#include "lf_view.h"

namespace mforms {
  namespace gtk {

    class ProgressBarImpl : public ViewImpl {
    protected:
      Gtk::ProgressBar *_pbar;
      sigc::connection _pbar_pulser;
      virtual auto get_outer() const -> Gtk::Widget * {
        return _pbar;
      }
      sigc::connection _idle_set_val;

      ProgressBarImpl(::mforms::ProgressBar *self) : ViewImpl(self) {
        _pbar = Gtk::manage(new Gtk::ProgressBar());
        _pbar->show();
        setup();
      }

      static auto create(::mforms::ProgressBar *self) -> bool {
        return new ProgressBarImpl(self) != 0;
      }

      static auto set_value(::mforms::ProgressBar *self, float pct) -> void {
        ProgressBarImpl *progressbar = self->get_data<ProgressBarImpl>();
        if (progressbar)
          progressbar->set_value(pct);
      }

      auto set_value(float pct) -> void {
        if (_pbar) {
          if (Utilities::in_main_thread())
            _pbar->set_fraction(pct);
          else {
            if (!_idle_set_val.empty())
              _idle_set_val.disconnect();

            _idle_set_val = Glib::signal_idle().connect(
              sigc::bind_return(sigc::bind(sigc::mem_fun(*_pbar, &Gtk::ProgressBar::set_fraction), pct), false));
          }
        }
      }

      auto pulse() -> bool {
        if (_pbar)
          this->_pbar->pulse();

        return true;
      }

      auto start() -> void {
        if (_pbar != NULL && _pbar_pulser.empty())
          _pbar_pulser = Glib::signal_timeout().connect(sigc::mem_fun(this, &ProgressBarImpl::pulse), 125);
      }

      auto stop() -> void {
        if (!_pbar_pulser.empty())
          _pbar_pulser.disconnect();

        if (_pbar)
          _pbar->set_fraction(0.0);
      }

      ~ProgressBarImpl() {
        if (_pbar) {
          if (!_idle_set_val.empty())
            _idle_set_val.disconnect();
          if (!_pbar_pulser.empty())
            _pbar_pulser.disconnect();
        }
      }

      static auto set_started(::mforms::ProgressBar *self, bool flag) -> void {
        ProgressBarImpl *progressbar = self->get_data<ProgressBarImpl>();
        if (progressbar) {
          if (flag)
            progressbar->start();
          else
            progressbar->stop();
        }
      }

      static auto set_indeterminate(::mforms::ProgressBar *self, bool flag) -> void {
      }

    public:
      static auto init() -> void {
        ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

        f->_progressbar_impl.create = &ProgressBarImpl::create;
        f->_progressbar_impl.set_value = &ProgressBarImpl::set_value;
        f->_progressbar_impl.set_started = &ProgressBarImpl::set_started;
        f->_progressbar_impl.set_indeterminate = &ProgressBarImpl::set_indeterminate;
      }
    };
  };
};

#endif /* _LF_PROGRESSBAR_H_ */
