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
#include "base/log.h"

#ifndef HAVE_PRECOMPILED_HEADERS

#include <functional>
#include <sstream>
#include <thread>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/pointer_cast.hpp>

#endif

#include "modules/mod_mysql_session.h"
#include "modules/mod_mysqlx_session.h"
#include "mforms/toolbar.h"


#include "ng_sheet.h"

using namespace ng;
using namespace shcore;

DEFAULT_LOG_DOMAIN("NG")

//----------------- NgSheet ------------------------------------------------------------------------

NgSheet::NgSheet(EditorLanguage language)
  : mforms::Box(false), _sidebar(language == ng::EditorLanguage::MySQL ? false : true),
    _sidebarSplitter(true), _editorSidebar(false),  _editorContainer(nullptr),  _lang(language), _activeSession(nullptr)
{
  setupUI();
}

//--------------------------------------------------------------------------------------------------

NgSheet::~NgSheet()
{
  _setupFinishedSig.disconnect();
  delete _editorContainer;

  // Clean up SessionHandlers, connections will be freed by shared_ptr from inside of shell
  for (auto it = _sessionList.begin(); it != _sessionList.end(); ++it)
    delete it->second;

  _sessionList.clear();
}

//--------------------------------------------------------------------------------------------------

bool NgSheet::willClose()
{
  if (onWillClose)
    onWillClose(this);
  return true;
}

//--------------------------------------------------------------------------------------------------

void NgSheet::triggerCacheRefill(mforms::TreeNodeRef node, bool expand)
{
  if (expand)
  {
    std::string tag = node->get_tag();
    if (tag == ng::NgSidebar::SCHEMA_TAG)
    {
      std::string uri = node->get_parent()->get_tag(); // Parent is uri tag so we use it to get autocomplete ;)
      auto it = _sessionList.find(uri);
      if (it != _sessionList.end())
      {
        it->second->_autoCompletionCache->getMatchingTableNames(node->get_string(0), "");
        it->second->_autoCompletionCache->getMatchingFunctionNames(node->get_string(0), "");
        it->second->_autoCompletionCache->getMatchingProcedureNames(node->get_string(0), "");
        it->second->_autoCompletionCache->getMatchingViewNames(node->get_string(0), "");
        it->second->_autoCompletionCache->getMatchingEvents(node->get_string(0), "");
      }
    }

//TODO: change this after optimizing mysql_object_names_cache
//    if (tag == ng::NgSidebar::TABLE_TAG)
//    {
//      _autoCompletionCache->getMatchingColumnNames(node->get_parent()->get_parent()->get_string(0), node->get_string(0), "");
//    }
//    if (tag == ng::NgSidebar::TRIGGERS_TAG)
//    {
//      _autoCompletionCache->getMatchingTriggerNames(node->get_parent()->get_parent()->get_parent()->get_string(0), node->get_parent()->get_string(0), "");
//    }
//    if (tag == ng::NgSidebar::FOREIGN_KEYS_TAG)
//    {
//      //TODO: implement forgein keys retrieval (or do we really need this?)
//    }

  }
}

//--------------------------------------------------------------------------------------------------

void NgSheet::doConnect(const dataTypes::NodeConnection &connection, EditorLanguage language)
{
  if (connection.isValid())
  {
    _connection = connection;
    auto session = connect(_connection, language == EditorLanguage::MySQL ? mysh::SessionType::Classic : mysh::SessionType::Application);
    _activeSession = new NgSessionHandler(language == EditorLanguage::MySQL ? mysh::SessionType::Classic : mysh::SessionType::Application, session, connection);
    _activeSession->_onDataArrived = std::bind(&NgSidebar::onDataArrived, &_sidebar, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    _sessionList.insert({session->uri(), _activeSession});
    _editorContainer->setSession(session);
  }
}

static bool findStoredPassword(const dataTypes::NodeConnection &connection, std::string &pwd)
{
  void *ret = mforms::Utilities::perform_from_main_thread(boost::bind<void*>([&connection, &pwd]() -> void* {
    bool ret = false;
    try
    {
      ret = mforms::Utilities::find_password(connection.hostIdentifier(), connection.userName, pwd);
    }
    catch (const std::exception &e)
    {
      logError("Error Looking Up Password %s", e.what());
    }

    return (void*)ret;
  }));

  if (ret)
    return true;
  return false;
}

static std::string doRequestPassword(const dataTypes::NodeConnection &connection, bool forceAsking)
{
  std::string passwordTmp;

  void *ret = mforms::Utilities::perform_from_main_thread(boost::bind<void*>([&connection, forceAsking, &passwordTmp]()-> void* {
    std::string userTmp = connection.userName;
    bool ret = false;
    try
    {
      ret = mforms::Utilities::find_or_ask_for_password("Connect to MySQL Server", connection.hostIdentifier(), userTmp,
                                                        forceAsking, passwordTmp);
    }
    catch (const std::exception &e)
    {
      logError("Error Looking Up Password %s", e.what());
    }

    return (void*)ret;
  }));
  if (ret)
    return passwordTmp;

  throw grt::user_cancelled("Canceled by user");
}

//--------------------------------------------------------------------------------------------------

boost::shared_ptr<mysh::ShellBaseSession> NgSheet::connect(dataTypes::NodeConnection &connection, mysh::SessionType type)
{
  Argument_list args;
  args.push_back(Value(connection.uri(true)));

  boost::shared_ptr<mysh::ShellBaseSession> newSession;
  if (type == mysh::SessionType::Application || type == mysh::SessionType::Node)
    newSession.reset(new mysh::mysqlx::XSession());
  else
    newSession.reset(new mysh::mysql::ClassicSession());

  enum PasswordMethod {
      NoPassword,
      KeychainPassword,
      InteractivePassword
  } current_method = NoPassword;

  while (true)
  {
    try {
      logInfo("Attempt to create connection: %s\n", connection.uri(false).c_str());
      newSession->connect(args);
      break;
    } catch (shcore::Exception &e)
    {
      std::size_t code = e.code();
      if (e.is_mysql()) //TODO: use e.is_server() after shell update, use also e.code()
      {
        if (code == 1045 && current_method == NoPassword)
        {
          if (connection.userPassword.empty())
          {
            std::string pwd;
            if (findStoredPassword(connection, pwd))
            {
              connection.userPassword = pwd;
              current_method = KeychainPassword;
              args.clear();
              args.push_back(Value(connection.uri(true)));
              continue;
            }
            else
            {
              connection.userPassword = doRequestPassword(connection, true);
              current_method = InteractivePassword;
              args.clear();
              args.push_back(Value(connection.uri(true)));
              continue;
            }
          }
          else
          {
            logError("Shexception: %s, message was: [%zu]: %s\n", connection.uri(false).c_str(), code, e.what());
            throw;
          }
        }
        else if (code == 2002) // Can't connect, we open shell anyway but without the session
        {
          return newSession;
        }
        else
        {
          logError("Shexception: %s, message was: [%zu]: %s\n", connection.uri(false).c_str(), code, e.what());
          throw;
        }
      }

      logError("Shexception: %s, message was: [%zu]: %s\n", connection.uri(false).c_str(), code, e.what());
      return newSession;
    }
  }
  logInfo("Succesfully connected to: %s\n", connection.uri(false).c_str());
  return newSession;
}


//--------------------------------------------------------------------------------------------------

void NgSheet::onCreateSession(const shcore::Value &session)
{
  _newSessionLoaderSignals.push_back(bec::GRTManager::get().run_once_when_idle([=]() ->void {
    loadNewSessionData(session);
  }));
}

//--------------------------------------------------------------------------------------------------

void NgSheet::loadNewSessionData(shcore::Value session)
{
  auto ssb = session.as_object<mysh::ShellBaseSession>();
  if (ssb)
  {
    logDebug3("New session created: %s\n", ssb->uri().c_str());
     NgSessionHandler *handler = new NgSessionHandler(base::same_string("ClassicSession", ssb->class_name().c_str()) ? mysh::SessionType::Classic : mysh::SessionType::Application, ssb, dataTypes::NodeConnection());
     handler->_onDataArrived = std::bind(&NgSidebar::onDataArrived, &_sidebar, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
     _sessionList.insert({ssb->uri(), handler});
  }
}

void NgSheet::removeDeadSessions()
{
  for (auto it = _sessionList.cbegin(); it != _sessionList.cend();)
  {
     if (!it->second->isValid())
     {
       delete it->second;
       _sessionList.erase(it++);
     }
     else
       ++it;
  }
}

void NgSheet::execute(ssize_t currentStatementOnly)
{
  editorContainer()->execute(currentStatementOnly == 1);
}

//--------------------------------------------------------------------------------------------------

void NgSheet::setupUI()
{
  set_back_color("#FFFFFF");
  set_padding(0);
  set_spacing(0);

  setupToolbar();
  add(_toolbar, false, true);

  _sidebar.signal_expand_toggle()->connect(boost::bind(&NgSheet::triggerCacheRefill, this, _1, _2));
  _sidebarSplitter.add(&_sidebar);

  // Setup editor sidebar.
  _editorSidebar.set_spacing(0);
  _editorSidebar.set_padding(0);

  _editorSidebar.set_size(200, 200);
  _sidebarSplitter.add(&_editorSidebar);
  add(&_sidebarSplitter, true, true);

  _editorContainer = new NgEditorContainer(_lang);
  _editorContainer->_onCreateSession = std::bind(&NgSheet::onCreateSession, this, std::placeholders::_1);
  _setupFinishedSig = _setupFinished.connect([&]() -> void {
    _editorContainer->focus();
  });;

#ifdef __APPLE__
  mforms::TabView *tabView = mforms::manage(new mforms::TabView(mforms::TabViewDocumentClosableX));
#else
  mforms::TabView *tabView = mforms::manage(new mforms::TabView(mforms::TabViewDocumentClosable));
#endif

  tabView->set_name("script_editor:tab");
  std::string tabName = toShortString(_lang) + " Shell";
  _tabId.endlessEditorTabId = tabView->add_page(_editorContainer, tabName);

  scoped_connect(tabView->signal_tab_changed(), boost::bind(&NgSheet::tabChanged, this));

  _editorSidebar.add(tabView, true, true);
  _sidebarSplitter.set_divider_position(40);
}

//--------------------------------------------------------------------------------------------------

void NgSheet::setupFinished()
{
  _setupFinished();
}

//--------------------------------------------------------------------------------------------------

dataTypes::NodeConnection& NgSheet::getConnection()
{
  return _connection;
}

//--------------------------------------------------------------------------------------------------

void NgSheet::tabChanged()
{

}

//--------------------------------------------------------------------------------------------------

void NgSheet::setupToolbar()
{
  _toolbar = mforms::manage(new mforms::ToolBar(mforms::MainToolBar));
#ifdef _WIN32
  _toolbar->set_size(300, 27);
#endif
  mforms::ToolBarItem *item;

  item = mforms::manage(new mforms::ToolBarItem(mforms::SwitcherItem));
  item->set_name("sidebar.hide");
  item->set_text("Sessions");
  item->set_icon(mforms::App::get()->get_resource_path("session-switch.png"));
  item->set_tooltip(_("Show or hide the session sidebar"));
  mforms::View::scoped_connect(item->signal_activated(), [this](mforms::ToolBarItem*) {
    if (_sidebar.is_shown()) // At least on OSX the splitter auto changes its splitter position when restoring an invisible view.
      _splitterPosition = _sidebarSplitter.get_divider_position();
    _sidebar.show(!_sidebar.is_shown());
    if (_sidebar.is_shown())
      _sidebarSplitter.set_divider_position(_splitterPosition);
  });

  _toolbar->add_item(item);
  _toolbar->add_item(mforms::manage(new mforms::ToolBarItem(mforms::SeparatorItem)));

  /* Not in this milestone.
  item = mforms::manage(new mforms::ToolBarItem(mforms::ActionItem));
  item->set_name("query.openFile");
  item->set_icon(bec::IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_open.png"));
  item->set_tooltip(_("Open a script file in this editor"));
  mforms::View::scoped_connect(item->signal_activated(), [this](mforms::ToolBarItem*){
//    if (_editorContainer != nullptr)
//      _editorContainer->getEditor()->openFile();
  });
  _toolbar->add_item(item);

  item = mforms::manage(new mforms::ToolBarItem(mforms::ActionItem));
  item->set_name("query.saveFile");
  item->set_icon(bec::IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_save.png"));
  item->set_tooltip(_("Save the script to a file."));
  mforms::View::scoped_connect(item->signal_activated(), [this](mforms::ToolBarItem*){
//    if (_editorContainer != nullptr)
//      _editorContainer->getEditor()->saveFile();
  });
  _toolbar->add_item(item);

  _toolbar->add_item(mforms::manage(new mforms::ToolBarItem(mforms::SeparatorItem)));
*/
  item = mforms::manage(new mforms::ToolBarItem(mforms::ActionItem));
  item->set_name("ng.execute");
  item->set_icon(bec::IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_execute.png"));
  item->set_tooltip(_("Execute the selected portion of the script or everything, if there is no selection"));
  mforms::View::scoped_connect(item->signal_activated(), [this](mforms::ToolBarItem*){
    execute(false);
  });
  _toolbar->add_item(item);

  item = mforms::manage(new mforms::ToolBarItem(mforms::ActionItem));
  item->set_name("query.execute_current_statement");
  item->set_icon(bec::IconManager::get_instance()->get_icon_path("qe_sql-editor-tb-icon_execute-current.png"));
  item->set_tooltip(_("Execute the statement under the keyboard cursor"));
  mforms::View::scoped_connect(item->signal_activated(), [this](mforms::ToolBarItem*){
      execute(true);
    });
  _toolbar->add_item(item);

}

//--------------------------------------------------------------------------------------------------

NgEditorContainer* NgSheet::editorContainer()
{
  return _editorContainer;
}

//--------------------------------------------------------------------------------------------------
