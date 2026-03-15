/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

/* Notice: This file and the matching cpp file should be completely free
 * of any dependencies to allow it to be compiled as part of mforms
 * instead of wbpublic
 */

#include "common.h"
#include "trackable.h"

#include <vector>
#include <string>

namespace mforms {
  class MenuBar;
  class ToolBar;
}; // namespace mforms

namespace bec {

  // XXX deprecated
  enum MenuItemType { MenuAction, MenuSeparator, MenuCascade, MenuCheck, MenuRadio, MenuUnavailable };

  // XXX deprecated
  struct MenuItem;
  using MenuItemList = std::vector<MenuItem>;
  struct BASELIBRARY_PUBLIC_FUNC MenuItem {
    std::string oid;
    std::string caption;
    std::string shortcut;
    std::string accessibilityName;
    std::string internalName;
    MenuItemType type;

    bool enabled;
    bool checked;

    MenuItemList subitems;

    MenuItem() : type(MenuAction), enabled(true), checked(false) {
    }
  };

  // XXX deprecated
  enum ToolbarItemType {
    ToolbarAction,
    ToolbarSeparator,
    ToolbarToggle,
    ToolbarCheck,
    ToolbarRadio,
    ToolbarLabel,
    ToolbarDropDown,
    ToolbarSearch
  };

  // XXX deprecated
  struct BASELIBRARY_PUBLIC_FUNC ToolbarItem {
    int icon;
    int alt_icon;
    std::string caption;
    std::string name;
    std::string command;
    std::string tooltip;
    ToolbarItemType type;

    bool enabled;
    bool checked;

    ToolbarItem() : icon(0), alt_icon(0), type(ToolbarAction), enabled(true), checked(false) {
    }

    ToolbarItem(int aicon, const std::string &acommand, const std::string &atooltip, ToolbarItemType atype)
      : icon(aicon),
        alt_icon(0),
        name(acommand),
        command(acommand),
        tooltip(atooltip),
        type(atype),
        enabled(true),
        checked(false) {
    }
  };

  // XXX deprecated
  using ToolbarItemList = std::vector<ToolbarItem>;

#define GNUIFormCreated "GNUIFormCreated"
#define GNUIFormDestroyed "GNUIFormDestroyed"

  /** Base class for application forms.
   *
   * This is the base class for application windows or panel backends that can
   * receive focus and contain a workarea in the application (eg: canvas, overview panel,
   * query pages etc). It provides some virtual methods for common editing operations.
   *
   * @ingroup begrt
   */
  class BASELIBRARY_PUBLIC_FUNC UIForm : public base::trackable {
  public:
    UIForm();
    virtual ~UIForm();

    // unique identifier for the form
    auto form_id() -> std::string;

    virtual auto get_title() -> std::string = 0;

    void set_owner_data(void *data);
    auto get_owner_data() -> void *;

    void set_frontend_data(void *data);
    auto get_frontend_data() -> void *;

    virtual auto is_main_form() -> bool;
    virtual auto get_form_context_name() const -> std::string = 0;

    // Target description for cut/copy/delete menu items and for paste, after a copy is made.
    virtual auto get_edit_target_name() -> std::string;

    virtual auto can_undo() -> bool;
    virtual auto can_redo() -> bool;
    virtual auto can_cut() -> bool;
    virtual auto can_copy() -> bool;
    virtual auto can_paste() -> bool;
    virtual auto can_delete() -> bool;
    virtual auto can_select_all() -> bool;

    virtual void undo();
    virtual void redo();
    virtual void cut();
    virtual void copy();
    virtual void paste();
    virtual void delete_selection();
    virtual void select_all();

    virtual auto can_close() -> bool {
      return true;
    }
    virtual void close() {
    }

    // for main forms
    virtual auto get_menubar() -> mforms::MenuBar * {
      return 0;
    }
    virtual auto get_toolbar() -> mforms::ToolBar * {
      return 0;
    }

  protected:
    void *_owner_data;
    void *_frontend_data; // No strong reference for OSX!

  public:
    static auto form_with_id(const std::string &id) -> bec::UIForm *;
  };
} // namespace bec
