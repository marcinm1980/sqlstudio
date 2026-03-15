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

#include "dictionary.h"
#include <base/string_utilities.h>

#include <iostream>

namespace mtemplate {

  //-----------------------------------------------------------------------------------
  //  DictionaryInterface stuff
  //-----------------------------------------------------------------------------------
  auto DictionaryInterface::setIntValue(const base::utf8string &key, long int value) -> void {
    setValue(key, base::strfmt("%ld", value));
  }

  auto DictionaryInterface::setValueAndShowSection(const base::utf8string &key, const base::utf8string &value,
                                                   const base::utf8string &section) -> void {
    if (value.size() == 0)
      return;
    DictionaryInterface *dict = addSectionDictionary(section);
    dict->setValue(key, value);
  }

  auto DictionaryInterface::setFormatedValue(const base::utf8string &key, const char *format, ...) -> void {
    va_list args;
    va_start(args, format);
    base::utf8string result = base::strfmt(format, args);
    va_end(args);

    setValue(key, result);
  }

  //-----------------------------------------------------------------------------------
  //  DictionaryGlobal stuff
  //-----------------------------------------------------------------------------------
  class DictionaryGlobal : public DictionaryInterface {
  protected:
    dictionary_storage _dictionary;
    section_dictionary_storage _no_section;

    auto getParent() -> DictionaryInterface * {
      return nullptr;
    }

  public:
    DictionaryGlobal() : DictionaryInterface("global") {
    }
    ~DictionaryGlobal() {
    }

    //  DictionaryInterface
    virtual auto setValue(const base::utf8string &key, const base::utf8string &value) -> void {
      _dictionary[key] = value;
    }
    virtual auto getValue(const base::utf8string &key) -> base::utf8string {
      return _dictionary.find(key) == _dictionary.end() ? "" : _dictionary[key];
    }

    virtual auto addSectionDictionary(const base::utf8string &name) -> DictionaryInterface * {
      return NULL;
    }
    virtual auto getSectionDictionaries(const base::utf8string &sections) -> section_dictionary_storage & {
      return _no_section;
    }

    virtual auto dump(int indent) -> void {
      base::utf8string indent_str(indent * 2, ' ');
      base::utf8string indent_plus_str((indent + 1) * 2, ' ');

      std::cout << indent_str << "[" << _name << "] = " << std::endl << indent_str << "{" << std::endl;

      for (auto item : _dictionary)
        std::cout << indent_plus_str << "[" << item.first << "] = \"" << item.second << "\"" << std::endl;

      std::cout << indent_str << "}" << std::endl;
    }
  } GlobalDictionary;

  //-----------------------------------------------------------------------------------
  //  Dictionary stuff
  //-----------------------------------------------------------------------------------
  auto Dictionary::setValue(const base::utf8string &key, const base::utf8string &value) -> void {
    _dictionary[key] = value;
  }

  auto Dictionary::getValue(const base::utf8string &key) -> base::utf8string {
    if (_dictionary.find(key) != _dictionary.end())
      return _dictionary[key];

    if (_parent)
      return _parent->getValue(key);

    return GlobalDictionary.getValue(key);
  }

  auto Dictionary::addSectionDictionary(const base::utf8string &name) -> DictionaryInterface * {
    base::utf8string newName = _name + name + base::utf8string("/");
    DictionaryInterface *_sectionDict = new Dictionary(newName, this);

    if (_section_dictionaries[name].size() > 0)
      _section_dictionaries[name].back()->setIsLast(false);

    _sectionDict->setIsLast(true);
    _section_dictionaries[name].push_back(_sectionDict);
    return _sectionDict;
  }

  auto Dictionary::getSectionDictionaries(const base::utf8string &section) -> Dictionary::section_dictionary_storage & {
    if (_section_dictionaries.find(section) == _section_dictionaries.end())
      return _no_section;
    return _section_dictionaries[section];
  }

  auto Dictionary::dump(int indent) -> void {
    base::utf8string indent_str(indent * 2, ' ');
    base::utf8string indent_plus_str((indent + 1) * 2, ' ');

    if (_dictionary.size() == 0 && _section_dictionaries.size() == 0) {
      std::cout << indent_str << "[" << _name << "] = "
                << "{  }" << std::endl;
      return;
    }

    std::cout << indent_str << "[" << _name << "] = " << std::endl << indent_str << "{" << std::endl;

    for (auto item : _dictionary)
      std::cout << indent_plus_str << "[" << item.first << "] = \"" << item.second << "\"" << std::endl;

    for (auto dict_item : _section_dictionaries)
      for (auto sect_item : dict_item.second)
        sect_item->dump(indent + 1);

    std::cout << indent_str << "}" << std::endl;
  }

  //-----------------------------------------------------------------------------------
  //  General stuff
  //-----------------------------------------------------------------------------------
  auto CreateMainDictionary() -> Dictionary * {
    return new Dictionary("/", NULL);
  }

  auto SetGlobalValue(const base::utf8string &key, const base::utf8string &value) -> void {
    GlobalDictionary.setValue(key, value);
  }

} //  namespace mtemplate
