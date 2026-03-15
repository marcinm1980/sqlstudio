/*
 * Copyright (c) 2007, 2019, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2026 dev4fun. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#pragma once

#include "base/geometry.h"
#include <functional>

namespace base {

  struct BASELIBRARY_PUBLIC_FUNC Accessible {
  protected:
    std::string _accessibilityName;
  public:
    enum Role {
      RoleNone,
      Window,
      Pane,
      Link,
      List,
      ListItem,
      PushButton,
      StaticText,
      Text,
      Outline,
      OutlineButton,
      OutlineItem
    };

    virtual ~Accessible();

    // Description + role are mandatory. The first is often referred to as "name".
    virtual auto getAccessibilityDescription() -> std::string = 0;
    virtual auto getAccessibilityRole() -> Role = 0;

    // The identifier is merely informative and not supported on all platforms.
    virtual auto getAccessibilityIdentifier() -> std::string;

    // The rest of the accessible methods are optional, but it is strongly recommended to implement them
    // in all descendants for property accessibility + testing support.
    virtual auto getAccessibilityTitle() -> std::string;  
    virtual auto getAccessibilityValue() -> std::string;
    virtual auto getAccessibilityChildCount() -> size_t;
    virtual auto getAccessibilityChild(size_t index) -> Accessible*;
    virtual auto getAccessibilityBounds() -> base::Rect;
    virtual auto accessibilityHitTest(ssize_t x, ssize_t y) -> Accessible*;
    virtual auto getAccessibilityDefaultAction() -> std::string;
    virtual auto accessibilityDoDefaultAction() -> void;
    virtual auto accessibilityShowMenu() -> void;
    virtual auto accessibilityGrabFocus() -> bool;

    std::function <void(Accessible*)> onDestroy;
  };

}
