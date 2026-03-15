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
#include "../lf_textbox.h"
#include "gtk_helpers.h"
#include <gdk/gdkkeysyms-compat.h>

namespace mforms {
  namespace gtk {

    /*
    Glib::RefPtr<Gtk::TextBuffer::Tag> TextBoxImpl::tag_for_text_attributes(const mforms::TextAttributes &attr)
    {
      unsigned int hash = 0;
      if (attr.color.is_valid())
      {
        hash |= (unsigned int)(attr.color.red * 255) << 8;
        hash |= (unsigned int)(attr.color.green * 255) << 16;
        hash |= (unsigned int)(attr.color.blue * 255) << 24;
      }
      if (attr.bold)
        hash |= 1;
      if (attr.italic)
        hash |= 2;
      char name[32];
      snprintf(name, sizeof(name), "%x", hash);
      Glib::RefPtr<Gtk::TextBuffer::Tag> tag;
      if (!(tag = _text->get_buffer()->get_tag_table()->lookup(name)))
      {
        tag = _text->get_buffer()->create_tag(name);
        if (attr.bold)
          tag->property_weight() = Pango::WEIGHT_BOLD;
        if (attr.italic)
          tag->property_style() = Pango::STYLE_ITALIC;
        if (attr.color.is_valid())
        {
          char color[16];
          snprintf(color, sizeof(color), "#%02x%02x%02x", (int)(attr.color.red*255), (int)(attr.color.green*255),
    (int)(attr.color.blue*255));
          tag->property_foreground() = color;
        }
      }
      return tag;
    }*/

    auto TextBoxImpl::create(::mforms::TextBox *self, mforms::ScrollBars scroll_type) -> bool {
      return new TextBoxImpl(self, scroll_type) != 0;
    }

    auto TextBoxImpl::set_text(::mforms::TextBox *self, const std::string &text) -> void {
      TextBoxImpl *cb = self->get_data<TextBoxImpl>();

      if (cb)
        cb->_text->get_buffer()->set_text(text);
    }

    auto TextBoxImpl::append_text(::mforms::TextBox *self, const std::string &text, bool scroll_to_end) -> void {
      TextBoxImpl *cb = self->get_data<TextBoxImpl>();

      if (cb) {
        Gtk::TextView *tv = cb->_text;
        if (tv) {
          Glib::RefPtr<Gtk::TextBuffer> buf = tv->get_buffer();
          buf->insert(buf->end(), text);

          if (scroll_to_end)
            gtk_text_view_scroll_to_mark(tv->gobj(), tv->get_buffer()->get_insert()->gobj(), 0.3, false, 0, 0);
        }
      }
    }

    /*
    void TextBoxImpl::append_text_with_attributes(::mforms::TextBox *self, const std::string &text, const
    ::mforms::TextAttributes &attr, bool scroll_to_end)
    {
      TextBoxImpl* cb = self->get_data<TextBoxImpl>();

      if ( cb )
      {
        Gtk::TextView *tv = cb->_text;
        if (tv)
        {
          Glib::RefPtr<Gtk::TextBuffer> buf = tv->get_buffer();
          buf->insert_with_tag(buf->end(), text, cb->tag_for_text_attributes(attr));

          if (scroll_to_end)
          {
            Gtk::TextIter it = buf->end();
            tv->scroll_to(it, 0.3);
          }
        }
      }
    }*/

    auto TextBoxImpl::get_text(::mforms::TextBox *self) -> std::string {
      TextBoxImpl *cb = self->get_data<TextBoxImpl>();
      std::string ret("");
      if (cb) {
        ret = cb->_text->get_buffer()->get_text().raw();
      }
      return ret;
    }

    auto TextBoxImpl::set_read_only(::mforms::TextBox *self, bool flag) -> void {
      TextBoxImpl *cb = self->get_data<TextBoxImpl>();
      if (cb && cb->_text)
        cb->_text->set_editable(!flag);
    }

    auto TextBoxImpl::set_padding(::mforms::TextBox *self, int pad) -> void {
      TextBoxImpl *cb = self->get_data<TextBoxImpl>();
      if (cb && cb->_swin)
        cb->_swin->set_border_width(pad);
    }

    auto TextBoxImpl::set_bordered(::mforms::TextBox *self, bool flag) -> void {
      TextBoxImpl *cb = self->get_data<TextBoxImpl>();
      if (cb)
        cb->_swin->set_shadow_type(flag ? Gtk::SHADOW_IN : Gtk::SHADOW_NONE);
    }

    auto TextBoxImpl::set_monospaced(::mforms::TextBox *self, bool flag) -> void {
      TextBoxImpl *cb = self->get_data<TextBoxImpl>();
      if (cb) {
        Pango::FontDescription font = cb->_text->get_pango_context()->get_font_description();

        if (flag) {
          font.set_family("Bitstream Vera Sans Mono");
          font.set_size(9 * Pango::SCALE);
        }
        cb->_text->override_font(font);
      }
    }

    auto TextBoxImpl::get_selected_range(::mforms::TextBox *self, int &start, int &end) -> void {
      TextBoxImpl *cb = self->get_data<TextBoxImpl>();
      if (cb) {
        Gtk::TextBuffer::iterator sbegin, send;
        if (cb->_text->get_buffer()->get_selection_bounds(sbegin, send)) {
          start = sbegin.get_offset();
          end = send.get_offset();
        } else {
          start = 0;
          end = 0;
        }
      }
    }

    auto TextBoxImpl::clear(::mforms::TextBox *self) -> void {
      TextBoxImpl *cb = self->get_data<TextBoxImpl>();
      if (cb && cb->_text)
        cb->_text->get_buffer()->set_text("");
    }

    auto TextBoxImpl::set_front_color(const std::string &color) -> void {
      this->_text->override_color(color_to_rgba(Gdk::Color(color)), Gtk::STATE_FLAG_NORMAL);
    }

    TextBoxImpl::TextBoxImpl(::mforms::TextBox *self, mforms::ScrollBars scroll_type) : ViewImpl(self) {
      _swin = Gtk::manage(new Gtk::ScrolledWindow());
      _text = Gtk::manage(new Gtk::TextView());
      _swin->add(*_text);

      Gtk::PolicyType h_scrollbar_policy = Gtk::POLICY_AUTOMATIC;
      Gtk::PolicyType v_scrollbar_policy = Gtk::POLICY_AUTOMATIC;

      switch (scroll_type) {
        case ::mforms::NoScrollBar: {
          h_scrollbar_policy = Gtk::POLICY_NEVER;
          v_scrollbar_policy = Gtk::POLICY_NEVER;
          _text->set_wrap_mode(Gtk::WRAP_WORD);
          break;
        }
        case ::mforms::HorizontalScrollBar: {
          h_scrollbar_policy = Gtk::POLICY_AUTOMATIC;
          v_scrollbar_policy = Gtk::POLICY_NEVER;
          _text->set_wrap_mode(Gtk::WRAP_NONE);
          break;
        }
        case ::mforms::VerticalScrollBar: {
          h_scrollbar_policy = Gtk::POLICY_NEVER;
          v_scrollbar_policy = Gtk::POLICY_AUTOMATIC;
          _text->set_wrap_mode(Gtk::WRAP_WORD_CHAR);
          break;
        }
        case ::mforms::SmallScrollBars:
        case ::mforms::BothScrollBars: {
          h_scrollbar_policy = Gtk::POLICY_AUTOMATIC;
          v_scrollbar_policy = Gtk::POLICY_AUTOMATIC;
          _text->set_wrap_mode(Gtk::WRAP_NONE);
          break;
        }
      }
      _swin->set_policy(h_scrollbar_policy, v_scrollbar_policy);
      _swin->set_shadow_type(Gtk::SHADOW_IN);
      _text->show();
      _swin->show();

      _text->get_buffer()->signal_changed().connect(sigc::mem_fun(*self, &::mforms::TextBox::callback));
      _text->add_events(Gdk::KEY_PRESS_MASK);
      _text->signal_key_press_event().connect(sigc::bind(sigc::mem_fun(this, &TextBoxImpl::on_key_press), self), true);
      setup();
    }

    auto TextBoxImpl::on_key_press(GdkEventKey *event, mforms::TextBox *self) -> bool {
      return !self->key_event(GetKeys(event->keyval), GetModifiers(event->state, event->keyval), "");
    }

    auto TextBoxImpl::init() -> void {
      ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

      f->_textbox_impl.create = &TextBoxImpl::create;
      f->_textbox_impl.set_bordered = &TextBoxImpl::set_bordered;
      f->_textbox_impl.set_text = &TextBoxImpl::set_text;
      f->_textbox_impl.get_text = &TextBoxImpl::get_text;
      f->_textbox_impl.append_text = &TextBoxImpl::append_text;
      //  f->_textbox_impl.append_text_with_attributes = &TextBoxImpl::append_text_with_attributes;
      f->_textbox_impl.set_read_only = &TextBoxImpl::set_read_only;
      f->_textbox_impl.set_padding = &TextBoxImpl::set_padding;
      f->_textbox_impl.get_selected_range = &TextBoxImpl::get_selected_range;
      f->_textbox_impl.set_monospaced = &TextBoxImpl::set_monospaced;
      f->_textbox_impl.clear = &TextBoxImpl::clear;
    }
  };
};
