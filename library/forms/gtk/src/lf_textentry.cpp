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

#include "../lf_mforms.h"
#include "../lf_textentry.h"
#include "gtk_helpers.h"

namespace mforms {
  namespace gtk {

    auto TextEntryImpl::create(::mforms::TextEntry *self, TextEntryType type) -> bool {
      return new TextEntryImpl(self, type);
    }

    auto TextEntryImpl::set_text(::mforms::TextEntry *self, const std::string &text) -> void {
      TextEntryImpl *cb = self->get_data<TextEntryImpl>();

      if (cb) {
        cb->set_text(text);
      }
    }

    auto TextEntryImpl::set_placeholder_text(::mforms::TextEntry *self, const std::string &text) -> void {
      TextEntryImpl *cb = self->get_data<TextEntryImpl>();

      if (cb) {
        cb->set_placeholder_text(text);
      }
    }

    auto TextEntryImpl::set_max_length(::mforms::TextEntry *self, int len) -> void {
      TextEntryImpl *cb = self->get_data<TextEntryImpl>();

      if (cb) {
        cb->_entry->set_max_length(len);
      }
    }

    auto TextEntryImpl::get_text(::mforms::TextEntry *self) -> std::string {
      TextEntryImpl *cb = self->get_data<TextEntryImpl>();
      std::string ret("");
      if (cb && cb->_has_real_text) {
        ret = cb->_entry->get_text().raw();
      }
      return ret;
    }

    auto TextEntryImpl::set_read_only(::mforms::TextEntry *self, bool flag) -> void {
      TextEntryImpl *cb = self->get_data<TextEntryImpl>();
      if (cb && cb->_entry)
        cb->_entry->set_editable(!flag);
    }

    auto TextEntryImpl::set_bordered(::mforms::TextEntry *self, bool flag) -> void {
      TextEntryImpl *te = self->get_data<TextEntryImpl>();
      if (te)
        te->_entry->set_has_frame(flag);
    }

    TextEntryImpl::TextEntryImpl(::mforms::TextEntry *self, TextEntryType type)
      : ViewImpl(self), _has_real_text(false), _changing_text(false) {
      _entry = Gtk::manage(new Gtk::Entry());
      _type = type;
      switch (type) {
        case mforms::NormalEntry:
          break;
        case mforms::PasswordEntry:
          _entry->set_visibility(false);
          break;
        case mforms::SearchEntry:
          _entry->set_icon_from_stock(Gtk::Stock::FIND);
          _entry->signal_icon_press().connect(sigc::mem_fun(this, &TextEntryImpl::icon_pressed));
          break;
      }
      _entry->set_width_chars(1); // We need to reset it to 1 so set_size_request will work.
      _entry->signal_changed().connect(sigc::bind(sigc::mem_fun(this, &TextEntryImpl::changed), self));
      _entry->signal_activate().connect(sigc::bind(sigc::mem_fun(this, &TextEntryImpl::activated), self));
      _entry->signal_key_press_event().connect(sigc::bind(sigc::mem_fun(this, &TextEntryImpl::key_press), self));
      _entry->signal_focus_in_event().connect_notify(sigc::mem_fun(this, &TextEntryImpl::focus_in));
      _entry->signal_focus_out_event().connect_notify(sigc::mem_fun(this, &TextEntryImpl::focus_out));
      _entry->add_events(Gdk::KEY_PRESS_MASK);
      _entry->show();
      _text_color = _entry->get_style_context()->get_color(Gtk::STATE_FLAG_NORMAL);
      Gdk::Color color("#888888");
      _placeholder_color = color_to_rgba(color);

      setup();
    }

    auto TextEntryImpl::icon_pressed(Gtk::EntryIconPosition pos, const GdkEventButton *ev) -> void {
      if (pos == Gtk::ENTRY_ICON_SECONDARY)
        set_text("");
    }

    auto TextEntryImpl::activated(mforms::TextEntry *self) -> void {
      self->action(mforms::EntryActivate);
    }

    auto TextEntryImpl::key_press(GdkEventKey *event, mforms::TextEntry *self) -> bool {
      if (event->keyval == GDK_KEY_Up) {
        if (event->state & GDK_CONTROL_MASK)
          self->action(mforms::EntryCKeyUp);
        else
          self->action(mforms::EntryKeyUp);
        return true;
      } else if (event->keyval == GDK_KEY_Down) {
        if (event->state & GDK_CONTROL_MASK)
          self->action(mforms::EntryCKeyDown);
        else
          self->action(mforms::EntryKeyDown);
        return true;
      } else if (event->keyval == GDK_KEY_Escape) {
        self->action(mforms::EntryEscape);
        return true;
      }
      return false;
    }

    auto TextEntryImpl::changed(mforms::TextEntry *self) -> void {
      if (_changing_text)
        return;
      if (_has_real_text) {
        if (_type == mforms::SearchEntry) {
          if (_entry->get_text().empty())
            _entry->set_icon_from_pixbuf(Glib::RefPtr<Gdk::Pixbuf>(), Gtk::ENTRY_ICON_SECONDARY);
          else
            _entry->set_icon_from_stock(Gtk::Stock::CLEAR, Gtk::ENTRY_ICON_SECONDARY);
        }

        if (_entry->get_text().empty())
          _has_real_text = false;
      } else
        _has_real_text = !_entry->get_text().empty();
      self->callback();
    }

    auto TextEntryImpl::set_front_color(const std::string &color) -> void {
      this->_text_color = color_to_rgba(Gdk::Color(color));
    }

    auto TextEntryImpl::set_back_color(const std::string &color) -> void {
      ViewImpl::set_back_color(color);
      Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
      if (!color.empty())
        provider->load_from_data(".entry { background: " + color + "; }");
      _entry->get_style_context()->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

    auto TextEntryImpl::set_text(const std::string &text) -> void {
      if (!text.empty()) {
        if (!_has_real_text)
          focus_out(NULL);
        _has_real_text = true;
      } else {
        if (_has_real_text)
          focus_in(NULL);
        _has_real_text = false;
      }
      _entry->set_text(text);
    }

    auto TextEntryImpl::set_placeholder_text(const std::string &text) -> void {
      _entry->set_placeholder_text(text);
    }

    auto TextEntryImpl::set_placeholder_color(::mforms::TextEntry *self, const std::string &color) -> void {
      TextEntryImpl *te = self->get_data<TextEntryImpl>();
      if (te)
        te->_placeholder_color = color_to_rgba(Gdk::Color(color));
    }

    auto TextEntryImpl::focus_in(GdkEventFocus *) -> void {
      if (!_has_real_text)
        _entry->override_color(_text_color, Gtk::STATE_FLAG_NORMAL);
    }

    auto TextEntryImpl::focus_out(GdkEventFocus *) -> void {
      if (!_has_real_text)
        _entry->override_color(_placeholder_color, Gtk::STATE_FLAG_NORMAL);
    }

    auto TextEntryImpl::cut(::mforms::TextEntry *self) -> void {
      TextEntryImpl *te = self->get_data<TextEntryImpl>();
      if (te)
        te->_entry->cut_clipboard();
    }

    auto TextEntryImpl::copy(::mforms::TextEntry *self) -> void {
      TextEntryImpl *te = self->get_data<TextEntryImpl>();
      if (te)
        te->_entry->copy_clipboard();
    }

    auto TextEntryImpl::paste(::mforms::TextEntry *self) -> void {
      TextEntryImpl *te = self->get_data<TextEntryImpl>();
      if (te)
        te->_entry->paste_clipboard();
    }

    auto TextEntryImpl::select(::mforms::TextEntry *self, const base::Range &range) -> void {
      TextEntryImpl *te = self->get_data<TextEntryImpl>();
      if (te) {
        if (range.size > 0)
          te->_entry->select_region(range.position, range.position + range.size);
        else
          te->_entry->set_position(range.position);
      }
    }

    auto TextEntryImpl::get_selection(::mforms::TextEntry *self) -> base::Range {
      TextEntryImpl *te = self->get_data<TextEntryImpl>();
      base::Range range;
      int start, end;
      if (te->_entry->get_selection_bounds(start, end)) {
        range.position = start;
        range.size = end - start;
      } else {
        range.position = te->_entry->get_position();
        range.size = 0;
      }
      return range;
    }

    auto TextEntryImpl::init() -> void {
      ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

      f->_textentry_impl.create = &TextEntryImpl::create;
      f->_textentry_impl.set_text = &TextEntryImpl::set_text;
      f->_textentry_impl.set_max_length = &TextEntryImpl::set_max_length;
      f->_textentry_impl.get_text = &TextEntryImpl::get_text;
      f->_textentry_impl.set_read_only = &TextEntryImpl::set_read_only;
      f->_textentry_impl.set_placeholder_text = &TextEntryImpl::set_placeholder_text;
      f->_textentry_impl.set_placeholder_color = &TextEntryImpl::set_placeholder_color;
      f->_textentry_impl.set_bordered = &TextEntryImpl::set_bordered;
      f->_textentry_impl.cut = &TextEntryImpl::cut;
      f->_textentry_impl.paste = &TextEntryImpl::paste;
      f->_textentry_impl.copy = &TextEntryImpl::copy;
      f->_textentry_impl.select = &TextEntryImpl::select;
      f->_textentry_impl.get_selection = &TextEntryImpl::get_selection;
    }
  };
};
