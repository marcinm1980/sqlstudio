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

#pragma once

#include "grt.h"
#include "grts/structs.db.h"
#include "grts/structs.db.mgmt.h"
#include "grts/structs.db.mysql.h"
#include "grts/structs.studio.physical.h"
#include <set>
#include "base/string_utilities.h"

#include "wbpublic_public_interface.h"

#include "charset_utils.h"
#include "catalog_templates.h"

// misc helper functions for db objects

// replaces old DbUtils.lua
// the functions here are also exposed as a GRT module in
// modules/db/dbutils.cpp

auto WBPUBLICBACKEND_PUBLIC_FUNC get_rdbms_for_db_object(const ::grt::ValueRef &object) -> db_mgmt_RdbmsRef;

namespace bec {

  // these constants for comparing db_SimpleDatatypeRef members. Cannot use -1 because for characterOctetLength it means
  // pow(10, -1)
  const int EMPTY_TYPE_MAXIMUM_LENGTH = 0;
  const int EMPTY_TYPE_OCTET_LENGTH = EMPTY_TYPE_MAXIMUM_LENGTH;
  const int EMPTY_TYPE_PRECISION = EMPTY_TYPE_MAXIMUM_LENGTH;
  const int EMPTY_TYPE_SCALE = EMPTY_TYPE_MAXIMUM_LENGTH;
  // these constants for comparing db_SimpleDatatypeRef members. Cannot use 0 because char(0) and char are different
  // types
  const int EMPTY_COLUMN_LENGTH = -1;
  const int EMPTY_COLUMN_PRECISION = EMPTY_COLUMN_LENGTH;
  const int EMPTY_COLUMN_SCALE = EMPTY_COLUMN_LENGTH;

  auto WBPUBLICBACKEND_PUBLIC_FUNC getModelOption(studio_physical_ModelRef model, const std::string &key, bool forceModel = false) -> grt::ValueRef;

  struct WBPUBLICBACKEND_PUBLIC_FUNC CatalogHelper {
    static auto apply_defaults(db_mysql_CatalogRef catalog, std::string default_engine) -> void;

    static auto is_type_valid_for_version(const db_SimpleDatatypeRef &type, const GrtVersionRef &target_version) -> bool;

    static auto dbobject_list_to_dragdata(const std::list<db_DatabaseObjectRef> &object) -> std::string;
    static auto dragdata_to_dbobject_list(const db_CatalogRef &catalog,
                                                                     const std::string &object) -> std::list<db_DatabaseObjectRef>;

    static auto dbobject_to_dragdata(const db_DatabaseObjectRef &object) -> std::string;
    static auto dragdata_to_dbobject(const db_CatalogRef &catalog, const std::string &object) -> db_DatabaseObjectRef;

  private:
    static auto apply_defaults(db_mysql_ColumnRef column) -> void;
  };

  struct WBPUBLICBACKEND_PUBLIC_FUNC SchemaHelper {
    static auto get_unique_foreign_key_name(const db_SchemaRef &schema, const std::string &prefix,
                                                   int maxlength) -> std::string;

    static auto get_foreign_key_names(const db_SchemaRef &schema) -> std::set<std::string>;
    static auto get_unique_foreign_key_name(std::set<std::string> &used_names, const std::string &prefix,
                                                   int maxlength) -> std::string;
  };

  struct WBPUBLICBACKEND_PUBLIC_FUNC TableHelper {
    static auto create_foreign_key_to_table(const db_TableRef &table, const db_TableRef &ref_table,
                                                        bool mandatory, bool ref_mandatory, bool many, bool identifying,
                                                        const db_mgmt_RdbmsRef &rdbms,
                                                        const grt::DictRef &global_options,
                                                        const grt::DictRef &options) -> db_ForeignKeyRef;

    static auto create_foreign_key_to_table(
      const db_TableRef &table, const std::vector<db_ColumnRef> &columns, const db_TableRef &ref_table,
      const std::vector<db_ColumnRef> &refcolumns, bool mandatory, bool many, const db_mgmt_RdbmsRef &rdbms,
      const grt::DictRef &global_options, const grt::DictRef &options) -> db_ForeignKeyRef;

    static auto create_index_for_fk_if_needed(db_ForeignKeyRef fk) -> bool;
    static auto create_index_for_fk(const db_ForeignKeyRef &fk, const size_t max_len = 64) -> db_IndexRef;
    static auto find_index_usable_by_fk(const db_ForeignKeyRef &fk,
                                               const db_IndexRef &other_than = db_IndexRef(),
                                               bool allow_any_order = false) -> db_IndexRef;
    static auto reorder_foreign_key_for_index(const db_ForeignKeyRef &fk, const db_IndexRef &index) -> void;
    static auto generate_foreign_key_name() -> std::string;
    static auto create_empty_foreign_key(const db_TableRef &table, const std::string &name) -> db_ForeignKeyRef;
    static auto update_foreign_key_index(const db_ForeignKeyRef &fk) -> void;

    static auto rename_foreign_key(const db_TableRef &table, db_ForeignKeyRef &fk, const std::string &new_name) -> bool;

    static auto update_foreign_keys_from_column_notnull(const db_TableRef &table, const db_ColumnRef &column) -> void;

    static auto create_missing_indexes_for_foreign_keys(const db_TableRef &table) -> bool;

    static auto is_identifying_foreign_key(const db_TableRef &table, const db_ForeignKeyRef &fk) -> bool;

    static auto create_associative_table(const db_SchemaRef &schema, const db_TableRef &table1,
                                                const db_TableRef &table2, bool mandatory1, bool mandatory2,
                                                const db_mgmt_RdbmsRef &rdbms, const grt::DictRef &global_options,
                                                const grt::DictRef &options) -> db_TableRef;

    static auto clone_table(const db_TableRef &table) -> db_TableRef;
    static auto get_engine_by_name(const std::string &name) -> db_mysql_StorageEngineRef;
    static auto get_sync_comment(const std::string &comment, const size_t max_len = 60) -> std::string;

    // add schema name and backticks to table names if needed
    static auto normalize_table_name_list(const std::string &schema, const std::string &table_name_list) -> std::string;

    static auto generate_comment_text(const std::string &comment_text, size_t comment_lenght) -> std::string;
  };

  enum ColumnTypeCompareResult {
    COLUMNS_TYPES_EQUAL = 0,
    COLUMNS_TYPES_DIFFER = 1,
    COLUMNS_CHARSETS_DIFFER = 2,
    COLUMNS_COLLATIONS_DIFFER = 3,
    COLUMNS_FLAGS_DIFFER = 4,
    COLUMNS_TYPES_EQUAL_LENGTH_DIFFER = 5
  };

  struct WBPUBLICBACKEND_PUBLIC_FUNC ColumnHelper {
    // static std::string quote_default_if_needed(const db_ColumnRef &column, const std::string &value);

    static auto copy_column(const db_ColumnRef &from, db_ColumnRef &to) -> void;
    static auto compare_column_types(const db_ColumnRef &from, const db_ColumnRef &to) -> ColumnTypeCompareResult;

    static auto set_default_value(db_ColumnRef column, const std::string &value) -> void;
  };

  struct Column_action {
    db_mysql_CatalogRef catalog;
    db_mgmt_RdbmsRef rdbms;

    Column_action(db_mysql_CatalogRef c, db_mgmt_RdbmsRef r) : catalog(c), rdbms(r) {
    }

    void operator()(db_mysql_ColumnRef column) {
      db_UserDatatypeRef udt = column->userType();
      if (udt.is_valid()) {
        column->setParseType(column->formattedType(), catalog->simpleDatatypes());
        column->flags().remove_all();
        std::vector<std::string> flags(base::split(*udt->flags(), ","));
        for (std::vector<std::string>::const_iterator i = flags.begin(); i != flags.end(); ++i) {
          if (column->flags().get_index(*i) == grt::BaseListRef::npos)
            column->flags().insert(*i);
        }
      }
    }
  };

  struct Table_action {
    db_mysql_CatalogRef catalog;
    db_mgmt_RdbmsRef rdbms;

    Table_action(db_mysql_CatalogRef c, db_mgmt_RdbmsRef r) : catalog(c), rdbms(r) {
    }

    void operator()(const db_mysql_TableRef &table) {
      Column_action ca(catalog, rdbms);
      ct::for_each<ct::Columns>(table, ca);
    }
  };

  struct Schema_action {
    db_mysql_CatalogRef catalog;
    db_mgmt_RdbmsRef rdbms;

    Schema_action(db_mysql_CatalogRef c, db_mgmt_RdbmsRef r) : catalog(c), rdbms(r) {
    }

    void operator()(const db_mysql_SchemaRef &schema) {
      Table_action table_action(catalog, rdbms);
      ct::for_each<ct::Tables>(schema, table_action);
    }
  };

  inline auto apply_user_datatypes(db_mysql_CatalogRef cat, db_mgmt_RdbmsRef rdbms) -> void {
    Schema_action sa(cat, rdbms);
    ct::for_each<ct::Schemata>(cat, sa);
  }

  inline auto is_int_datatype(const std::string &type) -> bool {
    return type == "BIGINT" || type == "MEDIUMINT" || type == "SMALLINT" || type == "TINYINT" || type == "INT";
  }

  auto WBPUBLICBACKEND_PUBLIC_FUNC get_default_collation_for_charset(const db_SchemaRef &schema,
                                                                            const std::string &character_set) -> std::string;
  auto WBPUBLICBACKEND_PUBLIC_FUNC get_default_collation_for_charset(const db_TableRef &table,
                                                                            const std::string &character_set) -> std::string;
};
