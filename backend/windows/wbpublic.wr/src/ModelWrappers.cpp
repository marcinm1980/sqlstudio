/*
 * Copyright (c) 2011, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "ConvUtils.h"
#include "GrtWrapper.h"
#include "GrtTemplates.h"
#include "DelegateWrapper.h"

#include "ModelWrappers.h"

using namespace MySQL::Grt;
using namespace MySQL::MySqlStudio;

//--------------------------------------------------------------------------------------------------

NodeIdWrapper::NodeIdWrapper(const bec::NodeId *inn) : inner(new bec::NodeId(*inn)) {
}

//--------------------------------------------------------------------------------------------------

NodeIdWrapper::NodeIdWrapper() : inner(new ::bec::NodeId()) {
}

//--------------------------------------------------------------------------------------------------

NodeIdWrapper::NodeIdWrapper(int index) : inner(new ::bec::NodeId(index)) {
}

//--------------------------------------------------------------------------------------------------

NodeIdWrapper::NodeIdWrapper(String ^ str) : inner(new ::bec::NodeId(NativeToCppString(str))) {
}

//--------------------------------------------------------------------------------------------------

NodeIdWrapper::~NodeIdWrapper() {
  delete inner;
}

//--------------------------------------------------------------------------------------------------

auto NodeIdWrapper::get_unmanaged_object() -> ::bec::NodeId * {
  return inner;
}

//--------------------------------------------------------------------------------------------------

bool NodeIdWrapper::operator==(NodeIdWrapper ^ node) {
  return inner->operator==(*node->inner);
}

//--------------------------------------------------------------------------------------------------

bool NodeIdWrapper::equals(NodeIdWrapper ^ node) {
  return inner->operator==(*node->inner);
}

//--------------------------------------------------------------------------------------------------

int NodeIdWrapper::operator[](int i) {
  return (int)inner->operator[](i);
}

//--------------------------------------------------------------------------------------------------

auto NodeIdWrapper::get_by_index(int i) -> int {
  return (int)inner->operator[](i);
}

//--------------------------------------------------------------------------------------------------

auto NodeIdWrapper::depth() -> int {
  return (int)inner->depth();
}

//--------------------------------------------------------------------------------------------------

auto NodeIdWrapper::end() -> int {
  return (int)inner->end();
}

//--------------------------------------------------------------------------------------------------

auto NodeIdWrapper::previous() -> bool {
  return inner->previous();
}

//--------------------------------------------------------------------------------------------------

auto NodeIdWrapper::next() -> bool {
  return inner->next();
}

//--------------------------------------------------------------------------------------------------

auto NodeIdWrapper::append(int i) -> NodeIdWrapper ^ {
  inner->append(i);
  return this;
}

//--------------------------------------------------------------------------------------------------

auto NodeIdWrapper::is_valid() -> bool {
  return inner->is_valid();
}

//--------------------------------------------------------------------------------------------------

auto NodeIdWrapper::toString() -> String ^ {
  return CppStringToNative(inner->toString());
}

//----------------- ListModel ----------------------------------------------------------------------

ListModelWrapper::ListModelWrapper(bec::ListModel *inn) : inner(inn) {
  native_connections = new std::vector<boost::signals2::connection>();
}

//--------------------------------------------------------------------------------------------------

ListModelWrapper::~ListModelWrapper() {
  if (!native_connections)
    return;

  for each(TreeRefreshSlot ^ handler in tree_refresh_handlers) delete handler;
  tree_refresh_handlers.Clear();

  delete native_connections;
  native_connections = NULL;
}

//--------------------------------------------------------------------------------------------------

auto ListModelWrapper::is_valid() -> bool {
  return inner != NULL;
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::equals(ListModelWrapper ^ other) {
  return (inner == other->inner);
}

//--------------------------------------------------------------------------------------------------

auto ListModelWrapper::count() -> int {
  return (int)inner->count();
}

//--------------------------------------------------------------------------------------------------

auto ListModelWrapper::get_node(int index) -> NodeIdWrapper ^ {
  return gcnew NodeIdWrapper(&inner->get_node(index));
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::get_field(NodeIdWrapper ^ node, int column, [Out] String ^ % value) {
  std::string str;
  bool retval = inner->get_field(*node->get_unmanaged_object(), column, str);
  if (retval && str.length() > 0)
    value = CppStringToNative(str);
  else
    value = "";
  return retval;
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::get_field(NodeIdWrapper ^ node, int column, [Out] int % value) {
  ssize_t v;
  bool retval = inner->get_field(*node->get_unmanaged_object(), column, v);
  value = (int)v;
  return retval;
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::get_field(NodeIdWrapper ^ node, int column, [Out] double % value) {
  double v;
  bool retval = inner->get_field(*node->get_unmanaged_object(), column, v);
  value = v;
  return retval;
}

//--------------------------------------------------------------------------------------------------

String ^ ListModelWrapper::get_field_description(NodeIdWrapper ^ node, int column) {
  return CppStringToNative(inner->get_field_description(*node->get_unmanaged_object(), column));
}

//--------------------------------------------------------------------------------------------------

IconId ListModelWrapper::get_field_icon(NodeIdWrapper ^ node, int column, IconSize size) {
  return (int)inner->get_field_icon(*node->get_unmanaged_object(), column, (bec::IconSize)size);
}

//--------------------------------------------------------------------------------------------------

GrtValue ^ ListModelWrapper::get_grt_value(NodeIdWrapper ^ node, int column) {
  return gcnew GrtValue(inner->get_grt_value(*node->get_unmanaged_object(), column));
}

//--------------------------------------------------------------------------------------------------

auto ListModelWrapper::refresh() -> void {
  inner->refresh();
}

//--------------------------------------------------------------------------------------------------

auto ListModelWrapper::reset() -> void {
  inner->reset();
}

//--------------------------------------------------------------------------------------------------

GrtValueType ^ ListModelWrapper::get_field_type(NodeIdWrapper ^ node, int column) {
  return static_cast<GrtValueType>(inner->get_field_type(*node->get_unmanaged_object(), column));
}

//--------------------------------------------------------------------------------------------------

// For editable lists only.
bool ListModelWrapper::set_field(NodeIdWrapper ^ node, int column, String ^ value) {
  return inner->set_field(*node->get_unmanaged_object(), column, NativeToCppString(value));
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::set_field(NodeIdWrapper ^ node, int column, double value) {
  return inner->set_field(*node->get_unmanaged_object(), column, value);
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::set_field(NodeIdWrapper ^ node, int column, int value) {
  return inner->set_field(*node->get_unmanaged_object(), column, (ssize_t)value);
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::set_convert_field(NodeIdWrapper ^ node, int column, String ^ value) {
  return inner->set_convert_field(*node->get_unmanaged_object(), column, NativeToCppString(value));
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::activate_node(NodeIdWrapper ^ node) {
  return inner->activate_node(*node->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

std::vector<bec::NodeId> ListModelWrapper::convert_node_list(List<NodeIdWrapper ^> ^ nodes) {
  return ObjectListToCppVector<NodeIdWrapper, bec::NodeId>(nodes);
}

//--------------------------------------------------------------------------------------------------

List<MySQL::Base::MenuItem ^> ^ ListModelWrapper::get_popup_items_for_nodes(List<NodeIdWrapper ^> ^ nodes) {
  bec::MenuItemList item_list = inner->get_popup_items_for_nodes(convert_node_list(nodes));
  List<MySQL::Base::MenuItem ^> ^ items = gcnew List<MySQL::Base::MenuItem ^>();

  for (bec::MenuItemList::const_iterator iter = item_list.begin(); iter != item_list.end(); ++iter)
    items->Add(gcnew MySQL::Base::MenuItem(*iter));

  return items;
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::activate_popup_item_for_nodes(String ^ name, List<NodeIdWrapper ^> ^ nodes) {
  try {
    return inner->activate_popup_item_for_nodes(NativeToCppString(name), convert_node_list(nodes));
  } catch (std::exception &e) {
    Logger::LogError("model wrapper", String::Format("Internal exception occured: {0}", CppStringToNative(e.what())));
    throw;
  }
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::delete_node(NodeIdWrapper ^ node) {
  return inner->delete_node(*node->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

void ListModelWrapper::reorder(NodeIdWrapper ^ node, int index) {
  inner->reorder(*node->get_unmanaged_object(), index);
}

//--------------------------------------------------------------------------------------------------

void ListModelWrapper::reorder_up(NodeIdWrapper ^ node) {
  inner->reorder_up(*node->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

void ListModelWrapper::reorder_down(NodeIdWrapper ^ node) {
  inner->reorder_down(*node->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

bool ListModelWrapper::is_editable(NodeIdWrapper ^ node) {
  return inner->is_editable(*node->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

void ListModelWrapper::add_tree_refresh_handler(TreeRefreshSlot::ManagedDelegate ^ slot) {
  TreeRefreshSlot ^ tree_refresh_handler = gcnew TreeRefreshSlot(slot);
  boost::signals2::connection connection = inner->tree_changed_signal()->connect(tree_refresh_handler->get_slot());
  tree_refresh_handlers.Add(tree_refresh_handler);
  native_connections->push_back(connection);
}

//--------------------------------------------------------------------------------------------------

void ListModelWrapper::remove_tree_refresh_handler(TreeRefreshSlot::ManagedDelegate ^ slot) {
  int i = 0;
  for each(TreeRefreshSlot ^ handler in tree_refresh_handlers) {
      if (handler->wraps_delegate(slot)) {
        tree_refresh_handlers.Remove(handler);

        std::vector<boost::signals2::connection>::iterator iterator = native_connections->begin() + i;
        iterator->disconnect();
        native_connections->erase(iterator);
        break;
      }
      i++;
    }
}

//--------------------------------------------------------------------------------------------------

TreeModelWrapper::TreeModelWrapper(::bec::TreeModel *inn) : ListModelWrapper(inn) {
}

//--------------------------------------------------------------------------------------------------

auto TreeModelWrapper::get_unmanaged_object() -> ::bec::TreeModel * {
  return static_cast<::bec::TreeModel *>(inner);
}

//--------------------------------------------------------------------------------------------------

auto TreeModelWrapper::get_root() -> NodeIdWrapper ^ {
  return gcnew NodeIdWrapper(&get_unmanaged_object()->get_root());
}

//--------------------------------------------------------------------------------------------------

int TreeModelWrapper::get_node_depth(NodeIdWrapper ^ node) {
  return (int)get_unmanaged_object()->get_node_depth(*node->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

NodeIdWrapper ^ TreeModelWrapper::get_parent(NodeIdWrapper ^ node) {
  return gcnew NodeIdWrapper(&get_unmanaged_object()->get_parent(*node->get_unmanaged_object()));
}

//--------------------------------------------------------------------------------------------------

int TreeModelWrapper::count_children(NodeIdWrapper ^ parent) {
  return (int)get_unmanaged_object()->count_children(*parent->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

NodeIdWrapper ^ TreeModelWrapper::get_child(NodeIdWrapper ^ parent, int index) {
  return gcnew NodeIdWrapper(&get_unmanaged_object()->get_child(*parent->get_unmanaged_object(), index));
}

//--------------------------------------------------------------------------------------------------

bool TreeModelWrapper::expand_node(NodeIdWrapper ^ node) {
  return get_unmanaged_object()->expand_node(*node->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

void TreeModelWrapper::collapse_node(NodeIdWrapper ^ node) {
  return get_unmanaged_object()->collapse_node(*node->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

bool TreeModelWrapper::is_expandable(NodeIdWrapper ^ node) {
  return get_unmanaged_object()->is_expandable(*node->get_unmanaged_object());
}

//--------------------------------------------------------------------------------------------------

GridModelWrapper::GridModelWrapper(::bec::GridModel *inn) : ListModelWrapper(inn) {
}

//--------------------------------------------------------------------------------------------------

auto GridModelWrapper::get_unmanaged_object() -> ::bec::GridModel * {
  return static_cast<::bec::GridModel *>(inner);
}

//--------------------------------------------------------------------------------------------------

auto GridModelWrapper::get_column_count() -> int {
  return (int)get_unmanaged_object()->get_column_count();
}

//--------------------------------------------------------------------------------------------------

auto GridModelWrapper::get_column_caption(int column) -> String ^ {
  return CppStringToNative(get_unmanaged_object()->get_column_caption(column));
}

//--------------------------------------------------------------------------------------------------

auto GridModelWrapper::get_column_type(int column) -> GridModelWrapper::ColumnType {
  return (GridModelWrapper::ColumnType)get_unmanaged_object()->get_column_type(column);
}

//--------------------------------------------------------------------------------------------------

auto GridModelWrapper::is_readonly() -> bool {
  return get_unmanaged_object()->is_readonly();
}

//--------------------------------------------------------------------------------------------------

auto GridModelWrapper::readonly_reason() -> String ^ {
  return CppStringToNative(get_unmanaged_object()->readonly_reason());
}

//--------------------------------------------------------------------------------------------------

bool GridModelWrapper::is_field_null(NodeIdWrapper ^ node, int column) {
  return get_unmanaged_object()->is_field_null(*node->get_unmanaged_object(), column);
}

//--------------------------------------------------------------------------------------------------

bool GridModelWrapper::set_field_null(NodeIdWrapper ^ node, int column) {
  return get_unmanaged_object()->set_field_null(*node->get_unmanaged_object(), column);
}

//--------------------------------------------------------------------------------------------------

bool GridModelWrapper::get_field_repr(NodeIdWrapper ^ node, int column, [Out] String ^ % value) {
  std::string str;
  bool retval = get_unmanaged_object()->get_field_repr(*node->get_unmanaged_object(), column, str);
  value = CppStringToNative(str);
  return retval;
}

//--------------------------------------------------------------------------------------------------

auto GridModelWrapper::set_edited_field(int row_index, int col_index) -> void {
  get_unmanaged_object()->set_edited_field(row_index, col_index);
}

//--------------------------------------------------------------------------------------------------

void GridModelWrapper::sort_columns([Out] List<int> ^ % indexes, [Out] List<int> ^ % orders) {
  indexes = gcnew List<int>();
  orders = gcnew List<int>();
  bec::GridModel::SortColumns sort_columns = get_unmanaged_object()->sort_columns();
  for (::bec::GridModel::SortColumns::const_iterator i = sort_columns.begin(), i_end = sort_columns.end(); i != i_end;
       ++i) {
    indexes->Add((int)i->first);
    orders->Add(i->second);
  }
}

//--------------------------------------------------------------------------------------------------
