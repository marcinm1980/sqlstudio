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

#pragma once

using namespace System::Runtime::InteropServices;

namespace MySQL {
  namespace Forms {

    // Message type for the C# interface.
  public
    enum class MessageType { MessageInfo, MessageWarning, MessageError };

    /**
     * A custom message box for the output as there is no predefined dialog which allows to
     * have custom button captions.
     */
  public
    ref class CustomMessageBox : System::Windows::Forms::Form {
    private:
      // Constructor is private. CustomMessageBox should be accessed through the public Show() method
      CustomMessageBox();

      // GUI Elements, we have 3 buttons whose text can be customized.
      System::Windows::Forms::Label ^ _messageLabel;
      System::Windows::Forms::Button ^ _button1;
      System::Windows::Forms::Button ^ _button2;
      System::Windows::Forms::Button ^ _button3;
      System::Windows::Forms::PictureBox ^ _picture;
      System::Windows::Forms::CheckBox ^ _checkbox;

      auto ComputeLayout() -> void;
      void ButtonClick(System::Object ^ sender, EventArgs ^ e);

      static auto ShowInternal(const std::string &title, const std::string &text, PCWSTR mainIcon,
                                               const std::string &buttonOK, const std::string &buttonCancel,
                                               const std::string &buttonOther, const std::string &checkbox,
                                               bool &checked) -> mforms::DialogResult;

    public:
      // C++ interface
      static auto Show(const std::string &title, const std::string &text, PCWSTR mainIcon,
                                       const std::string &buttonOK, const std::string &buttonCancel,
                                       const std::string &buttonOther, const std::string &checkbox, bool &checked) -> mforms::DialogResult;

      // C# interface
      static System::Windows::Forms::DialogResult Show(MessageType type, String ^ title, String ^ text,
                                                       String ^ buttonOK, String ^ buttonCancel, String ^ buttonOther,
                                                       String ^ checkbox, [Out] bool % checked);
      static System::Windows::Forms::DialogResult Show(MessageType type, String ^ title, String ^ text,
                                                       String ^ buttonOK);
    };

  private
    ref class InvokationResult {
    private:
      void *_result;

    public:
      InvokationResult(void *result) {
        _result = result;
      }

      auto get() -> property void *Result{void * {return _result;
    }
  };
};

private
ref class SlotWrapper {
public:
  const std::function<void *()> *_slot;

  SlotWrapper(const std::function<void *()> &slot) {
    // Make a copy of the slot or it will be invalid at the time we want to run it.
    _slot = new std::function<void *()>(slot);
  }

  ~SlotWrapper() {
    delete _slot;
  }
};

private
ref class DispatchControl : System::Windows::Forms::Control {
private:
  InvokationResult ^ RunSlot(SlotWrapper ^ wrapper);

public:
  auto RunOnMainThread(const std::function<void *()> &slot, bool wait) -> void *;
};

public
class UtilitiesWrapper {
private:
  static gcroot<DispatchControl ^> dispatcher;

  static gcroot<Drawing::Font ^> last_font;

  static auto load_passwords() -> void;
  static auto unload_passwords(bool store) -> void;

protected:
  UtilitiesWrapper();

  static auto beep() -> void;
  static auto show_message(const std::string &title, const std::string &text, const std::string &ok,
                          const std::string &cancel, const std::string &other) -> int;
  static auto show_error(const std::string &title, const std::string &text, const std::string &ok,
                        const std::string &cancel, const std::string &other) -> int;
  static auto show_warning(const std::string &title, const std::string &text, const std::string &ok,
                          const std::string &cancel, const std::string &other) -> int;
  static auto show_message_with_checkbox(const std::string &title, const std::string &text, const std::string &ok,
                                        const std::string &cancel, const std::string &other,
                                        const std::string &checkbox_text, bool &isChecked) -> int;
  static auto show_wait_message(const std::string &title, const std::string &text) -> void;
  static auto hide_wait_message() -> bool;
  static auto run_cancelable_wait_message(const std::string &title, const std::string &text,
                                          const std::function<void()> &signal_ready,
                                          const std::function<bool()> &cancel_slot) -> bool;
  static auto stop_cancelable_wait_message() -> void;

  static auto set_clipboard_text(const std::string &content) -> void;
  static auto get_clipboard_text() -> std::string;
  static auto get_special_folder(mforms::FolderType type) -> std::string;

  static auto open_url(const std::string &url) -> void;
  static auto move_to_trash(const std::string &file_name) -> bool;
  static auto reveal_file(const std::string &path) -> void;

  static auto add_timeout(float interval, const std::function<bool()> &slot) -> mforms::TimeoutHandle;
  static auto cancel_timeout(mforms::TimeoutHandle h) -> void;

  static auto store_password(const std::string &service, const std::string &account, const std::string &password) -> void;
  static auto find_password(const std::string &service, const std::string &account, std::string &password) -> bool;
  static auto forget_password(const std::string &service, const std::string &account) -> void;

  static auto perform_from_main_thread(const std::function<void *()> &slot, bool wait) -> void *;
  static auto set_thread_name(const std::string &name) -> void;

  static auto get_text_width(const std::string &text, const std::string &font) -> double;

public:
  static auto get_mainform() -> System::Windows::Forms::Form ^;

  static auto init() -> void;
};
}
;
}
;
