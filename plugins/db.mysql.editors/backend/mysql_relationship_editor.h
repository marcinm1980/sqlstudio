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

#ifndef _MYSQL_EDITOR_RELATIONSHIP_H_
#define _MYSQL_EDITOR_RELATIONSHIP_H_

#include "grt/editor_base.h"

#include "grts/structs.studio.physical.h"

#include "mysql_support_backend_public_interface.h"

#define RelationshipEditorBE_VERSION 1

class MYSQLWBMYSQLSUPPORTBACKEND_PUBLIC_FUNC RelationshipEditorBE : public bec::BaseEditor {
protected:
  studio_physical_ConnectionRef _relationship;

public: // editor interface
  enum VisibilityType { Visible = 1, Splitted = 2, Hidden = 3 };

  RelationshipEditorBE(const studio_physical_ConnectionRef &relationship);
  virtual auto should_close_on_delete_of(const std::string &oid) -> bool;

  auto model_only() -> bool {
    return *get_relationship()->foreignKey()->modelOnly() == 1;
  }
  auto set_model_only(bool flag) -> void;

  auto get_object() -> GrtObjectRef {
    return get_relationship();
  }

  auto get_relationship() -> studio_physical_ConnectionRef {
    return _relationship;
  }

  virtual auto get_title() -> std::string;

  auto set_caption(const std::string &caption) -> void;
  auto get_caption() -> std::string;
  auto get_caption_long() -> std::string;

  auto set_extra_caption(const std::string &caption) -> void;
  auto get_extra_caption() -> std::string;
  auto get_extra_caption_long() -> std::string;

  auto set_left_mandatory(bool flag) -> void;
  auto get_left_mandatory() -> bool;

  auto set_right_mandatory(bool flag) -> void;
  auto get_right_mandatory() -> bool;

  auto get_visibility() -> VisibilityType;
  auto set_visibility(VisibilityType type) -> void;

  auto open_editor_for_table(const db_TableRef &table) -> void;
  auto open_editor_for_left_table() -> void;
  auto open_editor_for_right_table() -> void;

  auto set_to_many(bool flag) -> void;
  auto get_to_many() -> bool;

  auto get_is_identifying() -> bool;
  auto set_is_identifying(bool flag) -> void;

  auto set_comment(const std::string &comment) -> void;
  auto get_comment() -> std::string;

  auto get_left_table_name() -> std::string;
  auto get_right_table_name() -> std::string;

  auto get_left_table_fk() -> std::string;

  auto get_left_table_info() -> std::string;
  auto get_right_table_info() -> std::string;

  auto edit_left_table() -> void;
  auto edit_right_table() -> void;
  auto invert_relationship() -> void;
};

#endif /* _EDITOR_RELATIONSHIP_H_ */
