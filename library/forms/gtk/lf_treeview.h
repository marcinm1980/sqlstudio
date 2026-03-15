/*
 * Copyright (c) 2008, 2019, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _LF_TREEVIEW_H_
#define _LF_TREEVIEW_H_

#include "mforms/mforms.h"

#include "lf_view.h"
#include "base/string_utilities.h"

namespace mforms {
  namespace gtk {

    struct TreeNodeDataRef {
      TreeNodeData *_data;

      TreeNodeDataRef() : _data(0) {
      }

      TreeNodeDataRef(TreeNodeData *data) : _data(data) {
        if (_data)
          _data->retain();
      }

      TreeNodeDataRef(const TreeNodeDataRef &other) : _data(other._data) {
        if (_data)
          _data->retain();
      }

      ~TreeNodeDataRef() {
        if (_data)
          _data->release();
      }

      TreeNodeDataRef &operator=(const TreeNodeDataRef &other) {
        if (_data != other._data) {
          if (_data)
            _data->release();
          _data = other._data;
          if (_data)
            _data->retain();
        }
        return *this;
      }
    };

    class CustomTreeStore : public Gtk::TreeStore {
    public:
      CustomTreeStore(const Gtk::TreeModelColumnRecord &columns);

      static auto create(const Gtk::TreeModelColumnRecord &columns) -> Glib::RefPtr<CustomTreeStore>;

      auto copy_iter(Gtk::TreeModel::iterator &from, Gtk::TreeModel::iterator &to) -> void;
    };

    class TreeViewImpl; // rename

    class RootTreeNodeImpl : public ::mforms::TreeNode {
    protected:
      TreeViewImpl *_treeview;
      int _refcount;

      inline auto ref_from_iter(const Gtk::TreeIter &iter) const -> TreeNodeRef;
      inline auto ref_from_path(const Gtk::TreePath &path) const -> TreeNodeRef;

      virtual auto is_root() const -> bool;

      virtual auto is_valid() const -> bool;

      virtual auto equals(const TreeNode &other) -> bool;

      virtual auto level() const -> int;

    public:
      RootTreeNodeImpl(TreeViewImpl *tree);

      virtual auto invalidate() -> void;

      virtual auto release() -> void;

      virtual auto retain() -> void;

      virtual auto count() const -> int;

      virtual auto can_expand() -> bool;

      virtual auto create_child(int index) -> Gtk::TreeIter;

      virtual auto create_child(int index, Gtk::TreeIter *other_parent) -> Gtk::TreeIter;

      virtual auto insert_child(int index) -> TreeNodeRef;

      virtual std::vector<mforms::TreeNodeRef> add_node_collection(const TreeNodeCollectionSkeleton &nodes,
                                                                   int position = -1);

      virtual auto add_children_from_skeletons(const std::vector<Gtk::TreeIter> &parents,
                                               const std::vector<TreeNodeSkeleton> &children) -> void;

      virtual auto remove_from_parent() -> void;

      virtual auto get_child(int index) const -> TreeNodeRef;

      virtual auto get_parent() const -> TreeNodeRef;

      virtual auto expand() -> void;

      virtual auto collapse() -> void;

      virtual auto is_expanded() -> bool;

      virtual auto set_attributes(int column, const mforms::TreeNodeTextAttributes &attrs) -> void;

      virtual auto set_icon_path(int column, const std::string &icon) -> void;

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

      virtual auto set_tag(const std::string &tag) -> void;

      virtual auto get_tag() const -> std::string;

      virtual auto set_data(TreeNodeData *data) -> void;

      virtual auto get_data() const -> TreeNodeData *;

      virtual auto previous_sibling() const -> TreeNodeRef;

      virtual auto next_sibling() const -> TreeNodeRef;

      virtual auto get_child_index(TreeNodeRef child) const -> int;

      virtual auto move_node(TreeNodeRef node, bool before) -> void;
    };

    class TreeNodeImpl : public RootTreeNodeImpl {
      // If _rowref becomes invalidated (eg because Model was deleted),
      // we just ignore all operations on the node
      Gtk::TreeRowReference _rowref;

    public:
      inline auto model() -> Glib::RefPtr<Gtk::TreeStore>;
      inline auto iter() -> Gtk::TreeIter;

      inline auto iter() const -> Gtk::TreeIter;

      inline auto path() -> Gtk::TreePath;

      virtual auto is_root() const -> bool;

      virtual auto duplicate_node(TreeNodeRef oldnode) -> Gtk::TreeIter;

    public:
      TreeNodeImpl(TreeViewImpl *tree, Glib::RefPtr<Gtk::TreeStore> model, const Gtk::TreePath &path);

      TreeNodeImpl(TreeViewImpl *tree, const Gtk::TreeRowReference &ref);

      virtual auto equals(const TreeNode &other) -> bool;

      virtual auto is_valid() const -> bool;

      virtual auto invalidate() -> void;

      virtual auto count() const -> int;

      virtual auto create_child(int index) -> Gtk::TreeIter;

      virtual auto remove_from_parent() -> void;

      virtual auto get_child(int index) const -> TreeNodeRef;

      virtual auto get_parent() const -> TreeNodeRef;

      virtual auto expand() -> void;

      virtual auto can_expand() -> bool;

      virtual auto collapse() -> void;

      virtual auto is_expanded() -> bool;

      virtual auto set_attributes(int column, const TreeNodeTextAttributes &attrs) -> void;

      virtual auto set_icon_path(int column, const std::string &icon) -> void;

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

      virtual auto set_tag(const std::string &tag) -> void;

      virtual auto get_tag() const -> std::string;

      virtual auto set_data(TreeNodeData *data) -> void;

      virtual auto get_data() const -> TreeNodeData *;

      virtual auto level() const -> int;

      virtual auto next_sibling() const -> TreeNodeRef;

      virtual auto previous_sibling() const -> TreeNodeRef;

      virtual auto get_child_index(TreeNodeRef child) const -> int;

      virtual auto move_node(TreeNodeRef node, bool before) -> void;
    };

    class TreeViewImpl : public ViewImpl // rename
    {
      friend class RootTreeNodeImpl;
      friend class TreeNodeImpl;

    private:
      class ColumnRecord : public Gtk::TreeModelColumnRecord {
        template <class C>
        Gtk::TreeModelColumn<C> *add_model_column() {
          Gtk::TreeModelColumn<C> *col = new Gtk::TreeModelColumn<C>();
          columns.push_back(col);
          add(*col);
          return col;
        }

        auto on_cell_editing_started(Gtk::CellEditable *e, const Glib::ustring &path) -> void;
        auto on_focus_out(GdkEventFocus *event, Gtk::Entry *e) -> bool;

      public:
        std::vector<Gtk::TreeModelColumnBase *> columns;
        Gtk::TreeModelColumn<std::string> _tag_column;
        Gtk::TreeModelColumn<TreeNodeDataRef> _data_column;
        std::vector<int> column_value_index;
        std::vector<int> column_attr_index;

        template <class C>
        const Gtk::TreeModelColumn<C> &get(int column) {
          Gtk::TreeModelColumnBase *c = columns[column];

          return *static_cast<Gtk::TreeModelColumn<C> *>(c);
        }

        virtual ~ColumnRecord();
        auto add_tag_column() -> void;
        auto add_data_column() -> void;
        auto tag_column() -> Gtk::TreeModelColumn<std::string> &;
        auto data_column() -> Gtk::TreeModelColumn<TreeNodeDataRef> &;
        auto add_string(Gtk::TreeView *tree, const std::string &title, bool editable, bool attr, bool with_icon,
                       bool align_right = false) -> int;
        auto add_integer(Gtk::TreeView *tree, const std::string &title, bool editable, bool attr) -> int;
        auto add_long_integer(Gtk::TreeView *tree, const std::string &title, bool editable, bool attr) -> int;
        auto add_float(Gtk::TreeView *tree, const std::string &title, bool editable, bool attr) -> int;
        auto add_check(Gtk::TreeView *tree, const std::string &title, bool editable, bool attr) -> int;
        auto add_tri_check(Gtk::TreeView *tree, const std::string &title, bool editable, bool attr) -> int;
        template <typename T>
        std::pair<Gtk::TreeViewColumn *, int> create_column(Gtk::TreeView *tree, const std::string &title,
                                                            bool editable, bool attr, bool with_icon,
                                                            bool align_right = false);
        auto format_tri_check(Gtk::CellRenderer *cell, const Gtk::TreeIter &iter,
                              const Gtk::TreeModelColumn<int> &column) -> void;
      };

      bool _is_drag_source;
      ColumnRecord _columns;

      Gtk::ScrolledWindow _swin;
      Gtk::TreeView _tree;

      sigc::connection _conn;
      int _row_height;
      bool _flat_list;
      bool _tagmap_enabled;
      bool _drag_source_enabled;

      Gtk::TreePath _overlayed_row;
      std::vector<Cairo::RefPtr<Cairo::ImageSurface> > _overlay_icons;
      int _hovering_overlay;
      int _clicking_overlay;
      bool _mouse_inside;

      int _drag_button;
      int _drag_start_x;
      int _drag_start_y;
      bool _drag_in_progress;

      Glib::RefPtr<Gtk::TreeStore> _tree_store;
      Glib::RefPtr<Gtk::TreeModelSort> _sort_model;
      std::map<std::string, Glib::RefPtr<Gdk::Pixbuf> > _pixbufs;

      std::map<std::string, Gtk::TreeRowReference> _tagmap;

      mforms::TreeNodeRef _root_node;

      auto find_node_at_row(const Gtk::TreeModel::Children &trow, int &c, int row) -> mforms::TreeNodeRef;

      auto tree_view() -> Gtk::TreeView * {
        return &_tree;
      }
      virtual auto get_outer() const -> Gtk::Widget * {
        return &(const_cast<Gtk::ScrolledWindow &>(_swin));
      }
      virtual auto get_inner() const -> Gtk::Widget * {
        return &(const_cast<Gtk::TreeView &>(_tree));
      }

      TreeViewImpl(TreeView *self, mforms::TreeOptions opts);
      ~TreeViewImpl();
      auto string_edited(const Glib::ustring &path, const Glib::ustring &new_text, int column) -> void;
      auto toggle_edited(const Glib::ustring &path, int column) -> void;
      auto on_activated(const Gtk::TreeModel::Path &, Gtk::TreeViewColumn *) -> void;
      auto on_will_expand(const Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Path &path) -> void;
      auto on_collapsed(const Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Path &path) -> void;
      auto on_realize() -> void;
      auto on_header_button_event(GdkEventButton *ev, int) -> bool;
      auto on_key_release(GdkEventKey *ev) -> bool;
      auto on_button_event(GdkEventButton *ev) -> bool;
      auto on_button_release(GdkEventButton *ev) -> bool;
      auto on_motion_notify(GdkEventMotion *ev) -> bool;
      bool on_draw_event(const ::Cairo::RefPtr< ::Cairo::Context> &context);
      auto on_enter_notify(GdkEventCrossing *ev) -> bool;
      auto on_leave_notify(GdkEventCrossing *ev) -> bool;

      auto slot_drag_end(const Glib::RefPtr<Gdk::DragContext> &context) -> void;
      auto slot_drag_failed(const Glib::RefPtr<Gdk::DragContext> &context, Gtk::DragResult result) -> bool;

      auto set_allow_sorting(bool flag) -> void;

      auto add_column(TreeColumnType type, const std::string &name, int initial_width, bool editable, bool attributed) -> int;
      auto end_columns() -> void;
      static auto create(TreeView *self, mforms::TreeOptions opt) -> bool;
      static auto add_column(TreeView *self, TreeColumnType type, const std::string &name, int width, bool editable,
                            bool attr) -> int;
      static auto end_columns(TreeView *self) -> void;
      static auto clear(TreeView *self) -> void;
      static auto root_node(TreeView *self) -> TreeNodeRef;
      static auto get_selected_node(TreeView *self) -> TreeNodeRef;
      static auto get_selection(TreeView *self) -> std::list<TreeNodeRef>;
      static auto set_selected(TreeView *self, TreeNodeRef node, bool flag) -> void;
      static auto scrollToNode(TreeView *self, TreeNodeRef node) -> void;
      static auto get_selection_mode(TreeView *self) -> TreeSelectionMode;
      static auto set_selection_mode(TreeView *self, TreeSelectionMode mode) -> void;
      static auto clear_selection(TreeView *self) -> void;

      static auto row_for_node(TreeView *self, TreeNodeRef node) -> int;
      static auto node_at_row(TreeView *self, int row) -> TreeNodeRef;
      static auto node_with_tag(TreeView *self, const std::string &tag) -> TreeNodeRef;

      static auto set_row_height(TreeView *self, int height) -> void;

      static auto set_allow_sorting(TreeView *self, bool flag) -> void;
      static auto freeze_refresh(TreeView *self, bool flag) -> void;
      static auto set_column_visible(TreeView *self, int column, bool flag) -> void;
      static auto get_column_visible(TreeView *self, int column) -> bool;
      static auto set_column_title(TreeView *self, int column, const std::string &title) -> void;

      static auto set_column_width(TreeView *self, int column, int width) -> void;
      static auto get_column_width(TreeView *self, int column) -> int;
      static auto node_at_position(TreeView *self, base::Point position) -> TreeNodeRef;

      auto to_list_iter(const Gtk::TreeModel::iterator &it) -> Gtk::TreeModel::iterator;
      auto to_list_path(const Gtk::TreeModel::Path &path) -> Gtk::TreeModel::Path;
      auto to_sort_iter(const Gtk::TreeModel::iterator &it) -> Gtk::TreeModel::iterator;
      auto to_sort_path(const Gtk::TreeModel::Path &path) -> Gtk::TreeModel::Path;

      void header_clicked(Gtk::TreeModelColumnBase *, Gtk::TreeViewColumn *);

      virtual auto set_back_color(const std::string &color) -> void;

    protected:
      auto get_drop_position() -> mforms::DropPosition;

    public:
      static auto init() -> void;

      auto index_for_column_attr(int i) -> int {
        return _columns.column_attr_index[i];
      }
      auto index_for_column(int i) -> int {
        return _columns.column_value_index[i];
      }
      auto tree_store() -> Glib::RefPtr<Gtk::TreeStore> {
        return _tree_store;
      }

      auto get_owner() -> TreeView *;
    };
  }
}

#endif
