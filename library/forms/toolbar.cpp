/*
 * Copyright (c) 2011, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "mforms/mforms.h"
#include "base/log.h"

DEFAULT_LOG_DOMAIN("mforms");

using namespace mforms;

ToolBar::ToolBar(ToolBarType type) : _type(type) {
  _impl = &ControlFactory::get_instance()->_tool_bar_impl;
  _impl->create_tool_bar(this, type);
}

ToolBar::~ToolBar() {
  for (std::vector<ToolBarItem *>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
    (*iter)->release();
  _items.clear();
}

auto ToolBar::find_item(const std::string &name) -> ToolBarItem * {
  for (std::vector<ToolBarItem *>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
    if ((*iter)->getInternalName() == name)
      return *iter;
  return 0;
}

auto ToolBar::validate() -> void {
  for (std::vector<ToolBarItem *>::iterator iter = _items.begin(); iter != _items.end(); ++iter)
    (*iter)->validate();
}

auto ToolBar::set_item_enabled(const std::string &name, bool flag) -> void {
  ToolBarItem *item = find_item(name);
  if (item)
    item->set_enabled(flag);
}

auto ToolBar::set_item_checked(const std::string &name, bool flag) -> void {
  ToolBarItem *item = find_item(name);
  if (item)
    item->set_checked(flag);
}

auto ToolBar::get_item_checked(const std::string &name) -> bool {
  ToolBarItem *item = find_item(name);
  if (item)
    return item->get_checked();
  return false;
}

auto ToolBar::add_item(ToolBarItem *item) -> void {
  insert_item(-1, item);
}

auto ToolBar::add_separator_item(const std::string &name) -> ToolBarItem * {
  ToolBarItem *item = mforms::manage(new ToolBarItem(SeparatorItem));
  item->set_name(name);
  add_item(item);
  return item;
}

auto ToolBar::insert_item(int index, ToolBarItem *item) -> void {
  assert(item->is_managed());

  if (index < 0 || index > (int)_items.size())
    index = (int)_items.size();
  _impl->insert_item(this, index, item);

  if (!item->_release_on_add) // Means: don't increase the ref count, the caller retained already, but won't release.
    item->retain();
  else
    item->_release_on_add = false;

  _items.push_back(item);
}

auto ToolBar::remove_all() -> void {
  for (std::vector<ToolBarItem *>::iterator iter = _items.begin(); iter != _items.end(); ++iter) {
    _impl->remove_item(this, *iter);
    (*iter)->release();
  }
  _items.clear();
}

auto ToolBar::remove_item(ToolBarItem *item) -> void {
  std::vector<ToolBarItem *>::iterator iter = std::find(_items.begin(), _items.end(), item);
  if (iter != _items.end()) {
    _impl->remove_item(this, *iter);
    (*iter)->release();
    _items.erase(iter);
  }
}

ToolBarItem::ToolBarItem(ToolBarItemType type) : _type(type), _updating(false) {
  _impl = &mforms::ControlFactory::get_instance()->_tool_bar_impl;
  _impl->create_tool_item(this, type);
}

auto ToolBarItem::set_text(const std::string &text) -> void {
  _updating = true;
  _impl->set_item_text(this, text);
  _updating = false;
}

auto ToolBarItem::get_text() -> std::string {
  return _impl->get_item_text(this);
}

auto ToolBarItem::set_tooltip(const std::string &text) -> void {
  _impl->set_item_tooltip(this, text);
}

auto ToolBarItem::set_icon(const std::string &path) -> void {
  _icon = path;
  _impl->set_item_icon(this, path);
}

auto ToolBarItem::set_alt_icon(const std::string &path) -> void {
  _alt_icon = path;
  _impl->set_item_alt_icon(this, path);
}

auto ToolBarItem::set_enabled(bool flag) -> void {
  _impl->set_item_enabled(this, flag);
}

auto ToolBarItem::get_enabled() -> bool {
  return _impl->get_item_enabled(this);
}

auto ToolBarItem::set_checked(bool flag) -> void {
  _updating = true;
  _impl->set_item_checked(this, flag);
  _updating = false;
}

auto ToolBarItem::get_checked() -> bool {
  return _impl->get_item_checked(this);
}

auto ToolBarItem::set_name(const std::string &name) -> void {
  if (_impl->set_item_name)
    _impl->set_item_name(this, name);
}

auto ToolBarItem::set_selector_items(const std::vector<std::string> &values) -> void {
  _updating = true;
  _impl->set_selector_items(this, values);
  _updating = false;
}

auto ToolBarItem::callback() -> void {
  try {
    if (!_updating)
      _clicked_signal(this);
  } catch (std::exception &exc) {
    logError("Unhandled exception in toolbar callback for %s: %s\n", _internalName.c_str(), exc.what());
    mforms::Utilities::show_error("Unhandled Exception", exc.what(), "OK");
  }
}

auto ToolBarItem::validate() -> void {
  if (_validate)
    set_enabled(_validate());
}

//--------------------------------------------------------------------------------------------------

auto ToolBarItem::search(const std::string &text) -> void {
  if (_search)
    _search(text);
}

//--------------------------------------------------------------------------------------------------

auto ToolBarItem::set_validator(const std::function<bool()> &slot) -> void {
  _validate = slot;
  validate();
}

//--------------------------------------------------------------------------------------------------

auto ToolBarItem::set_search_handler(const std::function<void(const std::string &)> &slot) -> void {
  _search = slot;
}

//--------------------------------------------------------------------------------------------------
