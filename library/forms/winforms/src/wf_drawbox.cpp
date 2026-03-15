/*
 * Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
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
#include "wf_drawbox.h"

using namespace System::Collections::Generic;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Windows::Forms::Layout;

using namespace MySQL;
using namespace MySQL::Forms;

using namespace System::Drawing;

//--------------------------------------------------------------------------------------------------

/**
 * Converts Windows specific mouse button identifiers to plain numbers for the back end.
 */
static auto convert_mouse_button(MouseButtons button) -> int {
  switch (button) {
    case MouseButtons::Left:
      return 0;
    case MouseButtons::Right:
      return 1;
    case MouseButtons::Middle:
      return 2;
    default:
      return -1;
  }
}

//--------------------------------------------------------------------------------------------------

static auto convert_accessible_role(base::Accessible::Role be_role) -> System::Windows::Forms::AccessibleRole {
  System::Windows::Forms::AccessibleRole role = System::Windows::Forms::AccessibleRole::None;

  switch (be_role) {
    case base::Accessible::RoleNone:
      role = System::Windows::Forms::AccessibleRole::None;
      break;

    case base::Accessible::Window:
      role = System::Windows::Forms::AccessibleRole::Window;
      break;

    case base::Accessible::Pane:
      role = System::Windows::Forms::AccessibleRole::Pane;
      break;

    case base::Accessible::Link:
      role = System::Windows::Forms::AccessibleRole::Link;
      break;

    case base::Accessible::List:
      role = System::Windows::Forms::AccessibleRole::List;
      break;

    case base::Accessible::ListItem:
      role = System::Windows::Forms::AccessibleRole::ListItem;
      break;

    case base::Accessible::PushButton:
      role = System::Windows::Forms::AccessibleRole::PushButton;
      break;

    case base::Accessible::StaticText:
      role = System::Windows::Forms::AccessibleRole::StaticText;
      break;

    case base::Accessible::Text:
      role = System::Windows::Forms::AccessibleRole::Text;
      break;

    case base::Accessible::Outline:
      role = System::Windows::Forms::AccessibleRole::Outline;
      break;

    case base::Accessible::OutlineButton:
      role = System::Windows::Forms::AccessibleRole::OutlineButton;
      break;

    case base::Accessible::OutlineItem:
      role = System::Windows::Forms::AccessibleRole::OutlineItem;
      break;
  }
  return role;
}

//----------------- WBControlAccessibleObject ------------------------------------------------------

WBControlAccessibleObject::WBControlAccessibleObject(Control ^ owner, base::Accessible *backendOwner)
  : ControlAccessibleObject(owner) {
  backend = backendOwner;
}

//--------------------------------------------------------------------------------------------------

auto WBControlAccessibleObject::Name::get() -> String ^ {
  return CppStringToNativeRaw(backend->getAccessibilityDescription());
}

//--------------------------------------------------------------------------------------------------

auto WBControlAccessibleObject::GetChildCount() -> int {
  return static_cast<int>(backend->getAccessibilityChildCount());
}

//--------------------------------------------------------------------------------------------------

auto WBControlAccessibleObject::Description::get() -> String ^ {
  return CppStringToNative(backend->getAccessibilityDescription());
}

//--------------------------------------------------------------------------------------------------

auto WBControlAccessibleObject::DefaultAction::get() -> String ^ {
  return CppStringToNativeRaw(backend->getAccessibilityDefaultAction());
}

//--------------------------------------------------------------------------------------------------

auto WBControlAccessibleObject::Value::get() -> String ^ {
  return CppStringToNative(backend->getAccessibilityValue());
}

//--------------------------------------------------------------------------------------------------

auto WBControlAccessibleObject::Role::get() -> System::Windows::Forms::AccessibleRole {
  return convert_accessible_role(backend->getAccessibilityRole());
}

//--------------------------------------------------------------------------------------------------

auto WBControlAccessibleObject::DoDefaultAction() -> void {
  return backend->accessibilityDoDefaultAction();
}

//--------------------------------------------------------------------------------------------------

auto WBControlAccessibleObject::GetChild(int index) -> System::Windows::Forms::AccessibleObject ^ {
  base::Accessible *child = backend->getAccessibilityChild(index);

  if (child)
    return gcnew WBAccessibleObject(child, this);
  else
    return nullptr;
}

//--------------------------------------------------------------------------------------------------

auto WBControlAccessibleObject::HitTest(int x, int y) -> System::Windows::Forms::AccessibleObject ^ {
  Point point = Owner->PointToClient(Point(x, y));

  base::Accessible *accessible = backend->accessibilityHitTest(point.X, point.Y);

  if (accessible)
    return gcnew WBAccessibleObject(accessible, this);
  else if (Owner->RectangleToScreen(Owner->ClientRectangle).Contains(x, y))
    return this;

  return nullptr;
}

//----------------- WBAccessibleObject -------------------------------------------------------------

WBAccessibleObject::WBAccessibleObject(base::Accessible *backendOwner, WBControlAccessibleObject ^ parent_control) {
  backend = backendOwner;
  parent = parent_control;
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::Name::get() -> String ^ {
  return CppStringToNativeRaw(backend->getAccessibilityDescription());
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::Description::get() -> String ^ {
  return CppStringToNative(backend->getAccessibilityDescription());
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::DefaultAction::get() -> String ^ {
  return CppStringToNativeRaw(backend->getAccessibilityDefaultAction());
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::Value::get() -> String ^ {
  return CppStringToNative(backend->getAccessibilityValue());
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::Role::get() -> System::Windows::Forms::AccessibleRole {
  return convert_accessible_role(backend->getAccessibilityRole());
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::Bounds::get() -> System::Drawing::Rectangle {
  base::Rect backend_bounds = backend->getAccessibilityBounds();
  System::Drawing::Rectangle bounds = System::Drawing::Rectangle(
    (int)backend_bounds.left(), (int)backend_bounds.top(), (int)backend_bounds.width(), (int)backend_bounds.height());

  if (parent)
    bounds = parent->Owner->RectangleToScreen(bounds);

  return bounds;
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::GetChildCount() -> int {
  return static_cast<int>(backend->getAccessibilityChildCount());
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::DoDefaultAction() -> void {
  return backend->accessibilityDoDefaultAction();
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::GetChild(int index) -> System::Windows::Forms::AccessibleObject ^ {
  base::Accessible *child = backend->getAccessibilityChild(index);

  if (child)
    return gcnew WBAccessibleObject(child, parent);
  else
    return nullptr;
}

//--------------------------------------------------------------------------------------------------

auto WBAccessibleObject::HitTest(int x, int y) -> System::Windows::Forms::AccessibleObject ^ {
  base::Accessible *accessible = backend->accessibilityHitTest(x, y);

  if (accessible && accessible != backend)
    return gcnew WBAccessibleObject(accessible, parent);
  else
    return nullptr;
}

//----------------- CanvasControl ------------------------------------------------------------------

CanvasControl::CanvasControl() {
  // Enable custom draw and double buffering for flicker reduction.
  SetStyle(ControlStyles::UserPaint, true);
  SetStyle(ControlStyles::AllPaintingInWmPaint, true);
  SetStyle(ControlStyles::DoubleBuffer, true);
  // SetStyle(ControlStyles::SupportsTransparentBackColor, true);
  SetStyle(ControlStyles::OptimizedDoubleBuffer, true);
  UpdateStyles();
}

//--------------------------------------------------------------------------------------------------

auto CanvasControl::CreateAccessibilityInstance() -> System::Windows::Forms::AccessibleObject ^ {
  return gcnew WBControlAccessibleObject(this, backend);
}

//--------------------------------------------------------------------------------------------------

auto CanvasControl::LayoutEngine::get() -> System::Windows::Forms::Layout::LayoutEngine ^ {
  if (layoutEngine == nullptr)
    layoutEngine = gcnew DrawBoxLayout();
  return layoutEngine;
}

//--------------------------------------------------------------------------------------------------

auto CanvasControl::GetPreferredSize(System::Drawing::Size proposedSize) -> System::Drawing::Size {
  base::Size nativeSize = backend->getLayoutSize(base::Size(proposedSize.Width, proposedSize.Height));
  System::Drawing::Size minSize = MinimumSize;
  if (minSize.Width > nativeSize.width)
    nativeSize.width = minSize.Width;
  if (minSize.Height > nativeSize.height)
    nativeSize.height = minSize.Height;
  return System::Drawing::Size((int)nativeSize.width, (int)nativeSize.height);
}

//--------------------------------------------------------------------------------------------------

void CanvasControl::Add(Control ^ control, mforms::Alignment alignment) {
  ViewWrapper::set_layout_dirty(this, true);
  alignments[control] = alignment;
  Controls->Add(control);
}

//--------------------------------------------------------------------------------------------------

void CanvasControl::Remove(Control ^ control) {
  Controls->Remove(control);
  alignments.Remove(control);
}

//--------------------------------------------------------------------------------------------------

void CanvasControl::Move(Control ^ control, int x, int y) {
  alignments[control] = mforms::NoAlign;
  control->Location = Point(x, y);
}

//--------------------------------------------------------------------------------------------------

mforms::Alignment CanvasControl::GetAlignment(Control ^ control) {
  return (mforms::Alignment)alignments[control];
}

//--------------------------------------------------------------------------------------------------

auto CanvasControl::SetBackend(mforms::DrawBox *backend) -> void {
  this->backend = backend;
}

//--------------------------------------------------------------------------------------------------

auto CanvasControl::DoRepaint() -> void {
  Invalidate();
}

//--------------------------------------------------------------------------------------------------

void CanvasControl::OnKeyDown(KeyEventArgs ^args) {
  __super::OnKeyDown(args);
  if (args->KeyCode == Keys::Escape)
    backend->cancel_operation();
}

//--------------------------------------------------------------------------------------------------

void CanvasControl::OnPaint(PaintEventArgs ^ args) {
  if (backend != NULL) {
    IntPtr hdc = args->Graphics->GetHdc();

    cairo_surface_t *surface = cairo_win32_surface_create(static_cast<HDC>(hdc.ToPointer()));
    cairo_t *cr = cairo_create(surface);
    backend->repaint(cr, args->ClipRectangle.X, args->ClipRectangle.Y, args->ClipRectangle.Width,
                     args->ClipRectangle.Height);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    args->Graphics->ReleaseHdc(hdc);
  }
}

//----------------- DrawBoxLayout ------------------------------------------------------------------

bool DrawBoxLayout::Layout(Object ^ sender, LayoutEventArgs ^ arguments) {
  CanvasControl ^ canvas = (CanvasControl ^) sender;
  Size canvasSize = canvas->Size;
  for each(Control ^ control in canvas->Controls) {
      int x, y;

      switch (canvas->GetAlignment(control)) {
        case mforms::BottomLeft:
        case mforms::MiddleLeft:
        case mforms::TopLeft:
          x = canvas->Padding.Left;
          break;

        case mforms::BottomCenter:
        case mforms::MiddleCenter:
        case mforms::TopCenter:
          x = (canvasSize.Width - control->Size.Width) / 2;
          break;

        case mforms::BottomRight:
        case mforms::MiddleRight:
        case mforms::TopRight:
          x = canvasSize.Width - canvas->Padding.Right - control->Size.Width;
          break;

        default:
          x = 0;
          break;
      }

      switch (canvas->GetAlignment(control)) {
        case mforms::BottomLeft:
        case mforms::BottomCenter:
        case mforms::BottomRight:
          y = canvasSize.Height - canvas->Padding.Bottom - control->Size.Height;
          break;

        case mforms::MiddleLeft:
        case mforms::MiddleCenter:
        case mforms::MiddleRight:
          y = (canvasSize.Height - control->Size.Height) / 2;
          break;

        case mforms::TopLeft:
        case mforms::TopCenter:
        case mforms::TopRight:
          y = canvas->Padding.Top;
          break;

        default:
          y = 0;
          break;
      }

      control->Location = Point(x, y);
    }

  return false; // We don't resize the control as result of a layout process.
}

//----------------- DrawBoxWrapper -----------------------------------------------------------------

DrawBoxWrapper::DrawBoxWrapper(mforms::DrawBox *backend) : ViewWrapper(backend) {
}

//--------------------------------------------------------------------------------------------------

auto DrawBoxWrapper::create(mforms::DrawBox *backend) -> bool {
  DrawBoxWrapper *wrapper = new DrawBoxWrapper(backend);

  CanvasControl ^ canvas = DrawBoxWrapper::Create<CanvasControl>(backend, wrapper);
  canvas->SetBackend(backend);

  return true;
}

//--------------------------------------------------------------------------------------------------

auto DrawBoxWrapper::set_needs_repaint(mforms::DrawBox *backend) -> void {
  CanvasControl ^ canvas = DrawBoxWrapper::GetManagedObject<CanvasControl>(backend);
  canvas->Invalidate();
}

//--------------------------------------------------------------------------------------------------

auto DrawBoxWrapper::add(mforms::DrawBox *backend, mforms::View *view, mforms::Alignment alignment) -> void {
  CanvasControl ^ canvas = DrawBoxWrapper::GetManagedObject<CanvasControl>(backend);
  canvas->Add(DrawBoxWrapper::GetControl(view), alignment);
}

//--------------------------------------------------------------------------------------------------

auto DrawBoxWrapper::remove(mforms::DrawBox *backend, mforms::View *view) -> void {
  CanvasControl ^ canvas = DrawBoxWrapper::GetManagedObject<CanvasControl>(backend);
  canvas->Remove(DrawBoxWrapper::GetControl(view));
}

//--------------------------------------------------------------------------------------------------

auto DrawBoxWrapper::move(mforms::DrawBox *backend, mforms::View *view, int x, int y) -> void {
  CanvasControl ^ canvas = DrawBoxWrapper::GetManagedObject<CanvasControl>(backend);
  canvas->Move(DrawBoxWrapper::GetControl(view), x, y);
}

//--------------------------------------------------------------------------------------------------

auto DrawBoxWrapper::drawFocus(::mforms::DrawBox *self, cairo_t *cr, const base::Rect r) -> void {
  auto bounds = r;
  bounds.use_inter_pixel = true;
  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
  cairo_rectangle(cr, bounds.left(), bounds.top(), bounds.width() - 2, bounds.height() - 2);
  double dashes[] = { 1.0, 2.0 };
  cairo_set_dash(cr, dashes, 2, 0);
  cairo_set_line_width(cr, 1);
  cairo_stroke(cr);
}

//--------------------------------------------------------------------------------------------------

auto DrawBoxWrapper::init() -> void {
  mforms::ControlFactory *f = mforms::ControlFactory::get_instance();

  f->_drawbox_impl.create = &DrawBoxWrapper::create;
  f->_drawbox_impl.set_needs_repaint = &DrawBoxWrapper::set_needs_repaint;
  f->_drawbox_impl.add = &DrawBoxWrapper::add;
  f->_drawbox_impl.remove = &DrawBoxWrapper::remove;
  f->_drawbox_impl.move = &DrawBoxWrapper::move;
  f->_drawbox_impl.drawFocus = &DrawBoxWrapper::drawFocus;
}

//--------------------------------------------------------------------------------------------------
