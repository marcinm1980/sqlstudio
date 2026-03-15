/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _DB_SQL_EDITOR_HISTORY_BE_H_
#define _DB_SQL_EDITOR_HISTORY_BE_H_

#include "studio/wb_backend_public_interface.h"
#include "sqlide/var_grid_model_be.h"
#include <time.h>
#include "mforms/menu.h"

class MYSQLWBBACKEND_PUBLIC_FUNC DbSqlEditorHistory {
public:
  using Ref = std::shared_ptr<DbSqlEditorHistory>;
  static auto create() -> Ref {
    return Ref(new DbSqlEditorHistory());
  }
  virtual ~DbSqlEditorHistory();

protected:
  DbSqlEditorHistory();

public:
  auto reset() -> void;
  auto add_entry(const std::list<std::string> &statements) -> void;
  auto current_entry() -> int {
    return _current_entry_index;
  }
  auto current_entry(int index) -> void;
  auto restore_sql_from_history(int entry_index, std::list<int> &detail_indexes) -> std::string;

protected:
  int _current_entry_index;

public:
  auto load() -> void;

public:
  class EntriesModel;
  class DetailsModel;

public:
  class DetailsModel : public VarGridModel {
  public:
    friend class DbSqlEditorHistory;
    using Ref = std::shared_ptr<DetailsModel>;
    static auto create() -> Ref {
      return Ref(new DetailsModel());
    }

  protected:
    DetailsModel();

  public:
    auto add_entries(const std::list<std::string> &statements) -> void;
    virtual auto refresh() -> void {
      refresh_ui();
    }
    auto get_context_menu() -> mforms::Menu * {
      return &_context_menu;
    }

    virtual auto reset() -> void;

    auto save() -> void;
    auto load(const std::string &storage_file_path) -> void;

  protected:
    int _last_loaded_row; // required to skip duplication of existing entries when dumping contents

  public:
    auto datestamp() const -> std::tm {
      return _datestamp;
    }
    auto datestamp(const std::tm &val) -> void {
      _datestamp = val;
    }

  protected:
    auto storage_file_path() const -> std::string;
    std::tm _datestamp; // raw datestamp for locale independent storage file name

  private:
    grt::StringRef _last_timestamp;
    grt::StringRef _last_statement;
    mforms::Menu _context_menu;
  };

public:
  class EntriesModel : public VarGridModel {
  private:
    bool _ui_usage;

  public:
    friend class DbSqlEditorHistory;

    using Ref = std::shared_ptr<EntriesModel>;
    static auto create(DbSqlEditorHistory *owner) -> Ref {
      return Ref(new EntriesModel(owner));
    }

  protected:
    EntriesModel(DbSqlEditorHistory *owner);

    DbSqlEditorHistory *_owner;

    auto add_statements(const std::list<std::string> &statements) -> void;

  public:
    auto insert_entry(const std::tm &t) -> bool;
    auto delete_all_entries() -> void;
    auto delete_entries(const std::vector<std::size_t> &rows) -> void;
    auto set_ui_usage(bool value) -> void {
      _ui_usage = value;
    }
    auto get_ui_usage() -> bool {
      return _ui_usage;
    }

    auto entry_path(std::size_t index) -> std::string;
    auto entry_date(std::size_t index) -> std::tm;

    virtual auto reset() -> void;
    auto load() -> void;

    virtual auto activate_popup_item_for_nodes(const std::string &action, const std::vector<bec::NodeId> &orig_nodes) -> bool;
    virtual auto get_popup_items_for_nodes(const std::vector<bec::NodeId> &nodes) -> bec::MenuItemList;
  };

public:
  auto entries_model() -> EntriesModel::Ref {
    return _entries_model;
  }
  auto details_model() -> DetailsModel::Ref {
    return _details_model;
  }
  auto write_only_details_model() -> DetailsModel::Ref {
    return _write_only_details_model;
  }

protected:
  EntriesModel::Ref _entries_model;
  DetailsModel::Ref _details_model;
  DetailsModel::Ref _write_only_details_model;

  auto update_timestamp(std::tm timestamp) -> void;
};

#endif /* _DB_SQL_EDITOR_HISTORY_BE_H_ */
