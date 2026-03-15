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

#ifndef _WB_TAG_EDITOR_BE_
#define _WB_TAG_EDITOR_BE_

#include <sigc++/sigc++.h>
#include "grts/structs.studio.physical.h"
#include "grt/tree_model.h"

class TagObjectListBE : public bec::ListModel {
public:
  struct Node {
    meta_TaggedObjectRef owner;

    db_DatabaseObjectRef object;
    std::string doc;
  };

private:
  db_CatalogRef _catalog;
  meta_TagRef _tag;

  std::vector<Node> _content;

public:
  enum { Name, Documentation };

  TagObjectListBE(const db_CatalogRef &catalog);

  auto add_dropped_objectdata(const std::string &data) -> bool;

  auto set_tag(const meta_TagRef &tag) -> void;

  virtual auto get_field(const bec::NodeId &node, int column, std::string &value) -> bool;
  virtual auto set_field(const bec::NodeId &node, int column, const std::string &value) -> bool;
  virtual auto get_field_icon(const bec::NodeId &node, int column, bec::IconSize size) -> bec::IconId;
  virtual auto refresh() -> void;
  virtual auto count() -> int;

  auto commit() -> bool;
};

class TagEditorBE : public sigc::trackable {
  TagObjectListBE _object_list;

  studio_physical_ModelRef _model;
  int _selected_category;
  int _selected_tag;
  bool _changed;

public:
  TagEditorBE(const studio_physical_ModelRef &model);

  auto get_object_list() -> TagObjectListBE * {
    return &_object_list;
  }

  auto get_categories() const -> std::vector<std::string>;

  auto edit_categories() -> void;
  auto set_selected_category(int index) -> void;
  auto get_selected_category() const -> int {
    return _selected_category;
  }

  auto get_tags() const -> std::vector<std::string>;

  auto add_tag() -> void;
  auto delete_tag() -> bool;

  auto begin_save() -> void;
  auto end_save() -> void;

  auto set_selected_tag(int index) -> void;

  auto set_tag_name(const std::string &name) -> void;
  auto set_tag_label(const std::string &label) -> void;
  auto set_tag_color(const std::string &color) -> void;
  auto set_tag_comment(const std::string &color) -> void;

  auto get_tag_name() -> std::string;
  auto get_tag_label() -> std::string;
  auto get_tag_color() -> std::string;
  auto get_tag_comment() -> std::string;

  auto get_objects() -> std::vector<std::string>;
};

#endif