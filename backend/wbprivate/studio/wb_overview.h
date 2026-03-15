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

#pragma once

#include "grt/tree_model.h"
#include "grt/grt_manager.h"
#include "base/ui_form.h"

#include "wb_command_ui.h"

#include "grts/structs.studio.h"

#include "wb_backend_public_interface.h"

#define DEFAULT_SECTION_HEIGHT 120

namespace wb {

  class WBContext;

  class MYSQLWBBACKEND_PUBLIC_FUNC OverviewBE : public bec::TreeModel, public bec::UIForm {
  public:
    enum OverviewColumn {
      Label, // editable
      NodeType,
      ChildNodeType,
      Expanded,
      Height,
      DisplayMode,

      FirstDetailField = 100
    };

    enum OverviewNodeType {
      ORoot,
      ODivision,
      OGroup,
      OSection,
      OItem,

      OSpecial
    };

    enum OverviewDisplayMode { MNone = 0, MLargeIcon = 1, MSmallIcon = 2, MList = 3 };

  public:
    OverviewBE(WBContext *wb);
    virtual ~OverviewBE();

    auto get_wb() -> WBContext * {
      return _wb;
    }

    virtual auto get_title() -> std::string;
    virtual auto identifier() const -> std::string = 0;

    virtual auto is_main_form() -> bool {
      return true;
    }

    virtual auto get_child(const bec::NodeId &parent, size_t index) -> bec::NodeId;
    virtual auto count_children(const bec::NodeId &parent) -> size_t;

    virtual auto get_grt_value(const bec::NodeId &node, ColumnId column) -> grt::ValueRef;
    virtual auto get_field(const bec::NodeId &node, ColumnId column, std::string &value) -> bool;
    virtual auto get_field(const bec::NodeId &node, ColumnId column, ssize_t &value) -> bool;

    auto get_details_field_count(const bec::NodeId &node) -> int;

    auto get_field_name(const bec::NodeId &node, ColumnId column) -> std::string;
    virtual auto get_field_description(const bec::NodeId &node, ColumnId column) -> std::string;
    virtual auto get_field_icon(const bec::NodeId &node, ColumnId column, bec::IconSize size) -> bec::IconId;

    virtual auto set_field(const bec::NodeId &node, ColumnId column, const std::string &value) -> bool;

    // only leaf nodes can be selected (for now)
    auto begin_selection_marking() -> void;
    auto end_selection_marking() -> void;
    auto unselect_all(const bec::NodeId &node) -> void;
    auto select_node(const bec::NodeId &node) -> void;

    auto signal_selection_changed() -> boost::signals2::signal<void()> * {
      return &_selection_change_signal;
    }
    auto get_selection() -> grt::ListRef<GrtObject>;

    auto get_selected_children(const bec::NodeId &node) -> std::list<int>;

    // only 1 node can be focused in each container node
    auto focus_node(const bec::NodeId &node) -> void;
    auto get_focused_child(const bec::NodeId &node) -> bec::NodeId;

    auto get_node_child_for_object(const bec::NodeId &node, const grt::ObjectRef &object) -> bec::NodeId;

    virtual auto is_expansion_disabled() -> bool {
      return false;
    }
    virtual auto get_default_tab_page_index() -> int {
      return -1;
    }

    auto search_child_item_node_matching(const bec::NodeId &node, const bec::NodeId &starting_node,
                                                const std::string &text) -> bec::NodeId;

    virtual auto activate_node(const bec::NodeId &node) -> bool;
    auto get_node_unique_id(const bec::NodeId &node) -> std::string;

    virtual auto is_editable(const bec::NodeId &node) const -> bool;
    virtual auto is_deletable(const bec::NodeId &node) const -> bool;
    virtual auto is_copyable(const bec::NodeId &node) const -> bool;
    auto request_add_object(const bec::NodeId &node) -> bool;
    auto request_delete_selected() -> int;
    auto request_delete_object(const bec::NodeId &node) -> bool;

    virtual auto get_model() -> model_ModelRef = 0;

    virtual auto get_popup_items_for_nodes(const std::vector<bec::NodeId> &nodes) -> bec::MenuItemList;
    virtual auto activate_popup_item_for_nodes(const std::string &name, const std::vector<bec::NodeId> &nodes) -> bool;

    virtual auto get_toolbar_items(const bec::NodeId &node) -> bec::ToolbarItemList;
    virtual auto activate_toolbar_item(const bec::NodeId &node, const std::string &name) -> bool;

    // for use by backend
    auto send_refresh_node(const bec::NodeId &node) -> void;
    auto send_refresh_children(const bec::NodeId &node) -> void;

    // for use by frontend
    std::function<void()> pre_refresh_groups;
    auto refresh() -> void;
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif
    // This is ok, as Overview contains children which also need to be refreshed.
    virtual auto refresh_node(const bec::NodeId &node, bool children) -> void = 0;
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

    virtual auto get_edit_target_name() -> std::string;
    auto get_target_name_for_nodes(const std::vector<bec::NodeId> &nodes) -> std::string;

    virtual auto can_cut() -> bool;
    virtual auto can_copy() -> bool;
    virtual auto can_paste() -> bool;
    virtual auto can_delete() -> bool;

    virtual auto cut() -> void;
    virtual auto copy() -> void;
    virtual auto paste() -> void;
    virtual auto delete_selection() -> void;

    // external drag & drop
    virtual auto get_node_drag_type(const bec::NodeId &node) -> std::string {
      return "";
    }

    virtual auto should_accept_file_drop_to_node(const bec::NodeId &node, const std::string &path) -> bool {
      return false;
    }

    virtual auto add_file_to_node(const bec::NodeId &node, const std::string &path) -> void {
    }
    virtual auto add_file_data_to_node(const bec::NodeId &node, const char *data, size_t length) -> void {
    }
    virtual auto get_file_for_node(const bec::NodeId &node) -> std::string {
      return "";
    }
    virtual auto get_file_data_for_node(const bec::NodeId &node, char *&data, size_t &length) -> bool {
      return true;
    }

    class MYSQLWBBACKEND_PUBLIC_FUNC Node {
    public:
      GrtObjectRef object;
      OverviewNodeType type;
      std::string label;
      std::string description;
      bec::IconId small_icon;
      bec::IconId large_icon;
      OverviewDisplayMode display_mode;
      bool expanded;
      bool selected;

      virtual auto get_child(size_t i) -> Node * {
        return 0;
      }
      virtual auto count_children() -> size_t {
        return 0;
      }
      virtual auto refresh() -> void {
      }

      virtual auto focus(OverviewBE *sender) -> void {
      }
      virtual auto activate(WBContext *wb) -> bool {
        return false;
      }
      virtual auto add_object(WBContext *wb) -> bool {
        return false;
      }
      virtual auto delete_object(WBContext *wb) -> void {
      }
      virtual auto is_deletable() -> bool {
        return false;
      }
      virtual auto copy_object(WBContext *wb, bec::Clipboard *clip) -> void {
      }
      virtual auto is_copyable() -> bool {
        return false;
      }
      virtual auto paste_object(WBContext *wb, bec::Clipboard *clip) -> void {
      }
      virtual auto is_pasteable(bec::Clipboard *clip) -> bool {
        return false;
      }

      virtual auto rename(WBContext *wb, const std::string &name) -> bool {
        return false;
      }
      virtual auto is_renameable() -> bool {
        return false;
      }

      virtual auto get_unique_id() -> std::string {
        return object.is_valid() ? object.id() : "";
      }

      virtual auto get_detail(int field) -> std::string {
        return "";
      }

      virtual auto get_popup_menu_items(WBContext *wb, bec::MenuItemList &items) -> int;

      virtual auto get_state() -> studio_OverviewPanelRef {
        studio_OverviewPanelRef panel = studio_OverviewPanelRef(grt::Initialized);

        panel->expandedHeight(0);
        panel->expanded(expanded ? 1 : 0);
        panel->itemDisplayMode((int)display_mode);

        return panel;
      }

      virtual auto restore_state(const studio_OverviewPanelRef &panel) -> void {
        expanded = *panel->expanded() ? true : false;
        display_mode = (OverviewDisplayMode)*panel->itemDisplayMode();
      }

      Node() : type(ORoot), small_icon(0), large_icon(0), display_mode(MNone), expanded(false), selected(false) {
      }

      Node(const Node &node)
        : type(node.type),
          label(node.label),
          description(node.description),
          small_icon(node.small_icon),
          large_icon(node.large_icon),
          display_mode(MNone),
          expanded(node.expanded),
          selected(false) {
      }

      virtual ~Node() {
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC ObjectNode : public Node {
    public:
      ObjectNode() {
        type = OverviewBE::OItem;
      }
      ObjectNode(const ObjectNode &copy) : Node(copy) {
        type = OverviewBE::OItem;
      }

      virtual auto activate(WBContext *wb) -> bool;
      virtual auto rename(WBContext *wb, const std::string &name) -> bool;

      virtual auto refresh() -> void {
        label = object->name();
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC AddObjectNode : public virtual Node {
      std::function<bool(WBContext *)> _add_slot;

    public:
      AddObjectNode(const std::function<bool(WBContext *)> &add_slot) : _add_slot(add_slot) {
        type = OverviewBE::OItem;
      }

      virtual auto activate(WBContext *wb) -> bool {
        return _add_slot(wb);
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC ContainerNode : public virtual Node {
    public:
      virtual auto init() -> void {
      }

    public:
      std::vector<Node *> children;
      Node *focused;
      OverviewNodeType child_type;

      ContainerNode(OverviewNodeType subtype) : focused(0), child_type(subtype) {
      }
      ContainerNode(const ContainerNode &node)
        : Node(node), children(node.children), focused(node.focused), child_type(node.child_type) {
      }

      virtual auto get_state() -> studio_OverviewPanelRef {
        studio_OverviewPanelRef panel = Node::get_state();

        // XXXfor (std::list<int>::const_iterator i= selection.begin(); i != selection.end(); ++i)
        //  panel.selectedItems().insert(*i);

        return panel;
      }

      virtual auto restore_state(const studio_OverviewPanelRef &panel) -> void {
        Node::restore_state(panel);

        // selection.clear();
        // XXX for (size_t c= panel.selectedItems().count(), i= 0; i < c; i++)
        //  selection.push_back(panel.selectedItems().get(i));
      }

      virtual ~ContainerNode() {
        clear_children();
      }

      virtual auto get_child(size_t i) -> Node * {
        if (i >= children.size())
          return 0;
        return children[i];
      }

      virtual auto count_children() -> size_t {
        return children.size();
      }

      auto clear_children() -> void {
        for (std::vector<Node *>::iterator iter = children.begin(); iter != children.end(); ++iter)
          delete *iter;
        children.clear();
      }

      virtual auto count_detail_fields() -> int {
        return 0;
      }
      virtual auto get_detail_name(int field) -> std::string {
        return "";
      }

      auto get_focused_index() -> int {
        int i = 0;
        for (std::vector<Node *>::iterator iter = children.begin(); iter != children.end(); ++iter) {
          if ((*iter) == focused)
            return i;
          ++i;
        }
        return -1;
      }

      virtual auto refresh_children() -> void {
      }
    };

  protected:
    WBContext *_wb;
    boost::signals2::signal<void()> _selection_change_signal;

    ContainerNode *_root_node;

    auto get_node_by_id(const bec::NodeId &node) const -> Node * {
      return do_get_node(node);
    }
    virtual auto do_get_node(const bec::NodeId &node) const -> Node *;

    auto get_deepest_focused() -> Node *;

    auto store_node_states(Node *node) -> void;
    auto store_state() -> void;
    auto restore_state() -> void;
  };
};
