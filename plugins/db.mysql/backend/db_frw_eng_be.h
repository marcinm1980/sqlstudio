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

#ifndef _DB_FRW_ENG_BE_H_
#define _DB_FRW_ENG_BE_H_

#include "db_mysql_public_interface.h"
#include "db_plugin_be.h"

// if this plugin is for generic db this shouldn't be here! -alfredo
#include "grts/structs.db.mysql.h"
#include "db_mysql_sql_export.h"
#include "db_mysql_validation_page.h"

class WBPLUGINDBMYSQLBE_PUBLIC_FUNC Db_frw_eng : public Db_plugin, public DbMySQLValidationPage {
public:
  Db_frw_eng();

  auto set_option(const std::string &name, bool value) -> void {
    _export.set_option(name, value);
  }
  auto set_option(const std::string &name, const std::string &value) -> void {
    _export.set_option(name, value);
  }
  auto set_up_dboptions() -> void {
    _export.set_db_options(_db_options);
  }

  auto start_export() -> void {
    _export.start_export(false);
  }
  auto export_sql_script() -> std::string {
    return _export.export_sql_script();
  }
  auto start_apply_script_to_db() -> void;

  auto export_task_finish_cb(Task_finish_cb cb) -> void {
    _export.task_finish_cb(cb);
  }

  auto setup_grt_string_list_models_from_catalog(
    bec::GrtStringListModel **users_model, bec::GrtStringListModel **users_exc_model,
    bec::GrtStringListModel **tables_model, bec::GrtStringListModel **tables_exc_model,
    bec::GrtStringListModel **views_model, bec::GrtStringListModel **views_exc_model,
    bec::GrtStringListModel **routines_model, bec::GrtStringListModel **routines_exc_model,
    bec::GrtStringListModel **triggers_model, bec::GrtStringListModel **triggers_exc_model) -> void;

private:
  DbMySQLSQLExport _export;
};

#endif // _DB_FRW_ENG_BE_H_
