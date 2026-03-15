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

#ifndef _DB_PLUGIN_BE_H_
#define _DB_PLUGIN_BE_H_

#include "db_mysql_public_interface.h"
#include "wb_plugin_be.h"
#include "grtui/db_conn_be.h"
#include "grt/grt_manager.h"
#include "grt/grt_string_list_model.h"
#include "grts/structs.studio.h"
#include "grtdb/diff_dbobjectmatch.h"

class Db_plugin;
#ifdef _MSC_VER
#pragma make_public(Db_plugin)
#endif

class WBPLUGINDBMYSQLBE_PUBLIC_FUNC Db_plugin : virtual public Wb_plugin {
public:
  Db_plugin() : _db_conn(0) {
  }
  virtual ~Db_plugin() {
    delete _db_conn;
  }
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif
  auto grtm(bool reveng) -> void;
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

  auto apply_script_to_db() -> grt::StringRef;

private:
  auto selected_rdbms() -> db_mgmt_RdbmsRef;
  auto task_desc() -> std::string;

protected:
  auto set_task_proc() -> void;

public:
  enum Db_object_type { dbotSchema, dbotTable, dbotView, dbotRoutine, dbotTrigger, dbotUser };

protected:
  struct Db_obj_handle {
    std::string schema;
    std::string name;
    std::string ddl;
  };
  typedef std::vector<Db_obj_handle> Db_objects;

  struct Db_objects_setup {
    Db_objects all;
    bec::GrtStringListModel selection;
    bec::GrtStringListModel exclusion;
    bool activated; // consider this type of db objects during operations
    Db_objects_setup() {
      activated = true;
    }
    auto reset() -> void {
      all.clear();
      selection.reset();
      exclusion.reset();
      selection.items_val_masks(&exclusion);
    }
    auto icon_id(bec::IconId icon_id) -> void {
      selection.icon_id(icon_id);
      exclusion.icon_id(icon_id);
    }
  };

  studio_DocumentRef _doc;
  DbConnection *_db_conn;
  db_CatalogRef _catalog;

  std::vector<std::string> _schemata;
  std::map<std::string, std::string> _schemata_ddl;

  std::map<std::string, std::string> _view_db_code;

  std::vector<std::string> _schemata_selection;

protected:
  Db_objects_setup _tables;
  Db_objects_setup _views;
  Db_objects_setup _routines;
  Db_objects_setup _triggers;
  Db_objects_setup _users;

  auto db_objects_setup_by_type(Db_object_type db_object_type) -> Db_objects_setup *;
  auto db_objects_type_to_string(Db_object_type db_object_type) -> const char *;

  auto dump_ddl(Db_object_type db_object_type, std::string &sql_script) -> void;

  auto process_sql_script_error(long long err_no, const std::string &err_msg, const std::string &statement) -> int;
  auto process_sql_script_progress(float progress_state) -> int;
  auto process_sql_script_statistics(long success_count, long err_count) -> int;

  std::string _sql_script;
  grt::DictRef _db_options;

public:
  auto schema_icon_id(bec::IconSize icon_size) -> bec::IconId {
    return bec::IconManager::get_instance()->get_icon_id(grt::GRT::get()->get_metaclass("db.Schema"), icon_size);
  }
  auto table_icon_id(bec::IconSize icon_size) -> bec::IconId {
    return bec::IconManager::get_instance()->get_icon_id(grt::GRT::get()->get_metaclass("db.Table"), icon_size);
  }
  auto view_icon_id(bec::IconSize icon_size) -> bec::IconId {
    return bec::IconManager::get_instance()->get_icon_id(grt::GRT::get()->get_metaclass("db.View"), icon_size);
  }
  auto routine_icon_id(bec::IconSize icon_size) -> bec::IconId {
    return bec::IconManager::get_instance()->get_icon_id(grt::GRT::get()->get_metaclass("db.Routine"), icon_size);
  }
  auto trigger_icon_id(bec::IconSize icon_size) -> bec::IconId {
    return bec::IconManager::get_instance()->get_icon_id(grt::GRT::get()->get_metaclass("db.Trigger"), icon_size);
  }
  auto user_icon_id(bec::IconSize icon_size) -> bec::IconId {
    return bec::IconManager::get_instance()->get_icon_id(grt::GRT::get()->get_metaclass("db.User"), icon_size);
  }

  auto db_conn() -> DbConnection * {
    return _db_conn;
  }

  auto db_catalog() -> db_CatalogRef;
  auto model_catalog() -> db_CatalogRef;

  auto db_objects_struct_name_by_type(Db_object_type db_object_type) -> std::string;

  /**
   * Check if on the server we're connecting we can expirence some case sensitivity problems,
   * return -1 if it's impossible to check
   * return 0 if everything is ok
   * return 1 if there can be some problems
   */
  auto check_case_sensitivity_problems() -> int;

  auto load_schemata(std::vector<std::string> &schemata) -> void;
  auto load_db_options() -> grt::DictRef {
    return _db_options.is_valid() ? _db_options : grt::DictRef(grt::Initialized);
  };
  //  void default_schemata_selection(std::vector<std::string> &selection);
  auto schemata_selection(const std::vector<std::string> &selection, bool sel_none_means_sel_all) -> void;
  auto load_db_objects(Db_object_type db_object_type) -> void;
  auto db_objects_activated(Db_object_type db_object_type, bool activated) -> void {
    db_objects_setup_by_type(db_object_type)->activated = activated;
  }
  auto db_objects_activated(Db_object_type db_object_type) -> bool {
    return db_objects_setup_by_type(db_object_type)->activated;
  }
  auto db_objects_selection_model(Db_object_type db_object_type) -> bec::GrtStringListModel * {
    return &db_objects_setup_by_type(db_object_type)->selection;
  }
  auto db_objects_exclusion_model(Db_object_type db_object_type) -> bec::GrtStringListModel * {
    return &db_objects_setup_by_type(db_object_type)->exclusion;
  }
  auto db_objects_enabled_flag(Db_object_type db_object_type) -> bool * {
    return &db_objects_setup_by_type(db_object_type)->activated;
  }
  auto validate_db_objects_selection(std::list<std::string> *messages) -> bool;

  auto dump_ddl(std::string &sql_script) -> void;
  auto read_back_view_ddl() -> void;

  auto sql_script() const -> std::string {
    return _sql_script;
  }
  auto sql_script(const std::string &sql_script) -> void {
    _sql_script = sql_script;
  }

  auto get_schemata() -> std::vector<std::string> {
    return _schemata;
  }
};

#endif /* _DB_PLUGIN_BE_H_ */
