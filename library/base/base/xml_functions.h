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
#include <libxml/tree.h>
#include <string>

namespace base {
  namespace xml {
    BASELIBRARY_PUBLIC_FUNC auto loadXMLDoc(const std::string &path) -> xmlDocPtr;
    BASELIBRARY_PUBLIC_FUNC auto xmlParseFragment(const std::string &buff) -> xmlDocPtr;
    BASELIBRARY_PUBLIC_FUNC auto getXmlRoot(xmlDocPtr doc) -> xmlNodePtr;
    BASELIBRARY_PUBLIC_FUNC auto nameIs(xmlNodePtr node, const std::string &name) -> bool;
    BASELIBRARY_PUBLIC_FUNC auto nameIs(xmlAttrPtr attrib, const std::string &name) -> bool;
    BASELIBRARY_PUBLIC_FUNC auto getXMLDocMetainfo(xmlDocPtr doc, std::string &doctype, std::string &docversion) -> void;
    BASELIBRARY_PUBLIC_FUNC auto getProp(xmlNodePtr node, const std::string &name) -> std::string;
    BASELIBRARY_PUBLIC_FUNC auto getContent(xmlNodePtr node) -> std::string;
    BASELIBRARY_PUBLIC_FUNC auto getContentRecursive(xmlNodePtr node) -> std::string;
    BASELIBRARY_PUBLIC_FUNC auto encodeEntities(const std::string &input) -> std::string;
  }; /* end of namespace xml */
};   /* end of namespace base */
