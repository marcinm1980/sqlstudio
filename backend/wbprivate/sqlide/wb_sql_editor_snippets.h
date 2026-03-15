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

#ifndef _DB_SQL_EDITOR_SNIPPETSBE_H_
#define _DB_SQL_EDITOR_SNIPPETSBE_H_

#include "studio/wb_backend_public_interface.h"
#include "base/ui_form.h"
#include "grt/tree_model.h"

#include <deque>

namespace wb {
  class WBContextSQLIDE;
};
class SqlEditorForm;

#define USER_SNIPPETS "My Snippets"
#define SHARED_SNIPPETS "Shared"

class MYSQLWBBACKEND_PUBLIC_FUNC DbSqlEditorSnippets : public bec::ListModel {
public:
  enum Column { Description, Script };

  static auto setup(wb::WBContextSQLIDE *sqlide, const std::string &path) -> void;
  static auto get_instance() -> DbSqlEditorSnippets *;

  auto load() -> void;
  auto save() -> void;
  auto load_from_db(SqlEditorForm *editor = 0) -> void;

  auto shared_snippets_usable() -> bool;

  auto get_category_list() -> std::vector<std::string>;
  auto select_category(const std::string &category) -> void;
  auto selected_category() -> std::string;

  virtual auto count() -> size_t;
  virtual auto get_field(const bec::NodeId &node, ColumnId column, std::string &value) -> bool;
  virtual auto set_field(const bec::NodeId &node, ColumnId column, const std::string &value) -> bool;
  virtual auto refresh() -> void {
  }

  // virtual bool activate_node(const bec::NodeId &node);

  auto activate_toolbar_item(const bec::NodeId &selected, const std::string &name) -> bool;

  //  virtual bec::MenuItemList get_popup_items_for_nodes(const std::vector<bec::NodeId> &nodes);
  //  virtual bool activate_popup_item_for_nodes(const std::string &name, const std::vector<bec::NodeId> &nodes);

  virtual auto can_delete_node(const bec::NodeId &node) -> bool;
  virtual auto delete_node(const bec::NodeId &node) -> bool;

protected:
  DbSqlEditorSnippets(wb::WBContextSQLIDE *sqlide, const std::string &path);
  wb::WBContextSQLIDE *_sqlide;
  std::string _path;
  std::string _snippet_db;
  std::string _selected_category;
  bool _shared_snippets_enabled;

  struct Snippet {
    std::string title;
    std::string code;
    int db_snippet_id; // only if it comes from the DB
  };

  std::deque<Snippet> _entries;

  auto toolbar_item_activated(const std::string &name) -> void;
  auto copy_original_file(const std::string &name, bool overwrite) -> void;

  auto add_db_snippet(const std::string &name, const std::string &code) -> int;
  auto delete_db_snippet(int snippet_id) -> void;

public:
  auto add_snippet(const std::string &name, const std::string &code, bool edit) -> void;
};

#endif /* _DB_SQL_EDITOR_SNIPPETSBE_H_ */
