/*
 * Copyright (c) 2012, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include <stack>

#include "grt/tree_model.h"
#include "grts/structs.db.h"
#include "grts/structs.db.mysql.h"
#include "grt.h"

#include "grtdb/catalog_templates.h"

auto utf_to_upper(const char *str) -> std::string;

#include "diff/diffchange.h"
#include "db_mysql_public_interface.h"

class DiffNode;

std::ostream &operator<<(std::ostream &os, const DiffNode &);

auto get_old_name_or_name(GrtNamedObjectRef obj) -> std::string;

template<typename T>
std::string get_catalog_map_key(grt::Ref<T> t) {
  typedef typename ct::Traits<T>::ParentType Parent;

  std::string parent_key(utf_to_upper(get_catalog_map_key(grt::Ref<Parent>::cast_from(t->owner())).c_str()));

  std::string obj_key(utf_to_upper(get_old_name_or_name(t).c_str()));

  return std::string(parent_key).append(".").append(T::static_class_name()).append(".`").append(obj_key).append("`");
}

template <>
std::string get_catalog_map_key<db_mysql_Catalog>(db_mysql_CatalogRef cat);

class DiffNodePart {
  GrtNamedObjectRef object;
  bool modified;

public:
  DiffNodePart(GrtNamedObjectRef obj) : object(obj), modified(false) {
  }

  auto is_modified() const -> bool {
    return modified;
  }
  auto set_modified(bool mod) -> void {
    modified = mod;
  }
  auto is_valid_object() const -> bool {
    return object.is_valid();
  }
  auto get_name() const -> std::string {
    return std::string(object->name().c_str());
  }
  auto get_object() const -> GrtNamedObjectRef {
    return object;
  }
};

class DiffNodeController;

class DiffNode {
public:
  enum ApplicationDirection { ApplyToModel = 20, ApplyToDb, DontApply, CantApply };

  typedef std::vector<DiffNode *> DiffNodeVector;

private:
  DiffNodePart model_part;
  DiffNodePart db_part;
  std::shared_ptr<grt::DiffChange> change;
  ApplicationDirection applyDirection;
  DiffNodeVector children;
  bool modified;

public:
  DiffNode(GrtNamedObjectRef model_object, GrtNamedObjectRef external_object, bool inverse,
           std::shared_ptr<grt::DiffChange> c = std::shared_ptr<grt::DiffChange>())
    : model_part(inverse ? external_object : model_object),
      db_part(inverse ? model_object : external_object),
      change(c),
      modified(false) {
    set_modified_and_update_dir(!model_object.is_valid() || !external_object.is_valid(), c);
  }

  ~DiffNode() {
    for (DiffNodeVector::iterator It = children.begin(); It != children.end(); ++It)
      delete *It;
  }

  auto dump(int depth = 0) -> void;

  auto get_change() const -> std::shared_ptr<grt::DiffChange> {
    return change;
  };

  auto apply_direction(const ApplicationDirection &d) -> void {
    applyDirection = d;
  }

  auto apply_direction() const -> ApplicationDirection {
    return applyDirection;
  }

  auto get_application_direction() const -> ApplicationDirection {
    return applyDirection;
  }

  auto get_model_part() const -> const DiffNodePart & {
    return model_part;
  }
  auto get_db_part() const -> const DiffNodePart & {
    return db_part;
  }

  auto append(DiffNode *child) -> void {
    children.push_back(child);
  }

  auto get_children_size() const -> size_t {
    return children.size();
  }
  auto get_child(size_t idx) -> DiffNode * {
    return children[idx];
  }
  auto find_node_for_object(const grt::ObjectRef obj) -> DiffNode *;
  auto find_child_by_db_part_name(const std::string &name) -> DiffNode *;

  auto get_children_begin() const -> DiffNodeVector::const_iterator {
    return children.begin();
  }
  auto get_children_end() const -> DiffNodeVector::const_iterator {
    return children.end();
  }

  auto is_modified() const -> bool {
    return modified;
  }
  auto is_modified_recursive() const -> bool {
    if (modified)
      return true;
    for (DiffNodeVector::const_iterator i = children.begin(); i != children.end(); ++i)
      if ((*i)->is_modified_recursive())
        return true;
    return false;
  }
  auto set_modified_and_update_dir(bool m, std::shared_ptr<grt::DiffChange> c) -> void;

  auto get_object_list_for_script(std::vector<grt::ValueRef> &vec) const -> void;
  auto get_object_list_to_apply_to_model(std::vector<grt::ValueRef> &vec,
                                         std::vector<grt::ValueRef> &removal_vec) const -> void;
};

class WBPLUGINDBMYSQLBE_PUBLIC_FUNC DiffNodeController {
public:
  DiffNodeController();
  DiffNodeController(const std::map<DiffNode::ApplicationDirection, DiffNode::ApplicationDirection> directions_map);
  auto set_next_apply_direction(DiffNode *node) const -> void;
  auto set_apply_direction(DiffNode *node, DiffNode::ApplicationDirection dir, bool recursive) const -> void;

protected:
  std::map<DiffNode::ApplicationDirection, DiffNode::ApplicationDirection> _directions_map;
};

class WBPLUGINDBMYSQLBE_PUBLIC_FUNC DiffTreeBE : public bec::TreeModel {
public:
  // icon, model-changed, model-object-name, apply-direction, icon, db-changed, db-object-name

  enum LayerColumns { ModelChanged = 10, ModelObjectName, ApplyDirection, DbChanged, DbObjectName };

private:
  DiffNodeController _node_controller;

  template <typename T>
  static T find_object_in_catalog_map(T cat, const CatalogMap &map);

  DiffNode *_root;
  bec::IconId change_nothing_icon, change_backward_icon, change_forward_icon, change_ignore_icon, alert_icon,
    create_alert_icon, drop_alert_icon;

  std::vector<std::string> _schemata;

  // static void build_catalog_map(db_mysql_CatalogRef catalog, CatalogMap& map);
  auto update_tree_with_changes(const std::shared_ptr<grt::DiffChange> diffchange) -> bool;
  auto apply_change(GrtObjectRef obj, std::shared_ptr<grt::DiffChange> change) -> void;

  auto fill_tree(DiffNode *root, db_mysql_CatalogRef catalog, const CatalogMap &map, bool inverse) -> void;
  auto fill_tree(DiffNode *schema_node, db_mysql_SchemaRef schema, const CatalogMap &map, bool inverse) -> void;
  auto fill_tree(DiffNode *table_node, db_mysql_TableRef table, const CatalogMap &map, bool inverse) -> void;

public:
  auto get_node_with_id(const bec::NodeId &nodeid) -> DiffNode *;
  DiffTreeBE(const std::vector<std::string> &schemata, db_mysql_CatalogRef model_catalogRef,
             db_mysql_CatalogRef external_catalog, std::shared_ptr<grt::DiffChange> diffchange,
             DiffNodeController controller = DiffNodeController());
  virtual ~DiffTreeBE() {
    delete _root;
  };

  virtual auto count_children(const bec::NodeId &) -> size_t;
  virtual auto get_child(const bec::NodeId &, size_t) -> bec::NodeId;
  virtual auto get_field(const bec::NodeId &node_id, ColumnId column, std::string &value) -> bool;
  virtual auto get_field_icon(const bec::NodeId &node, ColumnId column, bec::IconSize size) -> bec::IconId;
  virtual auto refresh() -> void {
  }

  auto set_next_apply_direction(const bec::NodeId &node_id) -> void;
  auto set_apply_direction(const bec::NodeId &node_id, DiffNode::ApplicationDirection dir, bool recursive) -> void;
  auto get_apply_direction(const bec::NodeId &node_id) -> DiffNode::ApplicationDirection;

  auto get_object_list_for_script(std::vector<grt::ValueRef> &vec) const -> void;
  auto get_object_list_to_apply_to_model(std::vector<grt::ValueRef> &vec,
                                         std::vector<grt::ValueRef> &removal_vec) const -> void;
};
