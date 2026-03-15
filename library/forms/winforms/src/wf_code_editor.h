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

#pragma once

namespace MySQL {
  namespace Forms {

  public
    ref class ScintillaControl : public System::Windows::Forms::Control {
    private:
      sptr_t direct_pointer;
      SciFnDirect message_function;
      mforms::CodeEditor *backend;
      mforms::DropDelegate *file_drop_target;
      bool destroying;

    protected:
      virtual void WndProc(System::Windows::Forms::Message % m) override;
      virtual void OnMouseDown(System::Windows::Forms::MouseEventArgs ^ args) override;
      virtual bool ProcessCmdKey(System::Windows::Forms::Message % msg, System::Windows::Forms::Keys keyData) override;

      virtual property System::Windows::Forms::CreateParams ^
        CreateParams { System::Windows::Forms::CreateParams ^ get() override; }

        public : ScintillaControl();

      auto direct_call(unsigned int message, uptr_t wParam, sptr_t lParam) -> sptr_t;
      auto SetBackend(mforms::CodeEditor *editor) -> void;
      auto SetDropTarget(mforms::DropDelegate *target) -> void;

      auto GetKeyCode(int code) -> mforms::KeyCode;
      auto GetModifiers(System::Windows::Forms::Keys keyData) -> mforms::ModifierKey;

      // For interaction with the UI we need some public methods/properties and forward these events
      // to the backend.
      auto get() -> property bool CanUndo { bool;
      }
      auto get() -> property bool CanRedo { bool;
      }
      auto get() -> property bool CanCopy { bool;
      }
      auto get() -> property bool CanCut { bool;
      }
      auto get() -> property bool CanPaste { bool;
      }
      auto get() -> property bool CanDelete { bool;
      }

      auto Undo() -> void;
      auto Redo() -> void;
      auto Copy() -> void;
      auto Cut() -> void;
      auto Paste() -> void;
      auto Delete() -> void;
      auto SelectAll() -> void;

      auto ShowFindPanel(bool doReplace) -> void;
    };

    ref class ScintillaControl;

  public
    class CodeEditorWrapper : public ViewWrapper {
    private:
    protected:
      CodeEditorWrapper(mforms::CodeEditor *backend);

      static auto create(mforms::CodeEditor *editor, bool showInfo) -> bool;
      static auto send_editor(mforms::CodeEditor *editor, unsigned int message, uptr_t wParam, sptr_t lParam) -> sptr_t;
      static auto show_find_panel(mforms::CodeEditor *editor, bool show) -> void;

      virtual auto register_file_drop(mforms::DropDelegate *target) -> void;

    public:
      static auto init() -> void;
    };
  };
};
