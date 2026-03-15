/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "db_mysql_public_interface.h"
#include "grts/structs.db.mysql.h"
#include "grt/grt_string_list_model.h"
#include "diff_tree.h"
#include "db_mysql_validation_page.h"
#include "grtdb/diff_dbobjectmatch.h"

class SynchronizeDifferencesPageBEInterface {
protected:
  std::shared_ptr<DiffTreeBE> _diff_tree;
  grt::StringRef _sync_profile_name;

public:
  SynchronizeDifferencesPageBEInterface(){};
  virtual ~SynchronizeDifferencesPageBEInterface(){};
  auto get_sync_profile_name() -> grt::StringRef {
    return _sync_profile_name;
  };
  auto set_sync_profile_name(grt::StringRef sync_profile_name) -> void {
    _sync_profile_name = sync_profile_name;
  };
  auto get_model_object(const bec::NodeId &node) const -> grt::ValueRef {
    return _diff_tree->get_node_with_id(node)->get_model_part().get_object();
  };
  auto get_db_object(const bec::NodeId &node) const -> grt::ValueRef {
    return _diff_tree->get_node_with_id(node)->get_db_part().get_object();
  };
  auto set_next_apply_direction(bec::NodeId nodeid) -> void {
    _diff_tree->set_next_apply_direction(nodeid);
  }
  auto set_apply_direction(bec::NodeId nodeid, DiffNode::ApplicationDirection dir, bool recursive) -> void {
    _diff_tree->set_apply_direction(nodeid, dir, recursive);
  }
  auto get_apply_direction(bec::NodeId nodeid) -> DiffNode::ApplicationDirection {
    return _diff_tree->get_apply_direction(nodeid);
  }
  virtual auto get_model_catalog() -> db_mysql_CatalogRef = 0;
  virtual auto get_compared_catalogs(db_CatalogRef &left, db_CatalogRef &right) -> void = 0;
  virtual auto get_col_name(const size_t col_id) -> std::string = 0;
  virtual auto get_sql_for_object(GrtNamedObjectRef obj) -> std::string = 0;
  virtual auto init_diff_tree(const std::vector<std::string> &schemata,
                                                     const grt::ValueRef &ext_cat, const grt::ValueRef &cat2,
                                                     grt::StringListRef SchemaSkipList, grt::DictRef options) -> std::shared_ptr<DiffTreeBE> = 0;
};

struct WBPLUGINDBMYSQLBE_PUBLIC_FUNC DbMySQLScriptSyncException : public std::logic_error {
  DbMySQLScriptSyncException(const std::string &message) : std::logic_error(message) {
  }
};

class WBPLUGINDBMYSQLBE_PUBLIC_FUNC DbMySQLScriptSync : public DbMySQLValidationPage,
                                                        public SynchronizeDifferencesPageBEInterface {
  // db_mysql_CatalogRef _catalog;
  db_mysql_CatalogRef _org_cat;
  db_mysql_CatalogRef _mod_cat_copy;
  grt::StringListRef _alter_list;
  grt::ListRef<GrtNamedObject> _alter_object_list;
  grt::DictRef _options;
  grt::DictRef _db_options;

  // options
  std::string _input_filename1, _input_filename2;
  std::string _output_filename;
  std::vector<std::string> schemata_list; // all schemata present on server (unfiltered)

  std::shared_ptr<grt::DiffChange> _alter_change;

  auto sync_finished(grt::ValueRef res) -> void;
  grt::ValueRef sync_task(grt::StringRef);
  auto get_cat_from_file_or_tree(std::string filename, std::string &error_msg) -> db_mysql_CatalogRef;

protected:
  virtual auto get_model_catalog() -> db_mysql_CatalogRef;
  virtual auto get_compared_catalogs(db_CatalogRef &left, db_CatalogRef &right) -> void;

public:
  DbMySQLScriptSync();
  virtual ~DbMySQLScriptSync();

  auto start_sync() -> void;

  auto set_option(const std::string &name, const std::string &value) -> void;

  auto init_diff_tree(const std::vector<std::string> &schemata, const grt::ValueRef &left,
                                             const grt::ValueRef &right, grt::StringListRef SchemaSkipList = grt::StringListRef(),
                                             grt::DictRef options = grt::DictRef()) -> std::shared_ptr<DiffTreeBE>;

  auto get_sql_for_object(GrtNamedObjectRef obj) -> std::string;

  auto set_options(grt::DictRef options) -> void {
    _options = options;
  }
  auto get_options() const -> grt::DictRef {
    return _options.is_valid() ? _options : grt::DictRef(true);
  }

  auto set_db_options(grt::DictRef db_options) -> void {
    _db_options = db_options;
  };
  auto get_db_options() const -> grt::DictRef {
    return _db_options.is_valid() ? _db_options : grt::DictRef(true);
  }

  auto generate_alter(db_mysql_CatalogRef org_cat, db_mysql_CatalogRef org_cat_copy,
                                db_mysql_CatalogRef mod_cat_copy) -> grt::StringRef;

  auto generate_diff_tree_script() -> std::string;
  auto generate_diff_tree_report() -> std::string;

  auto apply_changes_to_model() -> void;

  auto save_sync_profile() -> void;
  auto restore_sync_profile(db_CatalogRef catalog) -> void;
  auto get_col_name(const size_t col_id) -> std::string;

  auto restore_overriden_names() -> void;
};
