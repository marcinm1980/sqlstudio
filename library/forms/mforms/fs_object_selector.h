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

/**
 * Implementation of a composite used to select a file system object like a file, folder, device,
 * drive etc.
 */

#pragma once

#include "mforms/label.h"
#include "mforms/box.h"
#include "mforms/button.h"
#include "mforms/textentry.h"
#include "mforms/filechooser.h"
#include "base/trackable.h"

namespace mforms {

  class MFORMS_EXPORT FsObjectSelector : public Box {
  private:
    Button* _browse_button;
    TextEntry* _edit;
    FileChooserType _type;
    std::string _extensions;
    std::string _default_extension;
    std::function<void()> _on_validate;
    boost::signals2::scoped_connection
      _browse_connection; // The connection created when connecting the browse callback.
    bool _show_hidden;

  protected:
    auto enable_file_browsing() -> void;
    auto filename_changed() -> void;
    auto browse_file_callback() -> void;

  public:
    FsObjectSelector(bool horizontal = true);
    FsObjectSelector(Button* button, TextEntry* edit);
    ~FsObjectSelector();

    auto initialize(const std::string& initial_path, FileChooserType type, const std::string& extensions,
                    bool show_hidden = false, std::function<void()> on_validate = std::function<void()>()) -> void;
    auto set_filename(const std::string& path) -> void;
    auto get_filename() -> std::string;
    auto set_enabled(bool value) -> void;
    auto set_browse_callback(std::function<void()> browse_callback) -> void;

    auto get_entry() const -> TextEntry* {
      return _edit;
    }

    virtual auto get_string_value() -> std::string;
    virtual auto get_int_value() -> int;
    virtual auto get_bool_value() -> bool;

#ifndef SWIG
    boost::signals2::signal<void()>* signal_changed() {
      return _edit->signal_changed();
    }
#endif

    static auto clear_stored_filenames() -> void;
    static bool check_and_confirm_file_overwrite(TextEntry* entry, const std::string& default_extension = "");
    auto check_and_confirm_file_overwrite() -> bool;
  };
}
