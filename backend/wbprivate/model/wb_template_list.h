/*
 * Copyright (c) 2013, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "snippet_list.h"
#include "mforms/box.h"
#include "grts/structs.studio.physical.h"

namespace wb {
  class WBContextModel;
};

namespace mforms {
  class ToolBar;
  class ToolBarItem;
  class ScrollPanel;
};

class TableTemplatePanel;

class TableTemplateList : public BaseSnippetList, public bec::ListModel {
  TableTemplatePanel *_owner;

  auto prepare_context_menu() -> void;
  auto menu_will_show() -> void;

  virtual auto mouse_double_click(mforms::MouseButton button, int x, int y) -> bool;

  virtual auto count() -> size_t;
  virtual auto get_field(const bec::NodeId &node, ColumnId column, std::string &value) -> bool;
  virtual auto refresh() -> void;

public:
  auto get_selected_template() -> std::string;
  TableTemplateList(TableTemplatePanel *owner);
  ~TableTemplateList();
};

class TableTemplatePanel : public mforms::Box {
  TableTemplateList _templates;
  mforms::ToolBar *_toolbar;
  mforms::ScrollPanel *_scroll_panel;
  wb::WBContextModel *_context;

  auto toolbar_item_activated(mforms::ToolBarItem *item) -> void;

public:
  TableTemplatePanel(wb::WBContextModel *cmodel);

  auto on_action(const std::string &action) -> void;
};
