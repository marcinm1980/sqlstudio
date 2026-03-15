/*
 * Copyright (c) 2017, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "base/accessibility.h"

using namespace base;

//---------------------------------------------------------------------------------------------------------------------

Accessible::~Accessible() {
  if (onDestroy)
    onDestroy(this);
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::getAccessibilityIdentifier() -> std::string {
  return "";
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::getAccessibilityTitle() -> std::string {
  return "";
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::getAccessibilityDescription() -> std::string {
  return "";
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::getAccessibilityValue() -> std::string {
  return "";
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::getAccessibilityChildCount() -> size_t {
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::getAccessibilityChild(size_t index) -> Accessible* {
  return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::getAccessibilityBounds() -> base::Rect {
  return base::Rect();
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::accessibilityHitTest(ssize_t x, ssize_t y) -> Accessible* {
  return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::getAccessibilityDefaultAction() -> std::string {
  return "";
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::accessibilityDoDefaultAction() -> void {
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::accessibilityShowMenu() -> void {
}

//---------------------------------------------------------------------------------------------------------------------

auto Accessible::accessibilityGrabFocus() -> bool {
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
