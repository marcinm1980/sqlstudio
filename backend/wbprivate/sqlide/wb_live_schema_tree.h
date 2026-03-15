/*
 * Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "base/symbol-info.h"

#include "grt.h"
#include "grt/tree_model.h"
#include "studio/wb_backend_public_interface.h"
#include "base/string_utilities.h"
#include "mforms/treeview.h"
#include "base/trackable.h"

namespace wb {
  class MYSQLWBBACKEND_PUBLIC_FUNC LiveSchemaTree : base::trackable {
  public:
    friend class LiveSchemaTreeTester;

    static const short COLUMN_DATA;
    static const short TRIGGER_DATA;
    static const short INDEX_DATA;
    static const short FK_DATA;

    static const std::string SCHEMA_TAG;
    static const std::string TABLES_TAG;
    static const std::string VIEWS_TAG;
    static const std::string PROCEDURES_TAG;
    static const std::string FUNCTIONS_TAG;
    static const std::string TABLE_TAG;
    static const std::string VIEW_TAG;
    static const std::string ROUTINE_TAG;
    static const std::string COLUMNS_TAG;
    static const std::string INDEXES_TAG;
    static const std::string TRIGGERS_TAG;
    static const std::string FOREIGN_KEYS_TAG;
    static const std::string COLUMN_TAG;
    static const std::string INDEX_TAG;
    static const std::string TRIGGER_TAG;
    static const std::string FOREIGN_KEY_TAG;

    static const std::string FETCHING_CAPTION;
    static const std::string ERROR_FETCHING_CAPTION;
    static const std::string TABLES_CAPTION;
    static const std::string VIEWS_CAPTION;
    static const std::string PROCEDURES_CAPTION;
    static const std::string FUNCTIONS_CAPTION;

    static const std::string COLUMNS_CAPTION;
    static const std::string INDEXES_CAPTION;
    static const std::string TRIGGERS_CAPTION;
    static const std::string FOREIGN_KEYS_CAPTION;

    static const std::string LST_INFO_BOX_DETAIL_ROW;

    static const int TABLES_NODE_INDEX;
    static const int VIEWS_NODE_INDEX;
    static const int PROCEDURES_NODE_INDEX;
    static const int FUNCTIONS_NODE_INDEX;
    static const int TABLE_COLUMNS_NODE_INDEX;
    static const int TABLE_INDEXES_NODE_INDEX;
    static const int TABLE_FOREIGN_KEYS_NODE_INDEX;
    static const int TABLE_TRIGGERS_NODE_INDEX;

    enum FilterType { LocalLike, LocalRegexp, RemoteLike, RemoteRegexp };

    enum ObjectType {
      Schema,
      Table,
      View,
      Procedure,
      Function,

      TableCollection,
      ViewCollection,
      ProcedureCollection,
      FunctionCollection,

      ColumnCollection,
      IndexCollection,
      TriggerCollection,
      ForeignKeyCollection,

      Trigger,
      TableColumn,
      ViewColumn,
      ForeignKey,
      Index,

      ForeignKeyColumn,
      IndexColumn,
      Any,
      NoneType
    };

    // This will be used on different object type validations
    enum ObjectTypeValidation {
      DatabaseObject, // Schema, Table, View, Procedure, Function
      SchemaObject,   // Table, View, Procedure, Function
      TableOrView,    // Table, View
      ColumnObject,   // TableColumn, ViewVolumn
      RoutineObject   // Procedure, Function
    };

    // One entry in a list that describes a single object.
    struct ChangeRecord {
      ObjectType type;
      std::string schema;
      std::string name;
      std::string detail;
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC LSTData : public mforms::TreeNodeData {
    public:
      std::string details;
      LSTData();
      virtual auto get_type() -> ObjectType = 0;
      virtual auto copy(LSTData* other) -> void;
      virtual auto get_details(bool full, const mforms::TreeNodeRef& node) -> std::string;
      virtual auto get_object_name() -> std::string = 0;

      // Nodes requiring to enable this functionality should overwrite
      // and return true once their specific criteria is met
      virtual auto is_update_complete() -> bool {
        return false;
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC ColumnData : public LSTData {
    private:
      ObjectType _type;

    public:
      ColumnData(ObjectType type = TableColumn) : LSTData(), is_pk(false), is_fk(false), is_id(false), is_idx(false) {
        _type = type;
      }
      // NOTE than name will contain the column name duplicating this info as it
      //      is already on the TreeNode. Duplicating it is better than having
      //      full HTML for all the columns on all the tables in the db.
      std::string name;
      std::string type;
      std::string default_value;
      std::string charset_collation;
      bool is_pk;
      bool is_fk;
      bool is_id;
      bool is_idx;

      virtual auto copy(LSTData* other) -> void;
      virtual auto get_type() -> ObjectType {
        return _type;
      }
      virtual auto get_details(bool full, const mforms::TreeNodeRef& node) -> std::string;
      virtual auto get_object_name() -> std::string {
        return _("Column");
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC FKData : public LSTData {
    public:
      FKData() : LSTData(), update_rule(0), delete_rule(0) {
      }
      unsigned char update_rule;
      unsigned char delete_rule;
      std::string referenced_table;
      std::string from_cols;
      std::string to_cols;

      virtual auto copy(LSTData* other) -> void;
      virtual auto get_type() -> ObjectType {
        return LiveSchemaTree::ForeignKey;
      }
      virtual auto get_details(bool full, const mforms::TreeNodeRef& node) -> std::string;
      virtual auto get_object_name() -> std::string {
        return _("Foreign Key");
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC IndexData : public LSTData {
    public:
      IndexData() : LSTData(), visible(true), unique(false), type(0) {
      }
      bool visible;
      bool unique;
      unsigned char type;
      std::vector<std::string> columns;

      virtual auto copy(LSTData* other) -> void;
      virtual auto get_type() -> ObjectType {
        return LiveSchemaTree::Index;
      }
      virtual auto get_details(bool full, const mforms::TreeNodeRef& node) -> std::string;
      virtual auto get_object_name() -> std::string {
        return _("Index");
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC TriggerData : public LSTData {
    public:
      TriggerData() : LSTData(), event_manipulation(0), timing(0) {
      }
      unsigned char event_manipulation;
      unsigned char timing;

      virtual auto copy(LSTData* other) -> void;
      virtual auto get_type() -> ObjectType {
        return LiveSchemaTree::Trigger;
      }
      virtual auto get_details(bool full, const mforms::TreeNodeRef& node) -> std::string;
      virtual auto get_object_name() -> std::string {
        return _("Trigger");
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC ObjectData : public LSTData {
    public:
      ObjectData() : LSTData(), fetched(false), fetching(false) {
      }
      bool fetched;
      bool fetching;

      virtual auto copy(LSTData* other) -> void;
      virtual auto get_type() -> ObjectType {
        return LiveSchemaTree::Any;
      }
      virtual auto get_object_name() -> std::string {
        return _("Object");
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC ProcedureData : public ObjectData {
    public:
      ProcedureData() : ObjectData() {
      }
      virtual auto get_type() -> ObjectType {
        return LiveSchemaTree::Procedure;
      }
      virtual auto get_object_name() -> std::string {
        return _("Procedure");
      }
      virtual auto get_details(bool full, const mforms::TreeNodeRef& node) -> std::string;
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC FunctionData : public ObjectData {
    public:
      FunctionData() : ObjectData() {
      }
      virtual auto get_type() -> ObjectType {
        return LiveSchemaTree::Function;
      }
      virtual auto get_object_name() -> std::string {
        return _("Function");
      }
      virtual auto get_details(bool full, const mforms::TreeNodeRef& node) -> std::string;
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC ViewData : public ObjectData {
    public:
      ViewData() : ObjectData(), columns_load_error(false), _loaded_mask(0), _loading_mask(0), _reload_mask(0) {
      }
      bool columns_load_error;
      short _loaded_mask;
      short _loading_mask;
      short _reload_mask;

      virtual auto copy(LSTData* other) -> void;
      virtual auto get_type() -> ObjectType {
        return LiveSchemaTree::View;
      }
      auto set_reload_mask(short mask) -> void {
        _reload_mask = mask;
      }
      auto get_reload_mask() -> short {
        return _reload_mask;
      }
      virtual auto get_loaded_mask() -> short;
      virtual auto set_loaded_data(short mask) -> void;
      virtual auto set_unloaded_data(short mask) -> void;
      auto is_data_loaded(short mask) -> bool;
      virtual auto get_details(bool full, const mforms::TreeNodeRef& node) -> std::string;
      virtual auto get_object_name() -> std::string {
        return _("View");
      }
      auto get_loading_mask() -> short;
      auto set_loading_mask(short mask) -> void;
      virtual auto is_update_complete() -> bool;
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC TableData : public ViewData {
    public:
      TableData() : ViewData() {
      }

      virtual auto get_type() -> ObjectType {
        return LiveSchemaTree::Table;
      }

      virtual auto get_details(bool full, const mforms::TreeNodeRef& node) -> std::string;
      virtual auto get_object_name() -> std::string {
        return _("Table");
      }
    };

    class MYSQLWBBACKEND_PUBLIC_FUNC SchemaData : public LSTData {
    public:
      SchemaData() : LSTData(), fetched(false), fetching(false) {};
      bool fetched;
      bool fetching;

      virtual auto copy(LSTData* other) -> void;
      virtual auto get_type() -> ObjectType {
        return LiveSchemaTree::Schema;
      }
      virtual auto get_object_name() -> std::string {
        return _("Schema");
      }
    };

    using NewSchemaContentArrivedSlot = std::function<void(const std::string&, base::StringListPtr, base::StringListPtr,
                                                           base::StringListPtr, base::StringListPtr, bool)>;
    using NewObjectDetailsArrivedSlot = std::function<void(const std::string&, const std::string&, ObjectType,
                                                           ObjectType, const std::map<std::string, LSTData*>&)>;
    using NodeChildrenUpdaterSlot =
      std::function<bool(mforms::TreeNodeRef, base::StringListPtr, ObjectType, bool, bool)>;

    struct FetchDelegate {
      virtual auto fetch_schema_list() -> std::vector<std::string> = 0;
      virtual auto fetch_data_for_filter(const std::string&, const std::string&,
                                         const NewSchemaContentArrivedSlot&) -> bool = 0;
      virtual auto fetch_schema_contents(const std::string&, const NewSchemaContentArrivedSlot&) -> bool = 0;
      virtual auto fetch_object_details(const std::string& schema_name, const std::string& object_name,
                                        wb::LiveSchemaTree::ObjectType type, short, const NodeChildrenUpdaterSlot&) -> bool = 0;
      virtual auto fetch_routine_details(const std::string& schema_name, const std::string& object_name,
                                         wb::LiveSchemaTree::ObjectType type) -> bool = 0;
    };

    struct Delegate {
      virtual auto tree_refresh() -> void = 0;
      virtual auto sidebar_action(const std::string&) -> bool = 0;
      virtual auto tree_activate_objects(const std::string&, const std::vector<ChangeRecord>&) -> void = 0;
    };

    using SqlEditorTextInsertSignal = boost::signals2::signal<int(const std::string&)>;

    SqlEditorTextInsertSignal sql_editor_text_insert_signal;

  protected:
    std::weak_ptr<FetchDelegate> _fetch_delegate;
    std::weak_ptr<Delegate> _delegate;
    std::string _active_schema;
    mforms::TreeView* _model_view;

    GPatternSpec* _schema_pattern = nullptr;
    GPatternSpec* _object_pattern = nullptr;

    bool _case_sensitive_identifiers;

    auto schema_contents_arrived(const std::string& schema_name, base::StringListPtr tables, base::StringListPtr views,
                                 base::StringListPtr procedures, base::StringListPtr functions, bool just_append) -> void;
    auto load_table_details(mforms::TreeNodeRef& node, int fetch_mask) -> void;
    auto fetch_table_details(ObjectType object_type, const std::string schema_name, const std::string object_name,
                             int fetch_mask) -> void;
    auto load_routine_details(mforms::TreeNodeRef& node) -> void;
    auto load_schema_content(mforms::TreeNodeRef& schema_node) -> void;
    auto reload_object_data(mforms::TreeNodeRef& node) -> void;
    auto discard_object_data(mforms::TreeNodeRef& node, int data_mask) -> void;

    auto identifiers_equal(const std::string& a, const std::string& b) -> bool;

    auto overlay_icons_for_tree_node(mforms::TreeNodeRef node) -> std::vector<std::string>;

    // Filtering functions
    auto get_filter_wildcard(const std::string& filter, FilterType type = LocalLike) -> std::string;
    auto clean_filter() -> void;
    auto filter_children_collection(mforms::TreeNodeRef& source, mforms::TreeNodeRef& target) -> void;
    auto filter_children(ObjectType type, mforms::TreeNodeRef& source, mforms::TreeNodeRef& target,
                         GPatternSpec* pattern = NULL) -> bool;
    auto is_object_type(ObjectTypeValidation validation, ObjectType type) -> bool;

  public:
    LiveSchemaTree(base::MySQLVersion version);
    virtual ~LiveSchemaTree();

    auto set_model_view(mforms::TreeView* target) -> void;
    auto set_delegate(std::shared_ptr<Delegate> delegate) -> void;
    auto set_fetch_delegate(std::shared_ptr<FetchDelegate> delegate) -> void;

    auto load_table_details(ObjectType object_type, const std::string schema_name, const std::string object_name,
                            int fetch_mask) -> void;

    auto set_filter(std::string filter) -> void;
    auto getFilter() const -> std::string {
      return _filter;
    }

    auto filter_data() -> void;
    auto load_data_for_filter(const std::string& schema_filter, const std::string& object_filter) -> void;

    static auto internalize_token(const std::string& token) -> unsigned char;
    static auto externalize_token(unsigned char c) -> std::string;

    auto set_active_schema(const std::string& schema) -> void;

    auto set_no_connection() -> void;

    auto set_enabled(bool enabled) -> void;

    auto update_live_object_state(ObjectType type, const std::string& schema_name, const std::string& old_obj_name,
                                  const std::string& new_obj_name) -> void;

    virtual auto get_field_description(const mforms::TreeNodeRef& node) -> std::string;
    auto set_notify_on_reload(const mforms::TreeNodeRef& node) -> void;
    auto notify_on_reload(const mforms::TreeNodeRef& node) -> void;

    auto get_node_for_object(const std::string& schema_name, ObjectType type, const std::string& name) -> mforms::TreeNodeRef;
    auto create_node_for_object(const std::string& schema_name, ObjectType type,
                                               const std::string& name) -> mforms::TreeNodeRef;

    // Returns a list of db_query_LiveDBObjectRef.
    auto get_selected_objects() -> grt::BaseListRef;

    virtual auto activate_popup_item_for_nodes(const std::string& name,
                                               const std::list<mforms::TreeNodeRef>& orig_nodes) -> bool;
    virtual auto get_popup_items_for_nodes(const std::list<mforms::TreeNodeRef>& nodes) -> bec::MenuItemList;

    auto is_schema_contents_enabled() const -> bool;
    auto is_schema_contents_enabled(bool value) -> void;

    auto set_base(LiveSchemaTree* base) -> void {
      _base = base;
    }

    auto getBase() -> LiveSchemaTree* {
      return _base;
    }

    auto update_node_children(mforms::TreeNodeRef parent, base::StringListPtr children, ObjectType type,
                              bool sorted = false, bool just_append = false) -> bool;
    auto update_change_data(mforms::TreeNodeRef parent, base::StringListPtr children, ObjectType type,
                            std::vector<mforms::TreeNodeRef>& to_remove) -> void;
    auto insert_node(mforms::TreeNodeRef parent, const std::string& name, ObjectType type) -> mforms::TreeNodeRef;
    auto setup_node(mforms::TreeNodeRef node, ObjectType type, mforms::TreeNodeData* data = NULL,
                    bool ignore_null_data = false) -> void;
    auto update_node_icon(mforms::TreeNodeRef node) -> void;

    auto update_schemata(base::StringListPtr schema_list) -> void;
    auto binary_search_node(const mforms::TreeNodeRef& parent, int first, int last,
                                           const std::string& name, ObjectType type, int& position) -> mforms::TreeNodeRef;
    auto get_child_node(const mforms::TreeNodeRef& parent, const std::string& name,
                                       ObjectType type = Any, bool binary_search = true) -> mforms::TreeNodeRef;
    auto find_child_position(const mforms::TreeNodeRef& parent, const std::string& name, ObjectType type,
                             int& position) -> bool;
    auto expand_toggled(mforms::TreeNodeRef node, bool value) -> void;
    auto node_activated(mforms::TreeNodeRef node, int column) -> void;
    auto set_case_sensitive_identifiers(bool flag) -> void;
    auto get_schema_name(const mforms::TreeNodeRef& node) -> std::string;
    auto get_node_path(const mforms::TreeNodeRef& node) -> std::vector<std::string>;
    auto get_node_from_path(std::vector<std::string> path) -> mforms::TreeNodeRef;

    auto enable_events(bool enable) -> void {
      _enabled_events = enable;
    }

    auto getEnabledEvents() -> bool {
      return _enabled_events;
    }

  private:
    bool _is_schema_contents_enabled;
    bool _enabled_events;
    base::MySQLVersion _version;

    LiveSchemaTree* _base = nullptr;
    std::string _filter;
    ObjectType _filter_type;
    LSTData* notify_on_reload_data = nullptr;

    static const char* _schema_tokens[16];

    std::map<ObjectType, std::string> _icon_paths;

    std::map<ObjectType, mforms::TreeNodeCollectionSkeleton> _node_collections;

    auto fill_node_icons() -> void;
    auto get_node_icon_path(ObjectType type) -> std::string;
    auto get_node_icon(ObjectType type) -> bec::IconId;
  };
}; // namespace wb
