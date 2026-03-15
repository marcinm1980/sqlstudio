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

#pragma once

namespace MySQL {
  namespace Forms {

  private
    ref class FillLayout : public System::Windows::Forms::Layout::LayoutEngine {
    public:
      System::Drawing::Size ComputeLayout(System::Windows::Forms::Control ^ control, Drawing::Size proposedSize,
                                          bool resizeChildren);
      virtual bool Layout(Object ^ container, System::Windows::Forms::LayoutEventArgs ^ arguments) override;
      System::Drawing::Size GetPreferredSize(System::Windows::Forms::Control ^ control, Drawing::Size proposedSize);
    };

  private
    interface class ValueSetter {
      virtual void ApplyContentBounds(const Drawing::Rectangle % bounds);
    };

  private
    delegate System::Void ApplyBoundsDelegate(const Drawing::Rectangle % bounds);

    /**
     * A group box with a fill layout.
     */
  public
    ref class FillGroupBox : public System::Windows::Forms::GroupBox, ValueSetter {
    private:
      FillLayout ^ layoutEngine;

    protected:
      virtual void OnPaintBackground(System::Windows::Forms::PaintEventArgs ^ args) override;

    public:
      FillGroupBox();

      virtual System::Drawing::Size GetPreferredSize(System::Drawing::Size proposedSize) override;

      virtual property System::Windows::Forms::Layout::LayoutEngine ^
        LayoutEngine {
          System::Windows::Forms::Layout::LayoutEngine ^ get() override {
            if (layoutEngine == nullptr)
              layoutEngine = gcnew FillLayout();

            return layoutEngine;
          }
        }

        virtual void
        ApplyContentBounds(const System::Drawing::Rectangle % bounds) {
        if (Controls->Count > 0)
          Controls[0]->Bounds = bounds;
      };
    };

    /**
     * A panel with a fill layout.
     */
  public
    ref class FillPanel : public System::Windows::Forms::Panel, ValueSetter {
    private:
      FillLayout ^ layoutEngine;

    protected:
      virtual void OnPaintBackground(System::Windows::Forms::PaintEventArgs ^ args) override;

    public:
      FillPanel();

      virtual System::Drawing::Size GetPreferredSize(Drawing::Size proposedSize) override;

      virtual property System::Windows::Forms::Layout::LayoutEngine ^
        LayoutEngine {
          System::Windows::Forms::Layout::LayoutEngine ^ get() override {
            if (layoutEngine == nullptr)
              layoutEngine = gcnew FillLayout();

            return layoutEngine;
          }
        }

        virtual void
        ApplyContentBounds(const Drawing::Rectangle % bounds) {
        if (Controls->Count > 0)
          Controls[0]->Bounds = bounds;
      };
    };

    /**
     * A header panel with a fill layout.
     */
  public
    ref class HeaderFillPanel : public MySQL::Controls::HeaderPanel, ValueSetter {
    private:
      FillLayout ^ layoutEngine;
      System::Drawing::Bitmap ^ background;

    public:
      HeaderFillPanel();

      virtual Drawing::Size GetPreferredSize(Drawing::Size proposedSize) override;

      virtual property System::Windows::Forms::Layout::LayoutEngine ^
        LayoutEngine {
          System::Windows::Forms::Layout::LayoutEngine ^ get() override {
            if (layoutEngine == nullptr)
              layoutEngine = gcnew FillLayout();

            return layoutEngine;
          }
        }

        virtual void
        ApplyContentBounds(const Drawing::Rectangle % bounds) {
        if (Controls->Count > 0)
          Controls[0]->Bounds = bounds;
      };
    };

  public
    class PanelWrapper : public ViewWrapper {
    private:
      mforms::View *child;
      mforms::PanelType type;

    protected:
      PanelWrapper(mforms::View *backend);

      static auto create(mforms::Panel *backend, mforms::PanelType panelType) -> bool;
      static auto set_title(mforms::Panel *backend, const std::string &title) -> void;
      static auto set_back_color(mforms::Panel *backend, const std::string &color) -> void;
      static auto add(mforms::Panel *backend, mforms::View *view) -> void;
      static auto set_active(mforms::Panel *backend, bool value) -> void;
      static auto get_active(mforms::Panel *backend) -> bool;
      static auto remove(mforms::Panel *backend, mforms::View *view) -> void;

    public:
      virtual auto set_title(const std::string &title) -> void;
      virtual auto set_back_color(const std::string &color) -> void;
      virtual auto add(mforms::View *view) -> void;
      virtual auto set_active(bool value) -> void;
      virtual auto get_active() -> bool;
      virtual auto remove(mforms::View *view) -> void;
      virtual auto remove() -> void;

      static auto init() -> void;
    };
  };
};
