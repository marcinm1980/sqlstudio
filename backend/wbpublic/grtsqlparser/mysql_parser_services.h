/*
 * Copyright (c) 2013, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "wbpublic_public_interface.h"

#include "mysql/mysql-recognition-types.h"
#include "parsers-common.h"

#include "grtdb/db_helpers.h"

#include "grts/structs.db.mysql.h"

namespace parsers {

  class MySQLParser;
  class SymbolTable;

  // Describes a single statement out of a list in a string.
  struct WBPUBLICBACKEND_PUBLIC_FUNC StatementRange {
    size_t line;   // The line number of the statement.
    size_t start;  // The byte start offset of the statement.
    size_t length; // The length of the statements in bytes.
  };

  struct WBPUBLICBACKEND_PUBLIC_FUNC MySQLParserContext {
    using Ref = std::shared_ptr<MySQLParserContext>;

    virtual ~MySQLParserContext() {};

    virtual auto isCaseSensitive() -> bool = 0;
    virtual auto updateServerVersion(GrtVersionRef newVersion) -> void = 0;
    virtual auto updateSqlMode(const std::string &mode) -> void = 0;

    virtual auto serverVersion() const -> GrtVersionRef = 0;
    virtual auto sqlMode() const -> std::string = 0;
    virtual auto errorsWithOffset(size_t offset) const -> std::vector<ParserErrorInfo> = 0;

    virtual auto createScanner() -> Scanner = 0;

    // Identifier determination depends on e.g the sql mode, hence we need extra handling.
    virtual auto isIdentifier(size_t type) const -> bool = 0;
  };

  /**
   * Defines an abstract interface for parser services. The actual implementation is done in a module
   * (and hence a singleton).
   * The API is thread safe if no parser context is shared between threads.
   */
  class WBPUBLICBACKEND_PUBLIC_FUNC MySQLParserServices {
  public:
    virtual ~MySQLParserServices() {};
    using Ref = MySQLParserServices *; // We only have a singleton, so define Ref only to keep the pattern.

    static auto get() -> MySQLParserServices::Ref;
    virtual auto createParserContext(GrtCharacterSetsRef charsets, GrtVersionRef version, const std::string &sqlMode,
                                     bool caseSensitive) -> MySQLParserContext::Ref = 0;

    // Info services.
    virtual auto tokenFromString(MySQLParserContext::Ref context, const std::string &token) -> size_t = 0;
    virtual auto determineQueryType(MySQLParserContext::Ref context, const std::string &text) -> MySQLQueryType = 0;

    // DB objects.
    virtual auto parseTable(MySQLParserContext::Ref context, db_mysql_TableRef table, const std::string &sql)
      -> size_t = 0;
    virtual auto parseRoutine(MySQLParserContext::Ref context, db_mysql_RoutineRef routine, const std::string &sql)
      -> size_t = 0;
    virtual auto parseRoutines(MySQLParserContext::Ref context, db_mysql_RoutineGroupRef group, const std::string &sql)
      -> size_t = 0;
    virtual auto parseTrigger(MySQLParserContext::Ref context, db_mysql_TriggerRef trigger, const std::string &sql)
      -> size_t = 0;
    virtual auto parseView(MySQLParserContext::Ref context, db_mysql_ViewRef view, const std::string &sql)
      -> size_t = 0;
    virtual auto parseSchema(MySQLParserContext::Ref context, db_mysql_SchemaRef schema, const std::string &sql)
      -> size_t = 0;
    virtual auto parseIndex(MySQLParserContext::Ref context, db_mysql_IndexRef index, const std::string &sql)
      -> size_t = 0;
    virtual auto parseEvent(MySQLParserContext::Ref context, db_mysql_EventRef event, const std::string &sql)
      -> size_t = 0;
    virtual auto parseLogfileGroup(MySQLParserContext::Ref context, db_mysql_LogFileGroupRef group,
                                   const std::string &sql) -> size_t = 0;
    virtual auto parseServer(MySQLParserContext::Ref context, db_mysql_ServerLinkRef server, const std::string &sql)
      -> size_t = 0;
    virtual auto parseTablespace(MySQLParserContext::Ref context, db_mysql_TablespaceRef tablespace,
                                 const std::string &sql) -> size_t = 0;

    virtual auto parseSQLIntoCatalog(MySQLParserContext::Ref context, db_mysql_CatalogRef catalog,
                                     const std::string &sql, grt::DictRef options) -> size_t = 0;

    virtual auto checkSqlSyntax(MySQLParserContext::Ref context, const char *sql, size_t length,
                                MySQLParseUnit unitType) -> size_t = 0;
    virtual auto renameSchemaReferences(MySQLParserContext::Ref context, db_mysql_CatalogRef catalog,
                                        const std::string old_name, const std::string new_name) -> size_t = 0;

    virtual auto determineStatementRanges(const char *sql, size_t length, const std::string &initialDelimiter,
                                          std::vector<StatementRange> &ranges, const std::string &lineBreak = "\n")
      -> size_t = 0;

    virtual auto parseStatement(MySQLParserContext::Ref context, const std::string &sql) -> grt::DictRef = 0;

    // Data types.
    static auto findDataType(SimpleDatatypeListRef types, GrtVersionRef version, const std::string &name)
      -> db_SimpleDatatypeRef;

    virtual auto parseTypeDefinition(const std::string &typeDefinition, GrtVersionRef targetVersion,
                                     SimpleDatatypeListRef typeList, UserDatatypeListRef userTypes,
                                     SimpleDatatypeListRef defaultTypeList, db_SimpleDatatypeRef &simpleType,
                                     db_UserDatatypeRef &userType, int &precision, int &scale, int &length,
                                     std::string &datatypeExplicitParams) -> bool = 0;

    // Others.
    virtual auto getCodeCompletionCandidates(MySQLParserContext::Ref context, std::pair<size_t, size_t> caret,
                                             std::string const &sql, std::string const &defaultSchema,
                                             bool uppercaseKeywords, parsers::SymbolTable &symbolTable)
      -> std::vector<std::pair<int, std::string>> = 0;
  };

} // namespace parsers
