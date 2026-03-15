/*
 * Copyright (c) 2007, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "grt/action_list.h"

ActionList::ActionList() {
}

auto ActionList::reset() -> void {
  _actions.clear();
  _node_actions.clear();
  _nodes_actions.clear();
  _rows_col_actions.clear();
}

template <typename Slots, typename Slot>
void ActionList::register_action_(const std::string &name, Slots &slots, const Slot &slot) {
  slots[name] = slot;
}

template <typename Slots>
void ActionList::unregister_action_(const std::string &name, Slots &slots) {
  typename Slots::iterator action_iter = slots.find(name);
  if (slots.end() != action_iter)
  slots.erase(action_iter);
}

template <typename Slots>
bool ActionList::trigger_action_(const std::string &name, Slots &slots) {
  typename Slots::iterator action_iter = slots.find(name);
  if (slots.end() != action_iter) {
    action_iter->second();
    return true;
  } else {
    return false;
  }
}

template <typename Slots, typename Context>
bool ActionList::trigger_action_(const std::string &name, Slots &slots, const Context &context) {
  typename Slots::iterator action_iter = slots.find(name);
  if (slots.end() != action_iter) {
    action_iter->second(context);
    return true;
  } else {
    return false;
  }
}

template <typename Slots, typename Context1, typename Context2>
bool ActionList::trigger_action_(const std::string &name, Slots &slots, const Context1 &context1,
                                 const Context2 &context2) {
  typename Slots::iterator action_iter = slots.find(name);
  if (slots.end() != action_iter) {
    action_iter->second(context1, context2);
    return true;
  } else {
    return false;
  }
}

auto ActionList::register_action(const std::string &name, const ActionSlot &slot) -> void {
  register_action_(name, _actions, slot);
}

auto ActionList::register_node_action(const std::string &name, const NodeActionSlot &slot) -> void {
  register_action_(name, _node_actions, slot);
}

auto ActionList::register_nodes_action(const std::string &name, const NodesActionSlot &slot) -> void {
  register_action_(name, _nodes_actions, slot);
}

auto ActionList::register_rows_col_action(const std::string &name, const RowsColActionSlot &slot) -> void {
  register_action_(name, _rows_col_actions, slot);
}

auto ActionList::unregister_action(const std::string &name) -> void {
  unregister_action_(name, _actions);
}

auto ActionList::unregister_node_action(const std::string &name) -> void {
  unregister_action_(name, _node_actions);
}

auto ActionList::unregister_nodes_action(const std::string &name) -> void {
  unregister_action_(name, _nodes_actions);
}

auto ActionList::unregister_rows_col_action(const std::string &name) -> void {
  unregister_action_(name, _rows_col_actions);
}

auto ActionList::trigger_action(const std::string &name) -> bool {
  return trigger_action_(name, _actions);
}

auto ActionList::trigger_action(const std::string &name, const bec::NodeId &node) -> bool {
  return trigger_action_(name, _node_actions, node);
}

auto ActionList::trigger_action(const std::string &name, const std::vector<bec::NodeId> &nodes) -> bool {
  return trigger_action_(name, _nodes_actions, nodes);
}

auto ActionList::trigger_action(const std::string &name, const std::vector<int> &rows, int column) -> bool {
  return trigger_action_(name, _rows_col_actions, rows, column);
}
