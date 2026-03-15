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

#include "mforms/mforms.h"

using namespace mforms;

//--------------------------------------------------------------------------------------------------

TabView::TabView(TabViewType tabType) : _type(tabType), _aux_view(NULL), _menu_tab(0), _tab_menu(NULL) {
  _tabview_impl = &ControlFactory::get_instance()->_tabview_impl;

  _tabview_impl->create(this, tabType);
}

//--------------------------------------------------------------------------------------------------

TabView::~TabView() {
  if (_aux_view)
    _aux_view->release();
  _aux_view = NULL;
}

//--------------------------------------------------------------------------------------------------

auto TabView::set_active_tab(int index) -> void {
  _tabview_impl->set_active_tab(this, index);
}

//--------------------------------------------------------------------------------------------------

auto TabView::get_active_tab() -> int {
  return _tabview_impl->get_active_tab(this);
}

//--------------------------------------------------------------------------------------------------

auto TabView::add_page(View *page, const std::string &caption, bool hasCloseButton) -> int {
  cache_view(page);
  return _tabview_impl->add_page(this, page, caption, hasCloseButton);
}

//--------------------------------------------------------------------------------------------------

auto TabView::remove_page(View *page) -> void {
  page->retain();
  int i = get_page_index(page);
  _tabview_impl->remove_page(this, page);
  remove_from_cache(page);
  _signal_tab_closed(page, i);
  page->release();
}

//--------------------------------------------------------------------------------------------------

auto TabView::reordered(View *view, int index) -> void {
  int old_index = get_subview_index(view);
  reorder_cache(view, index);
  _signal_tab_reordered(view, old_index, index);
}

//--------------------------------------------------------------------------------------------------

auto TabView::pin_changed(int tab, bool pinned) -> void {
  _signal_tab_pin_changed(tab, pinned);
}

//--------------------------------------------------------------------------------------------------

auto TabView::page_count() -> int {
  return get_subview_count();
}

//--------------------------------------------------------------------------------------------------

auto TabView::get_page_index(View *page) -> int {
  return get_subview_index(page);
}

//--------------------------------------------------------------------------------------------------

auto TabView::get_page(int index) -> View * {
  return get_subview_at_index(index);
}

//--------------------------------------------------------------------------------------------------

auto TabView::set_tab_title(int page, const std::string &caption) -> void {
  _tabview_impl->set_tab_title(this, page, caption);
}

//--------------------------------------------------------------------------------------------------

auto TabView::can_close_tab(int index) -> bool {
  if (!_signal_tab_closing.empty())
    return *_signal_tab_closing(index);
  return true;
}

//--------------------------------------------------------------------------------------------------

auto TabView::set_aux_view(View *view) -> void {
  if (_aux_view)
    _aux_view->release();
  _aux_view = view;
  if (_aux_view)
    _aux_view->retain();
  _tabview_impl->set_aux_view(this, view);
}

//--------------------------------------------------------------------------------------------------

auto TabView::set_allows_reordering(bool flag) -> void {
  _tabview_impl->set_allows_reordering(this, flag);
}

//--------------------------------------------------------------------------------------------------

auto TabView::set_tab_menu(ContextMenu *menu) -> void {
  _tab_menu = menu;
}

//--------------------------------------------------------------------------------------------------

auto TabView::set_menu_tab(int tab) -> void {
  _menu_tab = tab;
}

//--------------------------------------------------------------------------------------------------

auto TabView::get_menu_tab() -> int {
  return _menu_tab;
}
