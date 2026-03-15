/*
 * Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "common.h"

#include "base/utf8string.h"

#include <map>
#include <vector>
#include <glib.h>

namespace mtemplate {

  struct NodeSection;
  class Template;

  class MTEMPLATELIBRARY_PUBLIC_FUNC DictionaryInterface {
  protected:
    base::utf8string _name;
    bool _is_last;

    DictionaryInterface(const base::utf8string &name) : _name(name), _is_last(false) {
    }

    typedef std::map<base::utf8string, base::utf8string> dictionary_storage;
    typedef dictionary_storage::iterator dictionary_storage_iterator;

    typedef std::vector<DictionaryInterface *> section_dictionary_storage;
    typedef section_dictionary_storage::iterator section_dictionary_storage_iterator;

    typedef std::map<base::utf8string, section_dictionary_storage> section_storage;

    virtual auto getParent() -> DictionaryInterface * = 0;

    friend NodeSection;
    friend Template;

  public:
    virtual ~DictionaryInterface() {
    }

    virtual auto setValue(const base::utf8string &key, const base::utf8string &value) -> void = 0;
    virtual auto getValue(const base::utf8string &key) -> base::utf8string = 0;

    auto setIntValue(const base::utf8string &key, long value) -> void;
    auto setValueAndShowSection(const base::utf8string &key, const base::utf8string &value,
                                const base::utf8string &section) -> void;
    auto setFormatedValue(const base::utf8string &key, const char *format, ...) -> void; // G_GNUC_PRINTF(2, 3);

    virtual auto addSectionDictionary(const base::utf8string &name) -> DictionaryInterface * = 0;
    virtual auto getSectionDictionaries(const base::utf8string &sections) -> section_dictionary_storage & = 0;

    auto setIsLast(bool value) -> void {
      _is_last = value;
    }
    auto isLast() -> bool {
      return _is_last;
    }

    virtual auto dump(int indent = 0) -> void = 0;
  };

  class MTEMPLATELIBRARY_PUBLIC_FUNC Dictionary : public DictionaryInterface {
  protected:
    DictionaryInterface *_parent;

    dictionary_storage _dictionary;
    section_storage _section_dictionaries;
    section_dictionary_storage _no_section;

    auto getParent() -> DictionaryInterface * {
      return _parent;
    }

  public:
    Dictionary(const base::utf8string &name, DictionaryInterface *parent = NULL)
      : DictionaryInterface(name), _parent(parent) {
    }
    virtual ~Dictionary() {
    }

    //  DictionaryInterface
    virtual auto setValue(const base::utf8string &key, const base::utf8string &value) -> void;
    virtual auto getValue(const base::utf8string &key) -> base::utf8string;

    virtual auto addSectionDictionary(const base::utf8string &name) -> DictionaryInterface *;
    virtual auto getSectionDictionaries(const base::utf8string &section) -> section_dictionary_storage &;

    virtual auto dump(int indent = 0) -> void;
  };

  auto CreateMainDictionary() -> MTEMPLATELIBRARY_PUBLIC_FUNC Dictionary *;
  auto SetGlobalValue(const base::utf8string &key, const base::utf8string &value) -> MTEMPLATELIBRARY_PUBLIC_FUNC void;

} //  namespace mtemplate
