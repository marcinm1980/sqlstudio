/*
 * Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "grt.h"
#include "grtpp_util.h"
#include "grtdb/db_object_helpers.h"
#include "grts/structs.db.mysql.h"
#include "grts/structs.studio.physical.h"

#include <fstream>

#include "grt/common.h"
#include "grt/grt_manager.h"
#include "grtdb/db_object_helpers.h"

#include "ILexer.h"

/**
 * For in-memory styling of SQL text we need a mockup-backend we can feed to the lexer accessor
 * (which does the actual styling). It holds the styles the lexer determined for the given text.
 */
class LexerDocument : public Scintilla::IDocument {
private:
  const std::string& _text;
  std::vector<std::pair<std::size_t, std::size_t> > _lines;

  char* _style_buffer;
  std::vector<int> _level_cache;
  Sci_Position _style_position;
  char _styling_mask;

public:
  LexerDocument(const std::string& text);
  virtual ~LexerDocument();

  // IDocument implementation.
  virtual auto Version() const -> int SCI_METHOD;
  virtual auto SetErrorStatus(int status) -> void SCI_METHOD;
  virtual auto Length() const -> Sci_Position SCI_METHOD;
  virtual auto GetCharRange(char *buffer, Sci_Position position, Sci_Position lengthRetrieve) const -> void SCI_METHOD;
  virtual auto StyleAt(Sci_Position position) const -> char SCI_METHOD;
  virtual auto LineFromPosition(Sci_Position position) const -> Sci_Position SCI_METHOD;
  virtual auto LineStart(Sci_Position line) const -> Sci_Position SCI_METHOD;
  virtual auto GetLevel(Sci_Position line) const -> int SCI_METHOD;
  virtual auto SetLevel(Sci_Position line, int level) -> int SCI_METHOD;
  virtual auto GetLineState(Sci_Position line) const -> int SCI_METHOD;
  virtual auto SetLineState(Sci_Position line, int state) -> int SCI_METHOD;
  virtual auto StartStyling(Sci_Position position) -> void SCI_METHOD;
  virtual auto SetStyleFor(Sci_Position length, char style) -> bool SCI_METHOD;
  virtual auto SetStyles(Sci_Position length, const char *styles) -> bool SCI_METHOD;
  virtual auto DecorationSetCurrentIndicator(int indicator) -> void SCI_METHOD;
  virtual auto DecorationFillRange(Sci_Position position, int value, Sci_Position fillLength) -> void SCI_METHOD;
  virtual auto ChangeLexerState(Sci_Position start, Sci_Position end) -> void SCI_METHOD;
  virtual auto CodePage() const -> int SCI_METHOD;
  virtual auto IsDBCSLeadByte(char ch) const -> bool SCI_METHOD;
  virtual auto BufferPointer() -> const char * SCI_METHOD;
  virtual auto GetLineIndentation(Sci_Position line) -> int SCI_METHOD;
  virtual auto LineEnd(Sci_Position line) const -> Sci_Position SCI_METHOD;
  virtual auto GetRelativePosition(Sci_Position positionStart, Sci_Position characterOffset) const -> Sci_Position SCI_METHOD;
  virtual auto GetCharacterAndWidth(Sci_Position position, Sci_Position *pWidth) const -> int SCI_METHOD;
};
