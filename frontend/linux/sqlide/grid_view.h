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

#ifndef __GRID_VIEW_H__
#define __GRID_VIEW_H__

#include "grid_view_model.h"
namespace mforms {
  class Menu;
};

class GridView : public Gtk::TreeView {
public:
  static auto create(bec::GridModel::Ref model, bool fixed_row_height = true, bool allow_cell_selection = true) -> GridView *;
  GridView(bec::GridModel::Ref model, bool fixed_row_height = true, bool allow_cell_selection = true);
  ~GridView();

  auto set_text_cell_fixed_height(bool val) -> void;

  auto allow_cell_selection() -> bool {
    return _allow_cell_selection;
  }

  auto set_context_menu(mforms::Menu *menu) -> void;
  auto set_context_menu_responder(const sigc::slot<void> &slot) -> void;

  auto get_selected_rows() -> std::vector<int>;

  auto model(bec::GridModel::Ref value) -> void;

  auto refresh(bool reset_columns) -> int;

  auto scroll_to(const int whence) -> void; // whence == 0 seeks to start, whence == 1 seeks to end

  auto selection_is_cell() -> bool {
    return _selected_cell;
  }
  auto current_cell(int &row, int &col) -> bec::NodeId;
  auto current_row() -> int;
  auto select_cell(int row, int col) -> void;
  auto select_cell(int row, Gtk::TreeViewColumn &col) -> void;

  auto on_column_header_clicked(Gtk::TreeViewColumn *column, int column_index) -> void;
  auto sort_by_column(int column_index, int sort_direction, bool retaining) -> void;

  auto row_count() const -> int;
  auto row_numbers_visible(bool value) -> void {
    _view_model->row_numbers_visible(value);
  }

  auto signal_cell_edited() -> sigc::signal<void, const Glib::ustring &, const Glib::ustring &> {
    return _signal_cell_edited;
  }
  // sigc::slot<void, const Glib::ustring&, const Glib::ustring&> slot_cell_edited() { return
  // _signal_cell_edited.make_slot(); }
  auto signal_row_count_changed() -> sigc::signal<void> {
    return _signal_row_count_changed;
  }
  sigc::signal<void, int, int, bool> signal_sort_by_column;

  auto on_cell_edited(const Glib::ustring &path_string, const Glib::ustring &new_text) -> void;
  auto on_cell_editing_started(Gtk::CellEditable *e, const Glib::ustring &path, Gtk::TreeViewColumn *column) -> void;
  auto on_text_insert(unsigned int position, const char *incoming_text, unsigned int character_num) -> void;
  auto on_cell_editing_done() -> void;

  auto set_ellipsize(const int column, const bool on) -> void {
    _view_model->set_ellipsize(column, on);
  }
  auto view_model() -> GridViewModel::Ref {
    return _view_model;
  }

  auto sync_row_count() -> void;

  std::function<void(std::vector<int>)> _copy_func_ptr;
  auto copy() -> void;

protected:
  virtual auto on_key_press_event(GdkEventKey *event) -> bool;
  virtual auto on_button_press_event(GdkEventButton *event) -> bool;
  auto on_focus_out(GdkEventFocus *event, Gtk::CellRenderer *cell, Gtk::Entry *e) -> bool;
  auto on_signal_cursor_changed() -> void;
  auto on_signal_button_release_event(GdkEventButton *ev) -> void;
  auto reset_sorted_columns() -> void;

private:
  virtual auto init() -> void;

  sigc::signal<void, const Glib::ustring &, const Glib::ustring &> _signal_cell_edited;
  sigc::signal<void> _signal_row_count_changed;

  auto activate_popup_menu_item(const std::string &action, const std::vector<int> &rows, int clicked_column) -> void;
  auto delete_selected_rows() -> void;

  bec::GridModel::Ref _model;
  GridViewModel::Ref _view_model;
  size_t _row_count;

  Gtk::TreePath _path_edited;
  Gtk::TreeViewColumn *_column_edited;
  Gtk::CellEditable *_cell_editable;

  mforms::Menu *_context_menu;
  sigc::slot<void> _context_menu_responder;

  bool _allow_cell_selection;
  bool _selected_cell;
  bool _text_cell_fixed_height;
};

#endif // __GRID_VIEW_H__
