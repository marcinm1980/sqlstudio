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

#ifndef _DB_QUERY_EDITOR_H_
#define _DB_QUERY_EDITOR_H_

#include <grts/structs.db.query.h>

#include "wbpublic_public_interface.h"

class MySQLEditor;

// Use an abstract class here because db_query_Editor.cpp is in wbpublic but
// actual query editor object is in wbprivate. So wbprivate must subclass this
// and assign instances to the grt object
class WBPUBLICBACKEND_PUBLIC_FUNC db_query_Editor::ImplData {
public:
  ImplData();
  virtual ~ImplData() {
  }
  virtual auto connection() const -> db_mgmt_ConnectionRef = 0;
  virtual auto sshConnection() const -> db_mgmt_SSHConnectionRef = 0;
  virtual auto getSSHTunnelPort() const -> grt::IntegerRef = 0;
  virtual auto isConnected() const -> grt::IntegerRef = 0;
  virtual auto addQueryEditor() -> db_query_QueryEditorRef = 0;
  virtual auto addToOutput(const std::string &text, long bringToFront) -> grt::IntegerRef = 0;
  virtual auto executeScript(const std::string &sql) -> grt::ListRef<db_query_Resultset> = 0;
  virtual auto executeScriptAndOutputToGrid(const std::string &sql) -> grt::IntegerRef = 0;
  virtual auto createTableEditResultset(const std::string &schema, const std::string &table,
                                                                 const std::string &where, bool showGrid) -> db_query_EditableResultsetRef = 0;

  virtual auto activeSchema(const std::string &schema) -> void = 0;
  virtual auto activeSchema() -> std::string = 0;
  virtual auto activeQueryEditor() -> db_query_QueryEditorRef = 0;
  virtual auto schemaTreeSelection() const -> grt::ListRef<db_query_LiveDBObject> = 0;
  virtual auto editLiveObject(const grt::Ref<db_DatabaseObject> &object, const db_CatalogRef &catalog) -> void = 0;
  virtual auto alterLiveObject(const std::string &type, const std::string &schemaName,
                               const std::string &objectName) -> void = 0;

  virtual auto executeQuery(const std::string &sql, bool log) -> db_query_ResultsetRef = 0;
  virtual auto executeCommand(const std::string &sql, bool log, bool background) -> void = 0;

  virtual auto executeManagementQuery(const std::string &sql, bool log) -> db_query_ResultsetRef = 0;
  virtual auto executeManagementCommand(const std::string &sql, bool log) -> void = 0;
};

#endif
