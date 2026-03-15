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

#include "mforms/treeview.h"

namespace MySQL {
  namespace Forms {
    // class TreeViewNode : mforms::Node
    //{
    // private:
    //  std::string* _mytag;
    //  mforms::TreeNodeData *_data;
    //  std::vector<std::string> _text;
    //  std::vector<mforms::TreeNodeTextAttributes>* _attributes;
    //  std::map<int, void*> _icon;

    // public:
    //  TreeViewNode();
    //  ~TreeViewNode();
    //};
  };
};

class TreeNodeWrapper : public mforms::TreeNode {
  friend struct mforms::TreeNodeRef;

  mforms::TreeNodeRef _parent;
  std::string _tag;
  std::vector<std::string> _values;
  std::vector<mforms::TreeNodeTextAttributes> _attributes;
  std::vector<std::string> _icons;
  std::vector<TreeNodeWrapper *> _children;
  mforms::TreeNodeData *pdata;
  bool _expanded;

  int _index;

protected:
  auto is_root() const -> bool;

public:
  TreeNodeWrapper();

  auto node_index() const -> int {
    return _index;
  }

  virtual auto release() -> void;
  virtual auto retain() -> void;

  virtual auto equals(const mforms::TreeNode &other) -> bool;
  virtual auto is_valid() const -> bool;
  virtual auto level() const -> int;

  virtual auto set_icon_path(int column, const std::string &icon) -> void;

  virtual auto set_attributes(int column, const mforms::TreeNodeTextAttributes &attrs) -> void;
  virtual auto set_string(int column, const std::string &value) -> void;
  virtual auto set_int(int column, int value) -> void;
  virtual auto set_long(int column, std::int64_t value) -> void;
  virtual auto set_bool(int column, bool value) -> void;
  virtual auto set_float(int column, double value) -> void;

  virtual auto get_string(int column) const -> std::string;
  virtual auto get_int(int column) const -> int;
  virtual auto get_long(int column) const -> std::int64_t;
  virtual auto get_bool(int column) const -> bool;
  virtual auto get_float(int column) const -> double;

  virtual auto count() const -> int;
  virtual auto insert_child(int index) -> mforms::TreeNodeRef;
  virtual auto insert_child(int index, const mforms::TreeNode &node) -> void;
  virtual auto move_child(mforms::TreeNodeRef child, int new_index) -> void;
  virtual auto remove_from_parent() -> void;
  virtual auto get_child(int index) const -> mforms::TreeNodeRef;
  virtual auto get_child_index(mforms::TreeNodeRef child) const -> int;
  virtual auto get_parent() const -> mforms::TreeNodeRef;
  virtual auto previous_sibling() const -> mforms::TreeNodeRef;
  virtual auto next_sibling() const -> mforms::TreeNodeRef;
  virtual auto remove_children() -> void;
  virtual auto move_node(mforms::TreeNodeRef node, bool before) -> void;

  virtual std::vector<mforms::TreeNodeRef> add_node_collection(const mforms::TreeNodeCollectionSkeleton &nodes,
                                                               int position = -1);
  auto add_children_from_skeletons(std::vector<TreeNodeWrapper *> &parents,
                                   const std::vector<mforms::TreeNodeSkeleton> &children) -> void;

  virtual auto expand() -> void;
  virtual auto collapse() -> void;
  virtual auto is_expanded() -> bool;

  virtual auto toggle() -> void;

  virtual auto set_tag(const std::string &tag) -> void;
  virtual auto get_tag() const -> std::string;

  virtual auto set_data(mforms::TreeNodeData *data) -> void;
  virtual auto get_data() const -> mforms::TreeNodeData *;
};
