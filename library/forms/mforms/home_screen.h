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

#pragma once

#include <ctime>

#include "base/accessibility.h"
#include "base/notifications.h"
#include "base/data_types.h"
#include "base/any.h"

#include "mforms/appview.h"
#include "mforms/drawbox.h"
#include "mforms/tabview.h"

#include "home_screen_helpers.h"

namespace mforms {
  class Menu;
  class ConnectionsSection;
  class XProjectsSection;
  class XProjectEntry;

  class CommandUI;
  class SidebarSection;
  class HomeScreen;

  struct SidebarEntry : base::Accessible {
    SidebarSection *owner;
    std::function<void()> callback;
    bool canSelect;

    cairo_surface_t *icon;
    std::string title;       // Shorted title, depending on available space.
    base::Rect title_bounds; // Relative bounds of the title text.
    base::Rect acc_bounds;   // Bounds to be used for accessibility

    SidebarEntry();

    virtual std::string getAccessibilityDescription() override;
    virtual base::Accessible::Role getAccessibilityRole() override;
    virtual base::Rect getAccessibilityBounds() override;
    virtual void accessibilityDoDefaultAction() override;
    std::string getAccessibilityDefaultAction() override;
  };

  class SidebarSection : public mforms::DrawBox {
  private:
    HomeScreen *_owner;

    std::vector<std::pair<SidebarEntry *, HomeScreenSection *>> _entries;

    SidebarEntry *_hotEntry;
    SidebarEntry *_activeEntry; // For the context menu.

    base::Color _indicatorColor;

  public:
    const int SIDEBAR_LEFT_PADDING = 18;
    const int SIDEBAR_TOP_PADDING = 18; // The vertical offset of the first shortcut entry.
    const int SIDEBAR_RIGHT_PADDING = 25;
    const int SIDEBAR_ROW_HEIGHT = 50;
    const int SIDEBAR_SPACING = 18; // Vertical space between entries.

    SidebarSection(HomeScreen *owner);
    virtual ~SidebarSection();

    auto updateColors() -> void;

    auto drawTriangle(cairo_t *cr, int x1, int y1, int x2, int y2, float alpha) -> void;
    auto repaint(cairo_t *cr, int areax, int areay, int areaw, int areah) -> void;
    auto shortcutFromPoint(int x, int y) -> int;
    auto addEntry(const std::string &title, const std::string &icon_name, HomeScreenSection *section,
                  std::function<void()> callback, bool canSelect) -> void;
    auto getActive() -> HomeScreenSection *;
    auto setActive(HomeScreenSection *section) -> void;
    virtual auto mouse_click(mforms::MouseButton button, int x, int y) -> bool;
    auto mouse_leave() -> bool;

    virtual auto mouse_move(mforms::MouseButton button, int x, int y) -> bool;
    virtual auto getAccessibilityChildCount() -> size_t;
    virtual auto getAccessibilityChild(size_t index) -> Accessible *;
    virtual auto getAccessibilityRole() -> Accessible::Role;
    virtual auto accessibilityHitTest(ssize_t x, ssize_t y) -> base::Accessible *;
  };

  /**
   * This class implements the main (home) screen in MySql Studio.
   */
  class MFORMS_EXPORT HomeScreen : public mforms::AppView, public base::Observer {
  private:
    SidebarSection *_sidebarSection;

    std::string _pending_script; // The path to a script that should be opened next time a connection is opened.
    mforms::TabView _tabView;
    bool _darkMode;

    std::vector<HomeScreenSection *> _sections;

  public:
    std::function<void(base::any, std::string)> handleContextMenu;
    std::function<void(HomeScreenAction action, const base::any &object)> onHomeScreenAction;

    HomeScreen();
    virtual ~HomeScreen();

    auto addSection(HomeScreenSection *section) -> void;
    auto addSectionEntry(const std::string &title, const std::string &icon_name, std::function<void()> callback,
                         bool canSelect) -> void;

    auto trigger_callback(HomeScreenAction action, const base::any &object) -> void;

    auto cancelOperation() -> void;

    auto set_menu(mforms::Menu *menu, HomeScreenMenuType type) -> void;

    auto on_resize() -> void;
    auto setup_done() -> void;
    auto showSection(size_t index) -> void;

    auto isDarkModeActive() const -> bool { return _darkMode; };

    auto updateColors() -> void;
    auto updateIcons() -> void;

    virtual auto handle_notification(const std::string &name, void *sender, base::NotificationInfo &info) -> void;
  };
}

