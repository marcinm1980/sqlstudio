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

#include "base/string_utilities.h"
#include "base/file_utilities.h"
#include "base/threading.h"
#include "base/log.h"

#include "mforms/popup.h"
#include "mforms/menu.h"
#include "mforms/menubar.h"
#include "mforms/utilities.h"
#include "mforms/drawbox.h"
#include "mforms/textentry.h"
#include "mforms/imagebox.h"
#include "mforms/scrollpanel.h"

#include "mforms/home_screen.h"
#include "mforms/home_screen_helpers.h"
#include "mforms/home_screen_connections.h"

#include "base/any.h"
#include "base/accessibility.h"

using namespace base;
using namespace mforms;

//----------------- ShortcutSection ------------------------------------------------------------------------------------

SidebarEntry::SidebarEntry() : owner(nullptr), canSelect(false), icon(nullptr) {
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarEntry::getAccessibilityDescription() -> std::string {
  return title;
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarEntry::getAccessibilityRole() -> Accessible::Role {
  return Accessible::PushButton;
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarEntry::getAccessibilityBounds() -> Rect {
  return acc_bounds;
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarEntry::accessibilityDoDefaultAction() -> void {
  if (owner != nullptr) {
    owner->mouse_move(MouseButtonLeft, (int)acc_bounds.center().x, (int)acc_bounds.center().y);
    owner->mouse_click(MouseButtonLeft, (int)acc_bounds.center().x, (int)acc_bounds.center().y);
  }
}

//----------------- SidebarSection -------------------------------------------------------------------------------------

auto SidebarEntry::getAccessibilityDefaultAction() -> std::string {
  return "click";
}

//----------------- SidebarSection -------------------------------------------------------------------------------------

SidebarSection::SidebarSection(HomeScreen *owner) {
  _owner = owner;
  _hotEntry = nullptr;
  _activeEntry = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

SidebarSection::~SidebarSection() {
  for (auto &it : _entries) {
    deleteSurface(it.first->icon);
    delete it.first;
  }
  _entries.clear();
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::updateColors() -> void {
  if (_owner->isDarkModeActive()) {
    _indicatorColor = base::Color::parse("#282a2b");
  } else {
    _indicatorColor = base::Color::parse("#ffffff");
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::drawTriangle(cairo_t *cr, int x1, int y1, int x2, int y2, float alpha) -> void {
  cairo_set_source_rgba(cr, _indicatorColor.red, _indicatorColor.green, _indicatorColor.blue, alpha);
  cairo_move_to(cr, x2, y1 + abs(y2 - y1) / 3);
  cairo_line_to(cr, x1 + abs(x2 - x1) * 0.6, y1 + abs(y2 - y1) / 2);
  cairo_line_to(cr, x2, y2 - abs(y2 - y1) / 3);
  cairo_fill(cr);
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::repaint(cairo_t *cr, int areax, int areay, int areaw, int areah) -> void {
  int height = get_height();

  // Section buttons.
  int yoffset = SIDEBAR_TOP_PADDING;
  if (_entries.size() > 0 && yoffset < height) {
    for (auto &iterator : _entries) {
      float alpha = (iterator.first == _activeEntry  || iterator.first == _hotEntry) ? 1 : 0.5f;
      if ((yoffset + SIDEBAR_ROW_HEIGHT) > height)
        alpha = 0.25f;

      Size size = mforms::Utilities::getImageSize(iterator.first->icon);
      iterator.first->acc_bounds.pos.x = SIDEBAR_LEFT_PADDING;
      iterator.first->acc_bounds.pos.y = yoffset;
      iterator.first->acc_bounds.size.width = size.width;
      iterator.first->acc_bounds.size.height = SIDEBAR_ROW_HEIGHT;

      mforms::Utilities::paint_icon(cr, iterator.first->icon, SIDEBAR_LEFT_PADDING, yoffset, alpha);

      if (iterator.first == _activeEntry)
        drawTriangle(cr, get_width() - SIDEBAR_RIGHT_PADDING, yoffset, get_width(), yoffset + SIDEBAR_ROW_HEIGHT,
                     alpha);

      yoffset += SIDEBAR_ROW_HEIGHT + SIDEBAR_SPACING;
      if (yoffset >= height)
        break;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::shortcutFromPoint(int x, int y) -> int {
  if (x < SIDEBAR_LEFT_PADDING || y < SIDEBAR_TOP_PADDING || x > get_width() - SIDEBAR_RIGHT_PADDING)
    return -1;

  y -= SIDEBAR_TOP_PADDING;
  int point_in_row = y % (SIDEBAR_ROW_HEIGHT + SIDEBAR_SPACING);
  if (point_in_row >= SIDEBAR_ROW_HEIGHT)
    return -1;  // In the spacing between entries.

  size_t row = y / (SIDEBAR_ROW_HEIGHT + SIDEBAR_SPACING);
  size_t height = get_height() - SIDEBAR_TOP_PADDING;
  size_t row_bottom = row * (SIDEBAR_ROW_HEIGHT + SIDEBAR_SPACING) + SIDEBAR_ROW_HEIGHT;
  if (row_bottom > height)
    return -1;  // The last shortcut is dimmed if it goes over the bottom border.
                // Take it out from the hit test too.

  if (row < _entries.size())
    return (int)row;

  return -1;
}

//----------------------------------------------------------------------------------------------------------------------

/**
 * Adds a new sidebar entry to the internal list. The function performs some sanity checks.
 */
auto SidebarSection::addEntry(const std::string &title, const std::string &icon_name, HomeScreenSection *section,
                              std::function<void()> callback, bool canSelect) -> void {
  SidebarEntry *entry = new SidebarEntry;

  entry->callback = callback;
  entry->canSelect = canSelect;
  entry->owner = this;
  entry->title = title;

  entry->icon = mforms::Utilities::load_icon(icon_name, true);
  if (entry->icon == nullptr)
    throw std::runtime_error("Icon not found: " + icon_name);

  _entries.push_back({ entry, section });
  if (_activeEntry == nullptr && entry->canSelect && section != nullptr) {
    _activeEntry = entry;
    // If this is first entry, we need to show it.
    section->get_parent()->show(true);
  }

  set_layout_dirty(true);
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::getActive() -> HomeScreenSection * {
  if (_activeEntry != nullptr) {
    for (auto &it : _entries) {
      if (it.first == _activeEntry)
        return it.second;
    }
  }

  return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::setActive(HomeScreenSection *section) -> void {
  SidebarEntry *entryForSection = nullptr;
  for (auto &it : _entries) {
    if (it.second == section) {
      if (it.first == _activeEntry)
        return; // Section already active. Nothing to do.

      entryForSection = it.first;
    }
  }

  if (_activeEntry != nullptr) {
    for (auto &it : _entries) {
      if (it.first == _activeEntry)
        it.second->get_parent()->show(false);
    }
  }

  _activeEntry = entryForSection;
  section->get_parent()->show(true);
  set_needs_repaint();
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::mouse_click(mforms::MouseButton button, int x, int y) -> bool {
  switch (button) {
    case mforms::MouseButtonLeft: {
      if (_hotEntry != nullptr && _hotEntry->canSelect) {
        _activeEntry = _hotEntry;
        set_needs_repaint();
      }

      if (_hotEntry != nullptr && _hotEntry->callback)
        _hotEntry->callback();
    }
      break;

    default:
      break;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::mouse_leave() -> bool {
  if (_hotEntry != nullptr) {
    _hotEntry = nullptr;
    set_needs_repaint();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::mouse_move(mforms::MouseButton button, int x, int y) -> bool {
  SidebarEntry *shortcut = nullptr;
  int row = shortcutFromPoint(x, y);
  if (row > -1)
    shortcut = _entries[row].first;
  if (shortcut != _hotEntry) {
    _hotEntry = shortcut;
    set_needs_repaint();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::getAccessibilityChildCount() -> size_t {
  return _entries.size();
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::getAccessibilityChild(size_t index) -> Accessible * {
  Accessible *accessible = nullptr;
  if (index < _entries.size())
    accessible = _entries[index].first;

  return accessible;
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::getAccessibilityRole() -> Accessible::Role {
  return Accessible::List;
}

//----------------------------------------------------------------------------------------------------------------------

auto SidebarSection::accessibilityHitTest(ssize_t x, ssize_t y) -> Accessible * {
  Accessible *accessible = nullptr;
  int row = shortcutFromPoint(static_cast<int>(x), static_cast<int>(y));
  if (row != -1)
    accessible = _entries[row].first;

  return accessible;
}

//----------------- HomeScreen -----------------------------------------------------------------------------------------

HomeScreen::HomeScreen() : AppView(true, "Home", "home", true) {
  set_name("Home Screen");
  setInternalName("homeScreen");

  _sidebarSection = new SidebarSection(this);
  _sidebarSection->set_name("homeScreenSideBar");
  _sidebarSection->set_size(85, -1);
  add(_sidebarSection, false, true);

  Box::scoped_connect(signal_resized(), std::bind(&HomeScreen::on_resize, this));
  NotificationCenter::get()->add_observer(this, "GNColorsChanged");
  NotificationCenter::get()->add_observer(this, "GNBackingScaleChanged");
}

//----------------------------------------------------------------------------------------------------------------------

HomeScreen::~HomeScreen() {
  NotificationCenter::get()->remove_observer(this);
  clear_subviews();  // Remove our sections or the View d-tor will try to release them.

  delete _sidebarSection;
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::updateColors() -> void {
  _darkMode = mforms::App::get()->isDarkModeActive();
  if (_darkMode) {
    set_back_color("#282a2b");
  } else {
    set_back_color("#ffffff");
  }

#ifdef __APPLE__
  _sidebarSection->set_back_color("#37393a");
#else
  _sidebarSection->set_back_color("#464646");
#endif

  _sidebarSection->updateColors();

  for (auto section: _sections) {
    section->updateColors();
    section->updateIcons(); // Also refresh icons, even though no resolution change happen (icon colors might change).
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::updateIcons() -> void {
  for (auto section: _sections)
    section->updateIcons();
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::addSection(HomeScreenSection *section) -> void {
  if (section == nullptr)
    throw std::runtime_error("Empty HomeScreenSection given");

  _sections.push_back(section);

  mforms::ScrollPanel *scroll = mforms::manage(new mforms::ScrollPanel(mforms::ScrollPanelNoAutoScroll));
  scroll->set_name("Home Screen Main Panel");
  scroll->setInternalName("Home Screen Main Panel");
  scroll->add(section->getContainer());
  add(scroll, true, true);

  scroll->show(false);

  bool isCallbackOnly = section->callback ? true : false;
  _sidebarSection->addEntry(section->getTitle(), section->getIcon(), section, [this, isCallbackOnly, section]() {
    if (isCallbackOnly)
      section->callback();
    else {
      for (auto &it : _sections) {
        if (it != section)
          it->getContainer()->get_parent()->show(false);
        else
          it->getContainer()->get_parent()->show(true);
      }
    }
  }, !isCallbackOnly);
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::addSectionEntry(const std::string &title, const std::string &icon_name, std::function<void()> callback,
                                 bool canSelect) -> void {
  _sidebarSection->addEntry(title, icon_name, nullptr, callback, canSelect);
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::trigger_callback(HomeScreenAction action, const any &object) -> void {
  onHomeScreenAction(action, object);
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::cancelOperation() -> void {
  for (auto &it : _sections)
    it->cancelOperation();
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::set_menu(mforms::Menu *menu, HomeScreenMenuType type) -> void {
  switch (type) {
    case HomeMenuConnection:
    case HomeMenuConnectionGroup:
    case HomeMenuConnectionGeneric: {
      for (auto &it : _sections)
        it->setContextMenu(menu, type);
      break;
    }
    case HomeMenuDocumentModelAction: {
      for (auto &it : _sections)
        it->setContextMenuAction(menu, type);
      break;
    }

    case HomeMenuDocumentModel:
      for (auto &it : _sections)
        it->setContextMenu(menu, type);
      break;

    case HomeMenuDocumentSQLAction:
      for (auto &it : _sections)
        it->setContextMenuAction(menu, type);
      break;

    case HomeMenuDocumentSQL:
      for (auto &it : _sections)
        it->setContextMenu(menu, type);
      break;
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::on_resize() -> void {
  // Resize changes the layout so if there is pending script loading the popup is likely misplaced.
  cancelOperation();
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::setup_done() -> void {
  if (_sidebarSection->getActive()) {
    _sidebarSection->getActive()->setFocus();
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::showSection(size_t index) -> void {
  if (index < _sections.size()) {
    if (_sidebarSection != nullptr) {
      _sidebarSection->setActive(_sections[index]);
      _sidebarSection->getActive()->setFocus();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto HomeScreen::handle_notification(const std::string &name, void *sender, NotificationInfo &info) -> void {
  if (name == "GNColorsChanged") {
    updateColors();
  } else if (name == "GNBackingScaleChanged") {
    updateIcons();
  }
}

//----------------------------------------------------------------------------------------------------------------------

