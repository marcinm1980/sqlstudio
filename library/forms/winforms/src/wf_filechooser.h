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

  public
    class FileChooserWrapper : public ViewWrapper {
    private:
      mforms::FileChooserType type;

    protected:
      FileChooserWrapper(mforms::FileChooser *form, mforms::Form *owner);

      static auto create(mforms::FileChooser *backend, mforms::Form *owner, mforms::FileChooserType type,
                         bool show_hidden) -> bool;
      static auto set_title(mforms::FileChooser *backend, const std::string &title) -> void;
      static auto run_modal(mforms::FileChooser *backend) -> bool;
      static auto set_directory(mforms::FileChooser *backend, const std::string &path) -> void;
      static auto set_path(mforms::FileChooser *backend, const std::string &path) -> void;
      static auto get_directory(mforms::FileChooser *backend) -> std::string;
      static auto get_path(mforms::FileChooser *backend) -> std::string;
      static auto set_extensions(mforms::FileChooser *backend, const std::string &extensions,
                                 const std::string &default_extension, bool allow_all_file_types = true) -> void;
      static auto add_selector_option(mforms::FileChooser *backend, const std::string &name, const std::string &label,
                                      const mforms::FileChooser::StringPairVector &options) -> void;
      static auto get_selector_option_value(mforms::FileChooser *backend, const std::string &name) -> std::string;

    public:
      static auto init() -> void;
    };
  };
};
