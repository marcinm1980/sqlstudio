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

#include "table_figure_wb.h"

using namespace wbfig;
using namespace base;

WBTable::WBTable(mdc::Layer *layer, FigureEventHub *hub, const model_ObjectRef &self)
  : Table(layer, hub, self, true),
    _content_box(layer, mdc::Box::Vertical),
    _column_box(layer, mdc::Box::Vertical),
    _index_title(layer, hub, this, true),
    _index_box(layer, mdc::Box::Vertical),
    _trigger_title(layer, hub, this, true),
    _trigger_box(layer, mdc::Box::Vertical),
    _footer(layer, hub, this, false) {
  scoped_connect(_title.signal_expand_toggle(), std::bind(&WBTable::toggle, this, std::placeholders::_1));

  //  _index_title.signal_expand_toggle().connect(std::bind(&WBTable::toggle_indexes, this));
  //  _trigger_title.signal_expand_toggle().connect(std::bind(&WBTable::toggle_triggers, this));

  _title.set_icon(mdc::ImageManager::get_instance()->get_image("studio.physical.TableFigure.16x16.png"));

  set_allowed_resizing(true, true);
  set_accepts_focus(true);
  set_accepts_selection(true);

  set_draggable(true);

  set_draw_background(true);
  set_background_corners(mdc::CAll, 8.0);
  set_background_color(Color::white());
  set_border_color(Color(0.5, 0.5, 0.5));

  _title.set_rounded(mdc::CTop);
  _title.set_draggable(true);
  _title.set_expanded(true);
  _title.set_title("Table");
  _title.set_font(mdc::FontSpec("Helvetica", mdc::SNormal, mdc::WBold, 12));
  _title.set_color(Color(0.59, 0.75, 0.85));

  _index_title.set_title("Indexes");
  _index_title.set_color(Color(0.77, 0.77, 0.77));
  _index_title.set_text_color(Color::white());
  _index_title.set_font(mdc::FontSpec("Helvetica", mdc::SNormal, mdc::WBold, 11));

  _trigger_title.set_title("Triggers");
  _trigger_title.set_color(Color(0.77, 0.77, 0.77));
  _trigger_title.set_text_color(Color::white());
  _trigger_title.set_font(mdc::FontSpec("Helvetica", mdc::SNormal, mdc::WBold, 11));

  _index_title.set_visible(false);
  _trigger_title.set_visible(false);

  _footer.set_fixed_min_size(Size(-1, 8));
  _footer.set_color(Color(0.59, 0.75, 0.85));
  _footer.set_rounded(mdc::CBottom);

  add(&_title, false, false, true);
  add(&_content_box, true, true, true);

  _column_box.set_spacing(1);
  //  _column_box.set_padding(3, 3);

  _content_box.add(&_column_box, true, true);
  _content_box.add(&_index_title, false, true);
  _content_box.add(&_index_box, false, true);
  _content_box.add(&_trigger_title, false, true);
  _content_box.add(&_trigger_box, false, true);
  _content_box.add(&_footer, false, false);
}

WBTable::~WBTable() {
  for (ItemList::iterator i = _columns.begin(); i != _columns.end(); ++i)
    delete *i;

  for (ItemList::iterator i = _indexes.begin(); i != _indexes.end(); ++i)
    delete *i;

  for (ItemList::iterator i = _triggers.begin(); i != _triggers.end(); ++i)
    delete *i;
}

auto WBTable::set_color(const Color &color) -> void {
  _title.set_color(color);
  _footer.set_color(color);
  set_needs_render();
}

auto WBTable::set_max_columns_shown(int count) -> void {
  _column_box.set_item_count_limit(count);
}

auto WBTable::set_allow_manual_resizing(bool flag) -> void {
  _title.set_auto_sizing(!flag);
  _index_title.set_auto_sizing(!flag);
  _trigger_title.set_auto_sizing(!flag);
  for (ItemList::iterator i = _columns.begin(); i != _columns.end(); ++i)
    (*i)->set_auto_sizing(!flag);

  for (ItemList::iterator i = _indexes.begin(); i != _indexes.end(); ++i)
    (*i)->set_auto_sizing(!flag);

  for (ItemList::iterator i = _triggers.begin(); i != _triggers.end(); ++i)
    (*i)->set_auto_sizing(!flag);

  _column_box.set_allow_manual_resizing(flag);

  Table::set_allow_manual_resizing(flag);

  if (!flag)
    relayout();
}

auto WBTable::set_dependant(bool flag) -> void {
}

auto WBTable::hide_columns() -> void {
  _hide_columns = true;
}

auto WBTable::get_expanded() -> bool {
  return _content_box.get_visible();
}

auto WBTable::toggle(bool flag) -> void {
  // save the original height of the column box
  if (!flag)
    _original_column_box_height = _column_box.get_size().height;

  _title.set_expanded(flag);
  _content_box.set_visible(flag);

  if (flag) {
    auto_size();

    if (_manual_resizing) {
      Size size(get_size());

      size.height = size.height - _column_box.get_size().height + _original_column_box_height;

      set_fixed_size(size);
      set_allowed_resizing(true, true);
      set_needs_relayout();
    }
    _title.set_rounded(mdc::CTop);
  } else {
    if (_manual_resizing) {
      set_fixed_size(_title.get_size());
      set_allowed_resizing(true, false);
    }
    _title.set_rounded(mdc::CAll);
  }
}

auto WBTable::hide_indices() -> void {
  _hide_indexes = true;
  _index_title.set_visible(false);
  _index_box.set_visible(false);
}

auto WBTable::hide_triggers() -> void {
  _hide_triggers = true;
  _trigger_title.set_visible(false);
  _trigger_box.set_visible(false);
}

auto WBTable::get_indexes_expanded() -> bool {
  return _index_box.get_visible();
}

auto WBTable::get_triggers_expanded() -> bool {
  return _trigger_box.get_visible();
}

auto WBTable::toggle_indexes(bool flag) -> void {
  _index_title.set_expanded(flag);
  if (!_hide_indexes) {
    Size size(get_size());
    Size index_size(_index_box.get_size());

    _index_box.set_visible(flag);

    if (_manual_resizing) {
      if (flag) {
        relayout();
        size.height += _index_box.get_size().height;
      } else
        size.height -= index_size.height;
      set_fixed_size(size);
    }
  }
}

auto WBTable::toggle_triggers(bool flag) -> void {
  _trigger_title.set_expanded(flag);
  if (!_hide_triggers) {
    Size size(get_size());
    Size trigger_size(_trigger_box.get_size());

    _trigger_box.set_visible(flag);

    if (_manual_resizing) {
      if (flag) {
        relayout();

        size.height += _trigger_box.get_size().height;
      } else
        size.height -= trigger_size.height;
      set_fixed_size(size);
    }
  }
}

auto WBTable::create_truncated_item(mdc::Layer *layer, wbfig::FigureEventHub *hub) -> wbfig::FigureItem * {
  wbfig::FigureItem *item = new wbfig::FigureItem(layer, hub, this);

  item->set_font(mdc::FontSpec("Helvetica", mdc::SNormal, mdc::WBold, 14.0));
  item->set_text_alignment(mdc::AlignCenter);

  return item;
}

auto WBTable::begin_columns_sync() -> WBTable::ItemList::iterator {
  return begin_sync(_column_box, _columns);
}

auto WBTable::sync_next_column(ItemList::iterator iter, const std::string &id, ColumnFlags flags,
                                                      const std::string &text) -> WBTable::ItemList::iterator {
  if (_hide_columns && (flags & (wbfig::ColumnPK | wbfig::ColumnFK)) == 0)
    return iter;

  if (flags & wbfig::ColumnListTruncated) {
    iter = sync_next(_column_box, _columns, iter, id, 0, text,
                     std::bind(&WBTable::create_truncated_item, this, std::placeholders::_1, std::placeholders::_2));
    return iter;
  }

  if ((flags & (wbfig::ColumnPK | wbfig::ColumnFK)) == (wbfig::ColumnPK | wbfig::ColumnFK))
    iter = sync_next(_column_box, _columns, iter, id,
                     mdc::ImageManager::get_instance()->get_image("db.Column.pkfk.11x11.png"), text,
                     std::bind(&WBTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                     std::bind(&WBTable::update_column_item, this, std::placeholders::_1, flags));
  else if (flags & wbfig::ColumnPK)
    iter =
      sync_next(_column_box, _columns, iter, id, mdc::ImageManager::get_instance()->get_image("db.Column.pk.11x11.png"),
                text, std::bind(&WBTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&WBTable::update_column_item, this, std::placeholders::_1, flags));
  else if ((flags & (wbfig::ColumnFK | wbfig::ColumnNotNull)) == (wbfig::ColumnFK | wbfig::ColumnNotNull))
    iter = sync_next(_column_box, _columns, iter, id,
                     mdc::ImageManager::get_instance()->get_image("db.Column.fknn.11x11.png"), text,
                     std::bind(&WBTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                     std::bind(&WBTable::update_column_item, this, std::placeholders::_1, flags));
  else if (flags & wbfig::ColumnFK)
    iter =
      sync_next(_column_box, _columns, iter, id, mdc::ImageManager::get_instance()->get_image("db.Column.fk.11x11.png"),
                text, std::bind(&WBTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&WBTable::update_column_item, this, std::placeholders::_1, flags));
  else if (flags & wbfig::ColumnNotNull)
    iter =
      sync_next(_column_box, _columns, iter, id, mdc::ImageManager::get_instance()->get_image("db.Column.nn.11x11.png"),
                text, std::bind(&WBTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&WBTable::update_column_item, this, std::placeholders::_1, flags));
  else
    iter =
      sync_next(_column_box, _columns, iter, id, mdc::ImageManager::get_instance()->get_image("db.Column.11x11.png"),
                text, std::bind(&WBTable::create_column_item, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&WBTable::update_column_item, this, std::placeholders::_1, flags));

  return iter;
}

auto WBTable::end_columns_sync(ItemList::iterator iter) -> void {
  end_sync(_column_box, _columns, iter);
}

auto WBTable::begin_indexes_sync() -> WBTable::ItemList::iterator {
  return begin_sync(_index_box, _indexes);
}

auto WBTable::sync_next_index(ItemList::iterator iter, const std::string &id,
                                                     const std::string &text) -> WBTable::ItemList::iterator {
  return sync_next(_index_box, _indexes, iter, id, NULL, text,
                   std::bind(&WBTable::create_index_item, this, std::placeholders::_1, std::placeholders::_2));
}

auto WBTable::end_indexes_sync(ItemList::iterator iter) -> void {
  end_sync(_index_box, _indexes, iter);
}

auto WBTable::begin_triggers_sync() -> WBTable::ItemList::iterator {
  if (!_hide_triggers && !_trigger_title.get_visible())
    _trigger_title.set_visible(true);

  return begin_sync(_trigger_box, _triggers);
}

auto WBTable::sync_next_trigger(ItemList::iterator iter, const std::string &id,
                                                       const std::string &text) -> WBTable::ItemList::iterator {
  return sync_next(_trigger_box, _triggers, iter, id, NULL, text);
}

auto WBTable::end_triggers_sync(ItemList::iterator iter) -> void {
  end_sync(_trigger_box, _triggers, iter);
}

auto WBTable::set_content_font(const mdc::FontSpec &font) -> void {
  super::set_content_font(font);

  for (ItemList::iterator i = _columns.begin(); i != _columns.end(); ++i)
    (*i)->set_font(font);

  for (ItemList::iterator i = _indexes.begin(); i != _indexes.end(); ++i)
    (*i)->set_font(font);

  for (ItemList::iterator i = _triggers.begin(); i != _triggers.end(); ++i)
    (*i)->set_font(font);
}
