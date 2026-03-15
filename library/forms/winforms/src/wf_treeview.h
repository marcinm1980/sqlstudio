/*
 * Copyright (c) 2012, 2018, Oracle and/or its affiliates. All rights reserved.
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
  namespace Forms {

    ref class TreeViewNode;

  public
    class TreeViewWrapper : public ViewWrapper {
    protected:
      TreeViewWrapper(mforms::TreeView *backend);
      virtual ~TreeViewWrapper();

      static auto create(mforms::TreeView *backend, mforms::TreeOptions options) -> bool;
      static auto add_column(mforms::TreeView *backend, mforms::TreeColumnType type, const std::string &name,
                            int initial_width, bool editable) -> int;
      static auto end_columns(mforms::TreeView *backend) -> void;

      static auto clear(mforms::TreeView *backend) -> void;

      static auto get_selection_mode(mforms::TreeView *backend) -> mforms::TreeSelectionMode;
      static auto set_selection_mode(mforms::TreeView *backend, mforms::TreeSelectionMode mode) -> void;
      static auto get_selection(mforms::TreeView *backend) -> std::list<mforms::TreeNodeRef>;
      static auto get_selected_node(mforms::TreeView *backend) -> mforms::TreeNodeRef;
      static auto clear_selection(mforms::TreeView *backend) -> void;
      static auto set_selected(mforms::TreeView *backend, mforms::TreeNodeRef node, bool flag) -> void;
      static auto scrollToNode(mforms::TreeView *backend, mforms::TreeNodeRef node) -> void;

      static auto set_allow_sorting(mforms::TreeView *backend, bool flag) -> void;
      static auto set_row_height(mforms::TreeView *backend, int h) -> void;

      static auto freeze_refresh(mforms::TreeView *backend, bool flag) -> void;

      static auto root_node(mforms::TreeView *backend) -> mforms::TreeNodeRef;

      static auto node_at_row(mforms::TreeView *backend, int row) -> mforms::TreeNodeRef;
      static auto node_at_position(mforms::TreeView *backend, base::Point position) -> mforms::TreeNodeRef;
      static auto row_for_node(mforms::TreeView *backend, mforms::TreeNodeRef node) -> int;
      static auto node_with_tag(mforms::TreeView *backend, const std::string &tag) -> mforms::TreeNodeRef;

      static auto set_column_title(mforms::TreeView *backend, int column, const std::string &title) -> void;

      static auto set_column_visible(mforms::TreeView *backend, int column, bool flag) -> void;
      static auto get_column_visible(mforms::TreeView *backend, int column) -> bool;

      static auto set_column_width(mforms::TreeView *backend, int column, int width) -> void;
      static auto get_column_width(mforms::TreeView *backend, int column) -> int;

      static auto BeginUpdate(mforms::TreeView *backend) -> void;
      static auto EndUpdate(mforms::TreeView *backend) -> void;

      virtual auto get_drop_position() -> mforms::DropPosition;

    public:
      auto set_row_height(int h) -> void;

      auto get_selection_mode() -> mforms::TreeSelectionMode;
      auto set_selection_mode(mforms::TreeSelectionMode mode) -> void;

      auto clear_selection() -> void;
      auto get_selection() -> std::list<mforms::TreeNodeRef>;
      auto set_selected(mforms::TreeNodeRef node, bool flag) -> void;
      auto scrollToNode(mforms::TreeNodeRef node) -> void;

      auto allow_column_sorting(bool flag) -> void;

      auto freeze_refresh(bool flag) -> void;

      auto root_node() -> mforms::TreeNodeRef;

      auto node_at_row(int row) -> mforms::TreeNodeRef;
      auto node_at_position(base::Point position) -> mforms::TreeNodeRef;
      auto row_for_node(mforms::TreeNodeRef node) -> int;

      auto set_column_title(int column, const std::string &title) -> void;

      auto set_column_visible(int column, bool flag) -> void;
      auto is_column_visible(int column) -> bool;

      auto set_column_width(int column, int width) -> void;
      auto get_column_width(int column) -> int;

      auto BeginUpdate() -> void;
      auto EndUpdate() -> void;

      // Internal functions.
      void process_mapping(Aga::Controls::Tree::TreeNodeAdv ^ node, const std::string &tag);
      auto node_value_set(int column) -> void;

      static auto init() -> void;
    };
  }
}
