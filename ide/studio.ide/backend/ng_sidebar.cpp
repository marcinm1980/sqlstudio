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

#include "ng_sidebar.h"

#include <utility>
#include "base/log.h"

const std::string ng::NgSidebar::SCHEMA_TAG = "_SCHEMA_";
const std::string ng::NgSidebar::COLLECTIONS_TAG = "_COLLECTIONS_";
const std::string ng::NgSidebar::TABLES_TAG = "_TABLES_";
const std::string ng::NgSidebar::VIEWS_TAG = "_VIEWS_";
const std::string ng::NgSidebar::PROCEDURES_TAG = "_PROCEDURES_";
const std::string ng::NgSidebar::FUNCTIONS_TAG = "_FUNCTIONS_";
const std::string ng::NgSidebar::EVENTS_TAG = "_EVENTS_";
const std::string ng::NgSidebar::TABLE_TAG = "_TABLE_";
const std::string ng::NgSidebar::VIEW_TAG = "_VIEW_";
const std::string ng::NgSidebar::ROUTINE_TAG = "_ROUTINE_";
const std::string ng::NgSidebar::COLUMNS_TAG = "_COLUMNS_";
const std::string ng::NgSidebar::INDEXES_TAG = "_INDEXES_";
const std::string ng::NgSidebar::TRIGGERS_TAG = "_TRIGGERS_";
const std::string ng::NgSidebar::FOREIGN_KEYS_TAG = "_FOREIGN_KEYS_";
const std::string ng::NgSidebar::COLUMN_TAG = "_COLUMN_";
const std::string ng::NgSidebar::INDEX_TAG = "_INDEX_";
const std::string ng::NgSidebar::TRIGGER_TAG = "_TRIGGER_";
const std::string ng::NgSidebar::FOREIGN_KEY_TAG = "_FOREIGN_KEY_";

const std::string ng::NgSidebar::FETCHING_CAPTION = _("fetching...");
const std::string ng::NgSidebar::ERROR_FETCHING_CAPTION = _("could not be fetched");
const std::string ng::NgSidebar::TABLES_CAPTION = _("Tables");
const std::string ng::NgSidebar::COLLECTIONS_CAPTION = _("Collections");
const std::string ng::NgSidebar::VIEWS_CAPTION = _("Views");
const std::string ng::NgSidebar::PROCEDURES_CAPTION = _("Stored Procedures");
const std::string ng::NgSidebar::FUNCTIONS_CAPTION = _("Functions");
const std::string ng::NgSidebar::EVENTS_CAPTION = _("Events");

const std::string ng::NgSidebar::COLUMNS_CAPTION = _("Columns");
const std::string ng::NgSidebar::INDEXES_CAPTION = _("Indexes");
const std::string ng::NgSidebar::TRIGGERS_CAPTION = _("Triggers");
const std::string ng::NgSidebar::FOREIGN_KEYS_CAPTION = _("Foreign Keys");

const int ng::NgSidebar::TABLE_COLUMNS_NODE_INDEX = 0;
const int ng::NgSidebar::TABLE_INDEXES_NODE_INDEX = 1;
const int ng::NgSidebar::TABLE_FOREIGN_KEYS_NODE_INDEX = 2;
const int ng::NgSidebar::TABLE_TRIGGERS_NODE_INDEX = 3;

DEFAULT_LOG_DOMAIN("NgSidebar")

ng::NgSidebar::NgSidebar(bool enableCollections)
  : mforms::TreeView(mforms::TreeNoBorder | mforms::TreeCanBeDragSource | mforms::TreeIndexOnTag
  #ifndef _WIN32
    | mforms::TreeNoHeader
  #endif
    ), _hasCollections(enableCollections)
{
  if (_hasCollections)
  {
    COLLECTIONS_NODE_INDEX = 0;
    TABLES_NODE_INDEX = 1;
    VIEWS_NODE_INDEX = 2;
    PROCEDURES_NODE_INDEX = 3;
    FUNCTIONS_NODE_INDEX = 4;
    EVENTS_NODE_INDEX = 5;
  }
  else
  {
    TABLES_NODE_INDEX = 0;
    VIEWS_NODE_INDEX = 1;
    PROCEDURES_NODE_INDEX = 2;
    FUNCTIONS_NODE_INDEX = 3;
    EVENTS_NODE_INDEX = 4;
    COLLECTIONS_NODE_INDEX = 5;
  }

  add_column(mforms::IconStringColumnType, _("Sessions"), 200, false, true);
  set_selection_mode(mforms::TreeSelectMultiple);
  end_columns();
  //  _tree.signal_expand_toggle()->connect(boost::bind(&NgSheet::triggerCacheRefill, this, _1, _2));
  _menu = new mforms::ContextMenu();
  _menu->signal_will_show()->connect(boost::bind(&NgSidebar::contextMenuWillShow, this, _1));
  set_context_menu(_menu);
  preloadIconPaths();

  // Setup the schema node collection skeleton
  mforms::TreeNodeCollectionSkeleton schema_nodes(_icon_paths[Schema]);

  if (_hasCollections)
  {
    mforms::TreeNodeSkeleton collections(COLLECTIONS_CAPTION, _icon_paths[CollectionCollection], COLLECTIONS_TAG);
    schema_nodes.children.push_back(collections);
  }

  mforms::TreeNodeSkeleton tables(TABLES_CAPTION, _icon_paths[TableCollection], TABLES_TAG);
  schema_nodes.children.push_back(tables);

  mforms::TreeNodeSkeleton views(VIEWS_CAPTION, _icon_paths[ViewCollection], VIEWS_TAG);
  schema_nodes.children.push_back(views);

  if (!_hasCollections)
  {
    mforms::TreeNodeSkeleton procedures(PROCEDURES_CAPTION, _icon_paths[ProcedureCollection], PROCEDURES_TAG);
    schema_nodes.children.push_back(procedures);

    mforms::TreeNodeSkeleton functions(FUNCTIONS_CAPTION, _icon_paths[FunctionCollection], FUNCTIONS_TAG);
    schema_nodes.children.push_back(functions);

    mforms::TreeNodeSkeleton events(EVENTS_CAPTION, _icon_paths[EventCollection], EVENTS_TAG);
    schema_nodes.children.push_back(events);
  }
  _nodeCollections[Schema] = schema_nodes;

  // Setup the table node collection skeleton
  mforms::TreeNodeCollectionSkeleton table_nodes(_icon_paths[Table]);

  mforms::TreeNodeSkeleton columns(COLUMNS_CAPTION, _icon_paths[ColumnCollection], COLUMNS_TAG);
  table_nodes.children.push_back(columns);

  mforms::TreeNodeSkeleton indexes(INDEXES_CAPTION, _icon_paths[IndexCollection], INDEXES_TAG);
  table_nodes.children.push_back(indexes);

  mforms::TreeNodeSkeleton foreign_keys(FOREIGN_KEYS_CAPTION, _icon_paths[ForeignKeyCollection], FOREIGN_KEYS_TAG);
  mforms::TreeNodeSkeleton fetching_fk(FETCHING_CAPTION, _icon_paths[ForeignKey], "");
  foreign_keys.children.push_back(fetching_fk);
  table_nodes.children.push_back(foreign_keys);

  mforms::TreeNodeSkeleton triggers(TRIGGERS_CAPTION, _icon_paths[TriggerCollection], TRIGGERS_TAG);
  mforms::TreeNodeSkeleton fetching_trigger(FETCHING_CAPTION, _icon_paths[Trigger], "");
  triggers.children.push_back(fetching_trigger);
  table_nodes.children.push_back(triggers);

  _nodeCollections[Table] = table_nodes;

  // Setup the view node collection skeleton
  mforms::TreeNodeCollectionSkeleton view_nodes(_icon_paths[View]);
  mforms::TreeNodeSkeleton fetching_view(FETCHING_CAPTION, _icon_paths[View], "");
  view_nodes.children.push_back(fetching_view);
  _nodeCollections[View] = view_nodes;

  // Setup the collection node collection skeleton
  mforms::TreeNodeCollectionSkeleton collection_nodes(_icon_paths[Collection]);
  mforms::TreeNodeSkeleton fetching_collection(FETCHING_CAPTION, _icon_paths[Collection], "");
  view_nodes.children.push_back(fetching_collection);
  _nodeCollections[Collection] = collection_nodes;

  if (!_hasCollections)
  {
    mforms::TreeNodeCollectionSkeleton procedure_nodes(_icon_paths[Procedure]);
    _nodeCollections[Procedure] = procedure_nodes;

    mforms::TreeNodeCollectionSkeleton function_nodes(_icon_paths[Function]);
    _nodeCollections[Function] = function_nodes;

    mforms::TreeNodeCollectionSkeleton event_nodes(_icon_paths[Event]);
    _nodeCollections[Event] = event_nodes;
  }


  mforms::TreeNodeCollectionSkeleton table_column_nodes(_icon_paths[TableColumn]);
  _nodeCollections[TableColumn] = table_column_nodes;

  mforms::TreeNodeCollectionSkeleton view_column_nodes(_icon_paths[ViewColumn]);
  _nodeCollections[ViewColumn] = view_column_nodes;


  mforms::TreeNodeCollectionSkeleton index_nodes(_icon_paths[Index]);
  _nodeCollections[Index] = index_nodes;


  mforms::TreeNodeCollectionSkeleton trigger_nodes(_icon_paths[Trigger]);
  _nodeCollections[Trigger] = trigger_nodes;


  mforms::TreeNodeCollectionSkeleton fk_nodes(_icon_paths[ForeignKey]);
  _nodeCollections[ForeignKey] = fk_nodes;
}

ng::NgSidebar::~NgSidebar()
{

}

void ng::NgSidebar::onDataArrived(const std::string &uri, const std::string &type, const std::vector<std::string> &path, const std::vector<std::string> &data)
{
  mforms::TreeNodeRef sessionNode = node_with_tag(uri);
  if (!sessionNode.is_valid())
  {
    // Start a new session subtree.
    if (type != "schemas")
    {
      logError("We didn't have schema but, but we got it's child that shouldn't happen.\n");
      return;
    }

    std::list<std::string> schemaList(data.begin(), data.end());
    schemaList.sort(std::bind(base::stl_string_compare, std::placeholders::_1, std::placeholders::_2, false));

    sessionNode = add_node();
    sessionNode->set_string(0, uri);
    sessionNode->set_tag(uri);
    sessionNode->set_icon_path(0, _icon_paths[Session]);

    mforms::TreeNodeRef schemaNode = sessionNode->add_child();
    schemaNode->set_string(0, _("Schemas"));
    schemaNode->set_icon_path(0, _icon_paths[Folder]);

    createSchemas(schemaNode, schemaList);

    sessionNode->expand();
  }
  else
  {
    mforms::TreeNodeRef schemaNode = sessionNode->get_child(0);
    std::list<std::string> objList(data.begin(), data.end());
    objList.sort(std::bind(base::stl_string_compare, std::placeholders::_1, std::placeholders::_2, false));

    if (type == "tables")
    {
      mforms::TreeNodeRef schema = getChildNode(schemaNode, path[0], SCHEMA_TAG, true);
      createTables(schema->get_child(TABLES_NODE_INDEX), objList);
    } else if (type == "collections" && _hasCollections)
    {
      mforms::TreeNodeRef schema = getChildNode(schemaNode, path[0], SCHEMA_TAG, true);
      createCollections(schema->get_child(COLLECTIONS_NODE_INDEX), objList);
    }
    else if (type == "views")
    {
      mforms::TreeNodeRef schema = getChildNode(schemaNode, path[0], SCHEMA_TAG, true);
      createViews(schema->get_child(VIEWS_NODE_INDEX), objList);
    }
    else if (type == "procedures" && !_hasCollections)
    {
      mforms::TreeNodeRef schema = getChildNode(schemaNode, path[0], SCHEMA_TAG, true);
      createProcedures(schema->get_child(PROCEDURES_NODE_INDEX), objList);
    }
    else if (type == "functions" && !_hasCollections)
    {
      mforms::TreeNodeRef schema = getChildNode(schemaNode, path[0], SCHEMA_TAG, true);
      createFunctions(schema->get_child(FUNCTIONS_NODE_INDEX), objList);
    }
    else if (type == "events" && !_hasCollections)
    {
      mforms::TreeNodeRef schema = getChildNode(schemaNode, path[0], SCHEMA_TAG, true);
      createEvents(schema->get_child(EVENTS_NODE_INDEX), objList);
    }

    else if (type == "columns")
    {
      mforms::TreeNodeRef schema = getChildNode(schemaNode, path[0], SCHEMA_TAG, true);
      mforms::TreeNodeRef columns = getChildNode(schema->get_child(TABLES_NODE_INDEX), path[1], COLUMNS_TAG, true);
      if (!columns) // It means that those are probably view columns
      {
        columns = getChildNode(schema->get_child(VIEWS_NODE_INDEX), path[1], VIEW_TAG, true);
        if (columns) //we need to remove the first fecthing
        {
          createViewColumns(columns, objList);
        }
      }
      else
        createTableColumns(columns, objList);
    }
    else if (type == "triggers")
    {
      mforms::TreeNodeRef schema = getChildNode(schemaNode, path[0], SCHEMA_TAG, true);
      mforms::TreeNodeRef triggers = getChildNode(schema->get_child(TABLES_NODE_INDEX), path[1], TRIGGERS_TAG, true);
      if (triggers)
        createTriggers(triggers, objList);
    }

  }
}

void ng::NgSidebar::createSchemas(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  _nodeCollections[Schema].captions.clear();
  for(auto s : names)
    _nodeCollections[Schema].captions.push_back(s);
  std::vector<mforms::TreeNodeRef> schemas = parent->add_node_collection(_nodeCollections[Schema], -1);
  for(auto schema_node : schemas)
  {
    schema_node->set_tag(SCHEMA_TAG);
    if (_hasCollections)
          schema_node->get_child(COLLECTIONS_NODE_INDEX)->set_string(0, COLLECTIONS_CAPTION + " " + FETCHING_CAPTION);

    schema_node->get_child(TABLES_NODE_INDEX)->set_string(0, TABLES_CAPTION + " " + FETCHING_CAPTION);
    schema_node->get_child(VIEWS_NODE_INDEX)->set_string(0, VIEWS_CAPTION + " " + FETCHING_CAPTION);
    if (!_hasCollections)
    {
      schema_node->get_child(PROCEDURES_NODE_INDEX)->set_string(0, PROCEDURES_CAPTION + " " + FETCHING_CAPTION);
      schema_node->get_child(FUNCTIONS_NODE_INDEX)->set_string(0, FUNCTIONS_CAPTION + " " + FETCHING_CAPTION);
      schema_node->get_child(EVENTS_NODE_INDEX)->set_string(0, EVENTS_CAPTION + " " + FETCHING_CAPTION);
    }
  }
  parent->expand();
}

void ng::NgSidebar::createCollections(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  parent->set_string(0, COLLECTIONS_CAPTION);
  _nodeCollections[Collection].captions.assign(names.begin(), names.end());
  std::vector<mforms::TreeNodeRef> tables = parent->add_node_collection(_nodeCollections[Collection], -1);
}

void ng::NgSidebar::createTables(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  parent->set_string(0, TABLES_CAPTION);
  _nodeCollections[Table].captions.assign(names.begin(), names.end());
  std::vector<mforms::TreeNodeRef> tables = parent->add_node_collection(_nodeCollections[Table], -1);
  for (auto node : tables) // We can also fillup columns and triggers as those are already loaded by the cache.
    node->set_tag(TABLE_TAG);
}

void ng::NgSidebar::createViews(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  parent->set_string(0, VIEWS_CAPTION);
  _nodeCollections[View].captions.assign(names.begin(), names.end());
  std::vector<mforms::TreeNodeRef> views = parent->add_node_collection(_nodeCollections[View], -1);
  for (auto node : views)
    node->set_tag(VIEW_TAG);
}

void ng::NgSidebar::createFunctions(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  parent->set_string(0, FUNCTIONS_CAPTION);
  _nodeCollections[Function].captions.assign(names.begin(), names.end());
  std::vector<mforms::TreeNodeRef> functions = parent->add_node_collection(_nodeCollections[Function], -1);
}

void ng::NgSidebar::createEvents(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  parent->set_string(0, EVENTS_CAPTION);
  _nodeCollections[Event].captions.assign(names.begin(), names.end());
  std::vector<mforms::TreeNodeRef> events = parent->add_node_collection(_nodeCollections[Event], -1);
}

void ng::NgSidebar::createTableColumns(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  mforms::TreeNodeRef columnsContainer = parent->get_child(TABLE_COLUMNS_NODE_INDEX);
  _nodeCollections[TableColumn].captions.assign(names.begin(), names.end());
  columnsContainer->add_node_collection(_nodeCollections[TableColumn], -1);
}

void ng::NgSidebar::createViewColumns(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  parent->remove_children();
  _nodeCollections[ViewColumn].captions.assign(names.begin(), names.end());
  parent->add_node_collection(_nodeCollections[ViewColumn], -1);
}

void ng::NgSidebar::createTriggers(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  mforms::TreeNodeRef triggersContainer = parent->get_child(TABLE_TRIGGERS_NODE_INDEX);
  triggersContainer->remove_children();
  _nodeCollections[Trigger].captions.assign(names.begin(), names.end());
  triggersContainer->add_node_collection(_nodeCollections[Trigger], -1);
}

void ng::NgSidebar::createProcedures(mforms::TreeNodeRef parent, const std::list<std::string> &names)
{
  parent->set_string(0, PROCEDURES_CAPTION);
  _nodeCollections[Procedure].captions.assign(names.begin(), names.end());
  std::vector<mforms::TreeNodeRef> tables = parent->add_node_collection(_nodeCollections[Procedure], -1);
}

void ng::NgSidebar::contextMenuWillShow(mforms::MenuItem *parent_item)
{

}

mforms::TreeNodeRef ng::NgSidebar::binarySearchNode(const mforms::TreeNodeRef& parent, int min, int max, const std::string &name, int &position)
{
  if (max < min)
    return mforms::TreeNodeRef();
  else
  {
    int middle = (max+min) / 2;
    position = middle;

    mforms::TreeNodeRef node = parent->get_child(middle);

    int comparison = base::string_compare(node->get_string(0), name, false);

    if (comparison < 0)
      return binarySearchNode(parent, middle + 1, max, name, ++position);
    else if (comparison > 0)
      return binarySearchNode(parent, min, middle - 1, name, position);
    else
      return node;
  }
}

/* Function : get_child_node
 * Description : Searches a specific child on a given node, supports both sequential search
 *               and binary search. Binary search should be used when searching for schemas,
 *               tables, views and routines which are sorted. Sequential search is there for
 *               the non sorted nodes.
 */
mforms::TreeNodeRef ng::NgSidebar::getChildNode(const mforms::TreeNodeRef& parent, const std::string& name, const std::string &tag, bool binarySearch)
{
  int last_position = 0;
  bool found = false;
  mforms::TreeNodeRef child;

  if (binarySearch)
  {
    if (parent && parent->count())
      child = binarySearchNode(parent, 0, parent->count()-1, name, last_position);

    if (child)
      found = true;
  }
  else
  {
    if (parent && parent->count())
    {
      for(int index=0; !found && index < parent->count(); index++)
      {
        child = parent->get_child(index);

        found = (base::string_compare(child->get_string(0), name, false) == 0);

        if (found && !tag.empty())
          found = (child->get_tag() == tag);
      }
    }
  }

  return found ? child : mforms::TreeNodeRef();
}



void ng::NgSidebar::preloadIconPaths()
{
  _icon_paths[Session] = getNodeIconPath(Session);
  _icon_paths[Folder] = getNodeIconPath(Folder);
  _icon_paths[Schema] = getNodeIconPath(Schema);
  _icon_paths[CollectionCollection] = getNodeIconPath(CollectionCollection);
  _icon_paths[TableCollection] = getNodeIconPath(TableCollection);
  _icon_paths[ViewCollection] = getNodeIconPath(ViewCollection);
  _icon_paths[ProcedureCollection] = getNodeIconPath(ProcedureCollection);
  _icon_paths[FunctionCollection] = getNodeIconPath(FunctionCollection);
  _icon_paths[EventCollection] = getNodeIconPath(EventCollection);
  _icon_paths[Table] = getNodeIconPath(Table);
  _icon_paths[View] = getNodeIconPath(View);
  _icon_paths[Procedure] = getNodeIconPath(Procedure);
  _icon_paths[Function] = getNodeIconPath(Function);
  _icon_paths[Event] = getNodeIconPath(Event);
  _icon_paths[ColumnCollection] = getNodeIconPath(ColumnCollection);
  _icon_paths[IndexCollection] = getNodeIconPath(IndexCollection);
  _icon_paths[ForeignKeyCollection] = getNodeIconPath(ForeignKeyCollection);
  _icon_paths[TriggerCollection] = getNodeIconPath(TriggerCollection);
  _icon_paths[ViewColumn] = getNodeIconPath(ViewColumn);
  _icon_paths[TableColumn] = getNodeIconPath(TableColumn);
  _icon_paths[Index] = getNodeIconPath(Index);
  _icon_paths[ForeignKey] = getNodeIconPath(ForeignKey);
  _icon_paths[Trigger] = getNodeIconPath(Trigger);

}

std::string ng::NgSidebar::getNodeIconPath(ObjectType type)
{
  bec::IconId icon = getNodeIcon(type);
  return bec::IconManager::get_instance()->get_icon_file(icon);
}

bec::IconId ng::NgSidebar::getNodeIcon(ObjectType type)
{
  bec::IconId icon;

  switch(type)
  {
  case Session:
    icon = bec::IconManager::get_instance()->get_icon_id("session.png", bec::Icon16);
    break;
  case Folder:
    icon = bec::IconManager::get_instance()->get_icon_id("folder.png", bec::Icon16);
    break;
  case Schema:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Schema.unloaded.side.$.png", bec::Icon16);
    break;
  case CollectionCollection: //TODO add collectioncollection icon
  case TableCollection:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Table.many.side.$.png", bec::Icon16);
    break;
  case ViewCollection:
    icon = bec::IconManager::get_instance()->get_icon_id("db.View.many.side.$.png", bec::Icon16);
    break;
  // TODO: Update the Procedure and Function collection icons to the correct value
  case ProcedureCollection:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Routine.many.side.$.png", bec::Icon16);
    break;
  case FunctionCollection:
  case EventCollection: //TODO add EventCollection icon
    icon = bec::IconManager::get_instance()->get_icon_id("db.Routine.many.side.$.png", bec::Icon16);
    break;
  case Table:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Table.side.$.png", bec::Icon16);
    break;
  case View:
    icon = bec::IconManager::get_instance()->get_icon_id("db.View.side.$.png", bec::Icon16);
    break;
  case Procedure:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Routine.side.$.png", bec::Icon16);
    break;
  case Function:
  case Event: //Todo: add event icon
    icon = bec::IconManager::get_instance()->get_icon_id("grt_function.png", bec::Icon16);
    break;
  case ColumnCollection:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Column.many.side.$.png", bec::Icon16);
    break;
  case IndexCollection:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Index.many.side.$.png", bec::Icon16);
    break;
  case ForeignKeyCollection:
    icon = bec::IconManager::get_instance()->get_icon_id("db.ForeignKey.many.side.$.png", bec::Icon16);
    break;
  case TriggerCollection:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Trigger.many.side.$.png", bec::Icon16);
    break;
  case ViewColumn:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Column.side.$.png", bec::Icon16);
    break;
  case TableColumn:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Column.side.$.png", bec::Icon16);
    break;
  case Index:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Index.side.$.png", bec::Icon16);
    break;
  case ForeignKey:
    icon = bec::IconManager::get_instance()->get_icon_id("db.ForeignKey.side.$.png", bec::Icon16);
    break;
  case Trigger:
    icon = bec::IconManager::get_instance()->get_icon_id("db.Trigger.side.$.png", bec::Icon16);
    break;
  default:
    icon = -1;
    break;
  }

  return icon;
}
