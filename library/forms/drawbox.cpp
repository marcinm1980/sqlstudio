/*
 * Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
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

DrawBox::DrawBox() : _focusedItem(-1), _lastFocusedItem(-1) {
  _drawbox_impl = &ControlFactory::get_instance()->_drawbox_impl;
  _drawbox_impl->create(this);
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::add(View *view, Alignment alignment) -> void {
  cache_view(view);
  _drawbox_impl->add(this, view, alignment);
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::remove(View *view) -> void {
  _drawbox_impl->remove(this, view);
  remove_from_cache(view);
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::move(View *view, int x, int y) -> void {
  _drawbox_impl->move(this, view, x, y);
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::set_layout_dirty(bool value) -> void {
  View::set_layout_dirty(value);
  if (value)
    _drawbox_impl->set_needs_repaint(this);
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::set_padding(int left, int top, int right, int bottom) -> void {
  _view_impl->set_padding(this, left, top, right, bottom);
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::set_needs_repaint() -> void {
  _drawbox_impl->set_needs_repaint(this);
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::set_needs_repaint_area(int x, int y, int w, int h) -> void {
  _drawbox_impl->set_needs_repaint_area(this, x, y, w, h);
}

//--------------------------------------------------------------------------------------------------

/**
 * The content of a draw box is, by nature, drawn by the box itself, so we need to know what
 * space the box needs. Overwritten by descendants. Subviews do not automatically add to the content
 * size. If that's needed then additional computations are needed by the host.
 */
auto DrawBox::getLayoutSize(base::Size proposedSize) -> base::Size {
  return proposedSize;
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::repaint(cairo_t *cr, int x, int y, int w, int h) -> void {
  if (_focusedItem != -1 && _focusedItem < static_cast<int>(_focusableList.size())) {
    drawFocus(cr, _focusableList[_focusedItem].getBounds());
  }
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::drawFocus(cairo_t *cr, const base::Rect r) -> void {
  if (_drawbox_impl->drawFocus) {
    _drawbox_impl->drawFocus(this, cr, r);
  }
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::addFocusableArea(FocusableArea fArea) -> void {
  if (fArea.getBounds)
  _focusableList.push_back(fArea);
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::clearFocusableAreas() -> void {
  _focusedItem = -1;
  _lastFocusedItem = -1;
  _focusableList.clear();
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::setFocusOnArea(const base::Point p) -> bool {
  auto it = std::find_if(_focusableList.begin(), _focusableList.end(), [&](mforms::FocusableArea const& item) {
    return item.getBounds().contains(p.x, p.y);
  });
  if (it != _focusableList.end()) {
    _focusedItem = static_cast<int>(it - _focusableList.begin());
    set_needs_repaint();
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::keyPress(KeyCode code, ModifierKey modifiers) -> bool {
  int handled = -1;
  if (_focusedItem > -1) {
    if (code == mforms::KeyTab && (modifiers & ModifierShift) == 0) {
      _lastFocusedItem = _focusedItem;
      _focusedItem++;
      if (_focusedItem >= static_cast<int>(_focusableList.size())) {
        _focusedItem = -1;
        handled = 0;
        set_needs_repaint();
      } else {
        handled = 1;
      }
    } else if (code == mforms::KeyTab && (modifiers & ModifierShift) != 0) {
      _lastFocusedItem = _focusedItem;
      _focusedItem--;
      if (_focusedItem < 0) {
        _focusedItem = -1;
        handled = 0;
        set_needs_repaint();
      } else {
        handled = 1;
      }
    } else if (code == mforms::KeyMenu || ((modifiers & ModifierControl) != 0 && code == mforms::KeyF10)) {
      if (_focusableList[_focusedItem].showContextMenu) {
        _focusableList[_focusedItem].showContextMenu();
      }
    } else if (code == mforms::KeyReturn) {
      if (_focusableList[_focusedItem].activate)
        _focusableList[_focusedItem].activate();
    }

    if (handled > -1 && _focusedItem > -1) {
      auto parent = dynamic_cast<mforms::ScrollPanel*>(get_parent());
      if (parent != nullptr) {
        parent->scroll_to(static_cast<int>(_focusableList[_focusedItem].getBounds().pos.x), static_cast<int>(_focusableList[_focusedItem].getBounds().pos.y));
      }
      set_needs_repaint();
    }
  }

  return handled == 1;
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::focusIn() -> bool {
  if (!_focusableList.empty() && _focusedItem == -1) {
    _focusedItem = _lastFocusedItem > -1 ? _lastFocusedItem :  0;
    set_needs_repaint();
  }
  return false;
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::focusOut() -> bool {
  if (_focusedItem > -1) {
    _lastFocusedItem = _focusedItem;
    _focusedItem = -1;
    set_needs_repaint();
  }
  return false;
}

//--------------------------------------------------------------------------------------------------

auto DrawBox::mouse_down(mforms::MouseButton button, int x, int y) -> bool {
  if (button == mforms::MouseButtonLeft) {

    auto it = std::find_if(_focusableList.begin(), _focusableList.end(), [&](mforms::FocusableArea const& item) {
      return item.getBounds().contains(x, y);
    });

    if (it != _focusableList.end()) {
      _lastFocusedItem = _focusedItem;
      _focusedItem = static_cast<int>(it - _focusableList.begin());
    } else {
      _focusedItem = -1;
    }
  }
  set_needs_repaint();
  return false;
}
