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

/*
 * TreeModel is the base class for all list or tree based
 * backend classes.
 */
#include <algorithm>

#include "grt.h"
#include "grt/icon_manager.h"
#include "grt/common.h"
#include "base/ui_form.h" // for menu stuff
#include "base/trackable.h"
#include "base/threading.h"

#include "wbpublic_public_interface.h"
#include <ctype.h>

#include <set>
#include <algorithm>

namespace mforms {
  class MenuBase;
}

namespace bec {
  /** A tree node index.
   * Used to index nodes in a tree or list backend that inherits from ListModel or TreeModel.
   * A nodeId is like an index, in the simplest case of a flat list, it will contain
   * a single integer index. For trees, it will contain one index for each parent node
   * until the leaf node it refers to.
   *
   * Since nodeIds are just indices, a nodeId may not point to the same item after
   * the list/tree contents change.
   *
   * @ingroup begrt
   */

  /**
    \class NodeId
    \brief descibes path to a node starting from root for a Tree or it will contain an index (single entry) for List

    Imagine we have a tree with a two root nodes and two children of each root. So to address first child of the
    first root node we need to have a path like that: "0.0". To address the second child of the first root node:
    "0.1"
  */
  struct WBPUBLICBACKEND_PUBLIC_FUNC NodeId {
    using uid = std::string *; // To map short-living NodeId path to a persistent value
                               // This is needed for Gtk::TreeModel iterators
    using Index = std::vector<size_t>;
    Index index;

    NodeId();
    NodeId(const NodeId &copy);
    NodeId(size_t i);
    NodeId(const std::string &str);
    ~NodeId();

    inline auto operator=(const NodeId &node) -> NodeId & {
      index = node.index;

      return *this;
    }

    auto operator<(const NodeId &r) const -> bool;

    inline auto operator==(const NodeId &node) const -> bool {
      return equals(node);
    }

    auto equals(const NodeId &node) const -> bool;

    inline auto depth() const -> size_t {
      return index.size();
    }

    auto operator[](size_t i) -> size_t &;
    auto operator[](size_t i) const -> const size_t &;

    auto end() const -> size_t;
    inline auto back() const -> size_t {
      return end();
    }

    auto previous() -> bool;
    auto next() -> bool;

    inline auto is_valid() const -> bool {
      return !index.empty();
    }

    auto parent() const -> NodeId;
    auto description() const -> std::string;
    auto toString(const char separator = '.') const -> std::string;

    auto append(size_t i) -> NodeId &;
    auto prepend(size_t i) -> NodeId &;
  };

  //----------------------------------------------------------------------------

  /**
    \class NodeIds
    \brief Mapper of short-living NodeId to a persistent item

    This class was added cause GtkTreeIter which is used in TreeModel for Gtk::TreeView
    has only three int-size fields describing iterator state. And it is not possible to fit entire NodeId
    there, moreover GtkTreeIter along with Gtk::TreeIter have no indication when iterator is
    destroyed. So the was a need to have something small which can be fit into those fields.
    Lifetime of the items named 'uid' is defined by the initial mapping when the uid is created
    and the issue of NodeIds::flush call. The mapped entity 'uid' is a pointer to a std::string stored
    in the std::set. For example if we need to have an item which lifetime is longer that NodeId("1.2.3")
    we should obtain mapped uid of the NodeId("1.2.3"). That mapped item - uid will be a pointer
    to a string in the std::set and the string itself will store "1.2.3". Having that uid allows us to get back
    path which can be stored between NodeId creations. That is used for example when we obtain Gtk::TreeIter
    and need to move to the next node in the Tree thus advance iterator.
    It is not advisable to dereference 'uid' to obtain std::string from std::string*, as future implementation
    may change that.
  */
  class NodeIds {
  public:
    NodeIds() = default;

    //! Resets map of NodeId paths to uid
    auto flush() -> void;

    //! Maps path with type of std::string from NodeId. This function is used for
    //! convenience. See map_node_id(const NodeId&)
    auto map_node_id(const std::string &path_from_nodeid) -> NodeId::uid;
    auto map_node_id(const NodeId &nid) -> NodeId::uid {
      return map_node_id(nid.toString());
    }

    //! Reverse mapping from 'uid' to a path
    auto map_node_id(const NodeId::uid nodeid) -> const std::string &;

  private:
    typedef std::set<std::string> Map;

    Map _map;
  };

  //------------------------------------------------------------------------------
  inline auto NodeIds::flush() -> void {
    _map.clear();
  }

  //------------------------------------------------------------------------------
  inline auto NodeIds::map_node_id(const std::string &path_from_nodeid) -> NodeId::uid {
    Map::const_iterator it = _map.find(path_from_nodeid);
    if (_map.end() != it)
      return (NodeId::uid) & (*it);
    else {
      // TODO: make a faster way. Probably we can use item from insert
      _map.insert(path_from_nodeid);
      return map_node_id(path_from_nodeid);
    }
  }

  //------------------------------------------------------------------------------
  inline auto NodeIds::map_node_id(const NodeId::uid nodeid) -> const std::string & {
    // Note that dereference of nodeid is dangerous after flush was called
    // That should be protected by stamp approach in TreeModel wrapper.
    static std::string empty;
    return nodeid ? *nodeid : empty;
  }

  /** Base list model class.
   */
  class WBPUBLICBACKEND_PUBLIC_FUNC ListModel : public base::trackable {
  private:
    NodeIds _nodeid_map;
    boost::signals2::signal<void(bec::NodeId, int)> _tree_changed_signal;

  public:
    typedef size_t ColumnId;
    typedef size_t RowId;

    virtual ~ListModel() {};

    virtual auto count() -> size_t = 0;
    virtual auto get_node(size_t index) -> NodeId;
    virtual auto has_next(const NodeId &node) -> bool;
    virtual auto get_next(const NodeId &node) -> NodeId;

    auto tree_changed_signal() -> boost::signals2::signal<void(bec::NodeId, int)> * {
      return &_tree_changed_signal;
    }

    void tree_changed(int old_child_count = -1, const bec::NodeId &parent = bec::NodeId()) {
      _tree_changed_signal(parent, old_child_count);
      _nodeid_map.flush();
    }

    auto nodeid_to_uid(const NodeId &nodeid) -> NodeId::uid {
      return _nodeid_map.map_node_id(nodeid);
    }

    auto nodeid_path_to_uid(const std::string &path) -> NodeId::uid {
      return _nodeid_map.map_node_id(path);
    }

    auto nodeuid_to_path(const NodeId::uid nodeuid) -> const std::string & {
      return _nodeid_map.map_node_id(nodeuid);
    }
    virtual auto get_field(const NodeId &node, ColumnId column, std::string &value) -> bool;
    virtual auto get_field(const NodeId &node, ColumnId column, ssize_t &value) -> bool;
    virtual auto get_field(const NodeId &node, ColumnId column, bool &value) -> bool;
    virtual auto get_field(const NodeId &node, ColumnId column, double &value) -> bool;

    virtual auto get_field_repr(const NodeId &node, ColumnId column, std::string &value) -> bool {
      return get_field(node, column, value);
    }

    // representation of the field as a GRT value
    virtual auto get_grt_value(const NodeId &node, ColumnId column) -> grt::ValueRef;

    virtual auto get_field_description(const NodeId &node, ColumnId column) -> std::string;
    virtual auto get_field_icon(const NodeId &node, ColumnId column, IconSize size) -> IconId;

    virtual auto refresh() -> void = 0;
    virtual auto refresh_node(const NodeId &node) -> void {
    }

    virtual auto reset() -> void {
    } //!

    virtual auto reorder(const NodeId &node, size_t index) -> void {
      throw std::logic_error("not implemented");
    }
    auto reorder_up(const NodeId &node) -> void;
    auto reorder_down(const NodeId &node) -> void;

    virtual auto activate_node(const NodeId &node) -> bool {
      throw std::logic_error("not implemented");
      return false;
    }

    // Parent can be NULL if the root node is meant.
    virtual auto update_menu_items_for_nodes(mforms::MenuBase *parent, const std::vector<NodeId> &nodes) -> void {};

    // Deprecated. Use update_menu_items_for_nodes for new code. MenuItemList and related code will go.
    virtual auto get_popup_items_for_nodes(const std::vector<NodeId> &nodes) -> MenuItemList {
      return MenuItemList();
    }
    //! Returns true if item was processed by BE, false - BE is unable to process command and FE should do it
    virtual auto activate_popup_item_for_nodes(const std::string &name, const std::vector<NodeId> &nodes) -> bool {
      throw std::logic_error("not implemented");
    }

    virtual auto can_delete_node(const NodeId &node) -> bool {
      return false;
    }
    virtual auto delete_node(const NodeId &node) -> bool {
      throw std::logic_error("not implemented");
    }

    // for editable lists only
    virtual auto get_field_type(const NodeId &node, ColumnId column) -> grt::Type;

    virtual auto set_field(const NodeId &node, ColumnId column, const std::string &value) -> bool;
    virtual auto set_field(const NodeId &node, ColumnId column, ssize_t value) -> bool;
    virtual auto set_field(const NodeId &node, ColumnId column, double value) -> bool;

    virtual auto set_convert_field(const NodeId &node, ColumnId column, const std::string &value) -> bool;

    //! By default we do not allow to edit items.
    //! This is a recently added method. It will replace occasionally used is_renameable
    virtual auto is_editable(const NodeId &node) const -> bool {
      return false;
    }
    virtual auto is_deletable(const NodeId &node) const -> bool {
      return false;
    }
    virtual auto is_copyable(const NodeId &node) const -> bool {
      return false;
    }

    // Indicates if a given node is to be visually exposed (e.g. an active schema in a schema tree).
    virtual auto is_highlighted(const NodeId &node) -> bool {
      return false;
    }

    virtual auto dump(int show_field) -> void;

  protected:
    // for internal use only
    virtual auto get_field_grt(const NodeId &node, ColumnId column, grt::ValueRef &value) -> bool;

    auto parse_value(grt::Type type, const std::string &value) -> grt::ValueRef;
  };

  /** Base tree model class.
   */
  class WBPUBLICBACKEND_PUBLIC_FUNC TreeModel : public ListModel {
  public:
    virtual auto count() -> size_t;
    virtual auto get_node(size_t index) -> NodeId;

    virtual auto get_root() const -> NodeId;
    virtual auto get_node_depth(const NodeId &node) -> size_t;
    inline auto get_parent(const NodeId &node) const -> NodeId {
      return node.parent();
    }

    virtual auto count_children(const NodeId &parent) -> size_t = 0;
    virtual auto get_child(const NodeId &parent, size_t index) -> NodeId {
      return NodeId(parent).append(index);
    }
    virtual auto has_next(const NodeId &node) -> bool;
    virtual auto get_next(const NodeId &node) -> NodeId;

    virtual auto is_expandable(const NodeId &node_id) -> bool;
    virtual auto expand_node(const NodeId &node) -> bool;
    virtual auto collapse_node(const NodeId &node) -> void;
    virtual auto is_expanded(const NodeId &node) -> bool;

    auto save_expand_info(const std::string &path) -> void;

    virtual auto dump(int show_field) -> void;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC GridModel : public ListModel {
  public:
    typedef std::shared_ptr<GridModel> Ref;

    enum ColumnType { UnknownType, StringType, NumericType, FloatType, DatetimeType, BlobType };

    virtual auto get_column_count() const -> size_t = 0;
    virtual auto get_column_caption(ColumnId column) -> std::string = 0;
    virtual auto get_column_type(ColumnId column) -> ColumnType = 0;
    virtual auto is_readonly() const -> bool {
      return false;
    } //!
    virtual auto readonly_reason() const -> std::string {
      return std::string();
    } //!
    virtual auto is_field_null(const bec::NodeId &node, ColumnId column) -> bool {
      return false;
    } //!
    virtual auto set_field_null(const bec::NodeId &node, ColumnId column) -> bool {
      return set_convert_field(node, column, "");
    } //!
    virtual auto set_edited_field(RowId row_index, ColumnId col_index) -> void {
    }

  public:
    typedef std::list<std::pair<ColumnId, int> > SortColumns;
    virtual auto sort_by(ColumnId column, int direction, bool retaining) -> void {
    }
    virtual auto sort_columns() const -> SortColumns {
      return SortColumns();
    }

  public:
    virtual auto floating_point_visible_scale() -> int {
      return 3;
    }
  };
}; // namespace bec
