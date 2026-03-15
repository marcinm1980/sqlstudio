/*
 * Copyright (c) 2010, 2018, Oracle and/or its affiliates. All rights reserved.
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

namespace MySQL {
  namespace Base {

  public
    enum class MenuItemType {
      MenuAction = bec::MenuAction,
      MenuSeparator = bec::MenuSeparator,
      MenuCascade = bec::MenuCascade,
      MenuCheck = bec::MenuCheck,
      MenuRadio = bec::MenuRadio,

      MenuUnavailable = bec::MenuUnavailable
    };

  public
    ref class MenuItem {
      System::String ^ caption;
      System::String ^ shortcut;
      System::String ^ internalName;
      MenuItemType type;

      bool enabled;
      bool checked;

      System::Collections::Generic::List<MenuItem ^> ^ subitems;

    public:
      MenuItem(const bec::MenuItem &item);

      auto get_caption() -> System::String ^;
      auto get_shortcut() -> System::String ^;
      auto getInternalName() -> System::String ^;
      auto get_type() -> MenuItemType;

      auto get_checked() -> bool;
      auto set_checked(bool value) -> void;

      auto get_enabled() -> bool;
      auto set_enabled(bool value) -> void;

      auto get_subitems() -> System::Collections::Generic::List<MenuItem ^> ^;
    };

  public
    enum class ToolbarItemType {
      ToolbarAction = bec::ToolbarAction,
      ToolbarSeparator = bec::ToolbarSeparator,
      ToolbarToggle = bec::ToolbarToggle,
      ToolbarLabel = bec::ToolbarLabel,
      ToolbarDropDown = bec::ToolbarDropDown,
      ToolbarRadio = bec::ToolbarRadio,
      ToolbarCheck = bec::ToolbarCheck,
      ToolbarSearch = bec::ToolbarSearch
    };

  public
    ref class ToolbarItem {
      int icon;
      int alt_icon;
      System::String ^ internalName;
      System::String ^ caption;
      System::String ^ command;
      System::String ^ tooltip;
      ToolbarItemType type;

      bool enabled;
      bool checked;

    public:
      ToolbarItem(const bec::ToolbarItem &item)
        : icon(item.icon),
          alt_icon(item.alt_icon),
          internalName(CppStringToNative(item.name)),
          caption(CppStringToNative(item.caption)),
          command(CppStringToNative(item.command)),
          tooltip(CppStringToNative(item.tooltip)),
          type((ToolbarItemType)item.type),
          enabled(item.enabled),
          checked(item.checked) {
      }

      auto get_icon() -> int {
        return icon;
      }

      auto get_alt_icon() -> int {
        return alt_icon;
      }

      auto getInternalName() -> System::String ^ { return internalName; }

        auto get_caption() -> System::String ^ { return caption; }

        auto get_command() -> System::String ^ { return command; }

        auto get_tooltip() -> System::String ^ { return tooltip; }

        auto get_type() -> ToolbarItemType {
        return type;
      }

      auto get_checked() -> bool {
        return checked;
      }

      auto get_enabled() -> bool {
        return enabled;
      }
    };

  public
    ref class UIForm {
    protected:
      bec::UIForm *inner;
      System::Runtime::InteropServices::GCHandle m_gch;

      UIForm(bec::UIForm *inn);
      UIForm();

      auto GetFixedId() -> System::IntPtr;
      auto ReleaseHandle() -> void;

    public:
      virtual ~UIForm();

      auto init(bec::UIForm *inn) -> void;
      auto get_unmanaged_object() -> bec::UIForm *;
      static auto GetFromFixedId(System::IntPtr ip) -> UIForm ^;
      auto can_close() -> bool;
      auto close() -> void;
      auto get_title() -> System::String ^;
      auto form_id() -> System::String ^;
    };
  }
}
