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

#ifndef _ACTION_LIST_H_
#define _ACTION_LIST_H_

#include "wbpublic_public_interface.h"
#include "grt/tree_model.h"

class WBPUBLICBACKEND_PUBLIC_FUNC ActionList {
public:
  ActionList();
  auto reset() -> void;

  /* actions that doesn't require context node(s) */
public:
  using ActionSlot = std::function<void()>;
  auto register_action(const std::string &name, const ActionSlot &slot) -> void;
  auto unregister_action(const std::string &name) -> void;
  auto trigger_action(const std::string &name) -> bool;

private:
  using ActionSlots = std::map<std::string, ActionSlot>;
  ActionSlots _actions;

  /* actions for single node */
public:
  using NodeActionSlot = std::function<void(const bec::NodeId &)>;
  auto register_node_action(const std::string &name, const NodeActionSlot &slot) -> void;
  auto unregister_node_action(const std::string &name) -> void;
  auto trigger_action(const std::string &name, const bec::NodeId &node) -> bool;

private:
  using NodeActionSlots = std::map<std::string, NodeActionSlot>;
  NodeActionSlots _node_actions;

  /* actions for multiple nodes */
public:
  using NodesActionSlot = std::function<void(const std::vector<bec::NodeId> &)>;
  auto register_nodes_action(const std::string &name, const NodesActionSlot &slot) -> void;
  auto unregister_nodes_action(const std::string &name) -> void;
  auto trigger_action(const std::string &name, const std::vector<bec::NodeId> &nodes) -> bool;

private:
  using NodesActionSlots = std::map<std::string, NodesActionSlot>;
  NodesActionSlots _nodes_actions;

  /* actions for multiple row indexes plus column index */
public:
  using RowsColActionSlot = std::function<void(const std::vector<int> &, int)>;
  auto register_rows_col_action(const std::string &name, const RowsColActionSlot &slot) -> void;
  auto unregister_rows_col_action(const std::string &name) -> void;
  auto trigger_action(const std::string &name, const std::vector<int> &rows, int column) -> bool;

private:
  using RowsColActionSlots = std::map<std::string, RowsColActionSlot>;
  RowsColActionSlots _rows_col_actions;

  /* aux templates */
private:
  template <typename Slots, typename Slot>
  void register_action_(const std::string &name, Slots &slots, const Slot &slot);

  template <typename Slots>
  void unregister_action_(const std::string &name, Slots &slots);

  template <typename Slots>
  bool trigger_action_(const std::string &name, Slots &slots);

  template <typename Slots, typename Context>
  bool trigger_action_(const std::string &name, Slots &slots, const Context &context);
  template <typename Slots, typename Context1, typename Context2>
  bool trigger_action_(const std::string &name, Slots &slots, const Context1 &context1, const Context2 &context2);
};

#endif /* _ACTION_LIST_H_ */
