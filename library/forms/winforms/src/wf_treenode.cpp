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

#include "wf_base.h"
#include "wf_view.h"
#include "wf_treeview.h"
#include "wf_treenode.h"

using namespace System::Drawing;
using namespace System::IO;
using namespace System::Windows::Forms;

using namespace Aga::Controls::Tree;

using namespace MySQL;
using namespace MySQL::Forms;
using namespace MySQL::Utilities;

//----------------- TreeViewNode -------------------------------------------------------------------

TreeViewNode::TreeViewNode() {
  myTag = NULL;
  data = NULL;
  attributes = new std::vector<mforms::TreeNodeTextAttributes>();
}

//--------------------------------------------------------------------------------------------------

TreeViewNode::~TreeViewNode() {
  delete attributes;
  delete myTag;
  if (data)
    data->release();
}

//--------------------------------------------------------------------------------------------------

auto TreeViewNode::DestroyDataRecursive() -> void {
  for (int i = 0; i < Nodes->Count; ++i)
    dynamic_cast<MySQL::Forms::TreeViewNode ^>(Nodes[i])->DestroyDataRecursive();
  Data = nullptr;
};

//--------------------------------------------------------------------------------------------------

auto TreeViewNode::MyTag::get() -> std::string {
  if (myTag != NULL)
    return *myTag;
  return "";
}

//--------------------------------------------------------------------------------------------------

auto TreeViewNode::MyTag::set(std::string s) -> void {
  if (myTag)
    delete myTag;
  myTag = new std::string(s);
}

//--------------------------------------------------------------------------------------------------

auto TreeViewNode::Data::get() -> mforms::TreeNodeData * {
  return data;
}

//--------------------------------------------------------------------------------------------------

auto TreeViewNode::Data::set(mforms::TreeNodeData *d) -> void {
  if (data != d) {
    if (data)
      data->release();
    data = d;
    if (data)
      data->retain();
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeViewNode::Caption::get(int index) -> String ^ {
  if (index < 0 || index >= captions.Count)
    return "";
  return captions[index] == nullptr ? "" : captions[index];
}

//--------------------------------------------------------------------------------------------------

void TreeViewNode::Caption::set(int index, String ^ newText) {
  while (index >= captions.Count)
    captions.Add(nullptr);
  captions[index] = newText;

  // The simple Text property of the base class is used to identify nodes (e.g. on sort) by the tree.
  // So we have to set text there too to make this work.
  newText = "";
  for each(String ^ caption in captions) newText += caption + ":";
  Text = newText;
}

//--------------------------------------------------------------------------------------------------

/**
 * Retrieves the caption of all columns in one string (elements are separated by tab).
 */
auto TreeViewNode::FullCaption::get() -> String ^ {
  String ^ result = "";
  for each(String ^ entry in captions) {
      if (result->Length > 0)
        result += "\t";
      result += entry == nullptr ? "" : entry;
    }
  return result;
}

//--------------------------------------------------------------------------------------------------

auto TreeViewNode::Icon::get(int index) -> Bitmap ^ {
  if (icons.ContainsKey(index))
    return icons[index];
  return nullptr;
}

//--------------------------------------------------------------------------------------------------

void TreeViewNode::Icon::set(int index, Bitmap ^ newIcon) {
  icons[index] = newIcon;
}

//--------------------------------------------------------------------------------------------------

auto TreeViewNode::Attributes::get(int index) -> mforms::TreeNodeTextAttributes {
  if (index < 0 || index >= (int)attributes->size())
    return mforms::TreeNodeTextAttributes();
  return (*attributes)[index];
}

//--------------------------------------------------------------------------------------------------

auto TreeViewNode::Attributes::set(int index, mforms::TreeNodeTextAttributes newAttributes) -> void {
  while (index >= (int)attributes->size())
    attributes->push_back(mforms::TreeNodeTextAttributes());

  (*attributes)[index] = newAttributes;
}

//----------------- TreeNodeWrapper ----------------------------------------------------------------

TreeNodeWrapper::TreeNodeWrapper(TreeViewWrapper *wrapper, TreeNodeAdv ^ node) {
  treeWrapper = wrapper;
  nativeNodeAdv = node;
  nativeNode = dynamic_cast<TreeViewNode ^>(node->Tag);
  isRoot = false;
  refCount = 0;
}

//--------------------------------------------------------------------------------------------------

TreeNodeWrapper::TreeNodeWrapper(TreeViewWrapper *wrapper) {
  treeWrapper = wrapper;
  nativeNodeAdv = treeWrapper->GetManagedObject<TreeViewAdv>()->Root;
  TreeModel ^ model = dynamic_cast<TreeModel ^>(treeWrapper->GetManagedObject<TreeViewAdv>()->Model);
  nativeNode = dynamic_cast<Node ^>(model->Root);
  isRoot = true;
  refCount = 0;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::release() -> void {
  refCount--;
  if (refCount == 0)
    delete this;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::retain() -> void {
  refCount++;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::node_index() -> int {
  if (isRoot)
    return -1;

  return nativeNode->Index;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::equals(const mforms::TreeNode &other) -> bool {
  const TreeNodeWrapper *oth = dynamic_cast<const TreeNodeWrapper *>(&other);
  if (oth)
    return (Node ^)oth->nativeNode == (Node ^)nativeNode;
  return false;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::is_valid() const -> bool {
  return !isRoot;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::level() const -> int {
  return nativeNodeAdv->Level;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_icon_path(int column, const std::string &icon) -> void {
  if (!isRoot && !icon.empty()) {
    bool invalidate = true;
    String ^ str_icon = CppStringToNative(icon);
    if (!TreeViewNode::iconStorage.ContainsKey(str_icon)) {
      String ^ path = CppStringToNative(mforms::App::get()->get_resource_path(icon));
      if (File::Exists(path))
        TreeViewNode::iconStorage[str_icon] = gcnew Bitmap(path);
      else
        invalidate = false;
    }

    if (invalidate) {
      TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
      node->Icon[column] = TreeViewNode::iconStorage[str_icon];
      treeWrapper->GetManagedObject<TreeViewAdv>()->Invalidate();
    }
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_selected(bool flag) -> void {
  nativeNodeAdv->IsSelected = flag;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::scrollToNode() -> void {
  TreeViewAdv ^ view = treeWrapper->GetManagedObject<TreeViewAdv>();
  if (view != nullptr) {
    view->ScrollTo(nativeNodeAdv);
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_attributes(int column, const mforms::TreeNodeTextAttributes &attrs) -> void {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    node->Attributes[column] = attrs;
    treeWrapper->GetManagedObject<TreeViewAdv>()->Invalidate();
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_string(int column, const std::string &value) -> void {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    node->Caption[column] = CppStringToNative(value);
    treeWrapper->node_value_set(column);
    treeWrapper->GetManagedObject<TreeViewAdv>()->Invalidate();
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_int(int column, int value) -> void {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    node->Caption[column] = Convert::ToString(value);
    treeWrapper->node_value_set(column);
    treeWrapper->GetManagedObject<TreeViewAdv>()->Invalidate();
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_long(int column, std::int64_t value) -> void {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    node->Caption[column] = Convert::ToString(value);
    treeWrapper->node_value_set(column);
    treeWrapper->GetManagedObject<TreeViewAdv>()->Invalidate();
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_bool(int column, bool value) -> void {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    node->Caption[column] = value ? "1" : "0";
    treeWrapper->node_value_set(column);
    treeWrapper->GetManagedObject<TreeViewAdv>()->Invalidate();
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_float(int column, double value) -> void {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    node->Caption[column] = Convert::ToString(value);
    treeWrapper->node_value_set(column);
    treeWrapper->GetManagedObject<TreeViewAdv>()->Invalidate();
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_string(int column) const -> std::string {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    return NativeToCppString(node->Caption[column]);
  }

  return "";
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_int(int column) const -> int {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    return Convert::ToInt32(node->Caption[column]);
  }

  return 0;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_bool(int column) const -> bool {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    return (node->Caption[column] == "Checked") || (node->Caption[column] == "1") ? true : false;
  }

  return false;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_long(int column) const -> std::int64_t {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    return Convert::ToInt64(node->Caption[column]);
  }

  return 0;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_float(int column) const -> double {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    return Convert::ToDouble(node->Caption[column]);
  }

  return 0;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::count() const -> int {
  return nativeNodeAdv->Children->Count;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_cached_icon(const std::string &icon_id) -> Bitmap ^ {
  Bitmap ^ icon;

  String ^ str_icon = CppStringToNative(icon_id);

  if (!TreeViewNode::iconStorage.ContainsKey(str_icon)) {
    String ^ path = CppStringToNative(mforms::App::get()->get_resource_path(icon_id));
    if (File::Exists(path))
      TreeViewNode::iconStorage[str_icon] = gcnew Bitmap(path);
  }

  if (TreeViewNode::iconStorage.ContainsKey(str_icon))
    icon = TreeViewNode::iconStorage[str_icon];

  return icon;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::add_node_collection(const mforms::TreeNodeCollectionSkeleton &nodes,
                                                                      int position) -> std::vector<mforms::TreeNodeRef> {
  std::vector<mforms::TreeNodeRef> result;

  if (!nodes.captions.empty()) {
    // The icon will be the same for all first level nodes.
    Bitmap ^ icon = nullptr;
    if (!nodes.icon.empty())
      icon = get_cached_icon(nodes.icon);

    std::vector<TreeNodeWrapper> parents;
    for (size_t i = 0; i < nodes.captions.size(); ++i) {
      MySQL::Forms::TreeViewNode ^ child = gcnew MySQL::Forms::TreeViewNode();
      TreeNodeAdv ^ treeNode;
      if (position < 0) {
        nativeNode->Nodes->Add(child);
        treeNode = nativeNodeAdv->Children[nativeNodeAdv->Children->Count - 1];
      } else {
        nativeNode->Nodes->Insert(position, child);
        treeNode = nativeNodeAdv->Children[position];
      }

      child->Caption[0] = CppStringToNative(nodes.captions[i]);
      child->Icon[0] = icon;

      TreeNodeWrapper *nodeWrapper = new TreeNodeWrapper(treeWrapper, treeNode);
      parents.push_back(*nodeWrapper);
      result.push_back(mforms::TreeNodeRef(nodeWrapper));
    }

    // Now add the child nodes.
    if (!nodes.children.empty())
      add_children_from_skeletons(parents, nodes.children);

    treeWrapper->GetManagedObject<TreeViewAdv>()->Invalidate();
  }

  return result;
}

//--------------------------------------------------------------------------------------------------

/**
 *	Adds the same list of child nodes (as given in the children vector) to each node in the parents vector.
 */
auto TreeNodeWrapper::add_children_from_skeletons(std::vector<TreeNodeWrapper> parents,
                                                  const std::vector<mforms::TreeNodeSkeleton> &children) -> void {
  for (size_t child_index = 0; child_index < children.size(); ++child_index) {
    String ^ caption = CppStringToNative(children[child_index].caption);
    Bitmap ^ icon = get_cached_icon(children[child_index].icon);
    std::string tag = children[child_index].tag;

    std::vector<TreeNodeWrapper> child_nodes;
    for (size_t parent_index = 0; parent_index < parents.size(); ++parent_index) {
      MySQL::Forms::TreeViewNode ^ child = gcnew MySQL::Forms::TreeViewNode();
      TreeNodeAdv ^ treeNode;
      parents[parent_index].nativeNode->Nodes->Add(child);
      treeNode =
        parents[parent_index].nativeNodeAdv->Children[parents[parent_index].nativeNodeAdv->Children->Count - 1];

      child->Caption[0] = caption;
      child->Icon[0] = icon;
      child->MyTag = tag;

      TreeNodeWrapper nodeWrapper(treeWrapper, treeNode);
      child_nodes.push_back(nodeWrapper);
    }

    // For each created node insert child nodes according to the children's child list.
    if (!children[child_index].children.empty())
      add_children_from_skeletons(child_nodes, children[child_index].children);
  }
}

//--------------------------------------------------------------------------------------------------

/**
 *	The compiler doesn't allow to assign that value from outside (another wrapper instance),
 *	so take a detour via an assignment function.
 */
void TreeNodeWrapper::node_changed(TreeNodeAdv ^ new_node) {
  nativeNodeAdv = new_node;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::insert_child(int index) -> mforms::TreeNodeRef {
  // Insert node into the model, which will implicitly create a tree node.
  MySQL::Forms::TreeViewNode ^ child = gcnew MySQL::Forms::TreeViewNode();
  TreeNodeAdv ^ treeNode;
  if (index < 0) {
    nativeNode->Nodes->Add(child);
    treeNode = nativeNodeAdv->Children[nativeNodeAdv->Children->Count - 1];
  } else {
    nativeNode->Nodes->Insert(index, child);
    treeNode = nativeNodeAdv->Children[index];
  }

  return mforms::TreeNodeRef(new TreeNodeWrapper(treeWrapper, treeNode));
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::insert_child(int index, const mforms::TreeNode &child) -> void {
  // Inserting an existing node only works if both belong to the same tree.
  TreeNodeWrapper *wrapper = (TreeNodeWrapper *)&child;
  if (treeWrapper != wrapper->treeWrapper)
    return;

  TreeNodeAdv ^ treeNode;
  if (index < 0) {
    nativeNode->Nodes->Add(wrapper->nativeNode);
    treeNode = nativeNodeAdv->Children[nativeNodeAdv->Children->Count - 1];
  } else {
    nativeNode->Nodes->Insert(index, wrapper->nativeNode);
    treeNode = nativeNodeAdv->Children[index];
  }

  wrapper->node_changed(treeNode);
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::remove_from_parent() -> void {
  if (!isRoot && nativeNode->Parent != nullptr) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    if (!node->MyTag.empty())
      treeWrapper->process_mapping(nullptr, node->MyTag);

    node->Parent->Nodes->Remove(nativeNode);
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::remove_children() -> void {
  for each(Node ^ child in nativeNode->Nodes) {
      TreeViewNode ^ node = dynamic_cast<TreeViewNode ^>(child);
      if (node != nullptr && !node->MyTag.empty())
        treeWrapper->process_mapping(nullptr, node->MyTag);
    }
  nativeNode->Nodes->Clear();
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::move_node(mforms::TreeNodeRef node, bool before) -> void {
  // Nodes must belong to the same tree.
  TreeNodeWrapper *wrapper = (TreeNodeWrapper *)node.ptr();
  if (treeWrapper != wrapper->treeWrapper)
    return;

  TreeViewNode ^ thisNode = (TreeViewNode ^)(Node ^)nativeNode;
  std::string tag = thisNode->MyTag;

  // Remove this node from parent and *after* that determine the target index, as this might
  // change when this and the other node have a common parent.
  remove_from_parent();
  int targetIndex = wrapper->nativeNode->Index;
  if (!before)
    ++targetIndex;

  wrapper->nativeNode->Parent->Nodes->Insert(targetIndex, nativeNode);

  // Update also the *Adv node as this has been recreated by the code above.
  nativeNodeAdv = wrapper->nativeNodeAdv->Parent->Children[targetIndex];
  treeWrapper->process_mapping(nativeNodeAdv, tag);
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_child(int index) const -> mforms::TreeNodeRef {
  TreeNodeAdv ^ child = nativeNodeAdv->Children[index];
  if (child != nullptr)
    return mforms::TreeNodeRef(new TreeNodeWrapper(treeWrapper, child));

  return mforms::TreeNodeRef();
}

//--------------------------------------------------------------------------------------------------

auto MySQL::Forms::TreeNodeWrapper::get_child_index(mforms::TreeNodeRef node) const -> int {
  const TreeNodeWrapper *wrapper = dynamic_cast<const TreeNodeWrapper *>(node.ptr());
  return nativeNode->Nodes->IndexOf(wrapper->nativeNode);
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_parent() const -> mforms::TreeNodeRef {
  if (!isRoot) {
    TreeNodeAdv ^ parent = nativeNodeAdv->Parent;
    if (parent != nullptr && parent->Index > -1) // The hidden root node has an index of -1;
      return mforms::TreeNodeRef(new TreeNodeWrapper(treeWrapper, parent));
    else
      return mforms::TreeNodeRef(new TreeNodeWrapper(treeWrapper));
  }

  return mforms::TreeNodeRef();
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::previous_sibling() const -> mforms::TreeNodeRef {
  if (isRoot || nativeNodeAdv->Index == 0)
    return mforms::TreeNodeRef();

  TreeNodeAdv ^ node = nativeNodeAdv->Parent->Children[nativeNodeAdv->Index - 1];
  return mforms::TreeNodeRef(new TreeNodeWrapper(treeWrapper, node));
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::next_sibling() const -> mforms::TreeNodeRef {
  if (isRoot || nativeNodeAdv->Index == nativeNodeAdv->Parent->Children->Count - 1)
    return mforms::TreeNodeRef();

  TreeNodeAdv ^ node = nativeNodeAdv->Parent->Children[nativeNodeAdv->Index + 1];
  return mforms::TreeNodeRef(new TreeNodeWrapper(treeWrapper, node));
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::expand() -> void {
  if (!isRoot)
    get_parent()->expand(); // Recursively expand all parent nodes.
  nativeNodeAdv->Expand();
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::collapse() -> void {
  nativeNodeAdv->Collapse();
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::is_expanded() -> bool {
  return nativeNodeAdv->IsExpanded;
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_tag(const std::string &tag) -> void {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    if (!node->MyTag.empty())
      treeWrapper->process_mapping(nullptr, node->MyTag);
    node->MyTag = tag;
    treeWrapper->process_mapping(nativeNodeAdv, node->MyTag);
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_tag() const -> std::string {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    return node->MyTag;
  }

  return "";
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::set_data(mforms::TreeNodeData *data) -> void {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    node->Data = data;
  }
}

//--------------------------------------------------------------------------------------------------

auto TreeNodeWrapper::get_data() const -> mforms::TreeNodeData * {
  if (!isRoot) {
    TreeViewNode ^ node = (TreeViewNode ^)(Node ^)nativeNode;
    return node->Data;
  }

  return NULL;
}

//--------------------------------------------------------------------------------------------------
