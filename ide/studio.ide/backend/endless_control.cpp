
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

#include "base/string_utilities.h"
#include "endless_control.h"
#include "mforms/box.h"
#include "mforms/code_editor.h"
#include "mforms/jsonview.h"
#include "mforms/label.h"
#include "mforms/textbox.h"
#include "ng_recordset_view.h"
#include "ng_editor_container.h"

#include "base/drawing.h"

using namespace ng;
using namespace mforms;

//--------------------------------------------------------------------------------------------------

class ResultSetWorker
{
public:
  ResultSetWorker(boost::shared_ptr<shcore::Value::Array_type> arrayResult, std::vector<shcore::Value> *dataset,
    NgRecordSetSqlView *view, bool &goRead, std::size_t idx)
    : _arrayResult(arrayResult), _dataset(dataset), _view(view), _goRead(goRead), _idx(idx)
  {
  }

  //--------------------------------------------------------------------------------------------------

  void operator()()
  {
    for (; _idx < _dataset->size() && _goRead; ++_idx)
    {
      auto& v_row = (*_dataset)[_idx];
      auto row = v_row.as_object<mysh::Row>();
      _view->addRowSilent(*row.get());
    }
    _view->endLoad();
  }

  //--------------------------------------------------------------------------------------------------

private:
  boost::shared_ptr<shcore::Value::Array_type> _arrayResult;
  std::vector<shcore::Value> *_dataset;
  NgRecordSetSqlView *_view;
  bool &_goRead;
  std::size_t _idx;
};

//--------------------------------------------------------------------------------------------------

void EndlessControl::createEditor()
{
  _editorBox = manage(new Box(true));
  _editorBox->set_padding(4);
  _editorBox->set_name("Editor Host");

  CodeEditor *editor = manage(new CodeEditor(nullptr, false));
  editor->set_name("Code Editor");
  editor->set_font(std::string(DEFAULT_MONOSPACE_FONT_FAMILY) + " 13");

  Label *label = nullptr;
  switch (_lang)
  {
    case EditorLanguage::ECMA:
      editor->set_language(LanguageJS);
      label = manage(new Label("JS"));
      break;
    case EditorLanguage::MySQL:
      editor->set_language(LanguageMySQL57);
      label = manage(new Label("SQL"));
      break;
    case EditorLanguage::Python:
      editor->set_language(LanguagePython);
      label = manage(new Label("PY"));
      break;
  }
  if (label)
  {
    label->set_name("Code Editor Label");
    label->set_size(20, -1);
    label->set_font(std::string(DEFAULT_MONOSPACE_FONT_FAMILY) + " 13 bold");
    label->set_text_align(TopCenter);
    _editorBox->add(label, false, true);
  }

  editor->set_features(FeatureReadOnly, false);
  editor->showMargin(CodeEditor::TextMargin, true);
  editor->setColor(CodeEditor::LineNumberMargin, base::Color::White());
  editor->setWidth(CodeEditor::LineNumberMargin, 0, "_99");
  editor->setWidth(CodeEditor::TextMargin, 0, "_>");
  editor->showMargin(CodeEditor::FolderMargin, false);
  editor->showMargin(CodeEditor::MarkersMargin, false);
  editor->setMarginText(">");
  editor->setScrollWidth(1000);

  scoped_connect(editor->key_event_signal(), boost::bind(&EndlessControl::keyEvent, this, _1, _2, _3));
  scoped_connect(editor->signal_changed(), boost::bind(&EndlessControl::editorChanged, this, _1, _2, _3, _4));

  _editorBox->add(editor, true, true);

  _master->add(_editorBox, false, true);
  _currentEditor = editor;

  resizeEditor(1);
}

//--------------------------------------------------------------------------------------------------

EndlessControl::EndlessControl(bool scriptMode)
  : ScrollPanel(ScrollPanelNoFlags), _scriptMode(scriptMode), _currentEditor(nullptr),
  _position(0), _historyEntrySize(0), _master(manage(new Box(false))), _index(1), _lang(EditorLanguage::ECMA),
  _editorBox(nullptr)
{
  set_name("Endless Control");
  _master->set_name("Endless Control Main");
  createEditor();

  add(_master);

  //we need this to scroll to bottom once we create new editor
  _resizeSig = signal_resized()->connect([=]() -> void {
      scroll_to(0, _master->get_height());
  });
  _master->set_back_color("#FFFFFF");
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::focus()
{
  _currentEditor->focus();
}

//--------------------------------------------------------------------------------------------------

EndlessControl::~EndlessControl()
{
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::editorChanged(int position, int length, int linesCount, bool inserted) const
{
  if (inserted && linesCount > 0)
  {
    resizeEditor(linesCount);

    size_t row, column;
    _currentEditor->get_line_column_pos(_currentEditor->get_caret_pos(), row, column);

    // Update one more line, as we cannot be sure if the current line has been moved down (taking the
    // previous margin text with it) or if there is a new line below.
    for (int i = 0; i <= linesCount; ++i)
      _currentEditor->setMarginText(">", row + i);
  }
}


//--------------------------------------------------------------------------------------------------

void EndlessControl::showJsonOutput(const JsonParser::JsonValue &value) const
{
  Box *output = manage(new Box(true));
  output->set_back_color("#e8e8e8");
  output->set_padding(5);

  Box *left = manage(new Box(false));
  left->set_padding(0, 5, 0, 0);
  Label *label = manage(new Label("RS " + base::to_string(_index)));
  label->set_size(30, -1);
  left->add(label, false, true);

  Box *right = manage(new Box(false));
  output->set_back_color("#e8e8e8");
  NgRecordSetJsonView *view = manage(new NgRecordSetJsonView);
  if (view != nullptr)
  {
    view->addRow(value);
    view->set_size(-1, 300);
  }
  right->add(view, false, true);

  output->add(left, false, true);
  output->add(right, true, true);
  _master->add(output, false, true);
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::showErrorLog(const std::string &text) const
{
  Box *output = manage(new Box(true));
  output->set_back_color("#e8e8e8");
  output->set_padding(5);

  Box *left = manage(new Box(false));
  left->set_padding(0, 5, 0, 0);
  Label *label = manage(new Label("ER " + base::to_string(_index)));
  label->set_size(30, -1);
  left->add(label, false, true);

  Box *right = manage(new Box(false));
  output->set_back_color("#e8e8e8");
  label = manage(new Label(text));
  label->set_color("#FF0000");
  right->add(label, false, true);

  output->add(left, false, true);
  output->add(right, true, true);
  _master->add(output, false, true);
}

//--------------------------------------------------------------------------------------------------

View* EndlessControl::createRecordSetView() const
{
  Box *output = manage(new Box(true));
  output->set_back_color("#e8e8e8");
  output->set_padding(5);

  Box *left = manage(new Box(false));
  left->set_padding(0, 5, 0, 0);
  Label *label = manage(new Label("RS " + base::to_string(_index)));
  label->set_size(30, -1);
  left->add(label, false, true);

  Box *right = manage(new Box(false));
  output->set_back_color("#e8e8e8");
  auto view = mforms::manage(new NgRecordSetSqlView());
  if (view != nullptr)
    view->set_size(-1, 300);
  right->add(view, false, true);

  output->add(left, false, true);
  output->add(right, true, true);
  _master->add(output, false, true);

  return view;
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::showRecorSetOutput(const shcore::Value& result)
{
  _go = false;
  if (_resultReader != nullptr && _resultReader->joinable())
    _resultReader->join();
  _runNextInBkg = false;
  try
  {
    auto object = result.as_object();
    auto queryResult = object->call("fetchAll", shcore::Argument_list());
    auto arrResult = queryResult.as_array();
    if (arrResult->begin() != arrResult->end())
    {
      auto view = createRecordSetView();
      std::stringstream ss;
      // a table result set is an array of shcore::Value's each of them in turn is an array (vector) of shcore::Value's (escalars)
      auto dataset = arrResult.get();
      auto rsView = dynamic_cast<NgRecordSetSqlView*>(view);
      scoped_connect(rsView->pageCompleted(), [this](){ this->_runNextInBkg = true; });
      rsView->setSize(dataset->size());
      if (dataset->size() > 0)
      {
        auto& v_row = (*dataset)[0];
        auto row = v_row.as_object<mysh::Row>();
        const auto &map = row->values;

        NgRecordSetSqlView::ColumnDescription columnNames;
        for (const auto &it : map)
          columnNames.push_back(std::make_pair(row->values[it.first].type, it.first));
        rsView->setCoulmnHeaderAndType(columnNames);
      }

      std::size_t i = 0;
      for (; i < dataset->size() && !_runNextInBkg; ++i)
      {
        auto& v_row = (*dataset)[i];
        auto row = v_row.as_object<mysh::Row>();
        auto text = rsView->addRow(*row.get());
      }
      _go = true;
      _resultReader = std::make_shared<std::thread>(std::thread(std::move(ResultSetWorker(arrResult, dataset, rsView, _go, i))));
    }
  }
  catch (std::exception &exc)
  {
    showErrorLog(exc.what());
  }
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::showTextOutput() const
{
  Box *output = manage(new Box(true));
  output->set_back_color("#e8e8e8");
  output->set_padding(5);

  Box *left = manage(new Box(false));
  left->set_padding(0, 5, 0, 0);
  Label *label = manage(new Label("TXT " + base::to_string(_index)));
  label->set_size(30, -1);
  left->add(label, false, true);

  Box *right = manage(new Box(false));
  output->set_back_color("#e8e8e8");

  TextBox* textbox = manage(new TextBox(mforms::BothScrollBars));
  textbox->set_monospaced(true);
  textbox->set_bordered(false);
  textbox->set_value(_textOut);
  textbox->set_read_only(true);
  textbox->set_size(-1, 150);
  textbox->set_back_color("#e8e8e8");
  right->add(textbox, false, true);

  output->add(left, false, true);
  output->add(right, true, true);
  _master->add(output, false, true);
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::showNewEditor()
{
  _currentEditor->set_read_only(true);
  createEditor();
  _currentEditor->focus();
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::setExecuteCallback(std::function<void(const std::string &command)> function)
{
  _execute = function;
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::resizeEditor(int linesCount) const
{
  // linesCount determines a relative size to grow or shrink the editor with.
  linesCount += _currentEditor->line_count();
  int height = get_height();
  int lineHeight = _currentEditor->getLineHeight(0); // Currently all lines are the same height.
  int newHeight = std::min(10, linesCount) * lineHeight;
  if (newHeight <= height / 3)
    _editorBox->set_size(-1, newHeight);
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::execute() const
{
  if (_execute)
  {
    auto text = _currentEditor->get_text(false);
    _execute(text);
  }
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::displayHistoryEntry(std::string value)
{
  if (_historyEntrySize > 0)
  {
    _currentEditor->set_selection(_position, _historyEntrySize);
    _currentEditor->replace_selected_text(value);
    _currentEditor->set_caret_pos(_position);
  }
  else
    _currentEditor->append_text(value.c_str(), value.size());
  _historyEntrySize = value.size();
}

//--------------------------------------------------------------------------------------------------

bool EndlessControl::keyEvent(mforms::KeyCode code, mforms::ModifierKey modifier, const std::string& text)
{
#ifdef __APPLE__
  bool withModifier = modifier & mforms::ModifierCommand;
#else
  bool withModifier = modifier & mforms::ModifierControl;
#endif

  switch (code) {
    case mforms::KeyReturn:
      if (withModifier)
      {
        _history.push_back(_currentEditor->get_text(false));
        execute();
      }
      break;

    case mforms::KeyUp:
      if (withModifier)
      {
        if (_position == 0)
          _position = _currentEditor->get_caret_pos();
        if (_history.empty())
          return false;
        std::string value = *_history.crbegin();
        _history.pop_back();
        _history.push_front(value);
        displayHistoryEntry(value);
      }
      break;

    case mforms::KeyDown:
      if (withModifier)
      {
        if (_position == 0)
          _position = _currentEditor->get_caret_pos();
        if (_history.empty())
          return false;
        std::string value = *_history.begin();
        _history.pop_front();
        _history.push_back(value);
        displayHistoryEntry(value);
      }
      break;
  }
  return true;
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::printError(const std::string& text)
{
  _textOut = "";
  showErrorLog(text);
  ++_index;
  showNewEditor();
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::processJson(const JsonParser::JsonValue& val)
{
  _textOut = "";
  showJsonOutput(val);
  ++_index;
  showNewEditor();
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::processSql(const shcore::Value& value)
{
  _textOut = "";
  showRecorSetOutput(value);
  ++_index;
  showNewEditor();
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::appendText(const std::string& text)
{
  _textOut = text;
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::processText()
{
  if(!_textOut.empty())
  {
    showTextOutput();
    ++_index;
    showNewEditor();
  }
  _textOut = "";
}

//--------------------------------------------------------------------------------------------------

void EndlessControl::setLanguage(EditorLanguage type)
{
  _lang = type;
}

std::string EndlessControl::getCurrentStatement(bool currentStatementOnly)
{
  if (_currentEditor != nullptr)
    return _currentEditor->get_text(false);
  return "";
}
