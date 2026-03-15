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

#include <ogrsf_frmts.h>
#include <ogr_api.h>
#include <gdal.h>

#include <grts/structs.db.query.h>
#include <grtpp_util.h>
#include "sqlide/recordset_be.h"
#include "db_query_Resultset.h"

#if defined(_WIN64) || defined(__LP64__) || defined(__APPLE__) // TODO: we only support 64bit now.
#define ENVIRONMENT_64
#endif

//================================================================================
// db_query_Resultset
db_query_Resultset::ImplData::ImplData(db_query_ResultsetRef aself)
  : self(dynamic_cast<db_query_Resultset *>(aself.valueptr())) {
}

db_query_Resultset::ImplData::~ImplData() {
}

//================================================================================

static auto getGeoRepresentation(grt::StringRef data, bool outputAsJson = false) -> grt::StringRef {
  OGRGeometry *geometry = NULL;
  OGRErr ret_val =
    OGRGeometryFactory::createFromWkb((unsigned char *)const_cast<char *>(&(*((*data).begin() + 4))), NULL, &geometry);
  if (ret_val != OGRERR_NONE) {
    if (geometry)
      CPLFree(geometry);
    throw std::exception();
  }

  if (geometry != NULL) {
    char *data = NULL;
    OGRErr err = OGRERR_NONE;
    if (outputAsJson)
      data = geometry->exportToJson();
    else
      err = geometry->exportToWkt(&data);

    if (err == OGRERR_NONE && data != NULL) {
      grt::StringRef tmp(data);
      CPLFree(data);
      CPLFree(geometry);
      return tmp;
    } else
      throw std::runtime_error("Conversion of OGR geometry data failed");
  }
  return grt::StringRef();
}

WBRecordsetResultset::WBRecordsetResultset(db_query_ResultsetRef aself, std::shared_ptr<Recordset> rset)
  : db_query_Resultset::ImplData(aself), cursor(0), recordset(rset) {
  const size_t last_column = recordset->get_column_count();
  for (size_t i = 0; i < last_column; i++) {
    column_by_name[recordset->get_column_caption(i)] = i;

    std::string type;
    switch (recordset->get_column_type(i)) {
      case bec::GridModel::UnknownType:
        type = "unknown";
        break;
      case bec::GridModel::StringType:
        type = "string";
        break;
      case bec::GridModel::NumericType:
        type = "numeric";
        break;
      case bec::GridModel::FloatType:
        type = "float";
        break;
      case bec::GridModel::DatetimeType:
        type = "datetime";
        break;
      case bec::GridModel::BlobType:
        type = "blob";
        break;
    }

    db_query_ResultsetColumnRef column(grt::Initialized);

    column->owner(aself);
    column->name(recordset->get_column_caption(i));
    column->columnType(type);

    self->columns().insert(column);
  }
}

auto WBRecordsetResultset::sql() const -> grt::StringRef {
  return grt::StringRef(recordset->generator_query());
}

auto WBRecordsetResultset::currentRow() const -> grt::IntegerRef {
  return grt::IntegerRef((long)cursor);
}

auto WBRecordsetResultset::rowCount() const -> grt::IntegerRef {
  return grt::IntegerRef(recordset->count());
}

auto WBRecordsetResultset::floatFieldValue(ssize_t column) -> grt::DoubleRef {
  double value;
  if (column >= 0 && (size_t)column < recordset->get_column_count()) {
    if (recordset->get_field(cursor, column, value))
      return grt::DoubleRef(value);
  } else
    throw std::invalid_argument(base::strfmt("invalid column %li for resultset", (long)column).c_str());
  return grt::DoubleRef(0.0);
}

auto WBRecordsetResultset::floatFieldValueByName(const std::string &column) -> grt::DoubleRef {
  double value;
  if (column_by_name.find(column) != column_by_name.end()) {
    if (recordset->get_field(cursor, column_by_name[column], value))
      return grt::DoubleRef(value);
  }
  throw std::invalid_argument(base::strfmt("invalid column %s for resultset", column.c_str()).c_str());
  return grt::DoubleRef(0.0);
}

auto WBRecordsetResultset::goToFirstRow() -> grt::IntegerRef {
  cursor = 0;
  return grt::IntegerRef(cursor < recordset->count());
}

auto WBRecordsetResultset::goToLastRow() -> grt::IntegerRef {
  if (recordset->count() > 0) {
    cursor = recordset->count() - 1;
    return grt::IntegerRef(1);
  }
  return grt::IntegerRef(0);
}

auto WBRecordsetResultset::goToRow(ssize_t row) -> grt::IntegerRef {
  if (row >= 0 && (size_t)row < recordset->count()) {
    cursor = row;
    return grt::IntegerRef(1);
  }
  return grt::IntegerRef(0);
}

auto WBRecordsetResultset::intFieldValue(ssize_t column) -> grt::IntegerRef {
  ssize_t value;
  if (column >= 0 && (size_t)column < recordset->get_column_count()) {
    if (recordset->get_field(bec::NodeId(cursor), column, value))
      return grt::IntegerRef(value);
  } else
    throw std::invalid_argument(base::strfmt("invalid column %li for resultset", (long)column).c_str());
  return grt::IntegerRef(0);
}

auto WBRecordsetResultset::intFieldValueByName(const std::string &column) -> grt::IntegerRef {
  ssize_t value;
  if (column_by_name.find(column) != column_by_name.end()) {
    if (recordset->get_field(bec::NodeId(cursor), column_by_name[column], value))
      return grt::IntegerRef(value);
  }
  throw std::invalid_argument(base::strfmt("invalid column %s for resultset", column.c_str()).c_str());
  return grt::IntegerRef(0);
}

auto WBRecordsetResultset::nextRow() -> grt::IntegerRef {
  if (cursor < recordset->count() - 1) {
    ++cursor;
    return grt::IntegerRef(1);
  }
  return grt::IntegerRef(0);
}

auto WBRecordsetResultset::previousRow() -> grt::IntegerRef {
  if (cursor > 0) {
    --cursor;
    return grt::IntegerRef(1);
  }
  return grt::IntegerRef(0);
}

auto WBRecordsetResultset::refresh() -> void {
  recordset->refresh();
}

auto WBRecordsetResultset::stringFieldValue(ssize_t column) -> grt::StringRef {
  std::string value;
  if (column >= 0 && (size_t)column < recordset->get_column_count()) {
    if (recordset->get_field_repr_no_truncate(bec::NodeId(cursor), column, value))
      return grt::StringRef(value);
  } else
    throw std::invalid_argument(base::strfmt("invalid column %li for resultset", (long)column).c_str());
  return grt::StringRef(); // NULL
}

auto WBRecordsetResultset::stringFieldValueByName(const std::string &column) -> grt::StringRef {
  std::string value;
  if (column_by_name.find(column) != column_by_name.end()) {
    if (recordset->get_field_repr_no_truncate(bec::NodeId(cursor), column_by_name[column], value))
      return grt::StringRef(value);
  }
  throw std::invalid_argument(base::strfmt("invalid column %s for resultset", column.c_str()).c_str());
  return grt::StringRef(); // NULL
}

auto WBRecordsetResultset::geoStringFieldValue(ssize_t column) -> grt::StringRef {
  return getGeoRepresentation(stringFieldValue(column), false);
}

auto WBRecordsetResultset::geoStringFieldValueByName(const std::string &column) -> grt::StringRef {
  return getGeoRepresentation(stringFieldValueByName(column), false);
}

auto WBRecordsetResultset::geoJsonFieldValue(ssize_t column) -> grt::StringRef {
  return getGeoRepresentation(stringFieldValue(column), false);
}

auto WBRecordsetResultset::geoJsonFieldValueByName(const std::string &column) -> grt::StringRef {
  return getGeoRepresentation(stringFieldValueByName(column), false);
}

auto WBRecordsetResultset::saveFieldValueToFile(ssize_t column, const std::string &file) -> grt::IntegerRef {
  if (column >= 0 && (size_t)column < recordset->get_column_count()) {
    recordset->save_to_file(bec::NodeId(cursor), column, file);
    return grt::IntegerRef(1);
  }
  return grt::IntegerRef(0);
}

//================================================================================

class WBPUBLICBACKEND_PUBLIC_FUNC CPPResultsetResultset : public db_query_Resultset::ImplData {
  std::shared_ptr<sql::ResultSet> recordset;

public:
  CPPResultsetResultset(db_query_ResultsetRef aself, std::shared_ptr<sql::ResultSet> rset)
    : ImplData(aself), recordset(rset) {
    sql::ResultSetMetaData *meta(recordset->getMetaData());
    const int last_column = meta->getColumnCount();
    for (int i = 1; i <= last_column; i++) {
      column_by_name[meta->getColumnLabel(i)] = i;

      std::string type;
      switch (meta->getColumnType(i)) {
        case sql::DataType::UNKNOWN:
          type = "unknown";
          break;
        case sql::DataType::BIT:
        case sql::DataType::TINYINT:
        case sql::DataType::SMALLINT:
        case sql::DataType::MEDIUMINT:
        case sql::DataType::INTEGER:
        case sql::DataType::BIGINT:
          type = "numeric";
          break;

        case sql::DataType::REAL:
        case sql::DataType::DOUBLE:
          type = "numeric";
          break;

        case sql::DataType::DECIMAL:
        case sql::DataType::NUMERIC:
          type = "string";
          break;

        case sql::DataType::CHAR:
        case sql::DataType::VARCHAR:
          type = "string";
          break;

        case sql::DataType::BINARY:
        case sql::DataType::VARBINARY:
        case sql::DataType::LONGVARCHAR:
        case sql::DataType::LONGVARBINARY:
          type = "blob";
          break;

        case sql::DataType::TIMESTAMP:
          type = "string";
          break;
        case sql::DataType::DATE:
          type = "string";
          break;
        case sql::DataType::TIME:
          type = "numeric";
          break;

        case sql::DataType::YEAR:
          type = "numeric";
          break;
        case sql::DataType::GEOMETRY:
          type = "string";
          break;
        case sql::DataType::ENUM:
        case sql::DataType::SET:
          type = "string";
          break;
        case sql::DataType::JSON:
          type = "json";
          break;
        case sql::DataType::SQLNULL:
          type = "null";
          break;
      }

      db_query_ResultsetColumnRef column(grt::Initialized);

      column->owner(aself);
      column->name(std::string(meta->getColumnLabel(i)));
      column->columnType(type);

      self->columns().insert(column);
    }
  }

  virtual auto sql() const -> grt::StringRef {
    return grt::StringRef("");
  }

  virtual auto currentRow() const -> grt::IntegerRef {
    return grt::IntegerRef((long)recordset->getRow());
  }

  virtual auto rowCount() const -> grt::IntegerRef {
    return grt::IntegerRef(recordset->rowsCount());
  }

  virtual auto floatFieldValue(ssize_t column) -> grt::DoubleRef {
    if (column >= 0 && column < (ssize_t)column_by_name.size())
      return grt::DoubleRef(recordset->getDouble((uint32_t)column + 1)); // Hard coded to 32bit, <sigh>.
    throw std::invalid_argument(base::strfmt("invalid column %li for resultset", (long)column).c_str());
    return grt::DoubleRef(0.0);
  }

  virtual auto floatFieldValueByName(const std::string &column) -> grt::DoubleRef {
    if (column_by_name.find(column) != column_by_name.end()) {
      return grt::DoubleRef(recordset->getDouble((uint32_t)column_by_name[column]));
    }
    throw std::invalid_argument(base::strfmt("invalid column %s for resultset", column.c_str()).c_str());
    return grt::DoubleRef(0.0);
  }

  virtual auto goToFirstRow() -> grt::IntegerRef {
    return grt::IntegerRef(recordset->first());
  }

  virtual auto goToLastRow() -> grt::IntegerRef {
    return grt::IntegerRef(recordset->last());
  }

  virtual auto goToRow(ssize_t row) -> grt::IntegerRef {
    return grt::IntegerRef(recordset->absolute((int)row));
  }

  virtual auto intFieldValue(ssize_t column) -> grt::IntegerRef {
    if (column >= 0 && column < (ssize_t)column_by_name.size()) {
#ifdef ENVIRONMENT_64
      return grt::IntegerRef((size_t)recordset->getInt64((uint32_t)column + 1));
#else
      return grt::IntegerRef(recordset->getInt((uint32_t)column + 1));
#endif
    }
    throw std::invalid_argument(base::strfmt("invalid column %li for resultset", (long)column).c_str());
    return grt::IntegerRef(0);
  }

  virtual auto intFieldValueByName(const std::string &column) -> grt::IntegerRef {
    if (column_by_name.find(column) != column_by_name.end()) {
#ifdef ENVIRONMENT_64
      return grt::IntegerRef((size_t)recordset->getInt64((uint32_t)column_by_name[column]));
#else
      return grt::IntegerRef(recordset->getInt((uint32_t)column_by_name[column]));
#endif
    }
    throw std::invalid_argument(base::strfmt("invalid column %s for resultset", column.c_str()).c_str());
    return grt::IntegerRef(0);
  }

  virtual auto nextRow() -> grt::IntegerRef {
    return grt::IntegerRef(recordset->next());
  }

  virtual auto previousRow() -> grt::IntegerRef {
    return grt::IntegerRef(recordset->previous());
  }

  virtual auto refresh() -> void {
  }

  virtual auto stringFieldValue(ssize_t column) -> grt::StringRef {
    if (column >= 0 && column < (ssize_t)column_by_name.size())
      return grt::StringRef(recordset->getString((uint32_t)column + 1));
    throw std::invalid_argument(base::strfmt("invalid column %li for resultset", (long)column).c_str());
    return grt::StringRef(); // NULL
  }

  virtual auto stringFieldValueByName(const std::string &column) -> grt::StringRef {
    if (column_by_name.find(column) != column_by_name.end()) {
      return grt::StringRef(recordset->getString((uint32_t)column_by_name[column]));
    }
    throw std::invalid_argument(base::strfmt("invalid column %s for resultset", column.c_str()).c_str());
    return grt::StringRef(); // NULL
  }

  virtual auto geoStringFieldValue(ssize_t column) -> grt::StringRef {
    if (column >= 0 && column < (ssize_t)column_by_name.size()) {
      grt::StringRef data(recordset->getString((uint32_t)column + 1));

      try {
        return getGeoRepresentation(data, false);
      } catch (std::exception &) {
        throw std::invalid_argument(
          base::strfmt("unable to convert geometry data to WKT for column %li", (long)column).c_str());
      }
    }
    throw std::invalid_argument(base::strfmt("invalid column %li for resultset", (long)column).c_str());
  }

  virtual auto geoStringFieldValueByName(const std::string &column) -> grt::StringRef {
    if (column_by_name.find(column) != column_by_name.end()) {
      grt::StringRef data(recordset->getString((uint32_t)column_by_name[column]));
      try {
        return getGeoRepresentation(data, false);
      } catch (std::exception &) {
        throw std::invalid_argument(
          base::strfmt("unable to convert geometry data to WKT for column %s", column.c_str()).c_str());
      }
    }
    throw std::invalid_argument(base::strfmt("invalid column %s for resultset", column.c_str()).c_str());
  }

  virtual auto geoJsonFieldValue(ssize_t column) -> grt::StringRef {
    if (column >= 0 && column < (ssize_t)column_by_name.size()) {
      grt::StringRef data(recordset->getString((uint32_t)column + 1));
      try {
        return getGeoRepresentation(data, true);
      } catch (std::exception &) {
        throw std::invalid_argument(
          base::strfmt("unable to convert geometry data to WKT for column %li", (long)column).c_str());
      }
    }
    throw std::invalid_argument(base::strfmt("invalid column %li for resultset", (long)column).c_str());
    return grt::StringRef(); // NULL
  }

  virtual auto geoJsonFieldValueByName(const std::string &column) -> grt::StringRef {
    if (column_by_name.find(column) != column_by_name.end()) {
      grt::StringRef data(recordset->getString((uint32_t)column_by_name[column]));
      try {
        return getGeoRepresentation(data, true);
      } catch (std::exception &) {
        throw std::invalid_argument(
          base::strfmt("unable to convert geometry data to WKT for column %s", column.c_str()).c_str());
      }
    }
    throw std::invalid_argument(base::strfmt("invalid column %s for resultset", column.c_str()).c_str());
    return grt::StringRef(); // NULL
  }

  virtual auto saveFieldValueToFile(ssize_t column, const std::string &file) -> grt::IntegerRef {
    return grt::IntegerRef(0);
  }
};

//================================================================================

auto grtwrap_recordset(GrtObjectRef owner, Recordset::Ref rset) -> db_query_ResultsetRef {
  db_query_ResultsetRef object(grt::Initialized);

  db_query_Resultset::ImplData *data = new WBRecordsetResultset(object, rset);

  object->owner(owner);

  object->set_data(data);

  return object;
}

auto grtwrap_recordset(GrtObjectRef owner, std::shared_ptr<sql::ResultSet> rset) -> db_query_ResultsetRef {
  db_query_ResultsetRef object(grt::Initialized);

  db_query_Resultset::ImplData *data = new CPPResultsetResultset(object, rset);

  object->owner(owner);

  object->set_data(data);

  return object;
}

auto db_query_Resultset::init() -> void {
  // _data init is delayed and done by grtwrap_recordset
}

db_query_Resultset::~db_query_Resultset() {
  delete _data;
}

auto db_query_Resultset::set_data(ImplData *data) -> void {
  _data = data;
}

auto db_query_Resultset::currentRow() const -> grt::IntegerRef {
  if (_data)
    return _data->currentRow();
  return grt::IntegerRef(0);
}

auto db_query_Resultset::sql() const -> grt::StringRef {
  return _data ? _data->sql() : grt::StringRef();
}

auto db_query_Resultset::rowCount() const -> grt::IntegerRef {
  return _data ? _data->rowCount() : grt::IntegerRef(0);
}

auto db_query_Resultset::floatFieldValue(ssize_t column) -> grt::DoubleRef {
  return _data ? _data->floatFieldValue(column) : grt::DoubleRef(0.0);
}

auto db_query_Resultset::floatFieldValueByName(const std::string &column) -> grt::DoubleRef {
  return _data ? _data->floatFieldValueByName(column) : grt::DoubleRef(0.0);
}

auto db_query_Resultset::goToFirstRow() -> grt::IntegerRef {
  return _data ? _data->goToFirstRow() : grt::IntegerRef(0);
}

auto db_query_Resultset::goToLastRow() -> grt::IntegerRef {
  return _data ? _data->goToLastRow() : grt::IntegerRef(0);
}

auto db_query_Resultset::goToRow(ssize_t row) -> grt::IntegerRef {
  return _data ? _data->goToRow(row) : grt::IntegerRef(0);
}

auto db_query_Resultset::intFieldValue(ssize_t column) -> grt::IntegerRef {
  return _data ? _data->intFieldValue(column) : grt::IntegerRef(0);
}

auto db_query_Resultset::intFieldValueByName(const std::string &column) -> grt::IntegerRef {
  return _data ? _data->intFieldValueByName(column) : grt::IntegerRef(0);
}

auto db_query_Resultset::nextRow() -> grt::IntegerRef {
  return _data ? _data->nextRow() : grt::IntegerRef(0);
}

auto db_query_Resultset::previousRow() -> grt::IntegerRef {
  return _data ? _data->previousRow() : grt::IntegerRef(0);
}

auto db_query_Resultset::refresh() -> grt::IntegerRef {
  if (_data)
    _data->refresh();

  return grt::IntegerRef(0);
}

auto db_query_Resultset::stringFieldValue(ssize_t column) -> grt::StringRef {
  return _data ? _data->stringFieldValue(column) : grt::StringRef();
}

auto db_query_Resultset::stringFieldValueByName(const std::string &column) -> grt::StringRef {
  return _data ? _data->stringFieldValueByName(column) : grt::StringRef();
}

auto db_query_Resultset::geoStringFieldValue(ssize_t column) -> grt::StringRef {
  return _data ? _data->geoStringFieldValue(column) : grt::StringRef();
}

auto db_query_Resultset::geoStringFieldValueByName(const std::string &column) -> grt::StringRef {
  return _data ? _data->geoStringFieldValueByName(column) : grt::StringRef();
}

auto db_query_Resultset::geoJsonFieldValue(ssize_t column) -> grt::StringRef {
  return _data ? _data->geoJsonFieldValue(column) : grt::StringRef();
}

auto db_query_Resultset::geoJsonFieldValueByName(const std::string &column) -> grt::StringRef {
  return _data ? _data->geoJsonFieldValueByName(column) : grt::StringRef();
}

auto db_query_Resultset::saveFieldValueToFile(ssize_t column, const std::string &file) -> grt::IntegerRef {
  return _data ? _data->saveFieldValueToFile(column, file) : grt::IntegerRef(0);
}
