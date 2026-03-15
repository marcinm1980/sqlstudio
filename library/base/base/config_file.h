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

#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include <vector>
#include <fstream>
#include <string>

#include "common.h"

namespace base {

  enum ConfigFileFlags {
    AutoCreateNothing = 0,
    AutoCreateSections = 1, // Automatically create sections if they don't exist and get a value.
    AutoCreateKeys = 2      // Create a key if written to but does not exist yet.
  };

  inline auto operator|(ConfigFileFlags a, ConfigFileFlags b) -> ConfigFileFlags {
    return ConfigFileFlags((int)a | (int)b);
  }

  class BASELIBRARY_PUBLIC_FUNC ConfigurationFile {
  public:
    ConfigurationFile(ConfigFileFlags flags);
    ConfigurationFile(std::string file_name, ConfigFileFlags flags);
    virtual ~ConfigurationFile();

    auto load(const std::string &file_name) -> bool;
    auto save(const std::string &file_name) -> bool;

    auto clear_includes(const std::string &section_name) -> void;
    auto add_include(const std::string &section_name, const std::string &include) -> void;
    auto add_include_dir(const std::string &section_name, const std::string &include) -> void;
    auto get_includes(const std::string &section_name) -> std::vector<std::string>;

    auto get_value(std::string key, std::string section = "") -> std::string;
    auto get_float(std::string key, std::string section = "") -> double;
    auto get_int(std::string key, std::string section = "") -> int;
    auto get_bool(std::string key, std::string section = "") -> bool;

    auto set_value(std::string key, std::string value, std::string section = "") -> bool;
    auto set_float(std::string key, float fValue, std::string section = "") -> bool;
    auto set_int(std::string key, int nValue, std::string section = "") -> bool;
    auto set_bool(std::string key, bool bValue, std::string section = "") -> bool;
    auto set_key_pre_comment(std::string key, std::string comment, std::string section = "") -> bool;
    auto set_key_post_comment(std::string key, std::string comment, std::string section = "") -> bool;
    auto set_section_comment(std::string section, std::string comment) -> bool;

    auto delete_key(std::string key, std::string from_section = "") -> bool;
    auto delete_section(std::string section) -> bool;

    auto create_key(std::string key, std::string value, std::string pre_comment = "", std::string post_comment = "",
                    std::string section = "") -> bool;
    auto create_section(std::string section_name, std::string comment = "") -> bool;

    auto section_count() -> int;
    auto key_count() -> int;
    auto key_count_for_secton(const std::string &section_name) -> int;
    auto clear() -> void;
    auto is_dirty() -> bool;
    auto has_key(const std::string &key, const std::string &section) -> bool;
    auto has_section(const std::string &section_name) -> bool;

  private:
    class Private;
    Private *data;
  };

} // namespace base

#endif // _CONFIG_FILE_H_
