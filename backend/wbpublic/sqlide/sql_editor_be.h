
/*
 * Copyright (c) 2007, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "wbpublic_public_interface.h"

#include "base/trackable.h"

#ifndef _MSC_VER
#include <memory>
#include <set>

#include "grts/structs.db.mgmt.h"
#include "grts/structs.db.query.h"

#endif

#include "grtsqlparser/mysql_parser_services.h"
#include "grtdb/db_helpers.h"

#include "Scintilla.h"

namespace bec {
  class GRTManager;
}

namespace mforms {
  class CodeEditor;
  class FindPanel;
  class Menu;
  class View;
  class CodeEditorConfig;
  class ToolBar;
  class DropDelegate;
}; // namespace mforms

class MySQLRecognizer;

namespace parsers {
  class SymbolTable;
}

/**
 * The legacy MySQL editor class.
 */
class WBPUBLICBACKEND_PUBLIC_FUNC MySQLEditor : public base::trackable {
public:
  enum ContentType {
    ContentTypeGeneral,
    ContentTypeTrigger,
    ContentTypeView,
    ContentTypeFunction,
    ContentTypeProcedure,
    ContentTypeUdf,
    ContentTypeRoutine,
    ContentTypeEvent,
  };

  using Ref = std::shared_ptr<MySQLEditor>;
  using Ptr = std::weak_ptr<MySQLEditor>;

  static auto create(parsers::MySQLParserContext::Ref syntaxCheckContext,
                    parsers::MySQLParserContext::Ref autocompleteContext,
                    std::vector<parsers::SymbolTable *> const &globalSymbols,
                    db_query_QueryBufferRef grtobj = db_query_QueryBufferRef()) -> Ref;

  virtual ~MySQLEditor();

  auto grtobj() -> db_query_QueryBufferRef;

  auto set_base_toolbar(mforms::ToolBar *toolbar) -> void;

  auto get_container() -> mforms::View *;
  auto get_toolbar(bool include_file_actions = true) -> mforms::ToolBar *;
  auto get_editor_control() -> mforms::CodeEditor *;
  auto get_find_panel() -> mforms::FindPanel *;

  auto show_special_chars(bool flag) -> void;
  auto enable_word_wrap(bool flag) -> void;

  auto int_option(std::string name) -> int;
  auto string_option(std::string name) -> std::string;

  auto set_current_schema(const std::string &schema) -> void;
  auto sql() -> std::string;
  auto text_ptr() -> std::pair<const char *, size_t>;
  auto sql(const char *sql) -> void;

  auto empty() -> bool;
  auto append_text(const std::string &text) -> void;

  auto current_statement() -> std::string;
  auto get_current_statement_range(size_t &start, size_t &end, bool strict = false) -> bool;

  auto cursor_pos() -> std::size_t;
  auto cursor_pos_row_column(bool local) -> std::pair<std::size_t, std::size_t>;
  auto set_cursor_pos(std::size_t position) -> void;

  auto selected_range(std::size_t &start, std::size_t &end) -> bool;
  auto set_selected_range(std::size_t start, std::size_t end) -> void;

  auto is_refresh_enabled() const -> bool;
  auto set_refresh_enabled(bool val) -> void;
  auto is_sql_check_enabled() const -> bool;
  auto set_sql_check_enabled(bool val) -> void;

  auto show_auto_completion(bool auto_choose_single) -> void;
  std::vector<std::pair<int, std::string>> update_auto_completion(const std::string &typed_part);
  auto cancel_auto_completion() -> void;

  auto selected_text() -> std::string;
  auto set_selected_text(const std::string &new_text) -> void;
  auto insert_text(const std::string &new_text) -> void;

  boost::signals2::signal<void()> *text_change_signal();

  auto sql_mode() -> std::string;
  auto set_sql_mode(const std::string &value) -> void;
  auto setServerVersion(GrtVersionRef version) -> void;

  auto restrict_content_to(ContentType type) -> void;

  auto has_sql_errors() const -> bool;

  auto stop_processing() -> void;

  auto focus() -> void;

  auto register_file_drop_for(mforms::DropDelegate *target) -> void;

protected:
  MySQLEditor(parsers::MySQLParserContext::Ref syntaxCheckContext,
              parsers::MySQLParserContext::Ref autocompleteContext);

private:
  class Private;
  Private *d;

  auto set_grtobj(db_query_QueryBufferRef grtobj) -> void;

  auto setup_auto_completion() -> void;
  auto run_code_completion() -> void *;

  auto getWrittenPart(size_t position) -> std::string;

  auto text_changed(Sci_Position position, Sci_Position length, Sci_Position lines_changed, bool added) -> void;
  auto char_added(int char_code) -> void;
  auto dwell_event(bool started, size_t position, int x, int y) -> void;

  auto setup_editor_menu() -> void;
  auto editor_menu_opening() -> void;
  auto activate_context_menu_item(const std::string &name) -> void;

  auto start_sql_processing() -> bool;
  auto do_statement_split_and_check(int id) -> bool; // Run in worker thread.

  auto on_report_sql_statement_border(int begin_lineno, int begin_line_pos, int end_lineno, int end_line_pos, int tag) -> int;
  auto on_sql_error(int lineno, int tok_line_pos, int tok_len, const std::string &msg, int tag) -> int;
  auto on_sql_check_progress(float progress, const std::string &msg, int tag) -> int;

  auto splitting_done() -> void *;
  auto update_error_markers() -> void *;

  auto code_completion_enabled() -> bool;
  auto auto_start_code_completion() -> bool;
  auto make_keywords_uppercase() -> bool;
};
