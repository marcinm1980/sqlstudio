/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "stub_view.h"
#include "stub_treenode.h"

namespace mforms {
  namespace stub {

    class TreeViewWrapper : public ViewWrapper {
    private:
      TreeNodeWrapper *_root;

      TreeViewWrapper(TreeView *self, mforms::TreeOptions opts) : ViewWrapper(self) {
        _root = new TreeNodeWrapper();
      }

      auto add_column(TreeColumnType type, const std::string &name, int initial_width, bool editable) -> int {
        return 0;
      }

      auto end_columns() -> void {
      }

      static auto create(TreeView *self, mforms::TreeOptions opt) -> bool {
        new TreeViewWrapper(self, opt);
        return true;
      }

      static auto add_column(TreeView *self, TreeColumnType type, const std::string &name, int width, bool editable) -> int {
        return 0;
      }

      static auto add_column(TreeView *self, TreeColumnType type, const std::string &name, int width, bool editable,
                            bool a) -> int {
        return 0;
      }

      static auto end_columns(TreeView *self) -> void {
      }

      static auto clear(TreeView *self) -> void {
      }

      static auto get_selection_mode(TreeView *self) -> TreeSelectionMode {
        return TreeSelectSingle;
      }

      static auto set_selection_mode(TreeView *self, TreeSelectionMode mode) -> void {
      }

      static auto get_selected_node(TreeView *self) -> TreeNodeRef {
        return TreeNodeRef();
      }

      static auto root_node(TreeView *tree) -> TreeNodeRef {
        TreeViewWrapper *ptree_node_view = dynamic_cast<TreeViewWrapper *>(ObjectWrapper::getData(tree));
        return ptree_node_view->root_node();
      }

      auto root_node() -> TreeNodeRef {
        return mforms::TreeNodeRef(_root);
      }

      static auto get_selection(TreeView *self) -> std::list<TreeNodeRef> {
        return std::list<TreeNodeRef>();
      }

      static auto set_selected(TreeView *self, TreeNodeRef node, bool flag) -> void {
      }

      static auto set_allow_sorting(TreeView *self, bool) -> void {
      }

      static auto freeze_refresh(TreeView *self, bool) -> void {
      }

      static auto row_for_node(TreeView *self, TreeNodeRef node) -> int {
        return 0;
      }

      static auto node_at_row(TreeView *self, int row) -> TreeNodeRef {
        return self->root_node();
      }

      static auto set_row_height(TreeView *self, int height) -> void {
      }

      static auto clear_selection(TreeView *self) -> void {
      }

      static auto node_with_tag(TreeView *self, const std::string &tag) -> TreeNodeRef {
        return TreeNodeRef();
      }

      static auto scrollToNode(TreeView *self, TreeNodeRef node) -> void {
      }

      static auto rowForNode(TreeView *self, TreeNodeRef node) -> int {
        return 0;
      }

      static auto nodeAtRow(TreeView *self, int row) -> TreeNodeRef {
        return TreeNodeRef();
      }

      static auto nodeAtPosition(TreeView *self, base::Point position) -> TreeNodeRef {
        return TreeNodeRef();
      }

      static auto setColumnTitle(TreeView *self, int column, const std::string &title) -> void {
      }

      static auto setColumnVisible(TreeView *self, int column, bool flag) -> void {
      }

      static auto getColumnVisible(TreeView *self, int column) -> bool {
        return false;
      }

      static auto setColumnWidth(TreeView *self, int column, int width) -> void {
      }

      static auto getColumnWidth(TreeView *self, int column) -> int {
        return 0;
      }

      static auto BeginUpdate(TreeView *self) -> void {
      }

      static auto EndUpdate(TreeView *self) -> void {
      }

    public:
      static auto init() -> void {
        ::mforms::ControlFactory *f = ::mforms::ControlFactory::get_instance();

        f->_treeview_impl.create = &TreeViewWrapper::create;
        f->_treeview_impl.add_column = &TreeViewWrapper::add_column;
        f->_treeview_impl.end_columns = &TreeViewWrapper::end_columns;
        f->_treeview_impl.clear = &TreeViewWrapper::clear;
        f->_treeview_impl.get_selected_node = &TreeViewWrapper::get_selected_node;
        f->_treeview_impl.set_selected = &TreeViewWrapper::set_selected;
        f->_treeview_impl.get_selection = &TreeViewWrapper::get_selection;
        f->_treeview_impl.set_allow_sorting = &TreeViewWrapper::set_allow_sorting;
        f->_treeview_impl.freeze_refresh = &TreeViewWrapper::freeze_refresh;
        f->_treeview_impl.set_selection_mode = &TreeViewWrapper::set_selection_mode;
        f->_treeview_impl.get_selection_mode = &TreeViewWrapper::get_selection_mode;
        f->_treeview_impl.root_node = &TreeViewWrapper::root_node;
        f->_treeview_impl.node_at_row = &TreeViewWrapper::node_at_row;
        f->_treeview_impl.row_for_node = &TreeViewWrapper::row_for_node;
        f->_treeview_impl.set_row_height = &TreeViewWrapper::set_row_height;
        f->_treeview_impl.clear_selection = &TreeViewWrapper::clear_selection;
        f->_treeview_impl.node_with_tag = &TreeViewWrapper::node_with_tag;
        f->_treeview_impl.scrollToNode = &TreeViewWrapper::scrollToNode;
        f->_treeview_impl.row_for_node = &TreeViewWrapper::rowForNode;
        f->_treeview_impl.node_at_row = &TreeViewWrapper::nodeAtRow;
        f->_treeview_impl.node_at_position = &TreeViewWrapper::nodeAtPosition;
        f->_treeview_impl.set_column_title = &TreeViewWrapper::setColumnTitle;
        f->_treeview_impl.set_column_visible = &TreeViewWrapper::setColumnVisible;
        f->_treeview_impl.get_column_visible = &TreeViewWrapper::getColumnVisible;
        f->_treeview_impl.set_column_width = &TreeViewWrapper::setColumnWidth;
        f->_treeview_impl.get_column_width = &TreeViewWrapper::getColumnWidth;
        f->_treeview_impl.BeginUpdate = &TreeViewWrapper::BeginUpdate;
        f->_treeview_impl.EndUpdate = &TreeViewWrapper::EndUpdate;
      }
    };
  }
}
