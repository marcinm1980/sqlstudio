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

#pragma once

#define DRAG_SOURCE_FORMAT_NAME "com.mysql.studio.drag-source"

namespace MySQL {
  namespace Forms {

  public
    enum class AutoResizeMode { ResizeNone, ResizeVertical, ResizeHorizontal, ResizeBoth };

    ref class ViewEventTarget;

  public
    class ViewWrapper : public ObjectWrapper {
    private:
      gcroot<System::Windows::Forms::ToolTip ^> tooltip;
      gcroot<Drawing::Image ^> backgroundImage;
      gcroot<ViewEventTarget ^> eventTarget;

      mforms::Alignment backgroundImageAlignment;
      bool layoutSuspended;
      AutoResizeMode _resize_mode; // Used to constrain certain layout operations.
    protected:
      ViewWrapper(mforms::View *view);

      static auto destroy(mforms::View *backend) -> void;
      static auto show(mforms::View *backend, bool show) -> void;
      static auto get_width(const mforms::View *backend) -> int;
      static auto get_height(const mforms::View *backend) -> int;
      static auto get_preferred_width(mforms::View *backend) -> int;
      static auto get_preferred_height(mforms::View *backend) -> int;
      static auto get_x(const mforms::View *backend) -> int;
      static auto get_y(const mforms::View *backend) -> int;
      static auto set_size(mforms::View *backend, int w, int h) -> void;
      static auto set_min_size(mforms::View *backend, int w, int h) -> void;
      static auto set_padding(mforms::View *backend, int left, int top, int right, int bottom) -> void;
      static auto set_position(mforms::View *backend, int x, int y) -> void;
      static auto client_to_screen(mforms::View *backend, int x, int y) -> std::pair<int, int>;
      static auto screen_to_client(mforms::View *backend, int x, int y) -> std::pair<int, int>;

      static auto set_enabled(mforms::View *backend, bool flag) -> void;
      static auto is_enabled(mforms::View *backend) -> bool;
      static auto find_subview(mforms::View *backend, std::string &name) -> mforms::View *;
      static auto set_name(mforms::View *backend, const std::string &text) -> void;
      static auto relayout(mforms::View *backend) -> void;
      static auto set_needs_repaint(mforms::View *backend) -> void;
      static auto set_tooltip(mforms::View *backend, const std::string &text) -> void;
      static auto set_font(mforms::View *backend, const std::string &text) -> void;
      static auto is_shown(mforms::View *backend) -> bool;
      static auto is_fully_visible(mforms::View *backend) -> bool;
      static auto suspend_layout(mforms::View *backend, bool flag) -> void;
      static auto set_front_color(mforms::View *backend, const std::string &color) -> void;
      static auto get_front_color(mforms::View *backend) -> std::string;
      static auto set_back_color(mforms::View *backend, const std::string &color) -> void;
      static auto get_back_color(mforms::View *backend) -> std::string;
      static auto set_back_image(mforms::View *backend, const std::string &path, mforms::Alignment alignment) -> void;
      static auto flush_events(mforms::View *backend) -> void;

      static auto register_drop_formats(mforms::View *backend, mforms::DropDelegate *target,
                                        const std::vector<std::string> &formats) -> void;
      static auto drag_text(mforms::View *backend, mforms::DragDetails details,
                                             const std::string &text) -> mforms::DragOperation;
      static auto drag_data(mforms::View *backend, mforms::DragDetails details, void *data,
                                             const std::string &format) -> mforms::DragOperation;
      static auto get_drop_position(mforms::View *backend) -> mforms::DropPosition;

      static void SetDragImage(System::Windows::Forms::DataObject ^ data, mforms::DragDetails details);

      static auto focus(mforms::View *backend) -> void;
      static auto has_focus(mforms::View *backend) -> bool;

      virtual auto Initialize() -> void;

      virtual void set_front_color(String ^ color);
      virtual auto set_padding(int left, int top, int right, int bottom) -> void;
      virtual auto set_font(const std::string &fontDescription) -> void;

      virtual auto register_file_drop(mforms::DropDelegate *target) -> void {};
      virtual auto get_drop_position() -> mforms::DropPosition {
        return mforms::DropPositionUnknown;
      };

    public:
      // Only containers allow drawing a background (restriction imposed by other platforms)
      // so we simulate this here by triggering background drawing only for those classes.
      void DrawBackground(System::Windows::Forms::PaintEventArgs ^ args);
      auto set_resize_mode(AutoResizeMode mode) -> void;

      // Utility functions need for event handlers.
      static bool use_min_width_for_layout(System::Windows::Forms::Control ^ control);
      static bool use_min_height_for_layout(System::Windows::Forms::Control ^ control);
      static void remove_auto_resize(System::Windows::Forms::Control ^ control, AutoResizeMode mode);
      static AutoResizeMode get_auto_resize(System::Windows::Forms::Control ^ control);
      static bool can_auto_resize_vertically(System::Windows::Forms::Control ^ control);
      static void set_full_auto_resize(System::Windows::Forms::Control ^ control);
      static bool can_auto_resize_horizontally(System::Windows::Forms::Control ^ control);
      static void set_auto_resize(System::Windows::Forms::Control ^ control, AutoResizeMode mode);
      static bool is_layout_dirty(System::Windows::Forms::Control ^ control);
      static void set_layout_dirty(System::Windows::Forms::Control ^ control, bool value);
      static void resize_with_docking(System::Windows::Forms::Control ^ control, System::Drawing::Size &size);
      static void adjust_auto_resize_from_docking(System::Windows::Forms::Control ^ control);
      static bool can_layout(System::Windows::Forms::Control ^ control, String ^ reason);

      static mforms::View *source_view_from_data(System::Windows::Forms::IDataObject ^ data);
      static auto GetModifiers(System::Windows::Forms::Keys keyData) -> mforms::ModifierKey;

      static auto init() -> void;
    };
  };
};
