/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#pragma once

#include "mforms/treeview.h"
#include "mforms/menubar.h"
#include "modules/base_session.h"
#include "grt/icon_manager.h"
#include "base/string_utilities.h"


namespace ng {
  using NodeSessionRef = boost::shared_ptr<mysh::ShellBaseSession>;
  class NgSidebar : public mforms::TreeView
  {
    enum ObjectType
    {
      Session,
      Schema,
      Collection,
      Table,
      View,
      Procedure,
      Function,
      Event,

      Folder,
      CollectionCollection,
      TableCollection,
      ViewCollection,
      ProcedureCollection,
      FunctionCollection,
      EventCollection,

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
      None
    };

    bool _hasCollections;
    mforms::ContextMenu *_menu;
    std::string _activeSchema;
    void contextMenuWillShow(mforms::MenuItem *parent_item);
    mforms::TreeNodeRef binarySearchNode(const mforms::TreeNodeRef& parent, int min, int max, const std::string &name, int &position);
    mforms::TreeNodeRef getChildNode(const mforms::TreeNodeRef& parent, const std::string& name, const std::string &tag, bool binarySearch);
    std::map<std::string, mforms::TreeNodeRef> _uriList;

    std::map<ObjectType, std::string> _icon_paths;
    std::map<ObjectType, mforms::TreeNodeCollectionSkeleton> _nodeCollections;

    void preloadIconPaths();
    bec::IconId getNodeIcon(ObjectType type);
    std::string getNodeIconPath(ObjectType type);

    void createSchemas(mforms::TreeNodeRef parent, const std::list<std::string> &names);
    void createCollections(mforms::TreeNodeRef parent, const std::list<std::string> &names);
    void createTables(mforms::TreeNodeRef parent, const std::list<std::string> &names);
    void createViews(mforms::TreeNodeRef parent, const std::list<std::string> &names);
    void createFunctions(mforms::TreeNodeRef parent, const std::list<std::string> &names);
    void createEvents(mforms::TreeNodeRef parent, const std::list<std::string> &names);
    void createProcedures(mforms::TreeNodeRef parent, const std::list<std::string> &names);
    void createTableColumns(mforms::TreeNodeRef parent, const std::list<std::string> &names);
    void createViewColumns(mforms::TreeNodeRef parent, const std::list<std::string> &names);
    void createTriggers(mforms::TreeNodeRef parent, const std::list<std::string> &names);
  public:
    NgSidebar(bool enableCollections = false);
    void onDataArrived(const std::string &uri, const std::string &type, const std::vector<std::string> &path, const std::vector<std::string> &data);
    virtual ~NgSidebar();

    static const std::string SCHEMA_TAG;
    static const std::string COLLECTIONS_TAG;
    static const std::string TABLES_TAG;
    static const std::string VIEWS_TAG;
    static const std::string PROCEDURES_TAG;
    static const std::string FUNCTIONS_TAG;
    static const std::string EVENTS_TAG;
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
    static const std::string COLLECTIONS_CAPTION;
    static const std::string VIEWS_CAPTION;
    static const std::string PROCEDURES_CAPTION;
    static const std::string FUNCTIONS_CAPTION;
    static const std::string EVENTS_CAPTION;

    static const std::string COLUMNS_CAPTION;
    static const std::string INDEXES_CAPTION;
    static const std::string TRIGGERS_CAPTION;
    static const std::string FOREIGN_KEYS_CAPTION;

    int TABLES_NODE_INDEX;
    int COLLECTIONS_NODE_INDEX;
    int VIEWS_NODE_INDEX;
    int PROCEDURES_NODE_INDEX;
    int FUNCTIONS_NODE_INDEX;
    int EVENTS_NODE_INDEX;
    static const int TABLE_COLUMNS_NODE_INDEX;
    static const int TABLE_INDEXES_NODE_INDEX;
    static const int TABLE_FOREIGN_KEYS_NODE_INDEX;
    static const int TABLE_TRIGGERS_NODE_INDEX;
  };
}
