/*
 * Copyright (c) 2011, 2019, Oracle and/or its affiliates. All rights reserved.
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
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#pragma once

#ifndef _MSC_VER
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

#include "grt.h"

#ifdef _MSC_VER
  #pragma warning(disable: 4355) // 'this' : used in base member initializer list
  #ifdef GRT_STRUCTS_DB_QUERY_EXPORT
  #define GRT_STRUCTS_DB_QUERY_PUBLIC __declspec(dllexport)
#else
  #define GRT_STRUCTS_DB_QUERY_PUBLIC __declspec(dllimport)
#endif
#else
  #define GRT_STRUCTS_DB_QUERY_PUBLIC
#endif

#include "grts/structs.h"
#include "grts/structs.ui.h"
#include "grts/structs.db.mgmt.h"

class db_query_LiveDBObject;
typedef grt::Ref<db_query_LiveDBObject> db_query_LiveDBObjectRef;
class db_query_ResultsetColumn;
typedef grt::Ref<db_query_ResultsetColumn> db_query_ResultsetColumnRef;
class db_query_Resultset;
typedef grt::Ref<db_query_Resultset> db_query_ResultsetRef;
class db_query_EditableResultset;
typedef grt::Ref<db_query_EditableResultset> db_query_EditableResultsetRef;
class db_query_ResultPanel;
typedef grt::Ref<db_query_ResultPanel> db_query_ResultPanelRef;
class db_query_QueryBuffer;
typedef grt::Ref<db_query_QueryBuffer> db_query_QueryBufferRef;
class db_query_QueryEditor;
typedef grt::Ref<db_query_QueryEditor> db_query_QueryEditorRef;
class db_query_Editor;
typedef grt::Ref<db_query_Editor> db_query_EditorRef;


namespace mforms { 
  class Object;
}; 

namespace grt { 
  class AutoPyObject;
}; 

/** object name from a live database */
class  db_query_LiveDBObject : public GrtObject {
  typedef GrtObject super;

public:
  db_query_LiveDBObject(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _schemaName(""),
      _type("") {
  }

  static auto static_class_name() -> std::string {
    return "db.query.LiveDBObject";
  }

  /**
   * Getter for attribute schemaName
   *
   * name of the schema the object belongs to
   * \par In Python:
   *    value = obj.schemaName
   */
  auto schemaName() const -> grt::StringRef { return _schemaName; }

  /**
   * Setter for attribute schemaName
   * 
   * name of the schema the object belongs to
   * \par In Python:
   *   obj.schemaName = value
   */
  virtual auto schemaName(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_schemaName);
    _schemaName = value;
    member_changed("schemaName", ovalue, value);
  }

  /**
   * Getter for attribute type
   *
   * type of the object (schema, table, view, routine)
   * \par In Python:
   *    value = obj.type
   */
  auto type() const -> grt::StringRef { return _type; }

  /**
   * Setter for attribute type
   * 
   * type of the object (schema, table, view, routine)
   * \par In Python:
   *   obj.type = value
   */
  virtual auto type(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_type);
    _type = value;
    member_changed("type", ovalue, value);
  }

protected:

  grt::StringRef _schemaName;
  grt::StringRef _type;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new db_query_LiveDBObject());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&db_query_LiveDBObject::create);
    {
      void (db_query_LiveDBObject::*setter)(const grt::StringRef &) = &db_query_LiveDBObject::schemaName;
      grt::StringRef (db_query_LiveDBObject::*getter)() const = &db_query_LiveDBObject::schemaName;
      meta->bind_member("schemaName", new grt::MetaClass::Property<db_query_LiveDBObject,grt::StringRef>(getter, setter));
    }
    {
      void (db_query_LiveDBObject::*setter)(const grt::StringRef &) = &db_query_LiveDBObject::type;
      grt::StringRef (db_query_LiveDBObject::*getter)() const = &db_query_LiveDBObject::type;
      meta->bind_member("type", new grt::MetaClass::Property<db_query_LiveDBObject,grt::StringRef>(getter, setter));
    }
  }
};

/** a database resultset column */
class  db_query_ResultsetColumn : public GrtObject {
  typedef GrtObject super;

public:
  db_query_ResultsetColumn(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _columnType("") {
  }

  static auto static_class_name() -> std::string {
    return "db.query.ResultsetColumn";
  }

  /**
   * Getter for attribute columnType
   *
   * the type of the column, string, int, real, blob, date, time, datetime, geo
   * \par In Python:
   *    value = obj.columnType
   */
  auto columnType() const -> grt::StringRef { return _columnType; }

  /**
   * Setter for attribute columnType
   * 
   * the type of the column, string, int, real, blob, date, time, datetime, geo
   * \par In Python:
   *   obj.columnType = value
   */
  virtual auto columnType(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_columnType);
    _columnType = value;
    member_changed("columnType", ovalue, value);
  }

protected:

  grt::StringRef _columnType;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new db_query_ResultsetColumn());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&db_query_ResultsetColumn::create);
    {
      void (db_query_ResultsetColumn::*setter)(const grt::StringRef &) = &db_query_ResultsetColumn::columnType;
      grt::StringRef (db_query_ResultsetColumn::*getter)() const = &db_query_ResultsetColumn::columnType;
      meta->bind_member("columnType", new grt::MetaClass::Property<db_query_ResultsetColumn,grt::StringRef>(getter, setter));
    }
  }
};

/** a query resultset. This object does not allow changes to the resultset, if you need to edit the resultset, see \ref db_query_EditableResultset */
class GRT_STRUCTS_DB_QUERY_PUBLIC db_query_Resultset : public GrtObject {
  typedef GrtObject super;

public:
  class ImplData;
  friend class ImplData;
  db_query_Resultset(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _columns(this, false),
      _data(nullptr) {
  }

  virtual ~db_query_Resultset();

  static auto static_class_name() -> std::string {
    return "db.query.Resultset";
  }

  // columns is owned by db_query_Resultset
  /**
   * Getter for attribute columns (read-only)
   *
   * the columns of the resultset
   * \par In Python:
   *    value = obj.columns
   */
  auto columns() const -> grt::ListRef<db_query_ResultsetColumn> { return _columns; }


private: // The next attribute is read-only.
  virtual auto columns(const grt::ListRef<db_query_ResultsetColumn> &value) -> void {
    grt::ValueRef ovalue(_columns);

    _columns = value;
    owned_member_changed("columns", ovalue, value);
  }
public:

  /**
   * Getter for attribute currentRow (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.currentRow
   */
  auto currentRow() const -> grt::IntegerRef;


private: // The next attribute is read-only.
public:

  /**
   * Getter for attribute rowCount (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.rowCount
   */
  auto rowCount() const -> grt::IntegerRef;


private: // The next attribute is read-only.
public:

  /**
   * Getter for attribute sql (read-only)
   *
   * the SQL statement that generated this resultset
   * \par In Python:
   *    value = obj.sql
   */
  auto sql() const -> grt::StringRef;


private: // The next attribute is read-only.
public:

  /**
   * Method. returns the float contents of the field at the given column index and current row
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto floatFieldValue(ssize_t column) -> grt::DoubleRef;
  /**
   * Method. returns the float contents of the field at the given column name and current row
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto floatFieldValueByName(const std::string &column) -> grt::DoubleRef;
  /**
   * Method. returns the contents of the field at the given column index and current geometry row as a geoJson string. If the column type is not geometry or it's empty, it will return empty string
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto geoJsonFieldValue(ssize_t column) -> grt::StringRef;
  /**
   * Method. returns the contents of the field at the given column name and current geometry row as a geoJson string. If the column type is not geometry or it's empty, it will return empty string
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto geoJsonFieldValueByName(const std::string &column) -> grt::StringRef;
  /**
   * Method. returns the contents of the field at the given column index and current geometry row as a string. If the column type is not geometry or it's empty, it will return empty string
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto geoStringFieldValue(ssize_t column) -> grt::StringRef;
  /**
   * Method. returns the contents of the field at the given column name and current geometry row as a string. If the column type is not geometry or it's empty, it will return empty string
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto geoStringFieldValueByName(const std::string &column) -> grt::StringRef;
  /**
   * Method. sets the current row index to the 1st
   * \return (boolean) 1 on success or 0 if the row number is out of bounds
   */
  virtual auto goToFirstRow() -> grt::IntegerRef;
  /**
   * Method. sets the current row index to the last
   * \return (boolean) 1 on success or 0 if the row number is out of bounds
   */
  virtual auto goToLastRow() -> grt::IntegerRef;
  /**
   * Method. sets the current row pointer to the given index
   * \param row 
   * \return (boolean) 1 on success or 0 if the row number is out of bounds
   */
  virtual auto goToRow(ssize_t row) -> grt::IntegerRef;
  /**
   * Method. returns the integer contents of the field at the given column index and current row
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto intFieldValue(ssize_t column) -> grt::IntegerRef;
  /**
   * Method. returns the integer contents of the field at the given column name and current row
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto intFieldValueByName(const std::string &column) -> grt::IntegerRef;
  /**
   * Method. moves the current row pointer to the next one
   * \return (boolean) 1 on success or 0 if the new row number is out of bounds
   */
  virtual auto nextRow() -> grt::IntegerRef;
  /**
   * Method. moves the current row pointer to the previous one
   * \return (boolean) 1 on success or 0 if the new row number is out of bounds
   */
  virtual auto previousRow() -> grt::IntegerRef;
  /**
   * Method. refreshes the resultset, re-executing the originator query
   * \return 
   */
  virtual auto refresh() -> grt::IntegerRef;
  /**
   * Method. saves the contents of the field at given column and current row to a file
   * \param column 
   * \param file 
   * \return (boolean)
   */
  virtual auto saveFieldValueToFile(ssize_t column, const std::string &file) -> grt::IntegerRef;
  /**
   * Method. returns the contents of the field at the given column index and current row as a string. If the column type is not string, it will be converted
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto stringFieldValue(ssize_t column) -> grt::StringRef;
  /**
   * Method. returns the contents of the field at the given column name and current row as a string. If the column type is not string, it will be converted
   * \param column 
   * \return value stored in cell (can be null)
   */
  virtual auto stringFieldValueByName(const std::string &column) -> grt::StringRef;

  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  grt::ListRef<db_query_ResultsetColumn> _columns;// owned

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new db_query_Resultset());
  }

  static grt::ValueRef call_floatFieldValue(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->floatFieldValue(grt::IntegerRef::cast_from(args[0])); }

  static grt::ValueRef call_floatFieldValueByName(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->floatFieldValueByName(grt::StringRef::cast_from(args[0])); }

  static grt::ValueRef call_geoJsonFieldValue(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->geoJsonFieldValue(grt::IntegerRef::cast_from(args[0])); }

  static grt::ValueRef call_geoJsonFieldValueByName(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->geoJsonFieldValueByName(grt::StringRef::cast_from(args[0])); }

  static grt::ValueRef call_geoStringFieldValue(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->geoStringFieldValue(grt::IntegerRef::cast_from(args[0])); }

  static grt::ValueRef call_geoStringFieldValueByName(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->geoStringFieldValueByName(grt::StringRef::cast_from(args[0])); }

  static grt::ValueRef call_goToFirstRow(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->goToFirstRow(); }

  static grt::ValueRef call_goToLastRow(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->goToLastRow(); }

  static grt::ValueRef call_goToRow(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->goToRow(grt::IntegerRef::cast_from(args[0])); }

  static grt::ValueRef call_intFieldValue(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->intFieldValue(grt::IntegerRef::cast_from(args[0])); }

  static grt::ValueRef call_intFieldValueByName(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->intFieldValueByName(grt::StringRef::cast_from(args[0])); }

  static grt::ValueRef call_nextRow(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->nextRow(); }

  static grt::ValueRef call_previousRow(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->previousRow(); }

  static grt::ValueRef call_refresh(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->refresh(); }

  static grt::ValueRef call_saveFieldValueToFile(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->saveFieldValueToFile(grt::IntegerRef::cast_from(args[0]), grt::StringRef::cast_from(args[1])); }

  static grt::ValueRef call_stringFieldValue(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->stringFieldValue(grt::IntegerRef::cast_from(args[0])); }

  static grt::ValueRef call_stringFieldValueByName(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Resultset*>(self)->stringFieldValueByName(grt::StringRef::cast_from(args[0])); }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&db_query_Resultset::create);
    {
      void (db_query_Resultset::*setter)(const grt::ListRef<db_query_ResultsetColumn> &) = &db_query_Resultset::columns;
      grt::ListRef<db_query_ResultsetColumn> (db_query_Resultset::*getter)() const = &db_query_Resultset::columns;
      meta->bind_member("columns", new grt::MetaClass::Property<db_query_Resultset,grt::ListRef<db_query_ResultsetColumn>>(getter, setter));
    }
    meta->bind_member("currentRow", new grt::MetaClass::Property<db_query_Resultset,grt::IntegerRef>(&db_query_Resultset::currentRow));
    meta->bind_member("rowCount", new grt::MetaClass::Property<db_query_Resultset,grt::IntegerRef>(&db_query_Resultset::rowCount));
    meta->bind_member("sql", new grt::MetaClass::Property<db_query_Resultset,grt::StringRef>(&db_query_Resultset::sql));
    meta->bind_method("floatFieldValue", &db_query_Resultset::call_floatFieldValue);
    meta->bind_method("floatFieldValueByName", &db_query_Resultset::call_floatFieldValueByName);
    meta->bind_method("geoJsonFieldValue", &db_query_Resultset::call_geoJsonFieldValue);
    meta->bind_method("geoJsonFieldValueByName", &db_query_Resultset::call_geoJsonFieldValueByName);
    meta->bind_method("geoStringFieldValue", &db_query_Resultset::call_geoStringFieldValue);
    meta->bind_method("geoStringFieldValueByName", &db_query_Resultset::call_geoStringFieldValueByName);
    meta->bind_method("goToFirstRow", &db_query_Resultset::call_goToFirstRow);
    meta->bind_method("goToLastRow", &db_query_Resultset::call_goToLastRow);
    meta->bind_method("goToRow", &db_query_Resultset::call_goToRow);
    meta->bind_method("intFieldValue", &db_query_Resultset::call_intFieldValue);
    meta->bind_method("intFieldValueByName", &db_query_Resultset::call_intFieldValueByName);
    meta->bind_method("nextRow", &db_query_Resultset::call_nextRow);
    meta->bind_method("previousRow", &db_query_Resultset::call_previousRow);
    meta->bind_method("refresh", &db_query_Resultset::call_refresh);
    meta->bind_method("saveFieldValueToFile", &db_query_Resultset::call_saveFieldValueToFile);
    meta->bind_method("stringFieldValue", &db_query_Resultset::call_stringFieldValue);
    meta->bind_method("stringFieldValueByName", &db_query_Resultset::call_stringFieldValueByName);
  }
};

/** a resultset created for editing table data. Changes made to the resultset are queued to be applied when \ref applyChanges() is called */
class GRT_STRUCTS_DB_QUERY_PUBLIC db_query_EditableResultset : public db_query_Resultset {
  typedef db_query_Resultset super;

public:
  class ImplData;
  friend class ImplData;
  db_query_EditableResultset(grt::MetaClass *meta = nullptr)
    : db_query_Resultset(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _schema(""),
      _table(""),
      _data(nullptr) {
  }

  virtual ~db_query_EditableResultset();

  static auto static_class_name() -> std::string {
    return "db.query.EditableResultset";
  }

  /**
   * Getter for attribute schema
   *
   * schema name of the table
   * \par In Python:
   *    value = obj.schema
   */
  auto schema() const -> grt::StringRef { return _schema; }

  /**
   * Setter for attribute schema
   * 
   * schema name of the table
   * \par In Python:
   *   obj.schema = value
   */
  virtual auto schema(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_schema);
    _schema = value;
    member_changed("schema", ovalue, value);
  }

  /**
   * Getter for attribute table
   *
   * name of the table being edited
   * \par In Python:
   *    value = obj.table
   */
  auto table() const -> grt::StringRef { return _table; }

  /**
   * Setter for attribute table
   * 
   * name of the table being edited
   * \par In Python:
   *   obj.table = value
   */
  virtual auto table(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_table);
    _table = value;
    member_changed("table", ovalue, value);
  }

  /**
   * Method. adds a new empty row to the resultset. The row contents must be set before applying changes
   * \return 
   */
  virtual auto addNewRow() -> grt::IntegerRef;
  /**
   * Method. generates a SQL script with all pending changes made to the resultset and executes it, once confirmed through a GUI wizard
   * \return 
   */
  virtual auto applyChanges() -> grt::IntegerRef;
  /**
   * Method. marks a row from the resultset for deletion. The row will only be deleted in the target database when applyChanges() is called
   * \param column 
   * \return 
   */
  virtual auto deleteRow(ssize_t column) -> grt::IntegerRef;
  /**
   * Method. loads the contents of an external file into the current row at the given column index
   * \param column 
   * \param file 
   * \return 
   */
  virtual auto loadFieldValueFromFile(ssize_t column, const std::string &file) -> grt::IntegerRef;
  /**
   * Method. discards all changes made to the resultset
   * \return 
   */
  virtual auto revertChanges() -> grt::IntegerRef;
  /**
   * Method. sets the contents of the current row at the given column index to NULL
   * \param column 
   * \return 
   */
  virtual auto setFieldNull(ssize_t column) -> grt::IntegerRef;
  /**
   * Method. sets the contents of the current row at the given column name to NULL
   * \param column 
   * \return 
   */
  virtual auto setFieldNullByName(const std::string &column) -> grt::IntegerRef;
  /**
   * Method. sets the contents of the current row at the given column index
   * \param column 
   * \param value 
   * \return 
   */
  virtual auto setFloatFieldValue(ssize_t column, double value) -> grt::IntegerRef;
  /**
   * Method. sets the contents of the current row at the given column name
   * \param column 
   * \param value 
   * \return 
   */
  virtual auto setFloatFieldValueByName(const std::string &column, double value) -> grt::IntegerRef;
  /**
   * Method. sets the contents of the current row at the given integer type column index
   * \param column 
   * \param value 
   * \return 
   */
  virtual auto setIntFieldValue(ssize_t column, ssize_t value) -> grt::IntegerRef;
  /**
   * Method. sets the contents of the current row at the given column name
   * \param column 
   * \param value 
   * \return 
   */
  virtual auto setIntFieldValueByName(const std::string &column, ssize_t value) -> grt::IntegerRef;
  /**
   * Method. sets the contents of the current row at the given column index
   * \param column 
   * \param value 
   * \return 
   */
  virtual auto setStringFieldValue(ssize_t column, const std::string &value) -> grt::IntegerRef;
  /**
   * Method. sets the contents of the current row at the given column name
   * \param column 
   * \param value 
   * \return 
   */
  virtual auto setStringFieldValueByName(const std::string &column, const std::string &value) -> grt::IntegerRef;

  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  grt::StringRef _schema;
  grt::StringRef _table;

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new db_query_EditableResultset());
  }

  static grt::ValueRef call_addNewRow(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->addNewRow(); }

  static grt::ValueRef call_applyChanges(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->applyChanges(); }

  static grt::ValueRef call_deleteRow(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->deleteRow(grt::IntegerRef::cast_from(args[0])); }

  static grt::ValueRef call_loadFieldValueFromFile(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->loadFieldValueFromFile(grt::IntegerRef::cast_from(args[0]), grt::StringRef::cast_from(args[1])); }

  static grt::ValueRef call_revertChanges(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->revertChanges(); }

  static grt::ValueRef call_setFieldNull(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->setFieldNull(grt::IntegerRef::cast_from(args[0])); }

  static grt::ValueRef call_setFieldNullByName(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->setFieldNullByName(grt::StringRef::cast_from(args[0])); }

  static grt::ValueRef call_setFloatFieldValue(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->setFloatFieldValue(grt::IntegerRef::cast_from(args[0]), grt::DoubleRef::cast_from(args[1])); }

  static grt::ValueRef call_setFloatFieldValueByName(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->setFloatFieldValueByName(grt::StringRef::cast_from(args[0]), grt::DoubleRef::cast_from(args[1])); }

  static grt::ValueRef call_setIntFieldValue(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->setIntFieldValue(grt::IntegerRef::cast_from(args[0]), grt::IntegerRef::cast_from(args[1])); }

  static grt::ValueRef call_setIntFieldValueByName(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->setIntFieldValueByName(grt::StringRef::cast_from(args[0]), grt::IntegerRef::cast_from(args[1])); }

  static grt::ValueRef call_setStringFieldValue(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->setStringFieldValue(grt::IntegerRef::cast_from(args[0]), grt::StringRef::cast_from(args[1])); }

  static grt::ValueRef call_setStringFieldValueByName(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_EditableResultset*>(self)->setStringFieldValueByName(grt::StringRef::cast_from(args[0]), grt::StringRef::cast_from(args[1])); }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&db_query_EditableResultset::create);
    {
      void (db_query_EditableResultset::*setter)(const grt::StringRef &) = &db_query_EditableResultset::schema;
      grt::StringRef (db_query_EditableResultset::*getter)() const = &db_query_EditableResultset::schema;
      meta->bind_member("schema", new grt::MetaClass::Property<db_query_EditableResultset,grt::StringRef>(getter, setter));
    }
    {
      void (db_query_EditableResultset::*setter)(const grt::StringRef &) = &db_query_EditableResultset::table;
      grt::StringRef (db_query_EditableResultset::*getter)() const = &db_query_EditableResultset::table;
      meta->bind_member("table", new grt::MetaClass::Property<db_query_EditableResultset,grt::StringRef>(getter, setter));
    }
    meta->bind_method("addNewRow", &db_query_EditableResultset::call_addNewRow);
    meta->bind_method("applyChanges", &db_query_EditableResultset::call_applyChanges);
    meta->bind_method("deleteRow", &db_query_EditableResultset::call_deleteRow);
    meta->bind_method("loadFieldValueFromFile", &db_query_EditableResultset::call_loadFieldValueFromFile);
    meta->bind_method("revertChanges", &db_query_EditableResultset::call_revertChanges);
    meta->bind_method("setFieldNull", &db_query_EditableResultset::call_setFieldNull);
    meta->bind_method("setFieldNullByName", &db_query_EditableResultset::call_setFieldNullByName);
    meta->bind_method("setFloatFieldValue", &db_query_EditableResultset::call_setFloatFieldValue);
    meta->bind_method("setFloatFieldValueByName", &db_query_EditableResultset::call_setFloatFieldValueByName);
    meta->bind_method("setIntFieldValue", &db_query_EditableResultset::call_setIntFieldValue);
    meta->bind_method("setIntFieldValueByName", &db_query_EditableResultset::call_setIntFieldValueByName);
    meta->bind_method("setStringFieldValue", &db_query_EditableResultset::call_setStringFieldValue);
    meta->bind_method("setStringFieldValueByName", &db_query_EditableResultset::call_setStringFieldValueByName);
  }
};

/** the GUI object that holds a query resultset and other related things */
class  db_query_ResultPanel : public GrtObject {
  typedef GrtObject super;

public:
  class ImplData;
  friend class ImplData;
  db_query_ResultPanel(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())) {
  }

  static auto static_class_name() -> std::string {
    return "db.query.ResultPanel";
  }

  // dockingPoint is owned by db_query_ResultPanel
  /**
   * Getter for attribute dockingPoint
   *
   * docking point for plugins to insert new tabs. The string argument of dock_view must point to an icon file.
   * \par In Python:
   *    value = obj.dockingPoint
   */
  auto dockingPoint() const -> mforms_ObjectReferenceRef { return _dockingPoint; }

  /**
   * Setter for attribute dockingPoint
   * 
   * docking point for plugins to insert new tabs. The string argument of dock_view must point to an icon file.
   * \par In Python:
   *   obj.dockingPoint = value
   */
  virtual auto dockingPoint(const mforms_ObjectReferenceRef &value) -> void {
    grt::ValueRef ovalue(_dockingPoint);

    _dockingPoint = value;
    owned_member_changed("dockingPoint", ovalue, value);
  }

  // resultset is owned by db_query_ResultPanel
  /**
   * Getter for attribute resultset
   *
   * the resultset grid. May be NULL
   * \par In Python:
   *    value = obj.resultset
   */
  auto resultset() const -> db_query_ResultsetRef { return _resultset; }

  /**
   * Setter for attribute resultset
   * 
   * the resultset grid. May be NULL
   * \par In Python:
   *   obj.resultset = value
   */
  virtual auto resultset(const db_query_ResultsetRef &value) -> void {
    grt::ValueRef ovalue(_resultset);

    _resultset = value;
    owned_member_changed("resultset", ovalue, value);
  }

protected:

  mforms_ObjectReferenceRef _dockingPoint;// owned
  db_query_ResultsetRef _resultset;// owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new db_query_ResultPanel());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&db_query_ResultPanel::create);
    {
      void (db_query_ResultPanel::*setter)(const mforms_ObjectReferenceRef &) = &db_query_ResultPanel::dockingPoint;
      mforms_ObjectReferenceRef (db_query_ResultPanel::*getter)() const = &db_query_ResultPanel::dockingPoint;
      meta->bind_member("dockingPoint", new grt::MetaClass::Property<db_query_ResultPanel,mforms_ObjectReferenceRef>(getter, setter));
    }
    {
      void (db_query_ResultPanel::*setter)(const db_query_ResultsetRef &) = &db_query_ResultPanel::resultset;
      db_query_ResultsetRef (db_query_ResultPanel::*getter)() const = &db_query_ResultPanel::resultset;
      meta->bind_member("resultset", new grt::MetaClass::Property<db_query_ResultPanel,db_query_ResultsetRef>(getter, setter));
    }
  }
};

/** a proxy to a SQL script editor buffer.\n This object cannot be instantiated directly. */
class GRT_STRUCTS_DB_QUERY_PUBLIC db_query_QueryBuffer : public GrtObject {
  typedef GrtObject super;

public:
  class ImplData;
  friend class ImplData;
  db_query_QueryBuffer(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _data(nullptr) {
  }

  virtual ~db_query_QueryBuffer();

  static auto static_class_name() -> std::string {
    return "db.query.QueryBuffer";
  }

  /**
   * Getter for attribute currentStatement (read-only)
   *
   * the SQL statement at current cursor location
   * \par In Python:
   *    value = obj.currentStatement
   */
  auto currentStatement() const -> grt::StringRef;


private: // The next attribute is read-only.
public:

  /**
   * Getter for attribute insertionPoint
   *
   * gets or sets the position of the current text insertion point (caret/cursor)
   * \par In Python:
   *    value = obj.insertionPoint
   */
  auto insertionPoint() const -> grt::IntegerRef;

  /**
   * Setter for attribute insertionPoint
   * 
   * gets or sets the position of the current text insertion point (caret/cursor)
   * \par In Python:
   *   obj.insertionPoint = value
   */
  virtual auto insertionPoint(const grt::IntegerRef &value) -> void;

  /**
   * Getter for attribute script (read-only)
   *
   * full contents of the script editor buffer
   * \par In Python:
   *    value = obj.script
   */
  auto script() const -> grt::StringRef;


private: // The next attribute is read-only.
public:

  /**
   * Getter for attribute selectedText (read-only)
   *
   * selected text
   * \par In Python:
   *    value = obj.selectedText
   */
  auto selectedText() const -> grt::StringRef;


private: // The next attribute is read-only.
public:

  /**
   * Getter for attribute selectionEnd
   *
   * ending index of text selection
   * \par In Python:
   *    value = obj.selectionEnd
   */
  auto selectionEnd() const -> grt::IntegerRef;

  /**
   * Setter for attribute selectionEnd
   * 
   * ending index of text selection
   * \par In Python:
   *   obj.selectionEnd = value
   */
  virtual auto selectionEnd(const grt::IntegerRef &value) -> void;

  /**
   * Getter for attribute selectionStart
   *
   * starting index of text selection
   * \par In Python:
   *    value = obj.selectionStart
   */
  auto selectionStart() const -> grt::IntegerRef;

  /**
   * Setter for attribute selectionStart
   * 
   * starting index of text selection
   * \par In Python:
   *   obj.selectionStart = value
   */
  virtual auto selectionStart(const grt::IntegerRef &value) -> void;

  /**
   * Method. replace the contents of the query buffer with the provided text
   * \param text 
   * \return 
   */
  virtual auto replaceContents(const std::string &text) -> grt::IntegerRef;
  /**
   * Method. replace the statement text under the cursor with the provided one, also selecting it
   * \param text 
   * \return 
   */
  virtual auto replaceCurrentStatement(const std::string &text) -> grt::IntegerRef;
  /**
   * Method. replace the currently selected text with the provided one, also selecting it
   * \param text 
   * \return 
   */
  virtual auto replaceSelection(const std::string &text) -> grt::IntegerRef;

  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:


private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new db_query_QueryBuffer());
  }

  static grt::ValueRef call_replaceContents(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_QueryBuffer*>(self)->replaceContents(grt::StringRef::cast_from(args[0])); }

  static grt::ValueRef call_replaceCurrentStatement(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_QueryBuffer*>(self)->replaceCurrentStatement(grt::StringRef::cast_from(args[0])); }

  static grt::ValueRef call_replaceSelection(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_QueryBuffer*>(self)->replaceSelection(grt::StringRef::cast_from(args[0])); }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&db_query_QueryBuffer::create);
    meta->bind_member("currentStatement", new grt::MetaClass::Property<db_query_QueryBuffer,grt::StringRef>(&db_query_QueryBuffer::currentStatement));
    {
      void (db_query_QueryBuffer::*setter)(const grt::IntegerRef &) = &db_query_QueryBuffer::insertionPoint;
      grt::IntegerRef (db_query_QueryBuffer::*getter)() const = &db_query_QueryBuffer::insertionPoint;
      meta->bind_member("insertionPoint", new grt::MetaClass::Property<db_query_QueryBuffer,grt::IntegerRef>(getter, setter));
    }
    meta->bind_member("script", new grt::MetaClass::Property<db_query_QueryBuffer,grt::StringRef>(&db_query_QueryBuffer::script));
    meta->bind_member("selectedText", new grt::MetaClass::Property<db_query_QueryBuffer,grt::StringRef>(&db_query_QueryBuffer::selectedText));
    {
      void (db_query_QueryBuffer::*setter)(const grt::IntegerRef &) = &db_query_QueryBuffer::selectionEnd;
      grt::IntegerRef (db_query_QueryBuffer::*getter)() const = &db_query_QueryBuffer::selectionEnd;
      meta->bind_member("selectionEnd", new grt::MetaClass::Property<db_query_QueryBuffer,grt::IntegerRef>(getter, setter));
    }
    {
      void (db_query_QueryBuffer::*setter)(const grt::IntegerRef &) = &db_query_QueryBuffer::selectionStart;
      grt::IntegerRef (db_query_QueryBuffer::*getter)() const = &db_query_QueryBuffer::selectionStart;
      meta->bind_member("selectionStart", new grt::MetaClass::Property<db_query_QueryBuffer,grt::IntegerRef>(getter, setter));
    }
    meta->bind_method("replaceContents", &db_query_QueryBuffer::call_replaceContents);
    meta->bind_method("replaceCurrentStatement", &db_query_QueryBuffer::call_replaceCurrentStatement);
    meta->bind_method("replaceSelection", &db_query_QueryBuffer::call_replaceSelection);
  }
};

class  db_query_QueryEditor : public db_query_QueryBuffer {
  typedef db_query_QueryBuffer super;

public:
  class ImplData;
  friend class ImplData;
  db_query_QueryEditor(grt::MetaClass *meta = nullptr)
    : db_query_QueryBuffer(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _resultPanels(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "db.query.QueryEditor";
  }

  /**
   * Getter for attribute activeResultPanel
   *
   * result panel that is currently selected in UI
   * \par In Python:
   *    value = obj.activeResultPanel
   */
  auto activeResultPanel() const -> db_query_ResultPanelRef { return _activeResultPanel; }

  /**
   * Setter for attribute activeResultPanel
   * 
   * result panel that is currently selected in UI
   * \par In Python:
   *   obj.activeResultPanel = value
   */
  virtual auto activeResultPanel(const db_query_ResultPanelRef &value) -> void {
    grt::ValueRef ovalue(_activeResultPanel);
    _activeResultPanel = value;
    member_changed("activeResultPanel", ovalue, value);
  }

  // resultDockingPoint is owned by db_query_QueryEditor
  /**
   * Getter for attribute resultDockingPoint
   *
   * 
   * \par In Python:
   *    value = obj.resultDockingPoint
   */
  auto resultDockingPoint() const -> mforms_ObjectReferenceRef { return _resultDockingPoint; }

  /**
   * Setter for attribute resultDockingPoint
   * 
   * 
   * \par In Python:
   *   obj.resultDockingPoint = value
   */
  virtual auto resultDockingPoint(const mforms_ObjectReferenceRef &value) -> void {
    grt::ValueRef ovalue(_resultDockingPoint);

    _resultDockingPoint = value;
    owned_member_changed("resultDockingPoint", ovalue, value);
  }

  // resultPanels is owned by db_query_QueryEditor
  /**
   * Getter for attribute resultPanels (read-only)
   *
   * list of open query result panels. Result panels contain the resultset grid and other views
   * \par In Python:
   *    value = obj.resultPanels
   */
  auto resultPanels() const -> grt::ListRef<db_query_ResultPanel> { return _resultPanels; }


private: // The next attribute is read-only.
  virtual auto resultPanels(const grt::ListRef<db_query_ResultPanel> &value) -> void {
    grt::ValueRef ovalue(_resultPanels);

    _resultPanels = value;
    owned_member_changed("resultPanels", ovalue, value);
  }
public:

protected:

  db_query_ResultPanelRef _activeResultPanel;
  mforms_ObjectReferenceRef _resultDockingPoint;// owned
  grt::ListRef<db_query_ResultPanel> _resultPanels;// owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new db_query_QueryEditor());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&db_query_QueryEditor::create);
    {
      void (db_query_QueryEditor::*setter)(const db_query_ResultPanelRef &) = &db_query_QueryEditor::activeResultPanel;
      db_query_ResultPanelRef (db_query_QueryEditor::*getter)() const = &db_query_QueryEditor::activeResultPanel;
      meta->bind_member("activeResultPanel", new grt::MetaClass::Property<db_query_QueryEditor,db_query_ResultPanelRef>(getter, setter));
    }
    {
      void (db_query_QueryEditor::*setter)(const mforms_ObjectReferenceRef &) = &db_query_QueryEditor::resultDockingPoint;
      mforms_ObjectReferenceRef (db_query_QueryEditor::*getter)() const = &db_query_QueryEditor::resultDockingPoint;
      meta->bind_member("resultDockingPoint", new grt::MetaClass::Property<db_query_QueryEditor,mforms_ObjectReferenceRef>(getter, setter));
    }
    {
      void (db_query_QueryEditor::*setter)(const grt::ListRef<db_query_ResultPanel> &) = &db_query_QueryEditor::resultPanels;
      grt::ListRef<db_query_ResultPanel> (db_query_QueryEditor::*getter)() const = &db_query_QueryEditor::resultPanels;
      meta->bind_member("resultPanels", new grt::MetaClass::Property<db_query_QueryEditor,grt::ListRef<db_query_ResultPanel>>(getter, setter));
    }
  }
};

/** a proxy to an instance of a connection to a DB server, equivalent to a SQL Editor tab.\n This object cannot be instantiated directly. */
class GRT_STRUCTS_DB_QUERY_PUBLIC db_query_Editor : public GrtObject {
  typedef GrtObject super;

public:
  class ImplData;
  friend class ImplData;
  db_query_Editor(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _customData(this, false),
      _queryEditors(this, false),
      _data(nullptr) {
  }

  virtual ~db_query_Editor();

  static auto static_class_name() -> std::string {
    return "db.query.Editor";
  }

  /**
   * Getter for attribute activeQueryEditor (read-only)
   *
   * query editor that is currently selected
   * \par In Python:
   *    value = obj.activeQueryEditor
   */
  auto activeQueryEditor() const -> db_query_QueryEditorRef;


private: // The next attribute is read-only.
  virtual auto activeQueryEditor(const db_query_QueryEditorRef &value) -> void {
    grt::ValueRef ovalue(_activeQueryEditor);
    _activeQueryEditor = value;
    member_changed("activeQueryEditor", ovalue, value);
  }
public:

  /**
   * Getter for attribute connection (read-only)
   *
   * connection data
   * \par In Python:
   *    value = obj.connection
   */
  auto connection() const -> db_mgmt_ConnectionRef;


private: // The next attribute is read-only.
public:

  /**
   * Getter for attribute customData (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.customData
   */
  auto customData() const -> grt::DictRef { return _customData; }


private: // The next attribute is read-only.
  virtual auto customData(const grt::DictRef &value) -> void {
    grt::ValueRef ovalue(_customData);
    _customData = value;
    member_changed("customData", ovalue, value);
  }
public:

  /**
   * Getter for attribute defaultSchema
   *
   * The default schema to use for queries (equivalent to USE schema)
   * \par In Python:
   *    value = obj.defaultSchema
   */
  auto defaultSchema() const -> grt::StringRef;

  /**
   * Setter for attribute defaultSchema
   * 
   * The default schema to use for queries (equivalent to USE schema)
   * \par In Python:
   *   obj.defaultSchema = value
   */
  virtual auto defaultSchema(const grt::StringRef &value) -> void;

  /**
   * Getter for attribute dockingPoint
   *
   * 
   * \par In Python:
   *    value = obj.dockingPoint
   */
  auto dockingPoint() const -> mforms_ObjectReferenceRef { return _dockingPoint; }

  /**
   * Setter for attribute dockingPoint
   * 
   * 
   * \par In Python:
   *   obj.dockingPoint = value
   */
  virtual auto dockingPoint(const mforms_ObjectReferenceRef &value) -> void {
    grt::ValueRef ovalue(_dockingPoint);
    _dockingPoint = value;
    member_changed("dockingPoint", ovalue, value);
  }

  /**
   * Getter for attribute getSSHTunnelPort (read-only)
   *
   * get port number used for tunnel
   * \par In Python:
   *    value = obj.getSSHTunnelPort
   */
  auto getSSHTunnelPort() const -> grt::IntegerRef;


private: // The next attribute is read-only.
public:

  /**
   * Getter for attribute isConnected (read-only)
   *
   * whether the editor is connected
   * \par In Python:
   *    value = obj.isConnected
   */
  auto isConnected() const -> grt::IntegerRef;


private: // The next attribute is read-only.
public:

  // queryEditors is owned by db_query_Editor
  /**
   * Getter for attribute queryEditors (read-only)
   *
   * list of open editor buffers. This list cannot be modified
   * \par In Python:
   *    value = obj.queryEditors
   */
  auto queryEditors() const -> grt::ListRef<db_query_QueryEditor> { return _queryEditors; }


private: // The next attribute is read-only.
  virtual auto queryEditors(const grt::ListRef<db_query_QueryEditor> &value) -> void {
    grt::ValueRef ovalue(_queryEditors);

    _queryEditors = value;
    owned_member_changed("queryEditors", ovalue, value);
  }
public:

  /**
   * Getter for attribute schemaTreeSelection (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.schemaTreeSelection
   */
  auto schemaTreeSelection() const -> grt::ListRef<db_query_LiveDBObject>;


private: // The next attribute is read-only.
public:

  /**
   * Getter for attribute serverVersion
   *
   * 
   * \par In Python:
   *    value = obj.serverVersion
   */
  auto serverVersion() const -> GrtVersionRef { return _serverVersion; }

  /**
   * Setter for attribute serverVersion
   * 
   * 
   * \par In Python:
   *   obj.serverVersion = value
   */
  virtual auto serverVersion(const GrtVersionRef &value) -> void {
    grt::ValueRef ovalue(_serverVersion);
    _serverVersion = value;
    member_changed("serverVersion", ovalue, value);
  }

  /**
   * Getter for attribute sidebar
   *
   * 
   * \par In Python:
   *    value = obj.sidebar
   */
  auto sidebar() const -> mforms_ObjectReferenceRef { return _sidebar; }

  /**
   * Setter for attribute sidebar
   * 
   * 
   * \par In Python:
   *   obj.sidebar = value
   */
  virtual auto sidebar(const mforms_ObjectReferenceRef &value) -> void {
    grt::ValueRef ovalue(_sidebar);
    _sidebar = value;
    member_changed("sidebar", ovalue, value);
  }

  /**
   * Getter for attribute sshConnection (read-only)
   *
   * ssh connection
   * \par In Python:
   *    value = obj.sshConnection
   */
  auto sshConnection() const -> db_mgmt_SSHConnectionRef;


private: // The next attribute is read-only.
public:

  /**
   * Method. adds a new query buffer/text editor tab in the UI and return it
   * \return the newly created query buffer proxy object
   */
  virtual auto addQueryEditor() -> db_query_QueryEditorRef;
  /**
   * Method. write a line of text into the SQL Editor output area
   * \param text 
   * \param bringToFront 
   * \return 
   */
  virtual auto addToOutput(const std::string &text, ssize_t bringToFront) -> grt::IntegerRef;
  /**
   * Method. Opens the object editor for the named DB object
   * \param type 
   * \param schemaName 
   * \param objectName 
   * \return 
   */
  virtual auto alterLiveObject(const std::string &type, const std::string &schemaName, const std::string &objectName) -> void;
  /**
   * Method. executes a SELECT statement on the table and returns an editable resultset that can be used to modify its contents
   * \param schema name of the table schema
   * \param table name of the table to edit
   * \param where not yet supported
   * \param showGrid whether the resultset should be displayed as a grid in the UI
   * \return an editable resultset that can be used to modify the table contents
   */
  virtual auto createTableEditResultset(const std::string &schema, const std::string &table, const std::string &where, ssize_t showGrid) -> db_query_EditableResultsetRef;
  /**
   * Method. Opens the object editor for the given DB object
   * \param object 
   * \param originalCatalog 
   * \return 
   */
  virtual auto editLiveObject(const db_DatabaseObjectRef &object, const db_CatalogRef &originalCatalog) -> void;
  /**
   * Method. Executes a statement on the main connection, optionally logging the query in the action log
   * \param statement 
   * \param log 
   * \param background 
   * \return 
   */
  virtual auto executeCommand(const std::string &statement, ssize_t log, ssize_t background) -> void;
  /**
   * Method. Executes a statement on the aux connection, optionally logging the query in the action log
   * \param statement 
   * \param log 
   * \return 
   */
  virtual auto executeManagementCommand(const std::string &statement, ssize_t log) -> void;
  /**
   * Method. Executes a query on the aux connection and return a plain resultset, optionally logging the query in the action log
   * \param query 
   * \param log 
   * \return 
   */
  virtual auto executeManagementQuery(const std::string &query, ssize_t log) -> db_query_ResultsetRef;
  /**
   * Method. Executes a query on the main connection and return a plain resultset, optionally logging the query in the action log
   * \param query 
   * \param log 
   * \return 
   */
  virtual auto executeQuery(const std::string &query, ssize_t log) -> db_query_ResultsetRef;
  /**
   * Method. execute the script passed as argument
   * \param sql 
   * \return the list of resultsets sent back by the server
   */
  virtual auto executeScript(const std::string &sql) -> grt::ListRef<db_query_Resultset>;
  /**
   * Method. execute the script passed as argument and displays the generated resultsets as grids in the UI
   * \param sql 
   * \return 
   */
  virtual auto executeScriptAndOutputToGrid(const std::string &sql) -> grt::IntegerRef;

  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  db_query_QueryEditorRef _activeQueryEditor;
  grt::DictRef _customData;
  mforms_ObjectReferenceRef _dockingPoint;
  grt::ListRef<db_query_QueryEditor> _queryEditors;// owned
  GrtVersionRef _serverVersion;
  mforms_ObjectReferenceRef _sidebar;

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new db_query_Editor());
  }

  static grt::ValueRef call_addQueryEditor(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Editor*>(self)->addQueryEditor(); }

  static grt::ValueRef call_addToOutput(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Editor*>(self)->addToOutput(grt::StringRef::cast_from(args[0]), grt::IntegerRef::cast_from(args[1])); }

  static grt::ValueRef call_alterLiveObject(grt::internal::Object *self, const grt::BaseListRef &args){ dynamic_cast<db_query_Editor*>(self)->alterLiveObject(grt::StringRef::cast_from(args[0]), grt::StringRef::cast_from(args[1]), grt::StringRef::cast_from(args[2])); return grt::ValueRef(); }

  static grt::ValueRef call_createTableEditResultset(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Editor*>(self)->createTableEditResultset(grt::StringRef::cast_from(args[0]), grt::StringRef::cast_from(args[1]), grt::StringRef::cast_from(args[2]), grt::IntegerRef::cast_from(args[3])); }

  static grt::ValueRef call_editLiveObject(grt::internal::Object *self, const grt::BaseListRef &args){ dynamic_cast<db_query_Editor*>(self)->editLiveObject(db_DatabaseObjectRef::cast_from(args[0]), db_CatalogRef::cast_from(args[1])); return grt::ValueRef(); }

  static grt::ValueRef call_executeCommand(grt::internal::Object *self, const grt::BaseListRef &args){ dynamic_cast<db_query_Editor*>(self)->executeCommand(grt::StringRef::cast_from(args[0]), grt::IntegerRef::cast_from(args[1]), grt::IntegerRef::cast_from(args[2])); return grt::ValueRef(); }

  static grt::ValueRef call_executeManagementCommand(grt::internal::Object *self, const grt::BaseListRef &args){ dynamic_cast<db_query_Editor*>(self)->executeManagementCommand(grt::StringRef::cast_from(args[0]), grt::IntegerRef::cast_from(args[1])); return grt::ValueRef(); }

  static grt::ValueRef call_executeManagementQuery(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Editor*>(self)->executeManagementQuery(grt::StringRef::cast_from(args[0]), grt::IntegerRef::cast_from(args[1])); }

  static grt::ValueRef call_executeQuery(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Editor*>(self)->executeQuery(grt::StringRef::cast_from(args[0]), grt::IntegerRef::cast_from(args[1])); }

  static grt::ValueRef call_executeScript(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Editor*>(self)->executeScript(grt::StringRef::cast_from(args[0])); }

  static grt::ValueRef call_executeScriptAndOutputToGrid(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<db_query_Editor*>(self)->executeScriptAndOutputToGrid(grt::StringRef::cast_from(args[0])); }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&db_query_Editor::create);
    {
      void (db_query_Editor::*setter)(const db_query_QueryEditorRef &) = &db_query_Editor::activeQueryEditor;
      db_query_QueryEditorRef (db_query_Editor::*getter)() const = &db_query_Editor::activeQueryEditor;
      meta->bind_member("activeQueryEditor", new grt::MetaClass::Property<db_query_Editor,db_query_QueryEditorRef>(getter, setter));
    }
    meta->bind_member("connection", new grt::MetaClass::Property<db_query_Editor,db_mgmt_ConnectionRef>(&db_query_Editor::connection));
    {
      void (db_query_Editor::*setter)(const grt::DictRef &) = &db_query_Editor::customData;
      grt::DictRef (db_query_Editor::*getter)() const = &db_query_Editor::customData;
      meta->bind_member("customData", new grt::MetaClass::Property<db_query_Editor,grt::DictRef>(getter, setter));
    }
    {
      void (db_query_Editor::*setter)(const grt::StringRef &) = &db_query_Editor::defaultSchema;
      grt::StringRef (db_query_Editor::*getter)() const = &db_query_Editor::defaultSchema;
      meta->bind_member("defaultSchema", new grt::MetaClass::Property<db_query_Editor,grt::StringRef>(getter, setter));
    }
    {
      void (db_query_Editor::*setter)(const mforms_ObjectReferenceRef &) = &db_query_Editor::dockingPoint;
      mforms_ObjectReferenceRef (db_query_Editor::*getter)() const = &db_query_Editor::dockingPoint;
      meta->bind_member("dockingPoint", new grt::MetaClass::Property<db_query_Editor,mforms_ObjectReferenceRef>(getter, setter));
    }
    meta->bind_member("getSSHTunnelPort", new grt::MetaClass::Property<db_query_Editor,grt::IntegerRef>(&db_query_Editor::getSSHTunnelPort));
    meta->bind_member("isConnected", new grt::MetaClass::Property<db_query_Editor,grt::IntegerRef>(&db_query_Editor::isConnected));
    {
      void (db_query_Editor::*setter)(const grt::ListRef<db_query_QueryEditor> &) = &db_query_Editor::queryEditors;
      grt::ListRef<db_query_QueryEditor> (db_query_Editor::*getter)() const = &db_query_Editor::queryEditors;
      meta->bind_member("queryEditors", new grt::MetaClass::Property<db_query_Editor,grt::ListRef<db_query_QueryEditor>>(getter, setter));
    }
    meta->bind_member("schemaTreeSelection", new grt::MetaClass::Property<db_query_Editor,grt::ListRef<db_query_LiveDBObject>>(&db_query_Editor::schemaTreeSelection));
    {
      void (db_query_Editor::*setter)(const GrtVersionRef &) = &db_query_Editor::serverVersion;
      GrtVersionRef (db_query_Editor::*getter)() const = &db_query_Editor::serverVersion;
      meta->bind_member("serverVersion", new grt::MetaClass::Property<db_query_Editor,GrtVersionRef>(getter, setter));
    }
    {
      void (db_query_Editor::*setter)(const mforms_ObjectReferenceRef &) = &db_query_Editor::sidebar;
      mforms_ObjectReferenceRef (db_query_Editor::*getter)() const = &db_query_Editor::sidebar;
      meta->bind_member("sidebar", new grt::MetaClass::Property<db_query_Editor,mforms_ObjectReferenceRef>(getter, setter));
    }
    meta->bind_member("sshConnection", new grt::MetaClass::Property<db_query_Editor,db_mgmt_SSHConnectionRef>(&db_query_Editor::sshConnection));
    meta->bind_method("addQueryEditor", &db_query_Editor::call_addQueryEditor);
    meta->bind_method("addToOutput", &db_query_Editor::call_addToOutput);
    meta->bind_method("alterLiveObject", &db_query_Editor::call_alterLiveObject);
    meta->bind_method("createTableEditResultset", &db_query_Editor::call_createTableEditResultset);
    meta->bind_method("editLiveObject", &db_query_Editor::call_editLiveObject);
    meta->bind_method("executeCommand", &db_query_Editor::call_executeCommand);
    meta->bind_method("executeManagementCommand", &db_query_Editor::call_executeManagementCommand);
    meta->bind_method("executeManagementQuery", &db_query_Editor::call_executeManagementQuery);
    meta->bind_method("executeQuery", &db_query_Editor::call_executeQuery);
    meta->bind_method("executeScript", &db_query_Editor::call_executeScript);
    meta->bind_method("executeScriptAndOutputToGrid", &db_query_Editor::call_executeScriptAndOutputToGrid);
  }
};



inline auto register_structs_db_query_xml() -> void {
  grt::internal::ClassRegistry::register_class<db_query_LiveDBObject>();
  grt::internal::ClassRegistry::register_class<db_query_ResultsetColumn>();
  grt::internal::ClassRegistry::register_class<db_query_Resultset>();
  grt::internal::ClassRegistry::register_class<db_query_EditableResultset>();
  grt::internal::ClassRegistry::register_class<db_query_ResultPanel>();
  grt::internal::ClassRegistry::register_class<db_query_QueryBuffer>();
  grt::internal::ClassRegistry::register_class<db_query_QueryEditor>();
  grt::internal::ClassRegistry::register_class<db_query_Editor>();
}

#ifdef AUTO_REGISTER_GRT_CLASSES
static struct _autoreg__structs_db_query_xml {
  _autoreg__structs_db_query_xml() {
    register_structs_db_query_xml();
  }
} __autoreg__structs_db_query_xml;
#endif

#ifndef _MSC_VER
  #pragma GCC diagnostic pop
#endif

