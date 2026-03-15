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

#include "base/drawing.h"

#include "wf_base.h"
#include "wf_view.h"
#include "wf_box.h"
#include "wf_appview.h"
#include "wf_toolbar.h"

using namespace System::Drawing::Drawing2D;
using namespace System::Windows::Forms;

using namespace MySQL::Forms;
using namespace MySQL::Controls;

//----------------- AppViewDockContent -------------------------------------------------------------

AppViewDockContent::AppViewDockContent() {
  appview = NULL;
};

//--------------------------------------------------------------------------------------------------

AppViewDockContent::~AppViewDockContent() {
  // if (appview != NULL)
  //   appview->release();
}

//--------------------------------------------------------------------------------------------------

auto AppViewDockContent::SetBackend(mforms::AppView *backend) -> void {
  appview = backend;
  // Don't hold a reference, the wrapper should be deleted when the backend object is deleted,
  // not the other way around.. this would cause a circular reference and leak
  // appview->retain();
}

//--------------------------------------------------------------------------------------------------

auto AppViewDockContent::GetBackend() -> mforms::AppView * {
  return appview;
}

//--------------------------------------------------------------------------------------------------

auto AppViewDockContent::GetAppViewIdentifier() -> String ^ {
  return CppStringToNative(appview->identifier());
}

//--------------------------------------------------------------------------------------------------

auto MySQL::Forms::AppViewDockContent::GetContextName() -> String ^ {
  return CppStringToNative(appview->get_form_context_name());
}

//--------------------------------------------------------------------------------------------------

auto AppViewDockContent::GetMenuBar() -> MenuStrip ^ {
  mforms::MenuBar *menu = appview->get_menubar();
  if (menu == NULL)
    return nullptr;

  return AppViewWrapper::GetManagedObject<MenuStrip>(menu);
}

//--------------------------------------------------------------------------------------------------

auto AppViewDockContent::GetToolBar() -> ToolStrip ^ {
  mforms::ToolBar *toolbar = appview->get_toolbar();
  if (toolbar == NULL)
    return nullptr;

  return AppViewWrapper::GetManagedObject<ToolStrip>(toolbar);
}

//--------------------------------------------------------------------------------------------------

auto AppViewDockContent::CanCloseDocument() -> bool {
  return appview->on_close();
}

//--------------------------------------------------------------------------------------------------

auto AppViewDockContent::CloseDocument() -> void {
  appview->close();
}

//--------------------------------------------------------------------------------------------------

auto AppViewDockContent::GetTitle() -> String ^ {
  return CppStringToNativeRaw(appview->get_title());
}

//--------------------------------------------------------------------------------------------------

void AppViewDockContent::SetTitle(String ^ title) {
  appview->set_title(NativeToCppStringRaw(title));
}

//--------------------------------------------------------------------------------------------------

auto AppViewDockContent::UpdateColors() -> void {
  // Change our own background or that of only child, if our content was embedded into a DrawablePanel
  // to implement a design with embedded menu/toolbar)
  if (Controls->Count > 0 && is<DrawablePanel>(Controls[0]))
    Controls[0]->BackColor = Conversions::GetApplicationColor(ApplicationColor::AppColorMainBackground, false);
  else
    BackColor = Conversions::GetApplicationColor(ApplicationColor::AppColorMainBackground, false);

  MenuStrip ^ menuStrip = GetMenuBar();
  if (menuStrip != nullptr) {
    menuStrip->BackColor = Conversions::GetApplicationColor(ApplicationColor::AppColorPanelToolbar, false);
    menuStrip->ForeColor = Conversions::GetApplicationColor(ApplicationColor::AppColorPanelToolbar, true);
    if (Conversions::UseWin8Drawing())
      menuStrip->Renderer = gcnew Win8MenuStripRenderer();
    else
      menuStrip->Renderer = gcnew TransparentMenuStripRenderer();
  }

  ToolStrip ^ toolStrip = GetToolBar();
  if (toolStrip != nullptr) {
    toolStrip->BackColor = Conversions::GetApplicationColor(ApplicationColor::AppColorPanelToolbar, false);
    toolStrip->ForeColor = Conversions::GetApplicationColor(ApplicationColor::AppColorPanelToolbar, true);
  }
}

//----------------- AppViewWrapper -----------------------------------------------------------------

AppViewWrapper::AppViewWrapper(mforms::AppView *app) : BoxWrapper(app), appview(app) {
}

//--------------------------------------------------------------------------------------------------

auto AppViewWrapper::create(mforms::AppView *backend, bool horizontal) -> bool {
  AppViewWrapper *wrapper = new AppViewWrapper(backend);
  wrapper->set_resize_mode(AutoResizeMode::ResizeNone);

  // In order to ease maintenance we create a special document host for our content.
  // This adds another nesting level, however.
  LayoutBox ^ box = Create<LayoutBox>(backend, wrapper);
  box->Horizontal = horizontal;

  wrapper->host = gcnew AppViewDockContent();
  wrapper->host->SetBackend(backend);

  box->BackColor = Drawing::Color::Transparent;
  wrapper->host->Controls->Add(box);
  box->Dock = DockStyle::Fill;

  return true;
}

//--------------------------------------------------------------------------------------------------

/**
 * Called when this app view is about to be docked in a host container. Create the frontend
 * tab document if not yet done and return it to the caller.
 */
auto AppViewWrapper::GetHost() -> AppViewDockContent ^ {
  return host;
}

//--------------------------------------------------------------------------------------------------

auto AppViewWrapper::init() -> void {
  mforms::ControlFactory *f = mforms::ControlFactory::get_instance();
  f->_app_view_impl.create = &AppViewWrapper::create;
}

//--------------------------------------------------------------------------------------------------

AppViewWrapper::~AppViewWrapper() {
  delete host;
  host = nullptr;
}

//--------------------------------------------------------------------------------------------------
