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

#include "../stub_treenode.h"

#include "base/string_utilities.h"

auto TreeNodeWrapper::is_root() const -> bool {
  return true;
};

auto TreeNodeWrapper::release() -> void {
}
auto TreeNodeWrapper::retain() -> void {
}

auto TreeNodeWrapper::equals(const mforms::TreeNode &other) -> bool {
  return true;
}

auto TreeNodeWrapper::is_valid() const -> bool {
  return true;
}

auto TreeNodeWrapper::level() const -> int {
  return 1;
}

auto TreeNodeWrapper::set_icon_path(int column, const std::string &icon) -> void {
  set_string(column + 1, icon);
}

auto TreeNodeWrapper::set_attributes(int column, const mforms::TreeNodeTextAttributes &attrs) -> void {
  mforms::TreeNodeTextAttributes attributes;

  while (_attributes.size() < (size_t)(column + 1))
    _attributes.push_back(attributes);

  attributes = attrs;
  _attributes[column] = attributes;
}

auto TreeNodeWrapper::set_string(int column, const std::string &value) -> void {
  while (_values.size() < (size_t)(column + 1))
    _values.push_back("");

  _values[column] = value;
}

auto TreeNodeWrapper::set_int(int column, int value) -> void {
}
auto TreeNodeWrapper::set_long(int column, std::int64_t value) -> void {
}
auto TreeNodeWrapper::set_bool(int column, bool value) -> void {
}
auto TreeNodeWrapper::set_float(int column, double value) -> void {
}

auto TreeNodeWrapper::get_string(int column) const -> std::string {
  return _values.size() > (size_t)column ? _values[column] : "";
}

auto TreeNodeWrapper::get_int(int column) const -> int {
  return 0;
}

auto TreeNodeWrapper::get_long(int column) const -> std::int64_t {
  return 0;
}

auto TreeNodeWrapper::get_bool(int column) const -> bool {
  return true;
}

auto TreeNodeWrapper::get_float(int column) const -> double {
  return 0;
}

auto TreeNodeWrapper::count() const -> int {
  return (int)_children.size();
}

auto TreeNodeWrapper::add_node_collection(const mforms::TreeNodeCollectionSkeleton &nodes,
                                                                      int position) -> std::vector<mforms::TreeNodeRef> {
  std::vector<TreeNodeWrapper *> added_nodes;
  std::vector<mforms::TreeNodeRef> result;

  for (std::vector<std::string>::const_iterator it = nodes.captions.begin(); it != nodes.captions.end(); ++it) {
    TreeNodeWrapper *child = new TreeNodeWrapper();
    child->_parent = mforms::TreeNodeRef(this);

    mforms::TreeNodeRef tmp(child);
    tmp->set_string(0, *it);

    added_nodes.push_back(child);
  }

  // If the new nodes will have children as well, inserts them
  if (nodes.children.size())
    add_children_from_skeletons(added_nodes, nodes.children);

  bool at_end = (position == -1);
  for (size_t index = 0; index < added_nodes.size(); index++) {
    added_nodes[index]->_parent = mforms::TreeNodeRef(this);

    if ((int)_children.size() > position && !at_end)
      _children.insert(_children.begin() + position, added_nodes[index]);
    else
      _children.push_back(added_nodes[index]);

    position++;

    result.push_back(mforms::TreeNodeRef(added_nodes[index]));
  }

  return result;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::insert_child(int index) -> mforms::TreeNodeRef {
  if (index < 0) {
    TreeNodeWrapper *child = new TreeNodeWrapper();
    child->_parent = mforms::TreeNodeRef(this);
    _children.push_back(child);

    return mforms::TreeNodeRef(child);
  }

  TreeNodeWrapper *child = new TreeNodeWrapper();
  child->_parent = mforms::TreeNodeRef(this);
  _children.insert(_children.begin() + index, child);

  return mforms::TreeNodeRef(child);
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::insert_child(int index, const mforms::TreeNode &node) -> void {
  TreeNodeWrapper *child = (TreeNodeWrapper *)&node;
  child->_parent = mforms::TreeNodeRef(this);
  if (index < 0)
    _children.push_back(child);
  else
    _children.insert(_children.begin() + index, child);
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::move_child(mforms::TreeNodeRef node, int new_index) -> void {
  TreeNodeWrapper *child = (TreeNodeWrapper *)node.ptr();

  std::vector<TreeNodeWrapper *>::iterator i = std::find(_children.begin(), _children.end(), child);
  if (i == _children.end())
    return;

  int old_index = int(i - _children.begin());
  if (old_index == new_index)
    return;

  _children.erase(i);
  if (old_index < new_index)
    --new_index;
  _children.insert(_children.begin() + new_index, child);
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::add_children_from_skeletons(std::vector<TreeNodeWrapper *> &parents,
                                                  const std::vector<mforms::TreeNodeSkeleton> &children) -> void {
  for (size_t child_index = 0; child_index < children.size(); child_index++) {
    // Creates "this" child for each parent
    std::vector<TreeNodeWrapper *> added_nodes;

    for (size_t index = 0; index < parents.size(); index++) {
      TreeNodeWrapper *child = new TreeNodeWrapper();
      child->set_string(0, children[child_index].caption);
      child->set_tag(children[child_index].tag);
      added_nodes.push_back(child);
    }

    // If the new nodes will have childrens as well, inserts them
    if (children[child_index].children.size())
      add_children_from_skeletons(added_nodes, children[child_index].children);

    // Now inserts each children to it's corresponding parent
    for (size_t parent_index = 0; parent_index < parents.size(); parent_index++) {
      added_nodes[parent_index]->_parent = mforms::TreeNodeRef(parents[parent_index]);
      parents[parent_index]->_children.push_back(added_nodes[parent_index]);
    }
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::remove_from_parent() -> void {
  TreeNodeWrapper *inner_parent = dynamic_cast<TreeNodeWrapper *>(_parent.ptr());

  if (inner_parent) {
    if (std::find(inner_parent->_children.begin(), inner_parent->_children.end(), this) !=
        inner_parent->_children.end()) {
      inner_parent->_children.erase(std::find(inner_parent->_children.begin(), inner_parent->_children.end(), this));
    }
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_child(int index) const -> mforms::TreeNodeRef {
  return (_children.size() > (size_t)index) ? mforms::TreeNodeRef(_children[index]) : mforms::TreeNodeRef();
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_child_index(mforms::TreeNodeRef node) const -> int {
  TreeNodeWrapper *child = (TreeNodeWrapper *)node.ptr();
  std::vector<TreeNodeWrapper *>::const_iterator i = std::find(_children.begin(), _children.end(), child);
  if (i == _children.end())
    return -1;

  return (int)(i - _children.begin());
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_parent() const -> mforms::TreeNodeRef {
  return _parent;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::previous_sibling() const -> mforms::TreeNodeRef {
  return mforms::TreeNodeRef();
}

auto TreeNodeWrapper::next_sibling() const -> mforms::TreeNodeRef {
  return mforms::TreeNodeRef();
}

auto TreeNodeWrapper::remove_children() -> void {
  mforms::TreeNode::remove_children();
}

auto TreeNodeWrapper::move_node(mforms::TreeNodeRef node, bool before) -> void {
}

auto TreeNodeWrapper::expand() -> void {
  _expanded = true;
}

auto TreeNodeWrapper::collapse() -> void {
  _expanded = false;
}

auto TreeNodeWrapper::is_expanded() -> bool {
  return _expanded;
}

auto TreeNodeWrapper::toggle() -> void {
  _expanded = !_expanded;
}

auto TreeNodeWrapper::set_tag(const std::string &tag) -> void {
  _tag = tag;
}
auto TreeNodeWrapper::get_tag() const -> std::string {
  return _tag;
}

auto TreeNodeWrapper::set_data(mforms::TreeNodeData *data) -> void {
  pdata = data;
}

auto TreeNodeWrapper::get_data() const -> mforms::TreeNodeData * {
  return pdata;
}

TreeNodeWrapper::TreeNodeWrapper() : pdata(NULL), _expanded(false) {
  _parent = mforms::TreeNodeRef();
}
//------------------------------------------------------------------------------
// void TreeNodeWrapper::init()
//{
//  ::mforms::ControlFactory *f= ::mforms::ControlFactory::get_instance();
//
//}
