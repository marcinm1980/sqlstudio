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

#include "mforms/mforms.h"

using namespace mforms;

TreeNodeSkeleton::TreeNodeSkeleton(const std::string &strcaption, const std::string &stricon,
                                   const std::string &strtag) {
  caption = strcaption;
  icon = stricon;
  tag = strtag;
}

TreeNodeCollectionSkeleton::TreeNodeCollectionSkeleton(const std::string &stricon) {
  icon = stricon;
}

TreeNodeRef::TreeNodeRef(TreeNode *anode) : node(anode) {
  if (node)
    node->retain();
}

TreeNodeRef::TreeNodeRef(const TreeNodeRef &other) {
  node = other.node;
  if (node)
    node->retain();
}

TreeNodeRef::~TreeNodeRef() {
  if (node)
    node->release();
}

TreeNodeRef &TreeNodeRef::operator=(const TreeNodeRef &other) {
  if (node != other.node) {
    if (other.node)
      other.node->retain();
    if (node)
      node->release();
    node = other.node;
  }
  return *this;
}

TreeNode *TreeNodeRef::operator->() const {
  if (!node)
    throw std::logic_error("Attempt to dereference NULL TreeNode");
  return node;
}

TreeNode *TreeNodeRef::operator->() {
  if (!node)
    throw std::logic_error("Attempt to dereference NULL TreeNode");
  return node;
}

bool TreeNodeRef::operator==(const TreeNodeRef &other) const {
  if (node == other.node)
    return true;

  if (other.node && node)
    return node->equals(*other.node);

  return false;
}

bool TreeNodeRef::operator!=(const TreeNodeRef &other) const {
  if (node == other.node)
    return false;

  if (other.node && node)
    return !node->equals(*other.node);

  return true;
}

auto mforms::TreeNodeRef::is_valid() -> bool {
  return node != NULL;
}

// -------------------------------------------------------------------------------------------------

auto TreeNode::remove_children() -> void {
  if (is_valid()) {
    for (int i = count() - 1; i >= 0; --i) {
      TreeNodeRef child(get_child(i));
      if (child)
        child->remove_from_parent();
    }
  }
}

auto TreeNode::find_child_with_tag(const std::string &tag) -> TreeNodeRef {
  for (int c = count(), i = 0; i < c; i++) {
    TreeNodeRef child(get_child(i));
    if (child && child->get_tag() == tag)
      return child;
  }
  return TreeNodeRef();
}

//--------------------------------------------------------------------------------------------------

auto TreeNode::toggle() -> void {
  if (can_expand()) {
    if (!is_expanded())
      expand();
    else
      collapse();
  }
}

//----------------- TreeView -----------------------------------------------------------------------

TreeView::TreeView(TreeOptions options) // yep
  : _context_menu(0),
    _header_menu(0),
    _update_count(0),
    _clicked_header_column(0),
    _end_column_called(false) {
  _treeview_impl = &ControlFactory::get_instance()->_treeview_impl;
  _index_on_tag = (options & TreeIndexOnTag) ? true : false;

  _treeview_impl->create(this, options);
}

//--------------------------------------------------------------------------------------------------

TreeView::~TreeView() {
  _update_count++; // Avoid any callbacks/events.
}

//--------------------------------------------------------------------------------------------------

auto TreeView::set_context_menu(ContextMenu *menu) -> void {
  _context_menu = menu;
}

//--------------------------------------------------------------------------------------------------

auto TreeView::set_header_menu(ContextMenu *menu) -> void {
  _header_menu = menu;
}

//--------------------------------------------------------------------------------------------------

auto TreeView::header_clicked(int column) -> void {
  _clicked_header_column = column;
}

//--------------------------------------------------------------------------------------------------

auto TreeView::add_column(TreeColumnType type, const std::string &name, int initial_width, bool editable,
                         bool attributed) -> int {
  if (_end_column_called)
    throw std::logic_error("Add column called, after end_columns has been called");
  _column_types.push_back(type);
#if defined(_MSC_VER)
  return _treeview_impl->add_column(this, type, name, initial_width, editable);
#else
  return _treeview_impl->add_column(this, type, name, initial_width, editable, attributed);
#endif
}

auto TreeView::set_column_title(int column, const std::string &title) -> void {
  _treeview_impl->set_column_title(this, column, title);
}

auto TreeView::get_column_type(int column) -> TreeColumnType {
  if (column >= 0 && column < (int)_column_types.size())
    return _column_types[column];
  return StringColumnType;
}

auto TreeView::set_allow_sorting(bool flag) -> void {
  if (!_end_column_called)
    throw std::logic_error("TreeView::set_allow_sorting() must be called after end_columns()");
  _treeview_impl->set_allow_sorting(this, flag);
}

auto TreeView::end_columns() -> void {
  _end_column_called = true;
  _treeview_impl->end_columns(this);
}

auto TreeView::clear() -> void {
  _treeview_impl->clear(this);
}

auto TreeView::root_node() -> TreeNodeRef {
  return _treeview_impl->root_node(this);
}

auto TreeView::add_node() -> TreeNodeRef {
  return root_node()->add_child();
}

auto TreeView::get_selected_node() -> TreeNodeRef {
  return _treeview_impl->get_selected_node(this);
}

auto TreeView::set_selection_mode(TreeSelectionMode mode) -> void {
  _treeview_impl->set_selection_mode(this, mode);
}

auto TreeView::get_selection_mode() -> TreeSelectionMode {
  return _treeview_impl->get_selection_mode(this);
}

auto TreeView::get_selected_row() -> int {
  TreeNodeRef node(get_selected_node());
  return row_for_node(node);
}

auto TreeView::get_selection() -> std::list<TreeNodeRef> {
  return _treeview_impl->get_selection(this);
}

auto TreeView::clear_selection() -> void {
  _treeview_impl->clear_selection(this);
}

auto TreeView::select_node(TreeNodeRef node) -> void {
  if (node.is_valid()) {
    _update_count++;
    clear_selection();
    _treeview_impl->set_selected(this, node, true);
    _update_count--;
  }
}

auto TreeView::set_node_selected(TreeNodeRef node, bool flag) -> void {
  if (node.is_valid()) {
    _update_count++;
    _treeview_impl->set_selected(this, node, flag);
    _update_count--;
  }
}

auto TreeView::scrollToNode(TreeNodeRef node) -> void {
  _treeview_impl->scrollToNode(this, node);
}

auto TreeView::set_row_height(int height) -> void {
  if (_treeview_impl->set_row_height)
    _treeview_impl->set_row_height(this, height);
}

//--------------------------------------------------------------------------------------------------

auto TreeView::set_cell_edit_handler(const std::function<void(TreeNodeRef, int, std::string)> &handler) -> void {
  _cell_edited = handler;
}

//--------------------------------------------------------------------------------------------------

auto TreeView::cell_edited(TreeNodeRef row, int column, const std::string &value) -> bool {
  if (_cell_edited) {
    _cell_edited(row, column, value);
    return false;
  }
  return true;
}

//--------------------------------------------------------------------------------------------------

auto TreeView::get_drag_data(DragDetails &details, void **data, std::string &format) -> bool {
  return false;
}

//--------------------------------------------------------------------------------------------------

auto TreeView::drag_finished(DragOperation operation) -> void {
}

//--------------------------------------------------------------------------------------------------

auto TreeView::column_resized(int column) -> void {
  _signal_column_resized(column);
}

//--------------------------------------------------------------------------------------------------

auto TreeView::changed() -> void {
  if (_update_count == 0)
    _signal_changed();
}

//--------------------------------------------------------------------------------------------------

auto TreeView::node_activated(TreeNodeRef row, int column) -> void {
  _signal_activated(row, column);
}

//--------------------------------------------------------------------------------------------------

auto TreeView::set_row_overlay_handler(
  const std::function<std::vector<std::string>(TreeNodeRef)> &overlay_icons_for_node) -> void {
  _overlay_icons_for_node = overlay_icons_for_node;
}

//--------------------------------------------------------------------------------------------------

auto TreeView::overlay_icons_for_node(TreeNodeRef row) -> std::vector<std::string> {
  if (_overlay_icons_for_node)
    return _overlay_icons_for_node(row);
  return std::vector<std::string>();
}

//--------------------------------------------------------------------------------------------------

auto TreeView::overlay_icon_for_node_clicked(TreeNodeRef row, int index) -> void {
  node_activated(row, -(index + 1));
}

//--------------------------------------------------------------------------------------------------

/**
 * Descendants can override this method to indicate expandability depending on other information.
 */
auto TreeView::can_expand(TreeNodeRef row) -> bool {
  return row->count() > 0;
}

//--------------------------------------------------------------------------------------------------

auto TreeView::expand_toggle(TreeNodeRef row, bool expanded) -> void {
  _signal_expand_toggle(row, expanded);
}

//--------------------------------------------------------------------------------------------------

auto TreeView::freeze_refresh() -> void {
  _treeview_impl->freeze_refresh(this, true);
}

auto TreeView::thaw_refresh() -> void {
  _treeview_impl->freeze_refresh(this, false);
}

auto TreeView::row_for_node(TreeNodeRef node) -> int {
  return _treeview_impl->row_for_node(this, node);
}

auto TreeView::node_at_row(int row) -> TreeNodeRef {
  return _treeview_impl->node_at_row(this, row);
}

auto mforms::TreeView::node_at_position(base::Point position) -> mforms::TreeNodeRef {
  return _treeview_impl->node_at_position(this, position);
}

auto TreeView::node_with_tag(const std::string &tag) -> TreeNodeRef {
  if (!_index_on_tag)
    throw std::logic_error("Tree was not created with TreeIndexOnTag");

  return _treeview_impl->node_with_tag(this, tag);
}

auto TreeView::set_column_visible(int column, bool flag) -> void {
  if (_treeview_impl->set_column_visible)
    _treeview_impl->set_column_visible(this, column, flag);
}

auto TreeView::get_column_visible(int column) -> bool {
  if (_treeview_impl->get_column_visible)
    return _treeview_impl->get_column_visible(this, column);
  return true;
}

auto TreeView::set_column_width(int column, int width) -> void {
  if (_treeview_impl->set_column_width)
    _treeview_impl->set_column_width(this, column, width);
}

auto TreeView::get_column_width(int column) -> int {
  if (_treeview_impl->get_column_width)
    return _treeview_impl->get_column_width(this, column);
  return 0;
}

auto TreeView::parse_string_with_unit(const char *s) -> double {
  char *end = NULL;
  double value = strtod(s, &end);

  if (*end == ' ')
    ++end;

  switch (*end) {
    case 'p': // pico
      value /= 1000000000000.0;
      break;
    case 'n': // nano
      value /= 1000000000.0;
      break;
    case 'u': // micro
      value /= 1000000.0;
      break;
    case 'm': // milli
      value /= 1000.0;
      break;

    case 'h': // special case for hours
      value *= 3600.0;
      break;

    case 'K': // kilo
    case 'k':
      if (*(end + 1) == 'i')
        value *= 1024.0;
      else
        value *= 1000.0;
      break;
    case 'M': // mega
      if (*(end + 1) == 'i')
        value *= 1048576.0;
      else
        value *= 1000000.0;
      break;
    case 'G': // giga
      if (*(end + 1) == 'i')
        value *= 1073741824.0;
      else
        value *= 1000000000.0;
      break;
    case 'T': // tera
      if (*(end + 1) == 'i')
        value *= 1099511627776.0;
      else
        value *= 1000000000000.0;
      break;
    case 'P': // peta
      if (*(end + 1) == 'i')
        value *= 1125899906842624.0;
      else
        value *= 1000000000000000.0;
      break;
  }
  return value;
}

auto TreeView::BeginUpdate() -> void {
  if (_treeview_impl->BeginUpdate)
    _treeview_impl->BeginUpdate(this);
}

auto TreeView::EndUpdate() -> void {
  if (_treeview_impl->EndUpdate)
    _treeview_impl->EndUpdate(this);
}
