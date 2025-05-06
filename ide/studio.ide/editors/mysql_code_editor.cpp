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

#ifndef HAVE_PRECOMPILED_HEADERS
  #include "grts/structs.db.mysql.h"
#endif

#include "base/log.h"
#include "base/string_utilities.h"
#include "base/file_utilities.h"
#include "base/threaded_timer.h"
#include "base/util_functions.h"

#include "grt/grt_manager.h"

#include "mforms/code_editor.h"

#include "code-completion/mysql_object_names_cache.h"
#include "code-completion/mysql-code-completion.h"

#include "mysql_code_editor.h"

DEFAULT_LOG_DOMAIN("MySQL code editor");

using namespace bec;
using namespace grt;
using namespace base;

using namespace parser;

using namespace ng;

//----------------- NgMySQLEditor ------------------------------------------------------------------

NgMySQLEditor::NgMySQLEditor(GrtVersionRef version, GrtCharacterSetsRef charsets,
  bool caseSensitive, bool noToolbar)
  : NgBaseEditor(noToolbar)
{
  _objectNamesCache = nullptr;

  createEditorConfigForVersion(version);

  _services = MySQLParserServices::get(); // Reference to parser module.
  _parserContext = _services->createParserContext(charsets, version, caseSensitive);
  _parseUnit = MySQLParseUnit::PuGeneric;
}

//--------------------------------------------------------------------------------------------------

NgMySQLEditor::~NgMySQLEditor()
{
}

//--------------------------------------------------------------------------------------------------

void NgMySQLEditor::showCodeCompletion(bool auto_choose_single)
{
  if (!isCodeCompletionEnabled())
    return;

  _codeEditor->auto_completion_options(true, auto_choose_single, false, true, false);

  // Get the statement and its absolute position.
  size_t caretPosition = _codeEditor->get_caret_pos();
  size_t caretLine = _codeEditor->line_from_position(caretPosition);

  ssize_t lineStart, lineEnd;
  _codeEditor->get_range_of_line(caretLine, lineStart, lineEnd);
  size_t caretOffset = caretPosition - lineStart; // This is a byte offset.

  std::size_t min, max;
  std::string statement;
  bool fixedCaretPos = false;
  if (currentStatementRange(min, max, true))
  {
    // If the caret is in the whitespaces before the query we would get a wrong line number
    // (because the statement splitter doesn't include these whitespaces in the determined ranges).
    // We set the caret pos to the first position in the query, which has the same effect for
    // code completion (we don't generate error line numbers).
    uint32_t codeStartLine = (uint32_t)_codeEditor->line_from_position(min);
    if (codeStartLine > caretLine)
    {
      caretLine = 0;
      caretOffset = 0;
      fixedCaretPos = true;
    }
    else
      caretLine -= codeStartLine;

    statement = _codeEditor->get_text_in_range(min, max);
  }
  else
  {
    // No query, means we have nothing typed yet in the current query (at least nothing valuable).
    caretLine = 0;
    caretOffset = 0;
    fixedCaretPos = true;
  }

  // Convert current caret position into a position of the single statement.
  // The byte-based offset in the line must be converted to a character offset.
  if (!fixedCaretPos)
  {
    std::string line_text = _codeEditor->get_text_in_range(lineStart, lineEnd);
    caretOffset = g_utf8_pointer_to_offset(line_text.c_str(), line_text.c_str() + caretOffset);
  }

  std::string writtenPart = getWrittenPart(caretPosition);
  _codeCompletionEntries = getCodeCompletionList(caretLine, caretOffset, writtenPart, _currentSchema,
    makeKeywordsUppercase(), _parserContext->createScanner(statement),
    _editorConfig->get_keywords()["Functions"], _objectNamesCache);
  updateCodeCompletion(writtenPart);
}

//--------------------------------------------------------------------------------------------------

void NgMySQLEditor::objectNamesCache(MySQLObjectNamesCache *cache)
{
  _objectNamesCache = cache;
}

//--------------------------------------------------------------------------------------------------

void NgMySQLEditor::sqlMode(const std::string &value)
{
  _sqlMode = value;
  _parserContext->use_sql_mode(value);
}

//--------------------------------------------------------------------------------------------------

/**
 * Update the parser's server version in case of external changes (e.g. model settings).
 */
void NgMySQLEditor::useServerVersion(GrtVersionRef version)
{
  _parserContext->use_server_version(version);
  createEditorConfigForVersion(version);
  startCodeProcessing();
}

//--------------------------------------------------------------------------------------------------

void NgMySQLEditor::restrictContentTo(ContentType type)
{
  switch (type)
  {
  case ContentTypeTrigger:
    _parseUnit = MySQLParseUnit::PuCreateTrigger;
    break;
  case ContentTypeView:
    _parseUnit = MySQLParseUnit::PuCreateView;
    break;
  case ContentTypeRoutine:
    _parseUnit = MySQLParseUnit::PuCreateRoutine;
    break;
  case ContentTypeEvent:
    _parseUnit = MySQLParseUnit::PuCreateEvent;
    break;

  default:
    _parseUnit = MySQLParseUnit::PuGeneric;
    break;
  }
}

//--------------------------------------------------------------------------------------------------

void NgMySQLEditor::setupCodeCompletion()
{
  _codeEditor->auto_completion_max_size(80, 15);

  static std::vector<std::pair<int, std::string>> ccImages = {
    { AC_KEYWORD_IMAGE, "ac_keyword.png" }, { AC_SCHEMA_IMAGE, "ac_schema.png" },
    { AC_TABLE_IMAGE, "ac_table.png" }, { AC_ROUTINE_IMAGE, "ac_routine.png" },
    { AC_FUNCTION_IMAGE, "ac_function.png" }, { AC_VIEW_IMAGE, "ac_view.png" },
    { AC_COLUMN_IMAGE, "ac_column.png" }, { AC_OPERATOR_IMAGE, "ac_operator.png" },
    { AC_ENGINE_IMAGE, "ac_engine.png" }, { AC_TRIGGER_IMAGE, "ac_trigger.png" },
    { AC_LOGFILE_GROUP_IMAGE, "ac_logfilegroup.png" }, { AC_USER_VAR_IMAGE, "ac_uservar.png" },
    { AC_SYSTEM_VAR_IMAGE, "ac_sysvar.png" }, { AC_TABLESPACE_IMAGE, "ac_tablespace.png" },
    { AC_EVENT_IMAGE, "ac_event.png" }, { AC_INDEX_IMAGE, "ac_index.png" },
    { AC_USER_IMAGE, "ac_user.png" }, { AC_CHARSET_IMAGE, "ac_charset.png" },
    { AC_COLLATION_IMAGE, "ac_collation.png" }
  };

  _codeEditor->auto_completion_register_images(ccImages);
  _codeEditor->auto_completion_stops("\t,.*;) "); // Will close ac even if we are in an identifier.
  _codeEditor->auto_completion_fillups("");

  // Note: initialization of the code completion library is done on startup of the ng ide.
}

//--------------------------------------------------------------------------------------------------

/**
 * Returns the text in the editor starting at the given position backwards until the line start.
 * If there's a back tick or double quote char then text until this quote char is returned. If there's
 * no quoting char but a space or dot char then everything up to (but not including) this is returned.
 */
std::string NgMySQLEditor::getWrittenPart(std::size_t position)
{
  ssize_t line = _codeEditor->line_from_position(position);
  ssize_t start, stop;
  _codeEditor->get_range_of_line(line, start, stop);
  std::string text = _codeEditor->get_text_in_range(start, position);
  if (text.empty())
    return "";

  const char *head = text.c_str();
  const char *run = head;

  while (*run != '\0')
  {
    if (*run == '\'' || *run == '"' || *run == '`')
    {
      // Entering a quoted text.
      head = run + 1;
      char quote_char = *run;
      while (true)
      {
        run = g_utf8_next_char(run);
        if (*run == quote_char || *run == '\0')
          break;

        // If there's an escape char skip it and the next char too (if we didn't reach the end).
        if (*run == '\\')
        {
          run++;
          if (*run != '\0')
            run = g_utf8_next_char(run);
        }
      }
      if (*run == '\0') // Unfinished quoted text. Return everything.
        return head;
      head = run + 1; // Skip over this quoted text and start over.
    }
    run++;
  }

  // If we come here then we are outside any quoted text. Scan back for anything we consider
  // to be a word stopper (for now anything below '0', char code wise).
  while (head < run--)
  {
    if (*run < '0')
      return run + 1;
  }
  return head;
}

//--------------------------------------------------------------------------------------------------

void NgMySQLEditor::splitStatementsIfRequired()
{
  // If we have restricted content (e.g. for object editors) then we don't split and handle the entire content
  // as a single statement. This will then show syntax errors for any invalid additional input.
  if (_splittingRequired)
  {
    logDebug3("Start splitting\n");
    _splittingRequired = false;

    base::RecMutexLock lock(_statementBordersMutex);

    _statementRanges.clear();
    if (_parseUnit == MySQLParseUnit::PuGeneric)
    {
      double start = timestamp();
      determineStatementRanges(_textInfo.first, _textInfo.second, ";", _statementRanges);
      logDebug3("Splitting ended after %f ticks\n", timestamp() - start);
    }
    else
      _statementRanges.push_back(std::make_pair(0, _textInfo.second));
  }
}

//--------------------------------------------------------------------------------------------------

void NgMySQLEditor::doSyntaxCheck()
{
  base::RecMutexLock lock(_errorsMutex);

  // Now do error checking for each of the statements, collecting error positions for later markup.
  for (auto &range : _statementRanges)
  {
    if (_stopProcessing)
      return;

    if (_services->checkSqlSyntax(_parserContext, _textInfo.first + range.first,
      range.second, _parseUnit) > 0)
    {
      std::vector<ParserErrorEntry> errors = _parserContext->get_errors_with_offset(range.first, true);
      _recognitionErrors.insert(_recognitionErrors.end(), errors.begin(), errors.end());
    }
  }
}

//--------------------------------------------------------------------------------------------------

void NgMySQLEditor::createEditorConfigForVersion(GrtVersionRef version)
{
  if (_editorConfig != NULL)
    delete _editorConfig;

  mforms::SyntaxHighlighterLanguage lang = mforms::LanguageMySQL;
  if (version.is_valid() && version->majorNumber() == 5)
  {
    switch (version->minorNumber())
    {
      case 0: lang = mforms::LanguageMySQL50; break;
      case 1: lang = mforms::LanguageMySQL51; break;
      case 5: lang = mforms::LanguageMySQL55; break;
      case 6: lang = mforms::LanguageMySQL56; break;
      case 7: lang = mforms::LanguageMySQL57; break;
    }
  }
  _editorConfig = new mforms::CodeEditorConfig(lang);
  _codeEditor->set_language(lang);
}

//--------------------------------------------------------------------------------------------------

