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

#include "mysql_relationship_editor.h"

#pragma make_public(::RelationshipEditorBE)

namespace MySQL {
  namespace Grt {
    namespace Db {

    public
      enum class RelationshipVisibilityType {
        Visible = RelationshipEditorBE::Visible,
        Splitted = RelationshipEditorBE::Splitted,
        Hidden = RelationshipEditorBE::Hidden
      };

    public
      ref class MySQLRelationshipEditorWrapper : public BaseEditorWrapper {
      public:
        MySQLRelationshipEditorWrapper(MySQL::Grt::GrtValue ^ arglist);
        ~MySQLRelationshipEditorWrapper();

        auto get_unmanaged_object() -> RelationshipEditorBE *;

        void set_caption(String ^ caption);
        auto get_caption() -> String ^;
        auto get_caption_long() -> String ^;
        void set_extra_caption(String ^ caption);
        auto get_extra_caption() -> String ^;
        auto get_extra_caption_long() -> String ^;

        auto get_right_table_name() -> String ^;
        auto get_left_table_name() -> String ^;
        auto get_right_table_info() -> String ^;
        auto get_left_table_info() -> String ^;

        auto get_left_table_fk() -> String ^;

        auto set_left_mandatory(bool flag) -> void;
        auto get_left_mandatory() -> bool;
        auto set_right_mandatory(bool flag) -> void;
        auto get_right_mandatory() -> bool;

        auto set_to_many(bool flag) -> void;
        auto get_to_many() -> bool;

        void set_comment(String ^ comment);
        auto get_comment() -> String ^;

        auto get_visibility() -> RelationshipVisibilityType;
        auto set_visibility(RelationshipVisibilityType v) -> void;

        auto open_editor_for_left_table() -> void;
        auto open_editor_for_right_table() -> void;

        auto get_is_identifying() -> bool;
        auto set_is_identifying(bool identifying) -> void;
      };

    } // namespace Db
  }   // namespace Grt
} // namespace MySQL
