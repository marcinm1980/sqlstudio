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
#include "modifier.h"

#include "base/utf8string.h"
#include <vector>
#include <memory>

namespace mtemplate {
  enum MTEMPLATELIBRARY_PUBLIC_FUNC PARSE_TYPE { DO_NOT_STRIP, STRIP_BLANK_LINES, STRIP_WHITESPACE };
  enum MTEMPLATELIBRARY_PUBLIC_FUNC TemplateObjectType {
    TemplateObject_Text,
    TemplateObject_Variable,
    TemplateObject_Section,
    TemplateObject_SectionSeparator,
    TemplateObject_NewLine
  };

  struct TemplateOutput;
  class DictionaryInterface;

  /**
   * @brief This is the base type for all other node types
   *
   */
  struct MTEMPLATELIBRARY_PUBLIC_FUNC NodeInterface {
    TemplateObjectType _type;
    base::utf8string _text;
    std::size_t _length;
    bool _hidden;

  protected:
    NodeInterface(TemplateObjectType type, const base::utf8string &text, std::size_t length)
      : _type(type), _text(text), _length(length), _hidden(false) {
    }

  public:
    virtual ~NodeInterface() {
    }

    auto type() const -> TemplateObjectType {
      return _type;
    }
    auto text() const -> const base::utf8string & {
      return _text;
    }
    auto length() const -> std::size_t {
      return _length;
    }

    virtual auto expand(TemplateOutput *output, DictionaryInterface *dict) -> bool = 0;
    virtual auto dump(int indent) -> void = 0;
    auto hide(bool hidden = true) -> void {
      _hidden = hidden;
    }
    auto isHidden() -> bool {
      return _hidden;
    }
  };

  typedef std::shared_ptr<NodeInterface> NodeStorageType;
  typedef std::vector<NodeStorageType> TemplateDocument;

  struct MTEMPLATELIBRARY_PUBLIC_FUNC NodeTextInterface : public NodeInterface {
  protected:
    NodeInterface *_associatedWith;

    NodeTextInterface(TemplateObjectType type, const base::utf8string &text, std::size_t length)
      : NodeInterface(type, text, length) {
    }

  public:
    auto getAssociation() -> NodeInterface * {
      return _associatedWith;
    }
    auto associateWith(NodeInterface *node) -> void {
      _associatedWith = node;
    }
  };

  struct MTEMPLATELIBRARY_PUBLIC_FUNC NodeText : public NodeTextInterface {
    bool _isBlank;
    NodeInterface *_associatedWith;
    NodeText(const base::utf8string &text, std::size_t length);

    virtual auto expand(TemplateOutput *output, DictionaryInterface *dict) -> bool;
    virtual auto dump(int indent) -> void;

    auto isBlank() -> bool {
      return _isBlank;
    }

    static auto parse(const base::utf8string &template_string, PARSE_TYPE type) -> NodeText *;
  };

  struct MTEMPLATELIBRARY_PUBLIC_FUNC NodeNewLine : public NodeTextInterface {
    NodeNewLine() : NodeTextInterface(TemplateObject_NewLine, "\n", 1) {
    }

    virtual auto expand(TemplateOutput *output, DictionaryInterface *dict) -> bool;
    virtual auto dump(int indent) -> void;

    static auto parse(const base::utf8string &template_string, PARSE_TYPE type) -> NodeNewLine *;
  };

  struct MTEMPLATELIBRARY_PUBLIC_FUNC NodeVariable : public NodeTextInterface {
    std::vector<ModifierAndArgument> _modifiers;
    NodeVariable(const base::utf8string &text, std::size_t length, const std::vector<ModifierAndArgument> &modifiers)
      : NodeTextInterface(TemplateObject_Variable, text, length), _modifiers(modifiers) {
    }

    virtual auto expand(TemplateOutput *output, DictionaryInterface *dict) -> bool;
    virtual auto dump(int indent) -> void;

    static auto parse(const base::utf8string &template_string, PARSE_TYPE type) -> NodeVariable *;
  };

  struct MTEMPLATELIBRARY_PUBLIC_FUNC NodeSection : public NodeInterface {
    TemplateDocument _contents;
    TemplateDocument::iterator _separator;
    bool _is_separator;

    NodeSection(const base::utf8string &text, std::size_t length, TemplateDocument &contents);

    auto set_is_separator(bool value = true) -> void {
      _is_separator = value;
    }
    auto is_separator() -> bool {
      return _is_separator;
    }

    //  NodeInterface
    virtual auto expand(TemplateOutput *output, DictionaryInterface *dict) -> bool;
    virtual auto dump(int indent) -> void;

    static auto parse(const base::utf8string &template_string, PARSE_TYPE type) -> NodeSection *;
  };

  auto parseTemplate(const base::utf8string &template_string, PARSE_TYPE type) -> MTEMPLATELIBRARY_PUBLIC_FUNC TemplateDocument;

} // namespace mtemplate
