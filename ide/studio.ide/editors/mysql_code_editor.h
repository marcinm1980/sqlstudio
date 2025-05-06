/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#pragma once

#include "ng_public_interface.h"
#include "code_editor_base.h"

#include "grtsqlparser/mysql_parser_services.h"

/**
 * The central MySQL editor class.
 */

class MySQLObjectNamesCache;

namespace ng {

  class NG_PUBLIC_TYPE NgMySQLEditor : public NgBaseEditor
  {
    friend class NgBaseEditor;

  public:
    typedef std::shared_ptr<NgMySQLEditor> Ref;

    enum ContentType
    {
      ContentTypeGeneral,
      ContentTypeTrigger,
      ContentTypeView,
      ContentTypeRoutine,
      ContentTypeEvent,
    };

    virtual ~NgMySQLEditor();

    void currentSchema(const std::string &schema);

    virtual void showCodeCompletion(bool auto_choose_single);
    void objectNamesCache(MySQLObjectNamesCache *cache);

    std::string sqlMode();
    void sqlMode(const std::string &value);
    void useServerVersion(GrtVersionRef version);

    void restrictContentTo(ContentType type);

  protected:
    NgMySQLEditor(GrtVersionRef version, GrtCharacterSetsRef charsets, bool caseSensitive, bool noToolbar);

    virtual void setupCodeCompletion();
    virtual std::string getWrittenPart(std::size_t position);
    virtual void splitStatementsIfRequired();
    virtual void doSyntaxCheck();

  private:
    void createEditorConfigForVersion(GrtVersionRef version);
    bool startSqlProcessing();

    parser::MySQLParserContext::Ref _parserContext;
    parser::MySQLParserServices::Ref _services;
    MySQLObjectNamesCache *_objectNamesCache;

    std::pair<const char*, size_t> _textInfo; // Only valid during a text processing run.
    MySQLParseUnit _parseUnit;  // The type of query we want to limit parsing to.

    std::string _currentSchema;
    std::string _sqlMode;
  };
  
}
