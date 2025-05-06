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
#include <string>
#include <thread>
#endif

#include "grt/grt_manager.h"
#include "shellcore/shell_core.h"
#include "driver_manager.h"

#include "mforms/textbox.h"
#include "mforms/tabview.h"
#include "mforms/box.h"

#include "modules/base_session.h"

#include "editors/code_editor_base.h"
#include "endless_control.h"

namespace ng
{
  using weakSessionRef = boost::weak_ptr<mysh::ShellBaseSession>;
  using sharedSessionRef = boost::shared_ptr<mysh::ShellBaseSession>;

  class NgEditorContainer : public mforms::Box
  {
  protected:
    EditorLanguage _type;
    mforms::TextBox _output;
    mforms::TabView _resultTab;

    weakSessionRef _session;
    shcore::Interpreter_delegate _delegate;
    std::shared_ptr<shcore::Shell_core> _shell;

    EndlessControl _endlessControl;

    bool _isScratch;

    std::string _filename;
    std::string _title;

    void setupUI(EditorLanguage type);
    void processJSON(const shcore::Value &result);
    void processSql(const shcore::Value &result);

  public:
    NgEditorContainer(EditorLanguage type);
    ~NgEditorContainer();

    void setSession(sharedSessionRef session);
    void appendOutput(const std::string &text);
    EditorLanguage getType();
    void execute(bool currentStatementOnly);
    void executeCommand(std::string script);
    void processResult(shcore::Value result);

    void triggerCodeCompletion();

    static void print(void *self, const char *text);
    static void printError(void *self, const char *text);
    static bool getPassword(void *self, const char *text, std::string &ret);
    static void source(void *self, const char *module);
    static bool prompt(void *self, const char *text, std::string &ret);
    static void onCreateSession(void *self, const shcore::Value &session);
    std::function<void(const shcore::Value&)> _onCreateSession;
    void focus();
  };
}
