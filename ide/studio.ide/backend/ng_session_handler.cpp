/*
* Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
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

#include "ng_session_handler.h"
#include "base/log.h"
#include <boost/smart_ptr/shared_ptr.hpp>
#include "modules/mod_mysql_session.h"
#include "modules/mod_mysql_resultset.h"
#include "modules/mod_mysqlx_session.h"
#include "modules/mod_mysqlx_resultset.h"

DEFAULT_LOG_DOMAIN("NG")

namespace ng {

NgSessionHandler::NgSessionHandler(const mysh::SessionType type, weakSessionTypeRef session, const dataTypes::NodeConnection &connection) : _autoCompletionCache(nullptr),
    sessionType(type), _connection(connection), weakSessionRef(session)
{

  base::NotificationCenter::get()->add_observer(this, "GNObjectCache");
  _autoCompletionCache = new MySQLObjectNamesCache(
            std::bind(&NgSessionHandler::runQueryForCache, this, std::placeholders::_1),
            std::bind(&NgSessionHandler::onCacheAction, this, std::placeholders::_1));
}

NgSessionHandler::~NgSessionHandler()
{
  if (_autoCompletionCache)
  {
    _autoCompletionCache->shutdown();
    delete _autoCompletionCache;
  }
}

bool NgSessionHandler::isValid()
{
  auto spt = weakSessionRef.lock();
  if (!spt)
    return false;

  return true;
}

void NgSessionHandler::handle_notification(const std::string &name, void *sender, base::NotificationInfo &info)
{

  if (name == "GNObjectCache" && sender == _autoCompletionCache && _onDataArrived)
  {
    mforms::Utilities::perform_from_main_thread([this, &info]() -> void* {
      std::vector<std::string> data;
      std::vector<std::string> parts = base::split(info["path"], "\1");
      if (info["type"] == "schemas")
        data = _autoCompletionCache->getMatchingSchemaNames("");
      else if(info["type"] == "tables")
        data = _autoCompletionCache->getMatchingTableNames(parts[0], "");
      else if(info["type"] == "procedures")
        data = _autoCompletionCache->getMatchingProcedureNames(parts[0], "");
      else if (info["type"] == "views")
        data = _autoCompletionCache->getMatchingViewNames(parts[0], "");
      else if (info["type"] == "functions")
        data = _autoCompletionCache->getMatchingFunctionNames(parts[0], "");
      else if (info["type"] == "columns")
        data = _autoCompletionCache->getMatchingColumnNames(parts[0], parts[1], "");
      else if (info["type"] == "triggers")
        data = _autoCompletionCache->getMatchingTriggerNames(parts[0], parts[1], "");
      else if (info["type"] == "events")
        data = _autoCompletionCache->getMatchingEvents(parts[0], "");

      auto spt = weakSessionRef.lock();
      if (spt)
        _onDataArrived(spt->uri(), info["type"], parts, data);

      return nullptr;
    });
  }
}

std::pair<base::RecMutexLock, sharedSessionTypeRef> NgSessionHandler::getSharedSessionLock(bool throw_on_block)
{
  base::RecMutexLock mutex_lock(_sessionMutex, throw_on_block);
  bool valid= false;

  auto spt = weakSessionRef.lock();
  if (spt && spt->is_connected())
    valid = true;

  if (!valid)
    throw grt::db_not_connected("DBMS connection is not available");

  return {mutex_lock, spt};
}

static std::vector<std::pair<std::string, std::string>> runSessionQueryForCache(boost::shared_ptr<mysh::mysql::ClassicSession> session, const std::string &query)
{

  std::vector<std::pair<std::string, std::string>> result;
  if (!session->is_connected())
  {
    logError("Session %s is not connected\n", session->uri().c_str());
    return result;
  }

  shcore::Argument_list q;
  q.push_back(shcore::Value(query));

  shcore::Value res = session->run_sql(q);
  shcore::Argument_list args;
  auto rset = res.as_object<mysh::mysql::ClassicResult>();

  {
    auto colCount = rset->get_member("columnCount").as_int();
    shcore::Value next_row = rset->fetch_one(args);
    while (next_row)
    {
      auto row = next_row.as_object<mysh::Row>();
      if (colCount > 1)
        result.push_back({ row->get_member(0).descr(), row->get_member(1).descr()});
      else
        result.push_back({ row->get_member(0).descr(), ""});

      next_row = rset->fetch_one(args);
    }
  }
  return result;
}


static std::vector<std::pair<std::string, std::string>> runSessionQueryForCache(boost::shared_ptr<mysh::mysqlx::BaseSession> session, const std::string &query)
{
  std::vector<std::pair<std::string, std::string>> result;
  if (!session->is_connected())
  {
    logError("Session %s is not connected\n", session->uri().c_str());
    return result;
  }
  shcore::Argument_list args;
  shcore::Value res = session->execute_sql(query, args);
  auto rset = res.as_object<mysh::mysqlx::SqlResult>();
  rset->get_member("columnCount");

  {
    auto colCount = rset->get_member("columnCount").as_int();
    shcore::Value next_row = rset->fetch_one(args);
    while (next_row)
    {
      auto row = next_row.as_object<mysh::Row>();
      if (colCount > 1)
        result.push_back({ row->get_member(0).descr(), row->get_member(1).descr()});
      else
        result.push_back({ row->get_member(0).descr(), ""});

      next_row = rset->fetch_one(args);
    }
  }
  return result;
}


/*
 * Called from the object name cache to get certain object names.
 * Here we run the necessary query and return the first or first two columns of the result.
 */
std::vector<std::pair<std::string, std::string>> NgSessionHandler::runQueryForCache(const std::string &query)
{
  logDebug3("Running object name cache query: %s\n", query.c_str());

  auto sessionLock = getSharedSessionLock();
  auto session = sessionLock.second;
  switch(sessionType)
  {
  case mysh::SessionType::Classic:
    return runSessionQueryForCache(boost::static_pointer_cast<mysh::mysql::ClassicSession>(session), query);
  case mysh::SessionType::Application:
  case mysh::SessionType::Node:
    return runSessionQueryForCache(boost::static_pointer_cast<mysh::mysqlx::BaseSession>(session), query);
  default:
    throw std::runtime_error("Unhandled Session Type\n");
  }
  return std::vector<std::pair<std::string, std::string>>();
}

/**
 * Triggered when the auto completion cache switches activity. We use this to update our busy
 * indicator.
 */
void NgSessionHandler::onCacheAction(bool active)
{
  if (_onCacheAction)
    _onCacheAction(active);

  if(active)
    std::cout << "is active" << std::endl;
  else

    std::cout << "not active" << std::endl;
}

} /* namespace ng */
