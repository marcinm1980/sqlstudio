/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef __CANVAS_H__
#define __CANVAS_H__

#include "mdc_canvas_view_windows.h"
#include "mdc_canvas_view_image.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

#pragma make_public(mdc::KeyInfo)
#pragma make_public(mdc::CanvasView)

namespace MySQL {
  namespace GUI {
    namespace Mdc {

      struct KeyCodeMapping {
        Keys key;
        mdc::KeyCode kcode;
      };

    public
      ref class BaseWindowsCanvasView {
      public:
        delegate void Void4IntDelegate(int, int, int, int);
        auto VoidVoidDelegate() -> delegate void;

      protected:
        ::mdc::CanvasView *inner;

      private:
        // needed for fixed pointer
        GCHandle m_gch;

        System::Windows::Forms::Form ^ owner_form;

        // void (int,int,int,int)
        [UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)] delegate void Void4IntWrapperDelegate(int, int,
                                                                                                            int, int);
        typedef void (*BaseWindowsCanvasView::VOID_4INT_CB)(int, int, int, int);

        // void ()
        [UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)] delegate void VoidVoidWrapperDelegate();
        typedef void (*BaseWindowsCanvasView::VOID_VOID_CB)();

        Void4IntDelegate ^ on_queue_repaint_delegate;
        Void4IntWrapperDelegate ^ on_queue_repaint_wrapper_delegate;

        VoidVoidDelegate ^ on_viewport_changed_delegate;
        VoidVoidWrapperDelegate ^ on_viewport_changed_wrapper_delegate;

        auto on_queue_repaint_wrapper(int x, int y, int w, int h) -> void;
        auto on_viewport_changed_wrapper() -> void;

      public:
        BaseWindowsCanvasView();
        ~BaseWindowsCanvasView();

        auto get_unmanaged_object() -> ::mdc::CanvasView *;
        auto GetFixedId() -> IntPtr;
        auto ReleaseHandle() -> void;
        static auto GetFromFixedId(IntPtr ip) -> BaseWindowsCanvasView ^;
        void set_on_queue_repaint(Void4IntDelegate ^ dt);
        void set_on_viewport_changed(VoidVoidDelegate ^ dt);
        auto initialize() -> bool;
        auto repaint(IntPtr hdc, int x, int y, int width, int height) -> void;
        auto repaint(IntPtr hdc) -> void;
        virtual auto set_target_context(HDC hdc) -> void;
        auto get_fps() -> double;

        void OnMouseMove(MouseEventArgs ^ e, Keys keystate, MouseButtons buttons);
        void OnMouseDown(MouseEventArgs ^ e, Keys keystate, MouseButtons buttons);
        void OnMouseUp(MouseEventArgs ^ e, Keys keystate, MouseButtons buttons);
        void OnMouseDoubleClick(MouseEventArgs ^ e, Keys keystate, MouseButtons buttons);
        bool OnKeyDown(KeyEventArgs ^ e, Keys keystate);
        void OnKeyUp(KeyEventArgs ^ e, Keys keystate);
        auto OnSizeChanged(int w, int h) -> void;
        void SetOwnerForm(System::Windows::Forms::Form ^ ownerForm);
        auto GetOwnerForm() -> System::Windows::Forms::Form ^;

        void get_viewport_range([Out] double % x, [Out] double % y, [Out] double % w, [Out] double % h);
        void get_viewport([Out] double % x, [Out] double % y, [Out] double % w, [Out] double % h);
        auto set_offset(double x, double y) -> void;
        auto scroll_to(double x, double y) -> void;
        void get_total_view_size([Out] double % w, [Out] double % h);
        void window_to_canvas(int x, int y, [Out] double % ox, [Out] double % oy);
        void window_to_canvas(int x, int y, int w, int h, [Out] double % ox, [Out] double % oy, [Out] double % ow,
                              [Out] double % oh);
        auto update_view_size(int w, int h) -> void;

        static auto getEventState(Keys keys, MouseButtons buttons) -> mdc::EventState;
        static mdc::KeyInfo getKeyInfo(KeyEventArgs ^ e);

        auto get() -> property float Zoom { float {
            return get_unmanaged_object()->get_zoom();
          }
          auto set(float value) -> void {
            get_unmanaged_object()->set_zoom(value);
          }
        }
      };

    public
      ref class WindowsGLCanvasView : public BaseWindowsCanvasView {
      public:
        WindowsGLCanvasView(IntPtr window, int width, int height) {
          inner = new ::mdc::WindowsGLCanvasView((HWND)window.ToPointer(), width, height);

          // get a fixed pointer to this object
          IntPtr ip = this->GetFixedId();

          // set it as the user data
          inner->set_user_data((void *)(intptr_t)ip);
        }
      };

    public
      ref class WindowsGDICanvasView : public BaseWindowsCanvasView {
      public:
        WindowsGDICanvasView(IntPtr window, int width, int height) {
          inner = new ::mdc::WindowsCanvasView(width, height);

          // get a fixed pointer to this object
          IntPtr ip = this->GetFixedId();

          // set it as the user data
          inner->set_user_data((void *)(intptr_t)ip);
        }

        virtual void set_target_context(HDC hdc) override {
          (dynamic_cast<::mdc::WindowsCanvasView *>(inner))->set_target_context(hdc);
        }
      };

    public
      ref class ImageCanvasView : public BaseWindowsCanvasView {
      public:
        ImageCanvasView(int width, int height) {
          inner = new ::mdc::ImageCanvasView(width, height, CAIRO_FORMAT_RGB24);

          // get a fixed pointer to this object
          IntPtr ip = this->GetFixedId();

          // set it as the user data
          inner->set_user_data((void *)(intptr_t)ip);
        }

        const IntPtr get_image_data([Out] int % size) {
          size_t c_size;
          const unsigned char *data = dynamic_cast<mdc::ImageCanvasView *>(inner)->get_image_data(c_size);
          size = (int)c_size;

          return IntPtr((void *)data);
        }

        virtual void set_target_context(HDC hdc) override {
        }
      };

    } // namespace MySqlStudio
  }   // namespace GUI
} // namespace MySQL

#endif // __CANVAS_H__
