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

using namespace mforms;

MenuBase::MenuBase() : _parent(0) {
  _impl = &mforms::ControlFactory::get_instance()->_menu_item_impl;
}

MenuBase::~MenuBase() {
  for (std::vector<MenuItem *>::iterator iter = _items.begin(); iter != _items.end(); ++iter) {
    (*iter)->release();
  }
  _items.clear();
}

auto MenuBase::add_item_with_title(const std::string &title, std::function<void()> action,
                                        const std::string &name, const std::string &internalName) -> MenuItem * {
  MenuItem *item = manage(new MenuItem(title));
  item->signal_clicked()->connect(action);
  add_item(item);
  item->set_name(name);
  item->setInternalName(internalName);
  return item;
}

auto MenuBase::add_check_item_with_title(const std::string &title, std::function<void()> action,
                                              const std::string &name, const std::string &internalName) -> MenuItem * {
  MenuItem *item = manage(new MenuItem(title, CheckedMenuItem));
  item->signal_clicked()->connect(action);
  add_item(item);
  item->set_name(name);
  item->setInternalName(internalName);
  return item;
}

auto MenuBase::add_separator() -> MenuItem * {
  MenuItem *item = manage(new MenuItem("", SeparatorMenuItem));
  add_item(item);
  return item;
}

auto MenuBase::add_item(MenuItem *item) -> void {
  insert_item(-1, item);
}

auto MenuBase::insert_item(int index, MenuItem *item) -> void {
  if (index < 0 || index > (int)_items.size())
    index = (int)_items.size();

  item->_parent = this;

  _impl->insert_item(this, index, item);
  _items.insert(_items.begin() + index, item);

  // item->retain();
}

auto MenuBase::remove_all() -> void {
  _impl->remove_item(this, NULL); // null means remove all
  std::vector<MenuItem *>::iterator iter;
  for (iter = _items.begin(); iter != _items.end(); ++iter) {
    (*iter)->_parent = 0;
    (*iter)->release();
  }
  _items.clear();
}

auto MenuBase::remove_item(MenuItem *item) -> void {
  std::vector<MenuItem *>::iterator iter = std::find(_items.begin(), _items.end(), item);
  if (iter != _items.end()) {
    (*iter)->_parent = 0;
    _impl->remove_item(this, item);
    item->release();
    _items.erase(iter);
  }
}

auto MenuBase::set_enabled(bool flag) -> void {
  _impl->set_enabled(this, flag);
}

auto MenuBase::get_enabled() -> bool {
  return _impl->get_enabled(this);
}

auto MenuBase::find_item(const std::string &name) -> MenuItem * {
  for (std::vector<MenuItem *>::const_iterator iter = _items.begin(); iter != _items.end(); ++iter) {
    if ((*iter)->getInternalName() == name)
      return *iter;
    MenuItem *item;
    if ((item = (*iter)->find_item(name)))
      return item;
  }
  return 0;
}

auto MenuBase::get_item(int i) -> MenuItem * {
  if (i < 0 || i >= (int)_items.size())
    return NULL;
  return _items[i];
}

auto MenuBase::get_item_index(MenuItem *item) -> int {
  std::vector<MenuItem *>::const_iterator it = std::find(_items.begin(), _items.end(), item);
  if (it == _items.end())
    return -1;
  return (int)(it - _items.begin());
}

auto MenuBase::item_count() -> int {
  return (int)_items.size();
}

auto MenuBase::validate() -> void {
  for (std::vector<MenuItem *>::const_iterator iter = _items.begin(); iter != _items.end(); ++iter)
    (*iter)->validate();
}

auto MenuBase::get_top_menu() -> MenuBase * {
  if (dynamic_cast<MenuBar *>(this) != NULL)
    return dynamic_cast<MenuBar *>(this);

  if (dynamic_cast<ContextMenu *>(this) != NULL)
    return dynamic_cast<ContextMenu *>(this);

  MenuBase *p = _parent;
  while (p && p->get_parent())
    p = p->get_parent();

  return p;
}

MenuItem::MenuItem(const std::string &title, const MenuItemType type) : MenuBase(), _type(type) {
  _impl->create_menu_item(this, title, type); // ! Warning there will be no checked menu with this!
}

auto MenuItem::set_title(const std::string &title) -> void {
  _impl->set_title(this, title);
}

auto MenuItem::get_title() -> std::string {
  return _impl->get_title(this);
}


auto MenuItem::set_name(const std::string &name) -> void {
  _impl->set_name(this, name);
}

auto MenuItem::set_shortcut(const std::string &shortcut) -> void {
  _shortcut = shortcut;
  _impl->set_shortcut(this, shortcut);
}

auto MenuItem::set_checked(bool flag) -> void {
  _impl->set_checked(this, flag);
}

auto MenuItem::get_checked() -> bool {
  return _impl->get_checked(this);
}

auto MenuItem::callback() -> void {
#if defined(_MSC_VER) || defined(__APPLE__)
  // toggle the state of checkbox items, so that the behaviour works the same as in linux
  if (_type == CheckedMenuItem)
    set_checked(!get_checked());
#endif
  _clicked_signal();
}

auto MenuItem::validate() -> void {
  bool result = true;
  for (validator_function val : _validators) {
    if (!val())
      result = false;
  }

  set_enabled(result);

  if (!_items.empty())
    MenuBase::validate();
}

auto MenuItem::add_validator(const validator_function &slot) -> void {
  _validators.push_back(slot);
}

auto MenuItem::clear_validators() -> void {
  _validators.clear();
}

MenuBar::MenuBar() : MenuBase() {
  _impl->create_menu_bar(this);
}

auto MenuBar::will_show_submenu_from(MenuItem *item) -> void {
  _signal_will_show(item);
}

auto MenuBar::set_item_enabled(const std::string &item_name, bool flag) -> void {
  MenuItem *item = find_item(item_name);
  if (item)
    item->set_enabled(flag);
}

auto MenuBar::set_item_checked(const std::string &item_name, bool flag) -> void {
  MenuItem *item = find_item(item_name);
  if (item)
    item->set_checked(flag);
}

ContextMenu::ContextMenu() : MenuBase() {
  _impl->create_context_menu(this);
}

auto ContextMenu::set_item_enabled(const std::string &item_name, bool flag) -> void {
  MenuItem *item = find_item(item_name);
  if (item)
    item->set_enabled(flag);
}

auto ContextMenu::set_item_checked(const std::string &item_name, bool flag) -> void {
  MenuItem *item = find_item(item_name);
  if (item)
    item->set_checked(flag);
}

auto ContextMenu::will_show() -> void {
  will_show_submenu_from(0);
}

auto ContextMenu::will_show_submenu_from(MenuItem *item) -> void {
  _signal_will_show(item);
}

auto ContextMenu::popup_at(View *owner, base::Point location) -> void {
  _impl->popup_at(this, owner, location);
}
