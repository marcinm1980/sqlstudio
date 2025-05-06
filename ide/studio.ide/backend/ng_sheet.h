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

#include "ng_public_interface.h"

#ifndef HAVE_PRECOMPILED_HEADERS

#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <thread>
#endif

// This include is a temporary solution to fix virtual d-tor warnings because of the incomplete Message type in mysqlx.h.
// It requires us to have the protobuf include file tree in win-res. As soon as this is fixed by the
// ng team we can remove the include (as well as the entire protobuf tree).
#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable: 4244) // Protobuf data type warnings.
#include <google/protobuf/message.h>
#pragma warning (pop)
#endif

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable: 4244) // Protobuf data type warnings.
#endif
#include "modules/base_session.h"
#include "modules/mod_mysqlx_schema.h"
#ifdef _WIN32
#pragma warning (pop)
#endif

#include "mforms/appview.h"
#include "mforms/label.h"
#include "mforms/box.h"
#include "mforms/splitter.h"

#include "base/data_types.h"

#include "code-completion/mysql_object_names_cache.h"

#include "ng_sidebar.h"
#include "ng_editor_container.h"
#include "ng_session_handler.h"

namespace ng {
  class NG_PUBLIC_TYPE NgSheet : public mforms::Box
  {
  public:
    NgSheet(EditorLanguage language);
    virtual ~NgSheet();
    void triggerCacheRefill(mforms::TreeNodeRef node, bool expand);

    bool willClose();
    void doConnect(const dataTypes::NodeConnection &connection, EditorLanguage language);
    void execute(ssize_t currentStatementOnly);
    void setupUI();
    void setupFinished();
    dataTypes::NodeConnection& getConnection();

    NgEditorContainer* editorContainer();
    std::function<void (NgSheet*)> onWillClose;

    boost::signals2::connection _setupFinishedSig;

  protected:
    void setupToolbar();

    boost::shared_ptr<mysh::ShellBaseSession> connect(dataTypes::NodeConnection &connection, mysh::SessionType type);

    void onCreateSession(const shcore::Value &session);
    void loadNewSessionData(shcore::Value session);
    void removeDeadSessions();

    std::deque<boost::signals2::connection> _newSessionLoaderSignals;
    std::map<std::string, NgSessionHandler*> _sessionList;

  private:
    void tabChanged();

    mforms::Label _label;
    ng::NgSidebar _sidebar;
    mforms::Splitter _sidebarSplitter;
    size_t _splitterPosition;

    mforms::Box _editorSidebar;
    NgEditorContainer* _editorContainer;
    mforms::ToolBar* _toolbar;
    dataTypes::NodeConnection _connection;
    struct { int endlessEditorTabId; int scriptTabId; } _tabId;

    std::shared_ptr<mysh::mysqlx::Schema> _schema;

    shcore::Value connect(const shcore::Argument_list &args);
    EditorLanguage _lang;
    NgSessionHandler *_activeSession;
    boost::signals2::signal<void()> _setupFinished;
  };

}
