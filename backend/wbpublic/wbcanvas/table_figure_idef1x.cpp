/*
 * Copyright (c) 2007, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "table_figure_idef1x.h"

using namespace mdc;
using namespace wbfig;
using namespace base;

auto Separator::draw_contents(mdc::CairoCtx *cr) -> void {
  cr->translate(get_position());

  if (_top_empty) {
    cr->move_to(0, 20.5);
    cr->line_to(get_size().width, 20.5);
  } else {
    cr->move_to(0, 0.5);
    cr->line_to(get_size().width, 0.5);
  }
  cr->set_line_width(1);
  cr->set_color(Color::black());
  cr->stroke();
}

auto Separator::calc_min_size() -> Size {
  if (_top_empty && _bottom_empty)
    return Size(80, 40);
  else if (_top_empty || _bottom_empty)
    return Size(80, 20);
  else
    return Size(80, 2);
}

auto Separator::set_top_empty(bool flag) -> void {
  _top_empty = flag;
  set_needs_relayout();
}

auto Separator::set_bottom_empty(bool flag) -> void {
  _bottom_empty = flag;
  set_needs_relayout();
}

Idef1xTable::Idef1xTable(mdc::Layer *layer, FigureEventHub *hub, const model_ObjectRef &self)
  : Table(layer, hub, self, false), _column_box(layer, mdc::Box::Vertical), _separator(layer) {
  set_allowed_resizing(true, true);
  set_accepts_focus(true);
  set_accepts_selection(true);
  magnetize_bounds();

  add(&_title, false, true, true);
  _title.set_font(mdc::FontSpec(_title.get_font().family, mdc::SNormal, mdc::WNormal, 12));

  _column_box.set_spacing(1);

  _column_box.set_border_color(Color::black());
  _column_box.set_background_color(Color::white());
  _column_box.set_draw_background(true);

  set_background_color(Color::white());
  set_draw_background(true);

  add(&_column_box, true, true, true);
}

auto Idef1xTable::set_color(const Color &color) -> void {
  _column_box.set_background_color(color);
  set_background_color(color);
  set_needs_render();
}

auto Idef1xTable::set_dependant(bool flag) -> void {
  if (flag)
    _column_box.set_background_corners(mdc::CAll, 8.0);
  else
    _column_box.set_background_corners(mdc::CNone, 0.0);

  set_needs_render();
}

auto Idef1xTable::begin_columns_sync() -> Table::ItemList::iterator {
  _unique_oids.clear();
  return begin_sync(_column_box, _columns);
}

auto Idef1xTable::sync_next_column(ItemList::iterator iter, const std::string &id,
                                                        ColumnFlags flags, const std::string &text) -> Table::ItemList::iterator {
  if (flags & wbfig::ColumnPK) {
    _unique_oids.insert(id);
    if (flags & wbfig::ColumnFK)
      return sync_next(_column_box, _columns, iter, id, NULL, text + " (FK)",
                       std::bind(&Idef1xTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                       std::bind(&Idef1xTable::update_column_item, this, std::placeholders::_1, flags));
    else
      return sync_next(_column_box, _columns, iter, id, NULL, text,
                       std::bind(&Idef1xTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                       std::bind(&Idef1xTable::update_column_item, this, std::placeholders::_1, flags));
  } else if (flags & wbfig::ColumnFK)
    return sync_next(_column_box, _columns, iter, id, NULL, text + " (FK)",
                     std::bind(&Idef1xTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                     std::bind(&Idef1xTable::update_column_item, this, std::placeholders::_1, flags));
  else
    return sync_next(_column_box, _columns, iter, id, NULL, text,
                     std::bind(&Idef1xTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                     std::bind(&Idef1xTable::update_column_item, this, std::placeholders::_1, flags));
}

auto Idef1xTable::end_columns_sync(ItemList::iterator iter) -> void {
  end_sync(_column_box, _columns, iter);
}

auto Idef1xTable::end_sync(mdc::Box &box, ItemList &list, ItemList::iterator iter) -> void {
  // everything after iter is outdated, so just delete everything
  while (iter != list.end()) {
    ItemList::iterator next = iter;

    ++next;

    delete *iter;
    list.erase(iter);

    iter = next;
  }

  // resync box
  box.remove_all();
  // first add PK columns
  for (ItemList::const_iterator i = list.begin(); i != list.end(); ++i) {
    if (_unique_oids.find((*i)->get_id()) != _unique_oids.end())
      box.add(*i, false, true, true);
  }

  _separator.set_top_empty(_unique_oids.empty());
  _separator.set_bottom_empty(_unique_oids.size() == list.size());

  // add separator
  box.add(&_separator, false, true, true);

  // then add rest
  for (ItemList::const_iterator i = list.begin(); i != list.end(); ++i) {
    if (_unique_oids.find((*i)->get_id()) == _unique_oids.end())
      box.add(*i, false, true, true);
  }

  box.set_needs_relayout();

  get_layer()->get_view()->unlock_redraw();
  get_layer()->get_view()->unlock();
}
