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

//!
//! \addtogroup linuxui Linux UI
//! @{
//!

#ifndef __TREEMODEL_WRAPPER_H__
#define __TREEMODEL_WRAPPER_H__

#include "listmodel_wrapper.h"
#include <set>

typedef std::set<std::string> ExpandedRowsStorage;

class TreeModelWrapper : public ListModelWrapper {
private:
  TreeModelWrapper(bec::TreeModel* tm, Gtk::TreeView* treeview, const std::string& name, const bec::NodeId& root_node,
                   bool as_list)
    : Glib::ObjectBase(typeid(TreeModelWrapper)),
      ListModelWrapper(tm, treeview, ("tv_" + name)),
      _root_node_path(root_node.toString()),
      _root_node_path_dot(root_node.toString() + "."),
      _show_as_list(as_list),
      _expanded_rows(0),
      _children_count_enabled(true),
      _delay_expanding_nodes(false) {
    if (treeview) {
      _expand_signal =
        treeview->signal_row_expanded().connect(sigc::mem_fun(this, &TreeModelWrapper::tree_row_expanded));
      _collapse_signal =
        treeview->signal_row_collapsed().connect(sigc::mem_fun(this, &TreeModelWrapper::tree_row_collapsed));
    }
  }

public:
  static auto create(bec::TreeModel* tm, Gtk::TreeView* treeview, const std::string& name,
                                               const bec::NodeId& root_node = bec::NodeId(), bool as_list = false) -> Glib::RefPtr<TreeModelWrapper> {
    bec::NodeId root = root_node.is_valid() ? root_node : tm->get_root();
    return Glib::RefPtr<TreeModelWrapper>(new TreeModelWrapper(tm, treeview, name, root, as_list));
  }

  auto set_delay_expanding_nodes(bool flag) -> void {
    _delay_expanding_nodes = flag;
  }

  auto expanded_rows_storage() const -> ExpandedRowsStorage* {
    return _expanded_rows;
  }
  auto set_expanded_rows_storage(ExpandedRowsStorage* s) -> void {
    _expanded_rows = s;
  }

  auto update_root_node(const bec::NodeId& root_node) -> void;

  virtual auto get_flags_vfunc() const -> Gtk::TreeModelFlags;

  virtual auto get_node_for_path(const Gtk::TreeModel::Path& path) const -> bec::NodeId;

  virtual auto get_icon_value(const iterator& iter, int column, const bec::NodeId& node, Glib::ValueBase& value) const -> void;

  virtual auto get_path_vfunc(const iterator& iter) const -> Gtk::TreeModel::Path;

  /**
  Sets @a iter to a valid iterator pointing to @a path

  @param path An path to a node.
  @param iter An iterator that will be set to refer to a node to the path, or will be set as invalid.
  @result true if the operation was possible.
  */
  virtual auto get_iter_vfunc(const Path& path, iterator& iter) const -> bool;

  /**
  Sets @a iter to refer to the first child of @a parent. If @a parent has no children,
  false is returned and @a iter is set to be invalid.

  @param parent An iterator.
  @param iter An iterator that will be set to refer to the firt child node, or will be set as invalid.
  @result true if the operation was possible.
  */
  virtual auto iter_children_vfunc(const iterator& parent, iterator& iter) const -> bool;

  /**
  Sets @a iter to be the parent of @a child. If @a child is at the toplevel, and
  doesn't have a parent, then @a iter is set to an invalid iterator and false
  is returned.

  @param child An iterator.
  @param iter An iterator that will be set to refer to the parent node, or will be set as invalid.
  @result true if the operation was possible.
  */
  virtual auto iter_parent_vfunc(const iterator& child, iterator& iter) const -> bool;

  /**
  Sets @a iter to be the child of @a parent using the given index.  The first
  index is 0.  If @a n is too big, or @a parent has no children, @a iter is set
  to an invalid iterator and false is returned.
  See also iter_nth_root_child_vfunc()

  @param parent An iterator.
  @param n The index of the child node to which @a iter should be set.
  @param iter An iterator that will be set to refer to the nth node, or will be set as invalid.
  @result true if the operation was possible.
  */
  virtual auto iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const -> bool;

  /**
  Sets @a iter to be the child of at the root level using the given index.  The first
  index is 0.  If @a n is too big, or if there are no children, @a iter is set
  to an invalid iterator and false is returned.
  See also iter_nth_child_vfunc().

  @param n The index of the child node to which @a iter should be set.
  @param iter An iterator that will be set to refer to the nth node, or will be set as invalid.
  @result true if the operation was possible.
  */
  virtual auto iter_nth_root_child_vfunc(int n, iterator& iter) const -> bool;

  /**
  Returns true if @a iter has children, false otherwise.

  @param iter The iterator to test for children.
  @result true if @a iter has children.
  */
  virtual auto iter_has_child_vfunc(const iterator& iter) const -> bool;

  /**
  Returns the number of children that @a iter has.
  See also iter_n_root_children_vfunc().

  @param iter The iterator to test for children.
  @result The number of children of @a iter.
  */
  virtual auto iter_n_children_vfunc(const iterator& iter) const -> int;

  /**
  Returns the number of toplevel nodes.
  See also iter_n_children().

  @result The number of children at the root level.
  */
  virtual auto iter_n_root_children_vfunc() const -> int;

  auto tree_row_expanded(const iterator& iter, const Path& path) -> void;
  auto tree_row_collapsed(const iterator& iter, const Path& path) -> void;

  auto block_expand_collapse_signals() -> void;
  auto unblock_expand_collapse_signals() -> void;

  auto children_count_enabled() -> bool {
    return _children_count_enabled;
  }
  auto children_count_enabled(bool value) -> void {
    _children_count_enabled = value;
  }

  // virtual bec::NodeId node_for_iter(const iterator &iter) const;
private:
  // virtual bool init_gtktreeiter(GtkTreeIter* it, const bec::NodeId &uid = bec::NodeId()) const;

  std::string _root_node_path;
  std::string _root_node_path_dot;
  bool _show_as_list;
  //! As bec::TreeModel::is_expanded is not working atm and ValueTreeBE::is_expanded neither,
  //! a workaround is used. Not every TreeModel needs to have ability to determine
  //! if a node is expanded or collapsed the optional storage is provided via _expanded_rows.
  //! _expanded_rows is a std::set of string representation of Gtk::TreeModel::Path.
  //! tree_row_expanded and tree_row_collapsed check if _expanded_rows is not null and
  //! insert/erase paths accordingly. TreeModelWrapper::set_expanded_rows_storage and
  //! TreeModelWrapper::expanded_rows_storage are used to get/set the storage.
  //! The storage is also used in expand_tree_nodes_as_in_be function, which expands
  //! Gtk::TreeView rows after the storage. This is used when a refresh is done for
  //! the TreeView/model.
  ExpandedRowsStorage* _expanded_rows;
  sigc::connection _expand_signal;
  sigc::connection _collapse_signal;
  bool _children_count_enabled;
  bool _delay_expanding_nodes;

  auto tm() const -> bec::TreeModel* {
    return static_cast<bec::TreeModel*>(get_be_model());
  }
};

#endif

//!
//! @}
//!
