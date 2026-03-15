/*
 * Copyright (c) 2016, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "record_grid_view.h"
#include "../sqlide/recordset_view.h"
#include "sqlide/recordset_be.h"
#include "mforms/menubar.h"
#include "gtk/lf_native.h"

using namespace mforms;

static auto create_record_grid(std::shared_ptr<Recordset> rset) -> mforms::GridView * {
  return new RecordGridView(rset);
}

auto lf_record_grid_init() -> void {
  mforms::GridView::register_factory(create_record_grid);
}

static auto destroy_nativecontainer(void *ptr) -> void {
  gtk::NativeContainerImpl *container = (gtk::NativeContainerImpl *)ptr;
  if (container)
    delete container;
}

RecordGridView::RecordGridView(Recordset::Ref rset) {
  viewer = RecordsetView::create(rset);
  viewer->grid_view()->view_model()->columns_resized =
    std::bind(&RecordGridView::columns_resized, this, std::placeholders::_1);
  viewer->grid_view()->view_model()->column_right_clicked = std::bind(
    &RecordGridView::column_right_clicked, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
  viewer->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  set_data(new gtk::NativeContainerImpl(this, viewer), destroy_nativecontainer);
  viewer->show_all();
  viewer->grid_view()->refresh(true);
}

RecordGridView::~RecordGridView() {
  delete viewer;
}

auto RecordGridView::get_column_count() -> int {
  return viewer->model()->get_column_count();
}

auto RecordGridView::get_column_width(int column) -> int {
  Gtk::TreeViewColumn *tc = viewer->grid_view()->get_column(column + 1);
  if (tc)
    return tc->get_width();
  return 0;
}

auto RecordGridView::set_column_width(int column, int width) -> void {
  viewer->grid_view()->view_model()->set_column_width(column, width);
}

auto RecordGridView::update_columns() -> void {
  viewer->grid_view()->refresh(true);
}

auto RecordGridView::current_cell(size_t &row, int &column) -> bool {
  int r, c;
  if (viewer->grid_view()->current_cell(r, c).is_valid())
    return false;
  row = r;
  column = c;
  return true;
}

auto RecordGridView::set_current_cell(size_t row, int column) -> void {
  viewer->grid_view()->select_cell(row, column);
}

auto RecordGridView::set_column_header_indicator(int column_index, ColumnHeaderIndicator order) -> void {
  Gtk::TreeViewColumn *column = viewer->grid_view()->get_column(column_index + 1);
  switch (order) {
    case NoIndicator:
      column->set_sort_indicator(false);
      break;
    case SortDescIndicator:
      column->set_sort_order(Gtk::SORT_DESCENDING);
      column->set_sort_indicator(true);
      break;
    case SortAscIndicator:
      column->set_sort_order(Gtk::SORT_ASCENDING);
      column->set_sort_indicator(true);
      break;
  }
}

auto RecordGridView::set_font(const std::string &font) -> void {
  viewer->grid_view()->override_font(Pango::FontDescription(font));
}

auto RecordGridView::column_right_clicked(int c, int x, int y) -> void {
  clicked_header_column(c);
  if (header_menu())
    header_menu()->popup_at(this, base::Point(x, y));
}
