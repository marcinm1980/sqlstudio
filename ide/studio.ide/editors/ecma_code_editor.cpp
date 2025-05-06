/* 
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
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

#include "ecma_code_editor.h"

DEFAULT_LOG_DOMAIN("ECMA code editor");

using namespace bec;
using namespace grt;
using namespace base;

using namespace parser;

using namespace ng;

//----------------- NgECMAEditor ------------------------------------------------------------------

NgECMAEditor::NgECMAEditor(bool noToolbar)
  : NgBaseEditor(noToolbar)
{
  //_objectNamesCache = nullptr;

  createEditorConfig();
}

//--------------------------------------------------------------------------------------------------

NgECMAEditor::~NgECMAEditor()
{
}

//--------------------------------------------------------------------------------------------------

void NgECMAEditor::showCodeCompletion(bool auto_choose_single)
{
  if (!isCodeCompletionEnabled())
    return;

  _codeEditor->auto_completion_options(true, auto_choose_single, false, true, false);

  // Get the statement and its absolute position.
  std::size_t caretPosition = _codeEditor->get_caret_pos();
  std::size_t caretLine = _codeEditor->line_from_position(caretPosition);

  ssize_t lineStart, lineEnd;
  _codeEditor->get_range_of_line(caretLine, lineStart, lineEnd);
  std::size_t caretOffset = caretPosition - lineStart; // This is a byte offset.

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
  /*
  _codeCompletionEntries = getCodeCompletionList(caretLine, caretOffset, writtenPart, _currentSchema,
    makeKeywordsUppercase(), _parserContext->createScanner(statement),
    _editor_config->get_keywords()["Functions"], _objectNamesCache);
   */
  updateCodeCompletion(writtenPart);
}

//--------------------------------------------------------------------------------------------------

void NgECMAEditor::setupCodeCompletion()
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
 * If there's a double quote char then text until this quote char is returned. If there's
 * no quoting char but a space or dot char then everything up to (but not including) this is returned.
 */
std::string NgECMAEditor::getWrittenPart(std::size_t position)
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
    if (*run == '\'' || *run == '"')
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

void NgECMAEditor::splitStatementsIfRequired()
{
  if (_splittingRequired)
  {
    logDebug3("Start splitting\n");
    _splittingRequired = false;

    double start = timestamp();

    // Here we use no dedicated splitter but simply parse the (usually small) full JS script,
    // and use the AST to collect the individual statements.
    _parser.parse(_textInfo.first, _textInfo.second, true);
    //std::string temp = _parser.dumpTree();
    //printf("%s", temp.c_str());

    {
      base::RecMutexLock lock(_statementBordersMutex);
      _statementRanges = _parser.statementRanges();
    }

    logDebug3("Splitting ended after %f ticks\n", timestamp() - start);
  }
}

//--------------------------------------------------------------------------------------------------

void NgECMAEditor::doSyntaxCheck()
{
  base::RecMutexLock lock(_errorsMutex);

  for (auto &error : _parser.errorInfo())
  {
    if (_stopProcessing)
      return;
    _recognitionErrors.push_back({ error.message, error.charOffset, error.line, error.length });
  }
}

//--------------------------------------------------------------------------------------------------

void NgECMAEditor::createEditorConfig()
{
  if (_editorConfig != NULL)
    delete _editorConfig;

  _editorConfig = new mforms::CodeEditorConfig(mforms::LanguageJS);
  _codeEditor->set_language(mforms::LanguageJS);
}

//--------------------------------------------------------------------------------------------------

