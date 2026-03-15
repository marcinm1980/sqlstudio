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

#ifndef _LF_LISTBOX_H_
#define _LF_LISTBOX_H_

#include "mforms/listbox.h"
#include "base/log.h"
#include "lf_view.h"

namespace mforms {
  namespace gtk {

    class ListBoxImpl : public ViewImpl {
      class ComboColumn : public Gtk::TreeModel::ColumnRecord {
      public:
        ComboColumn() {
          add(_item);
        }
        Gtk::TreeModelColumn<Glib::ustring> _item;
      };

      ComboColumn _ccol;
      Glib::RefPtr<Gtk::ListStore> _store;
      Gtk::TreeView _lbox;
      Gtk::ScrolledWindow _swin;

      virtual auto get_outer() const -> Gtk::Widget * {
        return &(const_cast<Gtk::ScrolledWindow &>(_swin));
      }
      virtual auto get_inner() const -> Gtk::Widget * {
        return &(const_cast<Gtk::TreeView &>(_lbox));
      }

    protected:
      ListBoxImpl(::mforms::ListBox *self, bool multi_select);
      static auto selection_changed(::mforms::ListBox *self) -> void;
      static auto create(::mforms::ListBox *self, bool multi_select) -> bool;
      static auto clear(::mforms::ListBox *self) -> void;
      static auto add_item(::mforms::ListBox *self, const std::string &item) -> size_t;
      static auto add_items(::mforms::ListBox *self, const std::list<std::string> &items) -> void;
      static auto remove_indices(mforms::ListBox *backend, const std::vector<size_t> &indices) -> void;
      static auto remove_index(mforms::ListBox *backend, size_t index) -> void;
      static auto get_text(::mforms::ListBox *self) -> std::string;
      static auto set_index(::mforms::ListBox *self, ssize_t index) -> void;
      static auto get_index(::mforms::ListBox *self) -> ssize_t;
      static auto set_heading(ListBox *self, const std::string &text) -> void;
      static auto get_selected_indices(ListBox *self) -> std::vector<size_t>;
      static auto get_count(ListBox *self) -> size_t;
      static auto get_string_value_from_index(ListBox *self, size_t index) -> std::string;

    public:
      static auto init() -> void;
    };
  }
}

#endif /* _LF_LISTBOX_H_ */
