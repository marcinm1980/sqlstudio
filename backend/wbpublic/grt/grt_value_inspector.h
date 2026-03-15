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

#include "grt.h"

#include "tree_model.h"
#include "refresh_ui.h"

#define ValueInspectorBE_VERSION 2

namespace bec {

  class WBPUBLICBACKEND_PUBLIC_FUNC ValueInspectorBE : public TreeModel, public RefreshUI {
  public:
    enum ValueInspectorColumns {
      Name,        // or group name
      Value,       // not valid for groups
      Description, // description
      IsReadonly,  // determines if given object member is editable
      EditMethod   // only for objects, defined with attr:editas in the struct xml
    };

    static auto create(const grt::ValueRef &value, bool grouped, bool process_editas_flag) -> ValueInspectorBE *;

    static auto create(const std::vector<grt::ObjectRef> &objects) -> ValueInspectorBE *;

    virtual auto add_item(NodeId &new_node) -> bool = 0;
    virtual auto delete_item(const NodeId &node) -> bool = 0;

    // virtual MYX_GRT_VALUE_TYPE get_field_type(const NodeId &node, ColumnId column)= 0;

    virtual auto get_grt_value(const NodeId &node, ColumnId column) -> grt::ValueRef;

    virtual auto set_convert_field(const NodeId &node, ColumnId column, const std::string &value) -> bool;
    virtual auto set_field(const NodeId &node, ColumnId column, const std::string &value) -> bool;
    virtual auto set_field(const NodeId &node, ColumnId column, double value) -> bool;
    virtual auto set_field(const NodeId &node, ColumnId column, ssize_t value) -> bool;

    virtual auto get_field_icon(const NodeId &node, ColumnId column, IconSize size) -> IconId;

  public: // Responder methods
  protected:
    ValueInspectorBE();

    virtual auto get_canonical_type(const NodeId &node) -> grt::Type = 0;

    virtual auto set_value(const NodeId &node, const grt::ValueRef &value) -> bool = 0;

    void monitor_object_changes(const grt::ObjectRef &obj);

  private:
    void changed_slot(const std::string &name, const grt::ValueRef &value);
    boost::signals2::scoped_connection _changed_conn;
  };
};
