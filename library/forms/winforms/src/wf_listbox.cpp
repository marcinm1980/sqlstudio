/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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
#include "wf_listbox.h"

using namespace System::Windows::Forms;

using namespace MySQL;
using namespace MySQL::Forms;

//----------------- MformsListBox ------------------------------------------------------------------

ref class MformsListBox : ListBox {
public:
  virtual void OnSelectedIndexChanged(System::EventArgs ^ args) override {
    __super ::OnSelectedIndexChanged(args);

    mforms::ListBox *backend = ListBoxWrapper::GetBackend<mforms::ListBox>(this);
    if (backend != NULL)
      backend->selection_changed();
  }
};

//----------------- ListBoxWrapper -----------------------------------------------------------------

ListBoxWrapper::ListBoxWrapper(mforms::ListBox *backend) : ViewWrapper(backend) {
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::create(mforms::ListBox *backend, bool multi_select) -> bool {
  ListBoxWrapper *wrapper = new ListBoxWrapper(backend);

  MformsListBox ^ listbox = ListBoxWrapper::Create<MformsListBox>(backend, wrapper);
  listbox->Size = Drawing::Size(100, 100);
  int verticalMinimumSize = 50;
  System::Drawing::Graphics ^ graph = listbox->CreateGraphics();
  if (graph != nullptr)
    verticalMinimumSize = (int)graph->MeasureString("Some text", listbox->Font).Height;
  // ListBox::IntegralHeight is set to true by default then we have to increase minimum height by one
  // to get tree visible rows otherwise we will see only two rows
  listbox->MinimumSize = Drawing::Size(50, verticalMinimumSize * 3 + 1);
  if (multi_select)
    listbox->SelectionMode = SelectionMode::MultiExtended;
  return true;
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::clear(mforms::ListBox *backend) -> void {
  ListBoxWrapper::GetManagedObject<ListBox>(backend)->Items->Clear();
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::set_heading(mforms::ListBox *backend, const std::string &text) -> void {
  // TODO: what's the heading of a listbox?
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::add_items(mforms::ListBox *backend, const std::list<std::string> &items) -> void {
  ListBox ^ listbox = ListBoxWrapper::GetManagedObject<ListBox>(backend);
  listbox->BeginUpdate();
  try {
    for each(std::string entry in items) listbox->Items->Add(CppStringToNative(entry));
  } finally {
    listbox->EndUpdate();
  }
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::add_item(mforms::ListBox *backend, const std::string &item) -> size_t {
  return ListBoxWrapper::GetManagedObject<ListBox>(backend)->Items->Add(CppStringToNative(item));
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::remove_indexes(mforms::ListBox *backend, const std::vector<size_t> &indices) -> void {
  ListBox ^ listbox = ListBoxWrapper::GetManagedObject<ListBox>(backend);
  listbox->BeginUpdate();
  try {
    for (std::vector<size_t>::const_reverse_iterator iterator = indices.rbegin(); iterator != indices.rend();
         ++iterator) {
      listbox->Items->RemoveAt((int)*iterator);
    }
  } finally {
    listbox->EndUpdate();
  }
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::remove_index(mforms::ListBox *backend, size_t index) -> void {
  return ListBoxWrapper::GetManagedObject<ListBox>(backend)->Items->RemoveAt((int)index);
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::get_text(mforms::ListBox *backend) -> std::string {
  ListBox ^ listbox = ListBoxWrapper::GetManagedObject<ListBox>(backend);
  if (listbox->SelectedIndex < 0)
    return "";

  return NativeToCppString(listbox->SelectedItem->ToString());
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::set_index(mforms::ListBox *backend, ssize_t index) -> void {
  ListBoxWrapper::GetManagedObject<ListBox>(backend)->SelectedIndex = (int)index;
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::get_index(mforms::ListBox *backend) -> ssize_t {
  return ListBoxWrapper::GetManagedObject<ListBox>(backend)->SelectedIndex;
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::get_selected_indices(mforms::ListBox *backend) -> std::vector<size_t> {
  std::vector<size_t> result;
  ListBox ^ listbox = ListBoxWrapper::GetManagedObject<ListBox>(backend);
  for each(int index in listbox->SelectedIndices) // It's an array of Int32. Don't change to size_t.
      result.push_back(index);
  return result;
}

//--------------------------------------------------------------------------------------------------

auto ListBoxWrapper::get_count(mforms::ListBox *backend) -> size_t {
  ListBox ^ listbox = ListBoxWrapper::GetManagedObject<ListBox>(backend);
  return listbox->Items->Count;
}

//------------------------------------------------------------------------------

auto ListBoxWrapper::get_string_value_from_index(mforms::ListBox *backend, size_t index) -> std::string {
  ListBox ^ listbox = ListBoxWrapper::GetManagedObject<ListBox>(backend);
  if ((size_t)listbox->Items->Count < index)
    return "";

  return NativeToCppString(listbox->Items[index]->ToString());
}

//------------------------------------------------------------------------------

auto ListBoxWrapper::init() -> void {
  mforms::ControlFactory *f = mforms::ControlFactory::get_instance();

  f->_listbox_impl.create = &ListBoxWrapper::create;
  f->_listbox_impl.clear = &ListBoxWrapper::clear;
  f->_listbox_impl.set_heading = &ListBoxWrapper::set_heading;
  f->_listbox_impl.add_items = &ListBoxWrapper::add_items;
  f->_listbox_impl.add_item = &ListBoxWrapper::add_item;

  f->_listbox_impl.remove_index = &ListBoxWrapper::remove_index;
  f->_listbox_impl.remove_indexes = &ListBoxWrapper::remove_indexes;

  f->_listbox_impl.get_text = &ListBoxWrapper::get_text;
  f->_listbox_impl.set_index = &ListBoxWrapper::set_index;
  f->_listbox_impl.get_index = &ListBoxWrapper::get_index;
  f->_listbox_impl.get_selected_indices = &ListBoxWrapper::get_selected_indices;
  f->_listbox_impl.get_count = &ListBoxWrapper::get_count;
  f->_listbox_impl.get_string_value_from_index = &ListBoxWrapper::get_string_value_from_index;
}

//--------------------------------------------------------------------------------------------------
