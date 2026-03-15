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

#ifndef _DB_QUERY_RESULTSET_H_
#define _DB_QUERY_RESULTSET_H_

#include <grts/structs.db.query.h>
#include "sqlide/recordset_be.h"
#include <cppconn/resultset.h>

auto grtwrap_recordset(GrtObjectRef owner, Recordset::Ref rset) -> db_query_ResultsetRef WBPUBLICBACKEND_PUBLIC_FUNC;
auto grtwrap_recordset(GrtObjectRef owner,
                                                                    std::shared_ptr<sql::ResultSet> result) -> db_query_ResultsetRef WBPUBLICBACKEND_PUBLIC_FUNC;

class WBPUBLICBACKEND_PUBLIC_FUNC db_query_Resultset::ImplData {
protected:
  ImplData(db_query_ResultsetRef aself);

  db_query_Resultset *self;

public:
  std::map<std::string, ssize_t> column_by_name;

  virtual ~ImplData();

  virtual auto refresh() -> void = 0;
  virtual auto sql() const -> grt::StringRef = 0;
  virtual auto currentRow() const -> grt::IntegerRef = 0;
  virtual auto rowCount() const -> grt::IntegerRef = 0;
  virtual auto floatFieldValue(ssize_t column) -> grt::DoubleRef = 0;
  virtual auto floatFieldValueByName(const std::string &column) -> grt::DoubleRef = 0;
  virtual auto goToFirstRow() -> grt::IntegerRef = 0;
  virtual auto goToLastRow() -> grt::IntegerRef = 0;
  virtual auto goToRow(ssize_t row) -> grt::IntegerRef = 0;
  virtual auto intFieldValue(ssize_t column) -> grt::IntegerRef = 0;
  virtual auto intFieldValueByName(const std::string &column) -> grt::IntegerRef = 0;
  virtual auto nextRow() -> grt::IntegerRef = 0;
  virtual auto previousRow() -> grt::IntegerRef = 0;
  virtual auto saveFieldValueToFile(ssize_t column, const std::string &file) -> grt::IntegerRef = 0;
  virtual auto stringFieldValue(ssize_t column) -> grt::StringRef = 0;
  virtual auto stringFieldValueByName(const std::string &column) -> grt::StringRef = 0;
  virtual auto geoStringFieldValue(ssize_t column) -> grt::StringRef = 0;
  virtual auto geoStringFieldValueByName(const std::string &column) -> grt::StringRef = 0;
  virtual auto geoJsonFieldValue(ssize_t column) -> grt::StringRef = 0;
  virtual auto geoJsonFieldValueByName(const std::string &column) -> grt::StringRef = 0;
};

class WBPUBLICBACKEND_PUBLIC_FUNC WBRecordsetResultset : public db_query_Resultset::ImplData {
public:
  size_t cursor;
  std::shared_ptr<Recordset> recordset;

  WBRecordsetResultset(db_query_ResultsetRef aself, std::shared_ptr<Recordset> rset);
  virtual auto sql() const -> grt::StringRef;
  virtual auto currentRow() const -> grt::IntegerRef;
  virtual auto rowCount() const -> grt::IntegerRef;
  virtual auto floatFieldValue(ssize_t column) -> grt::DoubleRef;
  virtual auto floatFieldValueByName(const std::string &column) -> grt::DoubleRef;
  virtual auto goToFirstRow() -> grt::IntegerRef;
  virtual auto goToLastRow() -> grt::IntegerRef;
  virtual auto goToRow(ssize_t row) -> grt::IntegerRef;
  virtual auto intFieldValue(ssize_t column) -> grt::IntegerRef;
  virtual auto intFieldValueByName(const std::string &column) -> grt::IntegerRef;
  virtual auto nextRow() -> grt::IntegerRef;
  virtual auto previousRow() -> grt::IntegerRef;

  virtual auto refresh() -> void;
  virtual auto stringFieldValue(ssize_t column) -> grt::StringRef;
  virtual auto stringFieldValueByName(const std::string &column) -> grt::StringRef;
  virtual auto geoStringFieldValue(ssize_t column) -> grt::StringRef;
  virtual auto geoStringFieldValueByName(const std::string &column) -> grt::StringRef;
  virtual auto geoJsonFieldValue(ssize_t column) -> grt::StringRef;
  virtual auto geoJsonFieldValueByName(const std::string &column) -> grt::StringRef;
  virtual auto saveFieldValueToFile(ssize_t column, const std::string &file) -> grt::IntegerRef;
};
#endif
