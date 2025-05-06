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

#include "base/log.h"
#include "base/string_utilities.h"
#include "base/threaded_timer.h"
#include "base/util_functions.h"

#include "grt/grt_manager.h"
#include "grt/grt_threaded_task.h"

#include "grtui/file_charset_dialog.h"

#include "mforms/code_editor.h"
#include "mforms/find_panel.h"
#include "mforms/toolbar.h"
#include "mforms/menu.h"
#include "mforms/filechooser.h"

#include "grts/structs.db.mysql.h"

#include "code_editor_base.h"
#include "mysql_code_editor.h"
#include "ecma_code_editor.h"

DEFAULT_LOG_DOMAIN("Code editor");

using namespace bec;
using namespace grt;
using namespace base;

using namespace parser;
using namespace ng;

//--------------------------------------------------------------------------------------------------

const std::string ng::toString(EditorLanguage value)
{
  static std::string strings[] = { "MySQL", "Javascript", "Python" };
  return strings[unsigned(value)]; // Casting to underlying type. Update this line if the type changes.
}

//--------------------------------------------------------------------------------------------------

const std::string ng::toShortString(EditorLanguage value)
{
  static std::string strings[] = { "SQL", "JS", "PY" };
  return strings[unsigned(value)];
}

//--------------------------------------------------------------------------------------------------

class NgBaseEditor::Private
{
public:
  mforms::Box* _container;
  mforms::Menu* _editorContextMenu;
  mforms::Menu* _editorTextSubmenu;

  mforms::ToolBar* _toolbar;

  int _lastTypedChar;

  // We use 2 timers here for delayed work. One is a grt timer to run a task in the main thread after a certain delay.
  // The other one is to run the actual work task in a background thread.
  bec::GRTManager::Timer* _currentDelayTimer;
  int _currentWorkTimer;

  std::set<size_t> _errorMarkerLines;

  bool _updatingStatementMarkers;
  std::set<size_t> _statementMarkerLines;

  boost::signals2::signal<void ()> _text_change_signal;

  Private()
  {
    _currentDelayTimer = NULL;
    _currentWorkTimer = -1;

    _container = NULL;
    _editorTextSubmenu = NULL;
    _editorContextMenu = NULL;
    _toolbar = NULL;
    _lastTypedChar = 0;
    _updatingStatementMarkers = false;
  }

  //------------------------------------------------------------------------------------------------

  /**
  * One or more markers on that line where changed. We have to stay in sync with our statement markers list
  * to make the optimized add/remove algorithm working.
  */
  void markerDidChange(const mforms::LineMarkupChangeset &changeset, bool deleted)
  {
    if (_updatingStatementMarkers || changeset.empty())
      return;

    if (deleted)
    {
      for (auto &entry : changeset)
      {
        if ((entry.markup & mforms::LineMarkupStatement) != 0)
          _statementMarkerLines.erase(entry.original_line);
        if ((entry.markup & mforms::LineMarkupError) != 0)
          _errorMarkerLines.erase(entry.original_line);
      }
    }
    else
    {
      for (auto &entry : changeset)
      {
        if ((entry.markup & mforms::LineMarkupStatement) != 0)
          _statementMarkerLines.erase(entry.original_line);
        if ((entry.markup & mforms::LineMarkupError) != 0)
          _errorMarkerLines.erase(entry.original_line);
      }

      for (auto &entry : changeset)
      {
        if ((entry.markup & mforms::LineMarkupStatement) != 0)
          _statementMarkerLines.insert(entry.new_line);
        if ((entry.markup & mforms::LineMarkupError) != 0)
          _errorMarkerLines.insert(entry.new_line);
      }
    }
  }

  //------------------------------------------------------------------------------------------------

};

//--------------------------------------------------------------------------------------------------

/**
 * Creates a code editor backend + UI depending on the given language. Some of the parameters are only
 * valid for certain editors.
 */
NgBaseEditor::Ref NgBaseEditor::create(EditorLanguage language, GrtVersionRef version,
  GrtCharacterSetsRef charsets, bool caseSensitive, bool noToolbar)
{
  switch (language)
  {
    case EditorLanguage::MySQL:
    {
      NgMySQLEditor::Ref editor = NgMySQLEditor::Ref(new NgMySQLEditor(version, charsets, caseSensitive, noToolbar));
      editor->setupCodeCompletion(); // Virtual call, hence outside of the c-tor.
      return editor;
    }

    case EditorLanguage::ECMA:
    {
      NgECMAEditor::Ref editor = NgECMAEditor::Ref(new NgECMAEditor(noToolbar));
      editor->setupCodeCompletion();
      return editor;
    }

    default:
      return Ref();
  }
  return Ref();
}

//--------------------------------------------------------------------------------------------------

NgBaseEditor::NgBaseEditor(bool noToolbar) : _editorConfig(nullptr)
{
  d = new Private();
  

  _codeEditor = new mforms::CodeEditor(this);
  _codeEditor->set_font(bec::GRTManager::get().get_app_option_string("studio.general.Editor:Font"));
  _codeEditor->set_features(mforms::FeatureUsePopup, false);
  _codeEditor->set_features(mforms::FeatureConvertEolOnPaste | mforms::FeatureAutoIndent, true);
  _codeEditor->set_name("Code Editor");

  scoped_connect(_codeEditor->signal_changed(), boost::bind(&NgBaseEditor::textChanged, this, _1, _2, _3, _4));
  scoped_connect(_codeEditor->signal_char_added(), boost::bind(&NgBaseEditor::charAdded, this, _1));
  scoped_connect(_codeEditor->signal_dwell(), boost::bind(&NgBaseEditor::dwellEvent, this, _1, _2, _3, _4));
  scoped_connect(_codeEditor->signal_marker_changed(), boost::bind(&NgBaseEditor::Private::markerDidChange, d, _1, _2));

  _splittingRequired = false;
  _noToolbar = noToolbar;

  setupEditorMenu();
}

//--------------------------------------------------------------------------------------------------

NgBaseEditor::~NgBaseEditor()
{
  stopProcessing();
  cancelCodeCompletion();

  {
    // We lock all mutexes for a moment here to ensure no background thread is still holding them.
    base::RecMutexLock lock1(_errorsMutex);
    base::RecMutexLock lock2(_statementBordersMutex);
  }

  delete d->_editorTextSubmenu;
  delete d->_editorContextMenu;

  if (!_noToolbar)
    delete d->_toolbar;

  delete _editorConfig;
  delete _codeEditor;

  delete d;
}

//--------------------------------------------------------------------------------------------------

mforms::CodeEditor* NgBaseEditor::editorControl() const
{
  return _codeEditor;
};

//--------------------------------------------------------------------------------------------------

static void showSpecialCharsInEditor(mforms::ToolBarItem *item, NgBaseEditor *editor)
{
  editor->showSpecialChars(item->get_checked());
}

//--------------------------------------------------------------------------------------------------

static void enableWordWrapInEditor(mforms::ToolBarItem *item, NgBaseEditor *editor)
{
  editor->enableWordWrap(item->get_checked());
}

//--------------------------------------------------------------------------------------------------

static void showFindPanelInEditor(NgBaseEditor *editor)
{
  editor->editorControl()->show_find_panel(false);
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::openFile()
{
  mforms::FileChooser fc(mforms::OpenFile);
  if (fc.run_modal())
  {
    std::string file = fc.get_path();

    gchar *contents;
    gsize length;
    GError *error = NULL;

    if (g_file_get_contents(file.c_str(), &contents, &length, &error))
    {
      char *converted;

      if (FileCharsetDialog::ensure_filedata_utf8(contents, length, "", file, converted))
      {
        _codeEditor->set_text_keeping_state(converted ? converted : contents);
        g_free(contents);
        g_free(converted);
      }
      else
      {
        g_free(contents);
        _codeEditor->set_text(_("Data is not UTF8 encoded and cannot be displayed."));
      }
    }
    else if (error)
    {
      mforms::Utilities::show_error("Load File", base::strfmt("Could not load file %s:\n%s", file.c_str(), error->message),
                                    "OK");
      g_error_free(error);
    }
  }
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::saveFile()
{
  mforms::FileChooser fc(mforms::SaveFile);
  fc.set_extensions("SQL Scripts (*.sql)|*.sql", "sql");
  
  if (fc.run_modal())
  {
    GError *error = NULL;
    std::string file = fc.get_path();

    std::pair<const char*, size_t> data = _codeEditor->get_text_ptr();

    if (!g_file_set_contents(file.c_str(), data.first, (gssize)data.second, &error) && error)
    {
      mforms::Utilities::show_error("Save File", base::strfmt("Could not save to file %s:\n%s", file.c_str(), error->message),
                                    "OK");
      g_error_free(error);
    }
  }
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::setBaseToolbar(mforms::ToolBar *toolbar)
{
  mforms::ToolBarItem *item;

  item = mforms::manage(new mforms::ToolBarItem(mforms::ActionItem));
  item->set_name("query.search");
  item->set_icon(IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_find.png"));
  item->set_tooltip(_("Show the Find panel for the editor"));
  scoped_connect(item->signal_activated(), boost::bind(showFindPanelInEditor, this));
  toolbar->add_item(item);

  item = mforms::manage(new mforms::ToolBarItem(mforms::ToggleItem));
  item->set_name("query.toggleInvisible");
  item->set_alt_icon(IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_special-chars-on.png"));
  item->set_icon(IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_special-chars-off.png"));
  item->set_tooltip(_("Toggle display of invisible characters (spaces, tabs, newlines)"));
  scoped_connect(item->signal_activated(), boost::bind(showSpecialCharsInEditor, item, this));
  toolbar->add_item(item);

  item = mforms::manage(new mforms::ToolBarItem(mforms::ToggleItem));
  item->set_name("query.toggleWordWrap");
  item->set_alt_icon(IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_word-wrap-on.png"));
  item->set_icon(IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_word-wrap-off.png"));
  item->set_tooltip(_("Toggle wrapping of long lines (keep this off for large files)"));
  scoped_connect(item->signal_activated(),boost::bind(enableWordWrapInEditor, item, this));
  toolbar->add_item(item);
}

//--------------------------------------------------------------------------------------------------

static void embedFindPanel(mforms::CodeEditor *editor, bool show, mforms::Box *container)
{
  mforms::View *panel = editor->get_find_panel();
  if (show)
  {
    if (!panel->get_parent())
      container->add(panel, false, true);
  }
  else
  {
    container->remove(panel);
    editor->focus();
  }
}

//--------------------------------------------------------------------------------------------------

mforms::View* NgBaseEditor::container()
{
  if (!d->_container)
  {
    d->_container = new mforms::Box(false);

    if (!_noToolbar)
      d->_container->add(toolbar(), false, true);

    editorControl()->set_show_find_panel_callback(boost::bind(embedFindPanel, _1, _2, d->_container));
    d->_container->add_end(editorControl(), true, true);
  }
  return d->_container;
};


//--------------------------------------------------------------------------------------------------

mforms::ToolBar* NgBaseEditor::toolbar(bool include_file_actions)
{
  if (!d->_toolbar)
  {
    d->_toolbar = new mforms::ToolBar(mforms::SecondaryToolBar);
#ifdef _WIN32
    d->_toolbar->set_size(-1, 27);
#endif
    if (include_file_actions)
    {
      mforms::ToolBarItem *item;

      item = mforms::manage(new mforms::ToolBarItem(mforms::ActionItem));
      item->set_name("query.openFile");
      item->set_icon(IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_open.png"));
      item->set_tooltip(_("Open a script file in this editor"));
      scoped_connect(item->signal_activated(), boost::bind(&NgBaseEditor::openFile, this));
      d->_toolbar->add_item(item);

      item = mforms::manage(new mforms::ToolBarItem(mforms::ActionItem));
      item->set_name("query.saveFile");
      item->set_icon(IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_save.png"));
      item->set_tooltip(_("Save the script to a file."));
      scoped_connect(item->signal_activated(),boost::bind(&NgBaseEditor::saveFile, this));
      d->_toolbar->add_item(item);

      d->_toolbar->add_item(mforms::manage(new mforms::ToolBarItem(mforms::SeparatorItem)));
      
    }
    setBaseToolbar(d->_toolbar);
  }
  return d->_toolbar;
};

//--------------------------------------------------------------------------------------------------

mforms::CodeEditorConfig* NgBaseEditor::editorSettings() const
{
  return _editorConfig;
}

//--------------------------------------------------------------------------------------------------

/**
 * Returns the text of the editor. Usage of this function is discouraged because it copies the
 * (potentially) large editor content. Use contentAsRef() instead if possible.
 */
std::string NgBaseEditor::content() const
{
  return _codeEditor->get_text(false);
}

//--------------------------------------------------------------------------------------------------

/**
 * Returns a direct pointer to the editor content, which is only valid until the next change.
 * So if you want to keep it for longer copy the text.
 * Note: since the text can be large don't do this unless absolutely necessary.
 */
std::pair<const char*, std::size_t> NgBaseEditor::contentAsRef() const
{
  return _codeEditor->get_text_ptr();
}

//--------------------------------------------------------------------------------------------------

/**
 * Used to the set the content of the editor from outside (e.g. when loading a file or for tests).
 */
void NgBaseEditor::content(const char *text)
{
  // Wait if the text is actually being processed.
  RecMutexLock sql_statement_borders_mutex(_statementBordersMutex);

  _codeEditor->set_text(text);
  _splittingRequired = true;
  d->_statementMarkerLines.clear();
  _codeEditor->set_eol_mode(mforms::EolLF, true);
}

//----------------------------------------------------------------------------------------------------------------------

bool NgBaseEditor::empty() const
{
  return _codeEditor->text_length() == 0;
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::appendText(const std::string &text)
{
  _codeEditor->append_text(text.data(), text.size());
}

//----------------------------------------------------------------------------------------------------------------------

std::size_t NgBaseEditor::caretPosition() const
{
  return _codeEditor->get_caret_pos();
}

//----------------------------------------------------------------------------------------------------------------------

/**
 * Returns the caret position as column/row pair. The returned column (char index) is utf-8 safe and computes
 * the actual character index as displayed in the editor, not the byte index in a std::string.
 * If @local is true then the line position is relative to the statement, otherwise that in the entire editor.
 */
std::pair<std::size_t, std::size_t> NgBaseEditor::caretPositionRowColumn(bool local)
{
  size_t position = _codeEditor->get_caret_pos();
  ssize_t line = _codeEditor->line_from_position(position);
  ssize_t line_start, line_end;
  _codeEditor->get_range_of_line(line, line_start, line_end);

  ssize_t offset = position - line_start; // This is a byte offset.
  std::string line_text = _codeEditor->get_text_in_range(line_start, line_end);
  offset = g_utf8_pointer_to_offset(line_text.c_str(), line_text.c_str() + offset);

  if (local)
  {
    std::size_t min, max;
    if (currentStatementRange(min, max))
      line -= _codeEditor->line_from_position(min);
  }
  
  return std::make_pair(offset, line);
}

//----------------------------------------------------------------------------------------------------------------------

void NgBaseEditor::caretPosition(std::size_t position)
{
  _codeEditor->set_caret_pos(position);
}

//----------------------------------------------------------------------------------------------------------------------

bool NgBaseEditor::selectedRange(std::size_t &start, std::size_t &end) const
{
  std::size_t length;
  _codeEditor->get_selection(start, length);
  end = start + length;
  return length > 0;
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::selectRange(std::size_t start, std::size_t end)
{
  _codeEditor->set_selection(start, end - start);
}

//--------------------------------------------------------------------------------------------------

boost::signals2::signal<void ()>* NgBaseEditor::text_change_signal() const
{
  return &d->_text_change_signal;
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::textChanged(int position, int length, int lines_changed, bool added)
{
  stopProcessing();
  if (_codeEditor->auto_completion_active() && !added)
  {
    // Update auto completion list if a char was removed, but not added.
    // When adding a char the caret is not yet updated leading to strange behavior.
    // So we use a different notification for adding chars.
    updateCodeCompletion(getWrittenPart(position));
  }
  
  _splittingRequired = true;
  _textInfo = _codeEditor->get_text_ptr();
  d->_currentDelayTimer = bec::GRTManager::get().run_every(boost::bind(&NgBaseEditor::startCodeProcessing, this), 0.05);
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::charAdded(int char_code)
{
  if (!_codeEditor->auto_completion_active())
      d->_lastTypedChar = char_code; // UTF32 encoded char.
  else
    updateCodeCompletion(getWrittenPart(_codeEditor->get_caret_pos()));
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::dwellEvent(bool started, std::size_t position, int x, int y)
{
  if (started)
  {
    if (_codeEditor->indicator_at(position) == mforms::RangeIndicatorError)
    {
      // TODO: sort by position and do a binary search.
      for (size_t i = 0; i < _recognitionErrors.size(); ++i)
      {
        ParserErrorEntry entry = _recognitionErrors[i];
        if (entry.position <= position && position <= entry.position + entry.length)
        {
          _codeEditor->show_calltip(true, position, entry.message);
          break;
        }
      }
    }
  }
  else
    _codeEditor->show_calltip(false, 0, "");
}

//--------------------------------------------------------------------------------------------------

/**
 * Prepares and triggers an code check run. Runs in the context of the main thread.
 */
bool NgBaseEditor::startCodeProcessing()
{
  // Here we trigger our text change signal, to avoid frequent signals for each key press.
  // Consumers are expected to use this signal for UI updates, so we need to coalesce messages.
  d->_text_change_signal();

  d->_currentDelayTimer = NULL; // The timer will be deleted by the grt manager.

  {
    RecMutexLock sql_errors_mutex(_errorsMutex);
    _recognitionErrors.clear();
  }

  _stopProcessing = false;

  _codeEditor->set_status_text("");
  if (!empty())
    d->_currentWorkTimer = ThreadedTimer::get()->add_task(TimerTimeSpan, 0.05, true,
      boost::bind(&NgBaseEditor::doStatementSplitAndCheck, this, _1));
  return false; // Don't re-run this task, it's a single-shot.
}

//--------------------------------------------------------------------------------------------------

bool NgBaseEditor::doStatementSplitAndCheck(int id)
{
  // TODO: there's no need always split and error-check all text in the editor.
  //       Only split and scan from the current caret position.
  splitStatementsIfRequired();
  
  // Start tasks that depend on the statement ranges (markers + auto completion).
  bec::GRTManager::get().run_once_when_idle(this, boost::bind(&NgBaseEditor::splittingDone, this));

  if (_stopProcessing)
    return false;

  doSyntaxCheck();

  if (_stopProcessing)
    return false;

  bec::GRTManager::get().run_once_when_idle(this, boost::bind(&NgBaseEditor::updateErrorMarkers, this));

  return false;
}

//--------------------------------------------------------------------------------------------------

/**
 * Updates the statement markup and starts auto completion if enabled. This is called in the
 * context of the main thread.
 */
void* NgBaseEditor::splittingDone()
{
  // Trigger auto completion for certain keys (if enabled).
  // This has to be done after our statement  splitter has completed (which is the case when we appear here).
  if (autoStartCodeCompletion() && !_codeEditor->auto_completion_active() &&
      (g_unichar_isalnum(d->_lastTypedChar) || d->_lastTypedChar == '.' || d->_lastTypedChar == '@'))
  {
    d->_lastTypedChar = 0;
    showCodeCompletion(false);
  }

  std::set<size_t> removal_candidates;
  std::set<size_t> insert_candidates;

  std::set<size_t> lines;
  for (auto &range :_statementRanges)
    lines.insert(_codeEditor->line_from_position(range.first));

  std::set_difference(lines.begin(), lines.end(), d->_statementMarkerLines.begin(), d->_statementMarkerLines.end(),
    inserter(insert_candidates, insert_candidates.begin()));

  std::set_difference(d->_statementMarkerLines.begin(), d->_statementMarkerLines.end(), lines.begin(), lines.end(),
    inserter(removal_candidates, removal_candidates.begin()));

  d->_statementMarkerLines.swap(lines);

  d->_updatingStatementMarkers = true;
  for (auto &candidate : removal_candidates)
    _codeEditor->remove_markup(mforms::LineMarkupStatement, candidate);

  for (auto &candidate : insert_candidates)
    _codeEditor->show_markup(mforms::LineMarkupStatement, candidate);
  d->_updatingStatementMarkers = false;

  return NULL;
}

//--------------------------------------------------------------------------------------------------

void* NgBaseEditor::updateErrorMarkers()
{
  std::set<size_t> removal_candidates;
  std::set<size_t> insert_candidates;

  std::set<size_t> lines;

  _codeEditor->remove_indicator(mforms::RangeIndicatorError, 0, _codeEditor->text_length());
  if (_recognitionErrors.size() > 0)
  {
    if (_recognitionErrors.size() == 1)
      _codeEditor->set_status_text(_("1 error found"));
    else
      _codeEditor->set_status_text(base::strfmt(_("%lu errors found"), (unsigned long)_recognitionErrors.size()));

    for (size_t i = 0; i < _recognitionErrors.size(); ++i)
    {
      _codeEditor->show_indicator(mforms::RangeIndicatorError, _recognitionErrors[i].position, _recognitionErrors[i].length);
      lines.insert(_codeEditor->line_from_position(_recognitionErrors[i].position));
    }
  }
  else
    _codeEditor->set_status_text("");

  std::set_difference(lines.begin(), lines.end(), d->_errorMarkerLines.begin(), d->_errorMarkerLines.end(),
    inserter(insert_candidates, insert_candidates.begin()));

  std::set_difference(d->_errorMarkerLines.begin(), d->_errorMarkerLines.end(), lines.begin(), lines.end(),
    inserter(removal_candidates, removal_candidates.begin()));

  d->_errorMarkerLines.swap(lines);

  for (std::set<size_t>::const_iterator iterator = removal_candidates.begin();
    iterator != removal_candidates.end(); ++iterator)
    _codeEditor->remove_markup(mforms::LineMarkupError, *iterator);

  for (std::set<size_t>::const_iterator iterator = insert_candidates.begin();
    iterator != insert_candidates.end(); ++iterator)
    _codeEditor->show_markup(mforms::LineMarkupError, *iterator);

  return NULL;
}

//--------------------------------------------------------------------------------------------------

/**
 * Updates the code completion list by filtering the determined entries by the text the user
 * already typed. If code completion is not yet active it becomes active here.
 * Returns the list sent to the editor for unit tests to validate them.
 */
std::vector<std::pair<int, std::string>> NgBaseEditor::updateCodeCompletion(const std::string &typed_part)
{
  logDebug2("Updating auto completion popup in editor\n");

  // Remove all entries that don't start with the typed text before showing the list.
  if (!typed_part.empty())
  {
    gchar *prefix = g_utf8_casefold(typed_part.c_str(), -1);

    std::vector<std::pair<int, std::string>> filteredEntries;
    for (auto &entry : _codeCompletionEntries)
    {
      gchar *folded = g_utf8_casefold(entry.second.c_str(), -1);
      if (g_str_has_prefix(folded, prefix))
        filteredEntries.push_back(entry);
      g_free(folded);
    }

    switch (filteredEntries.size())
    {
      case 0:
        logDebug2("Nothing to autocomplete - hiding popup if it was active\n");
        _codeEditor->auto_completion_cancel();
        break;
      case 1:
        // See if that single entry matches the typed part. If so we don't need to show ac either.
        if (base::same_string(filteredEntries[0].second, prefix, false)) // Exact (but case insensitive) match, not just string parts.
        {
          logDebug2("The only match is the same as the written input - hiding popup if it was active\n");
          _codeEditor->auto_completion_cancel();
          break;
        }
        /* no break */
        // Fall through.
      default:
        logDebug2("Showing auto completion popup\n");
        _codeEditor->auto_completion_show(typed_part.size(), filteredEntries);
        break;
    }

    g_free(prefix);

    return filteredEntries;
  }
  else
  {
    if (!_codeCompletionEntries.empty())
    {
      logDebug2("Showing auto completion popup\n");
      _codeEditor->auto_completion_show(0, _codeCompletionEntries);
    }
    else
    {
      logDebug2("Nothing to autocomplete - hiding popup if it was active\n");
      _codeEditor->auto_completion_cancel();
    }
  }

  return _codeCompletionEntries;
}

//--------------------------------------------------------------------------------------------------


void NgBaseEditor::cancelCodeCompletion()
{
  // Make sure a pending timed code completion won't kick in after we cancel it.
  d->_lastTypedChar = 0;
  _codeEditor->auto_completion_cancel();
}

//--------------------------------------------------------------------------------------------------

std::string NgBaseEditor::selectedText() const
{
  return _codeEditor->get_text(true);
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::replaceSelectedText(const std::string &new_text)
{
  _codeEditor->replace_selected_text(new_text);
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::insertText(const std::string &new_text)
{
  _codeEditor->clear_selection();
  _codeEditor->replace_selected_text(new_text);
}

//--------------------------------------------------------------------------------------------------

/**
 * Returns the statement at the current caret position.
 */
std::string NgBaseEditor::currentStatement()
{
  std::size_t min, max;
  if (currentStatementRange(min, max))
    return _codeEditor->get_text_in_range(min, max);
  return "";
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::setupEditorMenu()
{
  d->_editorContextMenu = new mforms::Menu();
  scoped_connect(d->_editorContextMenu->signal_will_show(), boost::bind(&NgBaseEditor::editorMenuOpening, this));
  
  d->_editorContextMenu->add_item(_("Undo"), "undo");
  d->_editorContextMenu->add_item(_("Redo"), "redo");
  d->_editorContextMenu->add_separator();
  d->_editorContextMenu->add_item(_("Cut"), "cut");
  d->_editorContextMenu->add_item(_("Copy"), "copy");
  d->_editorContextMenu->add_item(_("Paste"), "paste");
  d->_editorContextMenu->add_item(_("Delete"), "delete");
  d->_editorContextMenu->add_separator();
  d->_editorContextMenu->add_item(_("Select All"), "select_all");

  std::list<std::string> groups;
  groups.push_back("Menu/Text");

  bec::MenuItemList plugin_items;
  bec::ArgumentPool argpool;
  argpool.add_simple_value("selectedText", grt::StringRef(""));
  argpool.add_simple_value("document", grt::StringRef(""));

  _codeEditor->set_context_menu(d->_editorContextMenu);
  scoped_connect(d->_editorContextMenu->signal_on_action(), boost::bind(&NgBaseEditor::activateContextMenuItem, this, _1));
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::editorMenuOpening()
{
  int index = d->_editorContextMenu->get_item_index("undo");
  d->_editorContextMenu->set_item_enabled(index, _codeEditor->can_undo());
  index = d->_editorContextMenu->get_item_index("redo");
  d->_editorContextMenu->set_item_enabled(index, _codeEditor->can_redo());
  index = d->_editorContextMenu->get_item_index("cut");
  d->_editorContextMenu->set_item_enabled(index, _codeEditor->can_cut());
  index = d->_editorContextMenu->get_item_index("copy");
  d->_editorContextMenu->set_item_enabled(index, _codeEditor->can_copy());
  index = d->_editorContextMenu->get_item_index("paste");
  d->_editorContextMenu->set_item_enabled(index, _codeEditor->can_paste());
  index = d->_editorContextMenu->get_item_index("delete");
  d->_editorContextMenu->set_item_enabled(index, _codeEditor->can_delete());
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::activateContextMenuItem(const std::string &name)
{
  // Standard commands first.
  if (name == "undo")
    _codeEditor->undo();
  else if (name == "redo")
    _codeEditor->redo();
  else if (name == "cut")
    _codeEditor->cut();
  else if (name == "copy")
    _codeEditor->copy();
  else if (name == "paste")
    _codeEditor->paste();
  else if (name == "delete")
    _codeEditor->replace_selected_text("");
  else if (name == "select_all")
    _codeEditor->set_selection(0, _codeEditor->text_length());
  else {
    logWarning("Unhandled context menu item %s", name.c_str());
  }
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::showSpecialChars(bool flag)
{
  _codeEditor->set_features(mforms::FeatureShowSpecial, flag);
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::enableWordWrap(bool flag)
{
  _codeEditor->set_features(mforms::FeatureWrapText, flag);
}

//--------------------------------------------------------------------------------------------------

bool NgBaseEditor::isCodeCompletionEnabled()
{
  return bec::GRTManager::get().get_app_option_int("DbSqlEditor:CodeCompletionEnabled") == 1;
}

//--------------------------------------------------------------------------------------------------

bool NgBaseEditor::autoStartCodeCompletion()
{
  return bec::GRTManager::get().get_app_option_int("DbSqlEditor:AutoStartCodeCompletion") == 1;
}

//--------------------------------------------------------------------------------------------------

bool NgBaseEditor::makeKeywordsUppercase()
{
  return bec::GRTManager::get().get_app_option_int("DbSqlEditor:CodeCompletionUpperCaseKeywords") == 1;
}

//--------------------------------------------------------------------------------------------------

/**
 * Determines the start and end position of the current statement, that is, the statement
 * where the caret is in. For effective search in a large set binary search is used.
 *
 * Note: search can be done in two modes:
 *       - strict: whitespaces before a statement belong to that statement.
 *       - loose: such whitespaces belong to the previous statement (and are ignored).
 *       Loose mode allows to have the caret in the whitespaces after a statement and execute that,
 *       while strict mode is needed for code completion (should be done for the following statement then).
 *
 * @returns true if a statement could be found at the caret position, otherwise false.
 */
bool NgBaseEditor::currentStatementRange(std::size_t &start, std::size_t &end, bool strict)
{
  // In case the splitter is right now processing the text we wait here until its done.
  // If the splitter wasn't triggered yet (e.g. when typing fast and then immediately running a statement)
  // then we do the splitting here instead.
  RecMutexLock sql_statement_borders_mutex(_statementBordersMutex);
  splitStatementsIfRequired();

  if (_statementRanges.empty())
    return false;

  typedef std::vector<std::pair<size_t, size_t> >::iterator RangeIterator;

  size_t caret_position = _codeEditor->get_caret_pos();
  RangeIterator low = _statementRanges.begin();
  RangeIterator high = _statementRanges.end() - 1;
  while (low < high)
  {
    RangeIterator middle = low + (high - low + 1) / 2;
    if (middle->first > caret_position)
      high = middle - 1;
    else
    {
      size_t end = low->first + low->second;
      if (end >= caret_position)
        break;
      low = middle;
    }
  }

  if (low == _statementRanges.end())
    return false;

  // If we are between two statements (in white spaces) then the algorithm above returns the lower one.
  if (strict)
  {
    if (low->first + low->second < caret_position)
      ++low;
    if (low == _statementRanges.end())
      return false;
  }

  start = low->first;
  end = low->first + low->second;
  return true;
}

//--------------------------------------------------------------------------------------------------

bool NgBaseEditor::hasSyntaxErrors() const
{
  return !_recognitionErrors.empty();
}

//--------------------------------------------------------------------------------------------------

/**
 * Stops any ongoing processing like splitting, syntax checking etc.
 */
void NgBaseEditor::stopProcessing()
{
  _stopProcessing = true;

  ThreadedTimer::get()->remove_task(d->_currentWorkTimer);
  d->_currentWorkTimer = -1;

  if (d->_currentDelayTimer != NULL)
  {
    bec::GRTManager::get().cancel_timer(d->_currentDelayTimer);
    d->_currentDelayTimer = NULL;
  }
}

//--------------------------------------------------------------------------------------------------

void NgBaseEditor::focus()
{
  _codeEditor->focus();
}

//--------------------------------------------------------------------------------------------------

/**
 * Register a target for file drop operations which will handle these cases.
 */
void NgBaseEditor::registerFileDropFor(mforms::DropDelegate *target)
{
  std::vector<std::string> formats;
  formats.push_back(mforms::DragFormatFileName);
  _codeEditor->register_drop_formats(target, formats);
}

//--------------------------------------------------------------------------------------------------
