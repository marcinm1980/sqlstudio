/*
 * Copyright (c) 2010, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "base/ui_form.h"

#include "ConvUtils.h"
#include "UIForm.h"

using namespace System;
using namespace System::Collections::Generic;

using namespace MySQL::Base;

//----------------- MenuItem -----------------------------------------------------------------------

MenuItem::MenuItem(const ::bec::MenuItem& item)
  : caption(CppStringToNative(item.caption)),
    shortcut(CppStringToNative(item.shortcut)),
    internalName(CppStringToNative(item.internalName)),
    type((MenuItemType)item.type),
    enabled(item.enabled),
    checked(item.checked) {
  subitems = gcnew List<MenuItem ^>();
  for (bec::MenuItemList::const_iterator iterator = item.subitems.begin(); iterator != item.subitems.end(); ++iterator)
    subitems->Add(gcnew MenuItem(*iterator));
}

//--------------------------------------------------------------------------------------------------

auto MenuItem::get_caption() -> String ^ {
  return caption;
}

//--------------------------------------------------------------------------------------------------

auto MenuItem::get_shortcut() -> String ^ {
  return shortcut;
}

//--------------------------------------------------------------------------------------------------

auto MenuItem::getInternalName() -> String ^ {
  return internalName;
}

//--------------------------------------------------------------------------------------------------

auto MenuItem::get_type() -> MenuItemType {
  return type;
}

//--------------------------------------------------------------------------------------------------

auto MenuItem::get_checked() -> bool {
  return checked;
}

//--------------------------------------------------------------------------------------------------

auto MenuItem::set_checked(bool value) -> void {
  checked = value;
}

//--------------------------------------------------------------------------------------------------

auto MenuItem::get_enabled() -> bool {
  return enabled;
}

//--------------------------------------------------------------------------------------------------

auto MenuItem::set_enabled(bool value) -> void {
  enabled = value;
}

//--------------------------------------------------------------------------------------------------

auto MenuItem::get_subitems() -> List<MenuItem ^> ^ {
  return subitems;
}

//----------------- UIForm -------------------------------------------------------------------------

UIForm::UIForm(bec::UIForm* inn) {
  init(inn);
}

//--------------------------------------------------------------------------------------------------

UIForm::UIForm() : inner(NULL) {
}

//--------------------------------------------------------------------------------------------------

UIForm::~UIForm() {
  ReleaseHandle();
}

//--------------------------------------------------------------------------------------------------

/**
 * Returns a fixed pointer to this object that will not be modified by the GC
 */
auto UIForm::GetFixedId() -> System::IntPtr {
  if (!m_gch.IsAllocated)
    m_gch = System::Runtime::InteropServices::GCHandle::Alloc(this);
  return System::Runtime::InteropServices::GCHandle::ToIntPtr(m_gch);
}

//--------------------------------------------------------------------------------------------------

auto UIForm::ReleaseHandle() -> void {
  if (m_gch.IsAllocated)
    m_gch.Free();
}

//--------------------------------------------------------------------------------------------------

auto UIForm::init(bec::UIForm* inn) -> void {
  if (inner != NULL) {
    // Don't touch inner here. It's already gone at this point.
    ReleaseHandle();
  }

  // Just replace the inner pointer. We are not managing the inner object.
  inner = inn;

  if (inner != NULL) {
    // get a fixed pointer to this object
    System::IntPtr ip = this->GetFixedId();

    // set it as the user data
    inner->set_frontend_data((void*)(intptr_t)ip);
  }
}

//--------------------------------------------------------------------------------------------------

auto UIForm::get_unmanaged_object() -> bec::UIForm* {
  return inner;
}

//--------------------------------------------------------------------------------------------------

// Returns the object based on the fixed pointer retrieved by GetFixedId()
auto UIForm::GetFromFixedId(System::IntPtr ip) -> UIForm ^ {
  System::Runtime::InteropServices::GCHandle gcHandle = System::Runtime::InteropServices::GCHandle::FromIntPtr(ip);
  return (UIForm ^)gcHandle.Target;
}

//--------------------------------------------------------------------------------------------------

auto UIForm::can_close() -> bool {
  return get_unmanaged_object()->can_close();
}

//--------------------------------------------------------------------------------------------------

auto UIForm::close() -> void {
  get_unmanaged_object()->close();
}

//--------------------------------------------------------------------------------------------------

auto UIForm::get_title() -> System::String ^ {
  return CppStringToNativeRaw(get_unmanaged_object()->get_title());
}

//--------------------------------------------------------------------------------------------------

auto UIForm::form_id() -> System::String ^ {
  return CppStringToNativeRaw(get_unmanaged_object()->form_id());
}

//--------------------------------------------------------------------------------------------------
