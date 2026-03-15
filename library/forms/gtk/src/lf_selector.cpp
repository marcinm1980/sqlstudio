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
#include "../lf_selector.h"

namespace mforms {
  namespace gtk {

    // enum SelectorStyle
    //{
    //  SelectorSimple,       // The value list is always visible. The value is freely editable.
    //  SelectorDropDown,     // The value list is shown when clicking the arrow. The value is freely editable.
    //  SelectorDropDownList  // The value list is shown when clicking the arrow. The value can only be selected out of
    //  the
    //                        // values in the list.
    //};

    //==============================================================================
    //
    //==============================================================================
    class SelectorImpl::Impl : public sigc::trackable {
    public:
      virtual ~Impl(){};

      virtual auto widget() -> Gtk::Widget * = 0;
      virtual auto clear() -> void = 0;
      virtual auto add_item(const std::string &item) -> int = 0;
      virtual auto add_items(const std::list<std::string> &items) -> void = 0;
      virtual auto get_item(const int index) const -> std::string = 0;
      virtual auto get_text() const -> std::string = 0;
      virtual auto set_index(const int index) -> void = 0;
      virtual auto get_index() const -> int = 0;
      virtual auto get_item_count() const -> int = 0;
      virtual auto set_value(const std::string &) -> void {}; // It is only defined for editable combobox
    };

    //==============================================================================
    //
    //==============================================================================
    class SelectorPopupImpl : public SelectorImpl::Impl {
      //  private:

    public:
      SelectorPopupImpl(mforms::Selector *self) : _self(self), do_not_call_callback(false) {
        //      _list.signal_changed().connect(sigc::mem_fun(self, &mforms::Selector::callback));
        _list.signal_changed().connect(sigc::mem_fun(*this, &mforms::gtk::SelectorPopupImpl::wrap_callback_call));
        _list.set_row_separator_func(sigc::mem_fun(*this, &SelectorPopupImpl::is_separator));
      }

      auto widget() -> Gtk::Widget * {
        return &_list;
      }

      virtual auto clear() -> void {
        do_not_call_callback = true;
        _items.clear();
        _list.remove_all();
        do_not_call_callback = false;
      }

      auto is_separator(const Glib::RefPtr<Gtk::TreeModel> &model, const Gtk::TreeModel::iterator &iter) -> bool {
        Gtk::TreeRow row = *iter;
        Glib::ustring value;
        row.get_value(0, value);
        return value == "-";
      }

      virtual auto add_item(const std::string &item) -> int {
        _list.append(item);
        _items.push_back(item);
        if (_items.size() == 1)
          _list.set_active(0);
        return _items.size();
      }

      virtual auto add_items(const std::list<std::string> &items) -> void {
        std::list<std::string>::const_iterator it = items.begin();
        const std::list<std::string>::const_iterator last = items.end();
        for (; it != last; ++it) {
          _list.append(*it);
          _items.push_back(*it);
        }
        if (_items.size() > 0)
          _list.set_active(0);
      }

      virtual auto get_item(const int index) const -> std::string {
        if (index < 0 || index >= (int)_items.size())
          return "";
        return _items[index];
      }

      virtual auto get_text() const -> std::string {
        return _list.get_active_text();
      }

      virtual auto set_index(const int index) -> void {
        _list.set_active(index);
      }

      virtual auto get_index() const -> int {
        return _list.get_active_row_number();
      }

      virtual auto get_item_count() const -> int {
        return _items.size();
      }

    private:
      Gtk::ComboBoxText _list;
      std::vector<std::string> _items; // to impl get_item with GTK. [The rest of the comment is censored]
      mforms::Selector *_self;
      bool do_not_call_callback;

      virtual auto wrap_callback_call() -> void {
        if (do_not_call_callback)
          return;
        else
          _self->callback();
      }
    };

    //==============================================================================
    //
    //==============================================================================
    class SelectorComboboxImpl : public SelectorImpl::Impl {
    public:
      SelectorComboboxImpl(mforms::Selector *self) : _list(true) {
        _list.signal_changed().connect(sigc::mem_fun(self, &mforms::Selector::callback));
        _list.get_entry()->signal_insert_at_cursor().connect(
          sigc::hide(sigc::mem_fun(self, &mforms::Selector::callback)));
      }

      auto widget() -> Gtk::Widget * {
        return &_list;
      }

      virtual auto clear() -> void {
        _items.clear();
        _list.remove_all();
      }

      virtual auto add_item(const std::string &item) -> int {
        _items.push_back(item);
        _list.append(item);
        return _items.size();
      }

      virtual auto add_items(const std::list<std::string> &items) -> void {
        std::list<std::string>::const_iterator it = items.begin();
        const std::list<std::string>::const_iterator last = items.end();
        for (; it != last; ++it) {
          _list.append(*it);
          _items.push_back(*it);
        }
      }

      virtual auto get_item(const int index) const -> std::string {
        if (index < 0 || index >= (int)_items.size())
          return "";
        return _items[index];
      }

      virtual auto get_text() const -> std::string {
        return _list.get_entry()->get_text();
      }

      virtual auto set_index(const int index) -> void {
        _list.set_active(index);
      }

      virtual auto get_index() const -> int {
        return _list.get_active_row_number();
      }

      virtual auto get_item_count() const -> int {
        return _items.size();
      }

      virtual auto set_value(const std::string &value) -> void {
        _list.get_entry()->set_text(value);
      }

    private:
      Gtk::ComboBoxText _list;
      std::vector<std::string> _items; // to impl get_item with GTK. [The rest of the comment is censored]
    };

    //------------------------------------------------------------------------------
    SelectorImpl::SelectorImpl(::mforms::Selector *self, ::mforms::SelectorStyle style) : ViewImpl(self), _pimpl(0) {
      _outerBox = Gtk::manage(new Gtk::Box());
      // TODO: implement selector styles.
      //_pimpl= Gtk::manage(new Gtk::ComboBoxText());
      //_pimpl->show();
      if (style == SelectorCombobox)
        _pimpl = new SelectorComboboxImpl(self);
      else if (style == SelectorPopup)
        _pimpl = new SelectorPopupImpl(self);

      _outerBox->pack_start(*_pimpl->widget(), true, true);
      _outerBox->show_all();
      _pimpl->widget()->set_halign(Gtk::ALIGN_CENTER);
      _pimpl->widget()->set_valign(Gtk::ALIGN_CENTER);
    }

    //------------------------------------------------------------------------------
    SelectorImpl::~SelectorImpl() {
      delete _pimpl;
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::create(::mforms::Selector *self, ::mforms::SelectorStyle style) -> bool {
      return new SelectorImpl(self, style) != 0;
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::clear(::mforms::Selector *self) -> void {
      SelectorImpl *sel = self->get_data<SelectorImpl>();

      sel->_pimpl->clear();
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::add_item(::mforms::Selector *self, const std::string &item) -> int {
      SelectorImpl *sel = self->get_data<SelectorImpl>();

      int ret = 0;
      if (sel) {
        sel->_pimpl->add_item(item);
        ret = sel->_pimpl->get_item_count();
        if (ret == 1)
          sel->_pimpl->set_index(0);
      }

      return ret;
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::add_items(::mforms::Selector *self, const std::list<std::string> &items) -> void {
      SelectorImpl *sel = self->get_data<SelectorImpl>();

      if (sel)
        sel->_pimpl->add_items(items);
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::get_item(::mforms::Selector *self, int index) -> std::string {
      SelectorImpl *sel = self->get_data<SelectorImpl>();
      if (sel) {
        std::string value = sel->_pimpl->get_item(index);
        return value;
      }
      return "";
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::get_text(::mforms::Selector *self) -> std::string {
      SelectorImpl *sel = self->get_data<SelectorImpl>();
      if (sel) {
        std::string value = sel->_pimpl->get_text();
        return value;
      }
      return "";
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::set_index(::mforms::Selector *self, int index) -> void {
      SelectorImpl *sel = self->get_data<SelectorImpl>();

      if (sel)
        sel->_pimpl->set_index(index);
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::get_index(::mforms::Selector *self) -> int {
      SelectorImpl *sel = self->get_data<SelectorImpl>();
      int ret = -1;

      if (sel)
        ret = sel->_pimpl->get_index();

      return ret;
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::get_item_count(::mforms::Selector *self) -> int {
      SelectorImpl *sel = self->get_data<SelectorImpl>();

      int ret = -1;
      if (sel)
        ret = (int)sel->_pimpl->get_item_count();

      return ret;
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::set_value(::mforms::Selector *self, const std::string &value) -> void {
      SelectorImpl *sel = self->get_data<SelectorImpl>();

      if (sel)
        sel->_pimpl->set_value(value);
    }

    auto SelectorImpl::get_outer() const -> Gtk::Widget * {
      return _outerBox;
    }
    auto SelectorImpl::get_inner() const -> Gtk::Widget * {
      return _pimpl->widget();
    }

    //------------------------------------------------------------------------------
    auto SelectorImpl::init() -> void {
      ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

      f->_selector_impl.create = &SelectorImpl::create;
      f->_selector_impl.clear = &SelectorImpl::clear;
      f->_selector_impl.add_item = &SelectorImpl::add_item;
      f->_selector_impl.add_items = &SelectorImpl::add_items;
      f->_selector_impl.get_item = &SelectorImpl::get_item;
      f->_selector_impl.set_index = &SelectorImpl::set_index;
      f->_selector_impl.get_index = &SelectorImpl::get_index;
      f->_selector_impl.get_text = &SelectorImpl::get_text;
      f->_selector_impl.get_item_count = &SelectorImpl::get_item_count;
      f->_selector_impl.set_value = &SelectorImpl::set_value;
    }
  }
}
