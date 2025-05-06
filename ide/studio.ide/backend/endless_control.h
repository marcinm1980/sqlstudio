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

#pragma once

#ifndef HAVE_PRECOMPILED_HEADERS
#include <deque>
#include <functional>
#include <thread>
#endif

#include "ng_public_interface.h"
#include "mforms/scrollpanel.h"
#include "editors/code_editor_base.h"

namespace shcore {
  struct Value;
}

namespace JsonParser {
  class JsonValue;
}

namespace mforms { class CodeEditor; }

namespace ng {

  class NG_PUBLIC_TYPE EndlessControl : public mforms::ScrollPanel
  {
  public:
    EndlessControl(bool scriptMode = false);
    ~EndlessControl();

    void setExecuteCallback(std::function<void(const std::string& command)> function);
    void resizeEditor(int linesCount) const;
    void editorChanged(int position, int length, int linesCount, bool inserted) const;
    void printError(const std::string &text);
    void processJson(const JsonParser::JsonValue &val);
    void processSql(const shcore::Value& value);
    void appendText(const std::string& text);
    void processText();
    void setLanguage(EditorLanguage type);
    std::string getCurrentStatement(bool currentStatementOnly);
    void focus();

  private:
    void createEditor();
    void showJsonOutput(const JsonParser::JsonValue& value) const;
    void showErrorLog(const std::string& text) const;
    void showRecorSetOutput(const shcore::Value& value);
    void showTextOutput() const;
    void showNewEditor();
    void execute() const;
    void displayHistoryEntry(std::string value);
    bool keyEvent(mforms::KeyCode code, mforms::ModifierKey modifier, const std::string& text);
    View* createRecordSetView() const;

    bool _scriptMode;
    mforms::CodeEditor *_currentEditor;
    std::deque<std::string> _history;
    bool _historyMode;
    size_t _position;
    size_t _historyEntrySize;
    mforms::Box* _master;
    boost::signals2::connection _resizeSig;
    int _index;
    std::function<void(const std::basic_string<char>&)> _execute;
    std::shared_ptr<std::thread> _resultReader;
    bool _runNextInBkg;
    bool _go;
    std::string _textOut;
    EditorLanguage _lang;
    mforms::Box* _editorBox;
  };

}
