/*
 * Copyright (c) 2008, 2019, Oracle and/or its affiliates. All rights reserved.
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

/**
 * Implementation of the mforms view, which is the base for most of the visual controls in mforms.
 */

#include "mforms/mforms.h"

#include "base/log.h"
#include "base/notifications.h"

using namespace mforms;

//--------------------------------------------------------------------------------------------------

View::View() {
  _parent = NULL;
  _view_impl = &ControlFactory::get_instance()->_view_impl;
  _layout_dirty = true;
}

//--------------------------------------------------------------------------------------------------

View::~View() {
  set_destroying();
  if (_parent && !_parent->is_destroying())
    _parent->remove_from_cache(this);

  clear_subviews();

#ifdef __APPLE__
  // Let the frontend delete all resources it allocated.
  // This is only needed for OSX as on Win + Linux we use the data free function (set in set_data()) to free
  // platform resources.
  if (_view_impl->destroy)
    _view_impl->destroy(this);
#endif
}

//--------------------------------------------------------------------------------------------------

auto View::clear_subviews() -> void {
  while (_subviews.size() > 0)
    remove_from_cache(
      _subviews[0].first); // Let descendants adjust their child lists. This will also release the object if necessary.
}

//--------------------------------------------------------------------------------------------------

auto View::set_managed() -> void {
  Object::set_managed();
  if (_parent) {
    for (std::vector<std::pair<View *, bool> >::iterator iter = _parent->_subviews.begin();
         iter != _parent->_subviews.end(); ++iter) {
      if (iter->first == this) {
        iter->second = true;
        break;
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------

auto View::cache_view(View *sv) -> void {
  if (!sv)
    throw std::logic_error("mforms: attempt to add NULL subview");

  if (sv->get_parent() != NULL)
    throw std::logic_error("mforms: attempt to add a subview already contained somewhere");

  if (sv == this)
    throw std::logic_error("mforms: Can't add a view inside itself");

  sv->set_parent(this);
  if (!sv->_release_on_add) // Means: don't increase the ref count, the caller retained already, but won't release.
    sv->retain();
  else
    sv->_release_on_add = false;

  _subviews.push_back(std::make_pair(sv, sv->_managed));
}

//--------------------------------------------------------------------------------------------------

auto View::reorder_cache(View *sv, int position) -> void {
  int old = get_subview_index(sv);
  if (old < 0)
    throw std::invalid_argument("mforms: invalid subview");

  std::pair<View *, bool> value = _subviews[old];
  _subviews.erase(_subviews.begin() + old);
  _subviews.insert(_subviews.begin() + position, value);
}

//--------------------------------------------------------------------------------------------------

auto View::remove_from_cache(View *sv) -> void {
  sv->_parent = NULL;
  for (std::vector<std::pair<View *, bool> >::iterator iter = _subviews.begin(); iter != _subviews.end(); ++iter) {
    if (iter->first == sv) {
      _subviews.erase(iter);
      sv->release();
      break;
    }
  }
}

//--------------------------------------------------------------------------------------------------

/**
 * Searches for a subview with the given name in this view or any of its subviews using a depth-first search.
 */
auto View::find_subview(const std::string &name) -> View * {
  for (std::vector<std::pair<View *, bool> >::const_iterator iter = _subviews.begin(); iter != _subviews.end();
       ++iter) {
    if (iter->first->getInternalName() == name)
      return iter->first;

    View *sv = iter->first->find_subview(name);
    if (sv)
      return sv;
  }
  return 0;
}

//--------------------------------------------------------------------------------------------------

auto View::get_subview_index(View *sv) -> int {
  int i = 0;
  for (std::vector<std::pair<View *, bool> >::const_iterator iter = _subviews.begin(); iter != _subviews.end();
       ++iter, ++i) {
    if (iter->first == sv)
      return i;
  }
  return -1;
}

//--------------------------------------------------------------------------------------------------

auto View::get_subview_at_index(int index) -> View * {
  if (index < 0 || index >= (int)_subviews.size())
    return NULL;

  return _subviews[index].first;
}

//--------------------------------------------------------------------------------------------------

auto View::get_subview_count() -> int {
  return (int)_subviews.size();
}

//--------------------------------------------------------------------------------------------------

/**
 * Returns true if the given subview is a direct child of this view.
 */
auto View::contains_subview(View *subview) -> bool {
  return subview->get_parent() == this;
}

//--------------------------------------------------------------------------------------------------

auto View::set_name(const std::string &name) -> void {
  // Optional implementation.
  if (_view_impl->set_name)
    _view_impl->set_name(this, name);
}

//--------------------------------------------------------------------------------------------------

auto View::set_tooltip(const std::string &text) -> void {
  _view_impl->set_tooltip(this, text);
}

//--------------------------------------------------------------------------------------------------

auto View::set_font(const std::string &fontDescription) -> void {
  _view_impl->set_font(this, fontDescription);
}

//--------------------------------------------------------------------------------------------------

auto View::setInternalName(const std::string &name) -> void {
  _internalName = name;
}

//--------------------------------------------------------------------------------------------------

auto View::getInternalName() const -> std::string {
    return _internalName;
}


//--------------------------------------------------------------------------------------------------

auto View::set_parent(View *parent) -> void {
  _parent = parent;
  if (_managed)
    set_managed();
}

//--------------------------------------------------------------------------------------------------

auto View::get_parent() const -> View * {
  return _parent;
}

//--------------------------------------------------------------------------------------------------

auto View::get_parent_form() const -> Form * {
  View *parent = get_parent();
  Form *form = 0;
  while (parent && (form = dynamic_cast<Form *>(parent)) == 0)
    parent = parent->get_parent();

  return form;
}

//--------------------------------------------------------------------------------------------------

auto View::get_width() const -> int {
  return (*_view_impl->get_width)(this);
}

//--------------------------------------------------------------------------------------------------

auto View::get_height() const -> int {
  return (*_view_impl->get_height)(this);
}

//--------------------------------------------------------------------------------------------------

auto View::get_preferred_width() -> int {
  return (*_view_impl->get_preferred_width)(this);
}

//--------------------------------------------------------------------------------------------------

auto View::get_preferred_height() -> int {
  return (*_view_impl->get_preferred_height)(this);
}

//--------------------------------------------------------------------------------------------------

auto View::get_x() const -> int {
  return (*_view_impl->get_x)(this);
}

//--------------------------------------------------------------------------------------------------

auto View::get_y() const -> int {
  return (*_view_impl->get_y)(this);
}

//--------------------------------------------------------------------------------------------------

auto View::set_position(int x, int y) -> void {
  (*_view_impl->set_position)(this, x, y);
}

//--------------------------------------------------------------------------------------------------

auto View::set_size(int width, int height) -> void {
  set_layout_dirty(true);
  (*_view_impl->set_size)(this, width, height);
}

//--------------------------------------------------------------------------------------------------

auto View::set_min_size(int width, int height) -> void {
  set_layout_dirty(true);
  (*_view_impl->set_min_size)(this, width, height);
}

//--------------------------------------------------------------------------------------------------

auto View::client_to_screen(int x, int y) -> std::pair<int, int> {
  return (*_view_impl->client_to_screen)(this, x, y);
}

//--------------------------------------------------------------------------------------------------

auto View::screen_to_client(int x, int y) -> std::pair<int, int> {
  return (*_view_impl->screen_to_client)(this, x, y);
}

//--------------------------------------------------------------------------------------------------

auto View::show(bool flag) -> void {
  (*_view_impl->show)(this, flag);
}

//--------------------------------------------------------------------------------------------------

auto View::is_shown() -> bool {
  return (*_view_impl->is_shown)(this);
}

//--------------------------------------------------------------------------------------------------

auto View::is_fully_visible() -> bool {
  return (*_view_impl->is_fully_visible)(this);
}

//--------------------------------------------------------------------------------------------------

auto View::set_enabled(bool flag) -> void {
  (*_view_impl->set_enabled)(this, flag);
}

//--------------------------------------------------------------------------------------------------

auto View::is_enabled() -> bool {
  return (*_view_impl->is_enabled)(this);
}

//--------------------------------------------------------------------------------------------------

auto View::set_needs_repaint() -> void {
  _view_impl->set_needs_repaint(this);
}

//--------------------------------------------------------------------------------------------------

auto View::set_layout_dirty(bool value) -> void {
  _layout_dirty = value;
  if (_parent != NULL && value)
    _parent->set_layout_dirty(true);
}

//--------------------------------------------------------------------------------------------------

auto View::is_layout_dirty() -> bool {
  return _layout_dirty;
}

//--------------------------------------------------------------------------------------------------

auto View::relayout() -> void {
  _view_impl->relayout(this);
  if (_parent != nullptr) // Propagate relayout up the parent chain.
    _parent->relayout();
}

//--------------------------------------------------------------------------------------------------

auto View::suspend_layout() -> void {
  if (_view_impl->suspend_layout)
    _view_impl->suspend_layout(this, true);
}

//--------------------------------------------------------------------------------------------------

auto View::resume_layout() -> void {
  if (_view_impl->suspend_layout)
    _view_impl->suspend_layout(this, false);
}

//--------------------------------------------------------------------------------------------------

auto View::set_front_color(const std::string &color) -> void {
  _view_impl->set_front_color(this, color);
}

//--------------------------------------------------------------------------------------------------

auto View::get_front_color() -> std::string {
  return _view_impl->get_front_color(this);
}

//--------------------------------------------------------------------------------------------------

auto View::set_back_color(const std::string &color) -> void {
  _view_impl->set_back_color(this, color);
}

//--------------------------------------------------------------------------------------------------

auto View::get_back_color() -> std::string {
  return _view_impl->get_back_color(this);
}

//--------------------------------------------------------------------------------------------------

auto View::set_back_image(const std::string &path, Alignment align) -> void {
  _view_impl->set_back_image(this, path, align);
}

//--------------------------------------------------------------------------------------------------
// Below code is used only for debug purpose.
// It's using the object::retain_count.
#ifdef _0
auto View::show_retain_counts(int depth) -> void {
  printf("%*s '%s' (%i)\n", depth, "--", get_name().c_str(), retain_count());

  for (std::vector<std::pair<View *, bool> >::const_iterator iter = _subviews.begin(); iter != _subviews.end();
       ++iter) {
    iter->first->show_retain_counts(depth + 1);
  }
}
#endif
//--------------------------------------------------------------------------------------------------

auto View::flush_events() -> void {
  if (_view_impl->flush_events)
    _view_impl->flush_events(this);
}

//--------------------------------------------------------------------------------------------------

auto View::focus() -> void {
  _view_impl->focus(this);
}

//--------------------------------------------------------------------------------------------------

auto View::has_focus() -> bool {
  return _view_impl->has_focus(this);
}

//--------------------------------------------------------------------------------------------------

auto View::register_drop_formats(DropDelegate *target, const std::vector<std::string> &drop_formats) -> void {
  _view_impl->register_drop_formats(this, target, drop_formats);
}

//--------------------------------------------------------------------------------------------------

auto View::do_drag_drop(DragDetails details, const std::string &text) -> DragOperation {
  return _view_impl->drag_text(this, details, text);
}

//--------------------------------------------------------------------------------------------------

auto View::do_drag_drop(DragDetails details, void *data, const std::string &format) -> DragOperation {
  return _view_impl->drag_data(this, details, data, format);
}

//--------------------------------------------------------------------------------------------------

auto View::get_drop_position() -> DropPosition {
  return _view_impl->get_drop_position(this);
}

//--------------------------------------------------------------------------------------------------

auto View::mouse_leave() -> bool {
  if (_signal_mouse_leave.num_slots() > 0)
    return *_signal_mouse_leave();
  return false;
}

//--------------------------------------------------------------------------------------------------

/**
 * To be called by platform code when the active control changes (either by code or user interaction).
 */
auto View::focus_changed() -> void {
  _signal_got_focus();
  base::NotificationCenter::get()->send("GNFocusChanged", this);
}

//--------------------------------------------------------------------------------------------------

auto View::resize() -> void {
  _signal_resized();
}

//--------------------------------------------------------------------------------------------------
