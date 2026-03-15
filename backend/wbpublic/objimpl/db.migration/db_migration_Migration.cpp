/*
 * Copyright (c) 2007, 2018, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2026 dev4fun. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA 
 */

#include <grts/structs.db.migration.h>

#include <grtpp_util.h>

class db_migration_Migration::ImplData {
public:
  ImplData(){};
  virtual ~ImplData(){};
  auto addSourceObject(const std::string &id, const grt::Ref<GrtObject> &object) -> void {
    source_objects[id] = object;
  }

  auto getSourceObject(const std::string &id) -> grt::Ref<GrtObject> & {
    return source_objects[id];
  }

  auto addTargetObject(const std::string &id, const grt::Ref<GrtObject> &object) -> void {
    target_objects[id] = object;
  }

  auto getTargetObject(const std::string &id) -> grt::Ref<GrtObject> & {
    return target_objects[id];
  }

private:
  std::map<std::string, grt::Ref<GrtObject> > target_objects;
  std::map<std::string, grt::Ref<GrtObject> > source_objects;
};

//================================================================================
// db_migration_Migration

auto db_migration_Migration::init() -> void {
  if (!_data)
    _data = new db_migration_Migration::ImplData();
}

db_migration_Migration::~db_migration_Migration() {
  delete _data;
}

auto db_migration_Migration::addMigrationLogEntry(ssize_t type,
                                                                    const grt::Ref<GrtObject> &sourceObject,
                                                                    const grt::Ref<GrtObject> &targetObject,
                                                                    const std::string &message) -> grt::Ref<GrtLogObject> {
  GrtLogObjectRef log = findMigrationLogEntry(sourceObject, targetObject);
  if (!log.is_valid()) {
    log = GrtLogObjectRef(grt::Initialized);
    log->owner(this);
    log->logObject(sourceObject);
    log->refObject(targetObject);

    migrationLog().insert(log);
  }

  GrtLogEntryRef entry(grt::Initialized);
  entry->owner(log);
  entry->entryType(type);
  entry->name(grt::StringRef(message));
  log->entries().insert(entry);

  if (0 == type) {
    this->_data->addSourceObject(targetObject->id(), sourceObject);
    this->_data->addTargetObject(sourceObject->id(), targetObject);
  }

  return log;
}

auto db_migration_Migration::findMigrationLogEntry(const grt::Ref<GrtObject> &sourceObject,
                                                                     const grt::Ref<GrtObject> &targetObject) -> grt::Ref<GrtLogObject> {
  for (size_t c = migrationLog().count(), i = 0; i < c; i++) {
    GrtLogObjectRef log(migrationLog()[i]);
    if (log->logObject() == sourceObject && log->refObject() == targetObject)
      return log;
  }
  return GrtLogObjectRef();
}

auto db_migration_Migration::lookupMigratedObject(const grt::Ref<GrtObject> &sourceObject) -> grt::Ref<GrtObject> {
  return this->_data->getTargetObject(sourceObject->id());
}

auto db_migration_Migration::lookupSourceObject(const grt::Ref<GrtObject> &targetObject) -> grt::Ref<GrtObject> {
  return this->_data->getSourceObject(targetObject->id());
}
