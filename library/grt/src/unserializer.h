/*
 * Copyright (c) 2007, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "grt.h"
#include <set>

namespace grt {
  namespace internal {
    class Unserializer {
    public:
      Unserializer(bool check_crc);

      auto load_from_xml(const std::string &path, std::string *doctype = 0, std::string *docversion = 0) -> ValueRef;

      ValueRef unserialize_xmldoc(xmlDocPtr doc, const std::string &source_path = "");

      auto unserialize_xmldata(const char *data, size_t size) -> ValueRef;

    protected:
      std::string _source_name;
      std::map<std::string, ValueRef> _cache;
      std::set<std::string> _invalid_cache;
      bool _check_serialized_crc;

      auto unserialize_from_xml(xmlNodePtr node) -> ValueRef;
      auto traverse_xml_recreating_tree(xmlNodePtr node) -> ValueRef;
      auto traverse_xml_creating_objects(xmlNodePtr node) -> void;

      auto unserialize_object_step1(xmlNodePtr node) -> ObjectRef;
      auto unserialize_object_step2(xmlNodePtr node) -> ObjectRef;
      auto unserialize_object_contents(const ObjectRef &object, xmlNodePtr node) -> void;
      auto find_cached(const std::string &id) -> ValueRef;
    };
  };
};
