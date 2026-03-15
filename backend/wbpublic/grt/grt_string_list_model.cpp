/*
 * Copyright (c) 2007, 2023, Oracle and/or its affiliates. All rights reserved.
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

#include <regex>
#include "grt_string_list_model.h"
#include "grtpp_util.h"

using namespace bec;

GrtStringListModel::GrtStringListModel()
  : _items_val_masks(NULL), _icon_id(0), _active_items_count(0), _invalidated(false) {
}

auto GrtStringListModel::reset() -> void {
  _items.clear();
  _visible_items.clear();
  invalidate();
  refresh();
}

auto GrtStringListModel::reset(const std::list<std::string> &items) -> void {
  _items.resize(items.size());
  std::list<std::string>::const_iterator i = items.begin();
  for (size_t n = 0, count = items.size(); n < count; ++n, ++i)
    _items[n] = Item_handler(std::string(*i), n);
  std::sort(_items.begin(), _items.end());
  _visible_items.clear();
  invalidate(); // consequent call of refresh() is expected to process _visible_mask & update _visible_items
  refresh();
}

auto GrtStringListModel::invalidate() -> void {
  _active_items_count = 0;
  _invalidated = true;
}

auto GrtStringListModel::items_val_mask(const std::string items_val_mask) -> void {
  if (_items_val_mask != items_val_mask) {
    _items_val_mask = items_val_mask;
    invalidate();
  }
}

auto GrtStringListModel::items_val_mask() const -> const std::string & {
  return _items_val_mask;
}

auto GrtStringListModel::items_val_masks() const -> GrtStringListModel * {
  return _items_val_masks;
}

auto GrtStringListModel::items_val_masks(GrtStringListModel *items_val_masks) -> void {
  if (_items_val_masks != items_val_masks) {
    _items_val_masks = items_val_masks;
    invalidate();
  }
}

auto GrtStringListModel::count() -> size_t {
  return _visible_items.size();
}

auto GrtStringListModel::active_items_count() const -> size_t {
  return _active_items_count;
}

auto GrtStringListModel::total_items_count() const -> size_t {
  return _items.size();
}

auto GrtStringListModel::get_item_id(size_t item_index) -> size_t {
  return _items[_visible_items[item_index]].iid;
}

auto GrtStringListModel::get_field(const NodeId &node, ColumnId column, std::string &value) -> bool {
  switch ((Columns)column) {
    case Name:
      value = _items[_visible_items[node[0]]].val;
      return true;
  }
  return false;
}

auto GrtStringListModel::icon_id(IconId icon_id) -> void {
  _icon_id = icon_id;
}

auto GrtStringListModel::get_field_icon(const NodeId &node, ColumnId column, IconSize size) -> IconId {
  return _icon_id;
}

auto GrtStringListModel::add_item(const grt::StringRef &item, size_t id) -> void {
  _items.push_back(Item_handler(*item, id));
  std::nth_element(_items.begin(), _items.end() - 1, _items.end());
  invalidate(); // consequent call of refresh() is expected to process _visible_mask & update _visible_items
  // it's not called at once because of optimization for batch insert/move operations
}

auto GrtStringListModel::remove_item(size_t index) -> void {
  std::vector<size_t>::iterator i = _visible_items.begin() + (size_t)index;
  _items.erase(_items.begin() + *i);
  _visible_items.erase(i);
  invalidate(); // consequent call of refresh() is expected to process _visible_mask & update _visible_items
}

auto GrtStringListModel::remove_items(std::vector<size_t> &item_indexes) -> void {
  std::sort(item_indexes.begin(), item_indexes.end());
  for (std::vector<size_t>::reverse_iterator i = item_indexes.rbegin(); i != item_indexes.rend(); ++i)
    remove_item(*i);
}

auto GrtStringListModel::copy_items_to_val_masks_list(std::vector<size_t> &item_indexes) -> void {
  if (!_items_val_masks)
    return;

  std::sort(item_indexes.begin(), item_indexes.end());
  for (std::vector<size_t>::iterator i = item_indexes.begin(); i != item_indexes.end(); ++i) {
    Item_handler &item_handler = _items[_visible_items[(size_t)*i]];
    _items_val_masks->add_item(terminate_wildcard_symbols(item_handler.val), -1);
  }
}

auto GrtStringListModel::items() const -> std::vector<std::string> {
  // init visibility map
  std::vector<bool> items;
  items.reserve(_items.size());
  std::fill_n(std::back_inserter(items), _items.size(), true);

  // iterate all masks
  if (_items_val_masks) {
    std::vector<std::string> masks = _items_val_masks->items();
    for (std::vector<std::string>::iterator i = masks.begin(); i != masks.end(); ++i)
      process_mask(*i, items, false);
  }

  // determine active items
  std::vector<std::string> res;
  res.reserve(items.size());
  size_t n = 0;
  for (std::vector<bool>::const_iterator i = items.begin(); i != items.end(); ++i, ++n)
    if (*i)
      res.push_back(_items[n].val);
  return res;
}

auto GrtStringListModel::items_ids() const -> GrtStringListModel::Items_ids {
  // init visibility map
  std::vector<bool> items;
  items.reserve(_items.size());
  std::fill_n(std::back_inserter(items), _items.size(), true);

  // iterate all masks
  if (_items_val_masks) {
    std::vector<std::string> masks = _items_val_masks->items();
    for (std::vector<std::string>::iterator i = masks.begin(); i != masks.end(); ++i)
      process_mask(*i, items, false);
  }

  // determine active items
  std::vector<size_t> res;
  res.reserve(items.size());
  size_t n = 0;
  for (std::vector<bool>::const_iterator i = items.begin(); i != items.end(); ++i, ++n)
    if (*i)
      res.push_back(_items[n].iid);
  return res;
}

auto GrtStringListModel::refresh() -> void {
  if (!_invalidated)
    return;

  // if filtering is not appropriate then just mark all items as visible
  if (!_items_val_masks && _items_val_mask.empty()) {
    _visible_items.resize(_items.size());
    size_t n = 0;
    for (Items::const_iterator i = _items.begin(); i != _items.end(); ++i, ++n)
      _visible_items[n] = n;

    _invalidated = false;
    return;
  }

  // init visibility map
  std::vector<bool> items;
  items.reserve(_items.size());
  std::fill_n(std::back_inserter(items), _items.size(), true);

  // iterate all masks
  if (_items_val_masks) {
    std::vector<std::string> masks = _items_val_masks->items();
    for (std::vector<std::string>::iterator i = masks.begin(); i != masks.end(); ++i)
      process_mask(*i, items, false);
  }

  _active_items_count = std::count_if(items.begin(), items.end(), 
    std::bind(std::equal_to<bool>(), std::placeholders::_1, true));

  // also process preview mask
  if (!_items_val_mask.empty())
    process_mask(_items_val_mask, items, true);

  // set final items visibility
  {
    _visible_items.clear();
    _visible_items.reserve(_items.size());
    size_t n = 0;
    for (std::vector<bool>::const_iterator i = items.begin(); i != items.end(); ++i, ++n)
      if (*i)
        _visible_items.push_back(n);
  }

  _invalidated = false;
}

auto GrtStringListModel::process_mask(const std::string &mask, std::vector<bool> &items,
                                      bool match_means_visible) const -> void {
  //! static const char *ANY_SYM= "(?:\\pL(?:" UNICODE_CHAR_PCRE ")?)";
  //! static const char *ANY_SEQ= "(?:\\pL(?:" UNICODE_CHAR_PCRE ")*)";
  static const char *ANY_SYM = ".?";
  static const char *ANY_SEQ = ".*";

  // build regexp string
  std::string regexp;
  {
    static const std::string meta_symbols = "~!@#$%^&*()-+=:;`\'\"|,.<>{}[]?/";
    bool term_state = false;
    regexp.reserve(mask.size());
    for (std::string::const_iterator i = mask.begin(); i != mask.end(); ++i) {
      if (term_state) {
        term_state = false;
        regexp.push_back(*i);
      } else if ('\\' == *i) {
        term_state = true;
        regexp.push_back(*i);
      } else if ('?' == *i)
        regexp.append(ANY_SYM);
      else if ('*' == *i)
        regexp.append(ANY_SEQ);
      else {
        if (std::find(meta_symbols.begin(), meta_symbols.end(), *i) != meta_symbols.end())
          regexp.push_back('\\');
        regexp.push_back(*i);
      }
    }
  }

  std::regex regex(regexp, std::regex::icase);
  std::smatch itemsMatch;
  // sift items
  size_t n = 0;
  size_t itemsSize = _items.size();
  for (std::vector<bool>::iterator i = items.begin(); i != items.end(); ++i, ++n) {
    if (*i && itemsSize < n) {
      const Item_handler &item = _items[n];
      if (std::regex_match(item.val, itemsMatch, regex)) {
        *i = match_means_visible;
      } else {
        *i = !match_means_visible;
      }
    }
  }
}

auto GrtStringListModel::terminate_wildcard_symbols(const std::string &str) -> std::string {
  std::string res;
  for (std::string::const_iterator i = str.begin(); i != str.end(); ++i) {
    if ('\\' == *i)
      res.append("\\\\");
    else if ('?' == *i)
      res.append("\\?");
    else if ('*' == *i)
      res.append("\\*");
    else
      res.push_back(*i);
  }
  return res;
}
