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

#include "base/trackable.h"

#ifndef HAVE_PRECOMPILED_HEADERS

#include <memory>
#include <set>

#include "grts/structs.db.mgmt.h"
#include "grts/structs.db.query.h"

#endif

#include "grtdb/db_helpers.h"

#include "grtsqlparser/parser_services_common.h"

namespace bec {
  class GRTManager;
}

namespace mforms {
  class CodeEditor;
  class FindPanel;
  class Menu;
  class View;
  class CodeEditorConfig;
  class ToolBar;
  class DropDelegate;
};

namespace ng {

  enum class EditorLanguage : unsigned
  {
    MySQL,
    ECMA,
    Python,
  };
  const std::string toString(EditorLanguage value);
  const std::string toShortString(EditorLanguage value);

  /**
   * The base code editor class. Never instantiated directly, but used by MySQL, JS + Python editor classes.
   */
  class NG_PUBLIC_TYPE NgBaseEditor : public base::trackable
  {
  public:
    typedef std::shared_ptr<NgBaseEditor> Ref;

    static Ref create(EditorLanguage language, GrtVersionRef version,
      GrtCharacterSetsRef charsets, bool caseSensitive, bool noToolbar);

    virtual ~NgBaseEditor();

    mforms::View* container();
    mforms::ToolBar* toolbar(bool include_file_actions = true);
    mforms::CodeEditor* editorControl() const;
    mforms::FindPanel* findPanel() const;
    mforms::CodeEditorConfig* editorSettings() const;
    void setBaseToolbar(mforms::ToolBar *toolbar);

    void showSpecialChars(bool flag);
    void enableWordWrap(bool flag);

    int optionAsInt(std::string name) const;
    std::string optionAsString(std::string name) const;

    std::string content() const;
    std::pair<const char*, std::size_t> contentAsRef() const;
    void content(const char *text);

    bool empty() const;
    void appendText(const std::string &text);

    std::string currentStatement();
    bool currentStatementRange(std::size_t &start, std::size_t &end, bool strict = false);

    std::size_t caretPosition() const;
    std::pair<std::size_t, std::size_t> caretPositionRowColumn(bool local);
    void caretPosition(std::size_t position);

    bool selectedRange(std::size_t &start, std::size_t &end) const;
    void selectRange(std::size_t start, std::size_t end);

    virtual void showCodeCompletion(bool autoChooseSingle) = 0;
    std::vector<std::pair<int, std::string>> updateCodeCompletion(const std::string &typedPart);
    void cancelCodeCompletion();

    std::string selectedText() const;
    void replaceSelectedText(const std::string &newText);
    void insertText(const std::string &newText); // At caret position.

    boost::signals2::signal<void()>* text_change_signal() const; // TODO: replace by own C++11 signal impl.

    bool hasSyntaxErrors() const;
    virtual void stopProcessing();

    void focus();

    void registerFileDropFor(mforms::DropDelegate *target);

    void openFile();
    void saveFile();

  protected:
    NgBaseEditor(bool noToolbar);

    bool startCodeProcessing();

    virtual void setupCodeCompletion() = 0;
    virtual std::string getWrittenPart(std::size_t position) = 0;
    virtual void splitStatementsIfRequired() = 0;
    virtual void doSyntaxCheck() = 0;

    bool isCodeCompletionEnabled();
    bool autoStartCodeCompletion();
    bool makeKeywordsUppercase();

    bool _splittingRequired;
    bool _stopProcessing;      // To stop ongoing syntax checks (because of text changes etc.).
    bool _noToolbar;

    std::pair<const char*, size_t> _textInfo; // Only valid during a parse run.

    // Each entry is a pair of statement position (byte position) and statement length (also bytes).
    std::vector<std::pair<std::size_t, std::size_t> > _statementRanges;
    base::RecMutex _statementBordersMutex;

    base::RecMutex _errorsMutex;
    std::vector<parser::ParserErrorEntry> _recognitionErrors; // List of errors from the last syntax check run.

    // Entries determined the last time we started code completion. The actually shown list
    // is derived from these entries filtered by the current input.
    std::vector<std::pair<int, std::string> > _codeCompletionEntries;

    mforms::CodeEditor* _codeEditor;
    mforms::CodeEditorConfig *_editorConfig; // Maintained by descendants.
  private:
    class Private;
    Private *d;

    void textChanged(int position, int length, int lines_changed, bool added);
    void charAdded(int char_code);
    void dwellEvent(bool started, std::size_t position, int x, int y);

    void setupEditorMenu();
    void editorMenuOpening();
    void activateContextMenuItem(const std::string &name);

    bool doStatementSplitAndCheck(int id); // Run in worker thread.

    void* splittingDone();
    void* updateErrorMarkers();

  };
  
}
