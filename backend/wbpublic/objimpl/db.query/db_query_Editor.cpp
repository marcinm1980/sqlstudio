/*
 * Copyright (c) 2010, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include <grts/structs.db.query.h>

#include "db_query_Editor.h"
#include <grtpp_util.h>

db_query_Editor::ImplData::ImplData() {
}

//================================================================================
// db_query_Editor

auto db_query_Editor::init() -> void {
  // _data must be set with set_data() by WBContextSQLIDE
  // if (!_data) _data= new db_query_Editor::ImplData(this);
}

db_query_Editor::~db_query_Editor() {
  delete _data;
}

auto db_query_Editor::set_data(ImplData *data) -> void {
  _data = data;
}

auto db_query_Editor::connection() const -> db_mgmt_ConnectionRef {
  if (_data)
    return _data->connection();
  return db_mgmt_ConnectionRef();
}

auto db_query_Editor::getSSHTunnelPort() const -> grt::IntegerRef {
  if (_data)
    return _data->getSSHTunnelPort();
  return -1;
}

auto db_query_Editor::sshConnection() const -> db_mgmt_SSHConnectionRef {
  if (_data)
    return _data->sshConnection();
  return db_mgmt_SSHConnectionRef();
}

auto db_query_Editor::isConnected() const -> grt::IntegerRef {
  if (_data)
    return _data->isConnected();
  return grt::IntegerRef(0);
}

auto db_query_Editor::activeQueryEditor() const -> db_query_QueryEditorRef {
  if (_data)
    return _data->activeQueryEditor();
  return db_query_QueryEditorRef();
}

auto db_query_Editor::schemaTreeSelection() const -> grt::ListRef<db_query_LiveDBObject> {
  return _data->schemaTreeSelection();
}

auto db_query_Editor::defaultSchema() const -> grt::StringRef {
  if (_data)
    return _data->activeSchema();
  return grt::StringRef();
}

auto db_query_Editor::defaultSchema(const grt::StringRef &value) -> void {
  if (_data)
    _data->activeSchema(*value);
}

auto db_query_Editor::addQueryEditor() -> db_query_QueryEditorRef {
  if (_data)
    return _data->addQueryEditor();
  return db_query_QueryEditorRef();
}

auto db_query_Editor::addToOutput(const std::string &text, ssize_t bringToFront) -> grt::IntegerRef {
  if (_data)
    return _data->addToOutput(text, (long)bringToFront);
  return grt::IntegerRef(0);
}

auto db_query_Editor::createTableEditResultset(const std::string &schema,
                                                                        const std::string &table,
                                                                        const std::string &where, ssize_t showGrid) -> db_query_EditableResultsetRef {
  if (_data)
    return _data->createTableEditResultset(schema, table, where, showGrid != 0);
  return db_query_EditableResultsetRef();
}

auto db_query_Editor::editLiveObject(const grt::Ref<db_DatabaseObject> &object, const db_CatalogRef &catalog) -> void {
  if (_data)
    _data->editLiveObject(object, catalog);
}

auto db_query_Editor::alterLiveObject(const std::string &type, const std::string &schemaName,
                                      const std::string &objectName) -> void {
  if (_data)
    _data->alterLiveObject(type, schemaName, objectName);
}

auto db_query_Editor::executeScript(const std::string &sql) -> grt::ListRef<db_query_Resultset> {
  if (_data)
    return _data->executeScript(sql);
  return grt::ListRef<db_query_Resultset>();
}

auto db_query_Editor::executeScriptAndOutputToGrid(const std::string &sql) -> grt::IntegerRef {
  if (_data)
    return _data->executeScriptAndOutputToGrid(sql);
  return grt::IntegerRef(0);
}

auto db_query_Editor::executeManagementQuery(const std::string &sql, ssize_t log) -> db_query_ResultsetRef {
  if (_data)
    return _data->executeManagementQuery(sql, log != 0);
  return db_query_ResultsetRef();
}

auto db_query_Editor::executeManagementCommand(const std::string &sql, ssize_t log) -> void {
  if (_data)
    _data->executeManagementCommand(sql, log != 0);
}

auto db_query_Editor::executeQuery(const std::string &sql, ssize_t log) -> db_query_ResultsetRef {
  if (_data)
    return _data->executeQuery(sql, log != 0);
  return db_query_ResultsetRef();
}

auto db_query_Editor::executeCommand(const std::string &sql, ssize_t log, ssize_t background) -> void {
  if (_data)
    _data->executeCommand(sql, log != 0, background != 0);
}
