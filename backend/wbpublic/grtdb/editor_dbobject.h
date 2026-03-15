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

#include "grtpp_notifications.h"

#include "wbpublic_public_interface.h"
#include "grt/editor_base.h"

#include "grtsqlparser/mysql_parser_services.h"

namespace bec {

  class WBPUBLICBACKEND_PUBLIC_FUNC DBObjectEditorBE : public BaseEditor, public grt::GRTObserver {
  public:
    virtual ~DBObjectEditorBE();

    virtual auto should_close_on_delete_of(const std::string &oid) -> bool;

    virtual auto get_dbobject() -> db_DatabaseObjectRef {
      return db_DatabaseObjectRef::cast_from(get_object());
    };

    virtual auto get_name() -> std::string;
    virtual auto set_name(const std::string &name) -> void;

    virtual auto get_comment() -> std::string;
    virtual auto set_comment(const std::string &descr) -> void;

    virtual auto get_sql() -> std::string;
    virtual auto set_sql(const std::string &sql) -> void;

    virtual auto is_sql_commented() -> bool;
    virtual auto set_sql_commented(bool flag) -> void;

    virtual auto has_editor() -> bool;
    virtual auto get_sql_editor() -> MySQLEditor::Ref;
    virtual auto reset_editor_undo_stack() -> void;

    auto get_schema() -> db_SchemaRef;
    virtual auto get_schema_name() -> std::string;

    auto get_catalog() -> db_CatalogRef;
    auto get_schema_with_name(const std::string &schema_name) -> db_SchemaRef;

    virtual auto get_all_table_names() -> std::vector<std::string>;
    virtual auto get_all_schema_names() -> std::vector<std::string>;
    virtual auto get_schema_table_names() -> std::vector<std::string>;
    virtual auto get_table_column_names(const std::string &table_name) -> std::vector<std::string>;
    virtual auto get_table_column_names(const db_TableRef &table) -> std::vector<std::string>;

    // charsets and collations
    virtual auto get_charset_list() -> std::vector<std::string>;
    virtual auto get_charset_collation_list(const std::string &charset) -> std::vector<std::string>;
    virtual auto get_charset_collation_list() -> std::vector<std::string>;
    auto parse_charset_collation(const std::string &str, std::string &charset, std::string &collation) -> bool;
    auto format_charset_collation(const std::string &charset, const std::string &collation) -> std::string;

    auto update_change_date() -> void;
    auto send_refresh() -> void;
    auto set_sql_mode(const std::string &value) -> void;

    virtual auto is_editing_live_object() -> bool;
    virtual auto apply_changes_to_live_object() -> void;
    virtual auto refresh_live_object() -> void;
    virtual auto can_close() -> bool;

    std::function<bool(DBObjectEditorBE *, bool)> on_apply_changes_to_live_object;
    std::function<void(DBObjectEditorBE *)> on_refresh_live_object;
    std::function<void(DBObjectEditorBE *)> on_create_live_table_stubs;
    std::function<bool(DBObjectEditorBE *, std::string &, std::string &)> on_expand_live_table_stub;

  protected:
    parsers::MySQLParserContext::Ref _parserContext;
    parsers::MySQLParserContext::Ref _autocompletionContext;
    parsers::MySQLParserServices::Ref _parserServices;
    parsers::SymbolTable *_globalSymbols;

    DBObjectEditorBE(const db_DatabaseObjectRef &object);

  private:
    MySQLEditor::Ref _sql_editor;
    db_CatalogRef _catalog;

    boost::signals2::scoped_connection _val_notify_conn;
    auto notify_from_validation(const grt::Validator::Tag &tag, const grt::ObjectRef &, const std::string &,
                                const int level) -> void; // level is grt::MessageType
    // Real-time validation part
    grt::MessageType _last_validation_check_status;
    std::string _last_validation_message;

    virtual auto handle_grt_notification(const std::string &name, grt::ObjectRef sender, grt::DictRef info) -> void;
  };
};
