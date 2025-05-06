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
#include <vector>
#endif

#include <boost/smart_ptr/weak_ptr.hpp>
#include "code-completion/mysql_object_names_cache.h"
#include "modules/base_session.h"
#include "base/notifications.h"
#include "base/data_types.h"
#include "mforms/utilities.h"
#include "driver_manager.h"

namespace ng
{
  using weakSessionTypeRef = boost::weak_ptr<mysh::ShellBaseSession>;
  using sharedSessionTypeRef = boost::shared_ptr<mysh::ShellBaseSession>;

  class NgSessionHandler : public base::Observer
  {
  public:
    typedef std::shared_ptr<NgSessionHandler> Ref;
    NgSessionHandler(const mysh::SessionType type, weakSessionTypeRef session, const dataTypes::NodeConnection &connection);
    virtual ~NgSessionHandler();
    bool isValid();
    /**
     * Get shared_ptr from weak_ptr and lock mutex so we know this is in use.
     */
    std::pair<base::RecMutexLock, sharedSessionTypeRef> getSharedSessionLock(bool throw_on_block = false);

    virtual void handle_notification(const std::string &name, void *sender, base::NotificationInfo &info);
    std::function<void(const std::string &uri, const std::string &type, const std::vector<std::string> &path, const std::vector<std::string> &data)> _onDataArrived;
    std::function<void(bool)> _onCacheAction;


    MySQLObjectNamesCache *_autoCompletionCache;
    mysh::SessionType sessionType;
    dataTypes::NodeConnection _connection;

  protected:
    weakSessionTypeRef weakSessionRef;

    mutable base::RecMutex _sessionMutex;



    /*
     * Called from the object name cache to get certain object names.
     * Here we run the necessary query and return the first or first two columns of the result.
     */
    std::vector<std::pair<std::string, std::string>> runQueryForCache(const std::string &query);

    /**
     * Triggered when the auto completion cache switches activity. We use this to update our busy
     * indicator.
     */
    void onCacheAction(bool active);
  };

} /* namespace ng */
