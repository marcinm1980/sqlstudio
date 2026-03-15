/*
 * Copyright (c) 2011, 2019, Oracle and/or its affiliates. All rights reserved.
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
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#pragma once

#ifndef _MSC_VER
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

#include "grt.h"

#ifdef _MSC_VER
  #pragma warning(disable: 4355) // 'this' : used in base member initializer list
  #ifdef GRT_STRUCTS_MYSQLSTUDIO_EXPORT
  #define GRT_STRUCTS_MYSQLSTUDIO_PUBLIC __declspec(dllexport)
#else
  #define GRT_STRUCTS_MYSQLSTUDIO_PUBLIC __declspec(dllimport)
#endif
#else
  #define GRT_STRUCTS_MYSQLSTUDIO_PUBLIC
#endif

#include "grts/structs.h"
#include "grts/structs.app.h"
#include "grts/structs.db.h"
#include "grts/structs.db.query.h"
#include "grts/structs.db.mgmt.h"
#include "grts/structs.studio.physical.h"
#include "grts/structs.studio.logical.h"
#include "grts/structs.db.migration.h"

class studio_OverviewPanel;
typedef grt::Ref<studio_OverviewPanel> studio_OverviewPanelRef;
class studio_Document;
typedef grt::Ref<studio_Document> studio_DocumentRef;
class studio_MySqlStudio;
typedef grt::Ref<studio_MySqlStudio> studio_MySqlStudioRef;

namespace mforms {
  class Object;
};

namespace grt {
  class AutoPyObject;
};

class studio_OverviewPanel : public GrtObject {
  typedef GrtObject super;

public:
  studio_OverviewPanel(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _caption(""),
      _expanded(0),
      _expandedHeight(0),
      _hasTabSelection(0),
      _implModule(""),
      _itemActivationFunction(""),
      _itemCountFunction(""),
      _itemDisplayMode(0),
      _itemInfoFunction(""),
      _nodeId(""),
      _selectedItems(this, false),
      _tabActivationFunction(""),
      _tabCountFunction(""),
      _tabInfoFunction("") {
  }

  static auto static_class_name() -> std::string {
    return "studio.OverviewPanel";
  }

  /**
   * Getter for attribute caption
   *
   * the caption displayed on the panel's header
   * \par In Python:
   *    value = obj.caption
   */
  auto caption() const -> grt::StringRef {
    return _caption;
  }

  /**
   * Setter for attribute caption
   *
   * the caption displayed on the panel's header
   * \par In Python:
   *   obj.caption = value
   */
  virtual auto caption(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_caption);
    _caption = value;
    member_changed("caption", ovalue, value);
  }

  /**
   * Getter for attribute expanded
   *
   * specifies if the panel is currently expanded
   * \par In Python:
   *    value = obj.expanded
   */
  auto expanded() const -> grt::IntegerRef {
    return _expanded;
  }

  /**
   * Setter for attribute expanded
   *
   * specifies if the panel is currently expanded
   * \par In Python:
   *   obj.expanded = value
   */
  virtual auto expanded(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_expanded);
    _expanded = value;
    member_changed("expanded", ovalue, value);
  }

  /**
   * Getter for attribute expandedHeight
   *
   * the panel's height when it is expanded
   * \par In Python:
   *    value = obj.expandedHeight
   */
  auto expandedHeight() const -> grt::IntegerRef {
    return _expandedHeight;
  }

  /**
   * Setter for attribute expandedHeight
   *
   * the panel's height when it is expanded
   * \par In Python:
   *   obj.expandedHeight = value
   */
  virtual auto expandedHeight(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_expandedHeight);
    _expandedHeight = value;
    member_changed("expandedHeight", ovalue, value);
  }

  /**
   * Getter for attribute hasTabSelection
   *
   * specifies if the panel has attached tabs
   * \par In Python:
   *    value = obj.hasTabSelection
   */
  auto hasTabSelection() const -> grt::IntegerRef {
    return _hasTabSelection;
  }

  /**
   * Setter for attribute hasTabSelection
   *
   * specifies if the panel has attached tabs
   * \par In Python:
   *   obj.hasTabSelection = value
   */
  virtual auto hasTabSelection(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_hasTabSelection);
    _hasTabSelection = value;
    member_changed("hasTabSelection", ovalue, value);
  }

  /**
   * Getter for attribute implModule
   *
   * GRT module implementing the item functionality
   * \par In Python:
   *    value = obj.implModule
   */
  auto implModule() const -> grt::StringRef {
    return _implModule;
  }

  /**
   * Setter for attribute implModule
   *
   * GRT module implementing the item functionality
   * \par In Python:
   *   obj.implModule = value
   */
  virtual auto implModule(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_implModule);
    _implModule = value;
    member_changed("implModule", ovalue, value);
  }

  /**
   * Getter for attribute itemActivationFunction
   *
   * the function that is called when the item is activated
   * \par In Python:
   *    value = obj.itemActivationFunction
   */
  auto itemActivationFunction() const -> grt::StringRef {
    return _itemActivationFunction;
  }

  /**
   * Setter for attribute itemActivationFunction
   *
   * the function that is called when the item is activated
   * \par In Python:
   *   obj.itemActivationFunction = value
   */
  virtual auto itemActivationFunction(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_itemActivationFunction);
    _itemActivationFunction = value;
    member_changed("itemActivationFunction", ovalue, value);
  }

  /**
   * Getter for attribute itemCountFunction
   *
   * the function that returns the number of items
   * \par In Python:
   *    value = obj.itemCountFunction
   */
  auto itemCountFunction() const -> grt::StringRef {
    return _itemCountFunction;
  }

  /**
   * Setter for attribute itemCountFunction
   *
   * the function that returns the number of items
   * \par In Python:
   *   obj.itemCountFunction = value
   */
  virtual auto itemCountFunction(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_itemCountFunction);
    _itemCountFunction = value;
    member_changed("itemCountFunction", ovalue, value);
  }

  /**
   * Getter for attribute itemDisplayMode
   *
   * defines how the items on the panel are displayed. Setting it to 1 will cause a large icon display, 2 a small icon
   * display and 3 a list
   * \par In Python:
   *    value = obj.itemDisplayMode
   */
  auto itemDisplayMode() const -> grt::IntegerRef {
    return _itemDisplayMode;
  }

  /**
   * Setter for attribute itemDisplayMode
   *
   * defines how the items on the panel are displayed. Setting it to 1 will cause a large icon display, 2 a small icon
   * display and 3 a list
   * \par In Python:
   *   obj.itemDisplayMode = value
   */
  virtual auto itemDisplayMode(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_itemDisplayMode);
    _itemDisplayMode = value;
    member_changed("itemDisplayMode", ovalue, value);
  }

  /**
   * Getter for attribute itemInfoFunction
   *
   * the function that returns the item information
   * \par In Python:
   *    value = obj.itemInfoFunction
   */
  auto itemInfoFunction() const -> grt::StringRef {
    return _itemInfoFunction;
  }

  /**
   * Setter for attribute itemInfoFunction
   *
   * the function that returns the item information
   * \par In Python:
   *   obj.itemInfoFunction = value
   */
  virtual auto itemInfoFunction(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_itemInfoFunction);
    _itemInfoFunction = value;
    member_changed("itemInfoFunction", ovalue, value);
  }

  /**
   * Getter for attribute nodeId
   *
   * the identifier of panel in the GUI
   * \par In Python:
   *    value = obj.nodeId
   */
  auto nodeId() const -> grt::StringRef {
    return _nodeId;
  }

  /**
   * Setter for attribute nodeId
   *
   * the identifier of panel in the GUI
   * \par In Python:
   *   obj.nodeId = value
   */
  virtual auto nodeId(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_nodeId);
    _nodeId = value;
    member_changed("nodeId", ovalue, value);
  }

  /**
   * Getter for attribute selectedItems (read-only)
   *
   * specifies the indexes of the currently selected items on the panel
   * \par In Python:
   *    value = obj.selectedItems
   */
  auto selectedItems() const -> grt::IntegerListRef {
    return _selectedItems;
  }

private: // The next attribute is read-only.
  virtual auto selectedItems(const grt::IntegerListRef &value) -> void {
    grt::ValueRef ovalue(_selectedItems);
    _selectedItems = value;
    member_changed("selectedItems", ovalue, value);
  }

public:
  /**
   * Getter for attribute tabActivationFunction
   *
   * the function that is called when the tab is activated
   * \par In Python:
   *    value = obj.tabActivationFunction
   */
  auto tabActivationFunction() const -> grt::StringRef {
    return _tabActivationFunction;
  }

  /**
   * Setter for attribute tabActivationFunction
   *
   * the function that is called when the tab is activated
   * \par In Python:
   *   obj.tabActivationFunction = value
   */
  virtual auto tabActivationFunction(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_tabActivationFunction);
    _tabActivationFunction = value;
    member_changed("tabActivationFunction", ovalue, value);
  }

  /**
   * Getter for attribute tabCountFunction
   *
   * the function that returns the number of tabs
   * \par In Python:
   *    value = obj.tabCountFunction
   */
  auto tabCountFunction() const -> grt::StringRef {
    return _tabCountFunction;
  }

  /**
   * Setter for attribute tabCountFunction
   *
   * the function that returns the number of tabs
   * \par In Python:
   *   obj.tabCountFunction = value
   */
  virtual auto tabCountFunction(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_tabCountFunction);
    _tabCountFunction = value;
    member_changed("tabCountFunction", ovalue, value);
  }

  /**
   * Getter for attribute tabInfoFunction
   *
   * the function that returns the tab information
   * \par In Python:
   *    value = obj.tabInfoFunction
   */
  auto tabInfoFunction() const -> grt::StringRef {
    return _tabInfoFunction;
  }

  /**
   * Setter for attribute tabInfoFunction
   *
   * the function that returns the tab information
   * \par In Python:
   *   obj.tabInfoFunction = value
   */
  virtual auto tabInfoFunction(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_tabInfoFunction);
    _tabInfoFunction = value;
    member_changed("tabInfoFunction", ovalue, value);
  }

protected:
  grt::StringRef _caption;
  grt::IntegerRef _expanded;
  grt::IntegerRef _expandedHeight;
  grt::IntegerRef _hasTabSelection;
  grt::StringRef _implModule;
  grt::StringRef _itemActivationFunction;
  grt::StringRef _itemCountFunction;
  grt::IntegerRef _itemDisplayMode;
  grt::StringRef _itemInfoFunction;
  grt::StringRef _nodeId;
  grt::IntegerListRef _selectedItems;
  grt::StringRef _tabActivationFunction;
  grt::StringRef _tabCountFunction;
  grt::StringRef _tabInfoFunction;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_OverviewPanel());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_OverviewPanel::create);
    {
      void (studio_OverviewPanel::*setter)(const grt::StringRef &) = &studio_OverviewPanel::caption;
      grt::StringRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::caption;
      meta->bind_member("caption",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::StringRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::IntegerRef &) = &studio_OverviewPanel::expanded;
      grt::IntegerRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::expanded;
      meta->bind_member("expanded",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::IntegerRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::IntegerRef &) = &studio_OverviewPanel::expandedHeight;
      grt::IntegerRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::expandedHeight;
      meta->bind_member("expandedHeight",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::IntegerRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::IntegerRef &) = &studio_OverviewPanel::hasTabSelection;
      grt::IntegerRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::hasTabSelection;
      meta->bind_member("hasTabSelection",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::IntegerRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::StringRef &) = &studio_OverviewPanel::implModule;
      grt::StringRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::implModule;
      meta->bind_member("implModule",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::StringRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::StringRef &) =
        &studio_OverviewPanel::itemActivationFunction;
      grt::StringRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::itemActivationFunction;
      meta->bind_member("itemActivationFunction",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::StringRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::StringRef &) = &studio_OverviewPanel::itemCountFunction;
      grt::StringRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::itemCountFunction;
      meta->bind_member("itemCountFunction",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::StringRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::IntegerRef &) = &studio_OverviewPanel::itemDisplayMode;
      grt::IntegerRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::itemDisplayMode;
      meta->bind_member("itemDisplayMode",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::IntegerRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::StringRef &) = &studio_OverviewPanel::itemInfoFunction;
      grt::StringRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::itemInfoFunction;
      meta->bind_member("itemInfoFunction",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::StringRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::StringRef &) = &studio_OverviewPanel::nodeId;
      grt::StringRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::nodeId;
      meta->bind_member("nodeId",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::StringRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::IntegerListRef &) = &studio_OverviewPanel::selectedItems;
      grt::IntegerListRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::selectedItems;
      meta->bind_member("selectedItems",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::IntegerListRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::StringRef &) = &studio_OverviewPanel::tabActivationFunction;
      grt::StringRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::tabActivationFunction;
      meta->bind_member("tabActivationFunction",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::StringRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::StringRef &) = &studio_OverviewPanel::tabCountFunction;
      grt::StringRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::tabCountFunction;
      meta->bind_member("tabCountFunction",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::StringRef>(getter, setter));
    }
    {
      void (studio_OverviewPanel::*setter)(const grt::StringRef &) = &studio_OverviewPanel::tabInfoFunction;
      grt::StringRef (studio_OverviewPanel::*getter)() const = &studio_OverviewPanel::tabInfoFunction;
      meta->bind_member("tabInfoFunction",
                        new grt::MetaClass::Property<studio_OverviewPanel, grt::StringRef>(getter, setter));
    }
  }
};

class studio_Document : public app_Document {
  typedef app_Document super;

public:
  studio_Document(grt::MetaClass *meta = nullptr)
    : app_Document(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _overviewPanels(this, false),
      _physicalModels(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "studio.Document";
  }

  // logicalModel is owned by studio_Document
  /**
   * Getter for attribute logicalModel
   *
   * the logical model
   * \par In Python:
   *    value = obj.logicalModel
   */
  auto logicalModel() const -> studio_logical_ModelRef {
    return _logicalModel;
  }

  /**
   * Setter for attribute logicalModel
   *
   * the logical model
   * \par In Python:
   *   obj.logicalModel = value
   */
  virtual auto logicalModel(const studio_logical_ModelRef &value) -> void {
    grt::ValueRef ovalue(_logicalModel);

    _logicalModel = value;
    owned_member_changed("logicalModel", ovalue, value);
  }

  /**
   * Getter for attribute overviewCurrentModelType
   *
   * specifies if the panel is currently selected
   * \par In Python:
   *    value = obj.overviewCurrentModelType
   */
  auto overviewCurrentModelType() const -> model_ModelRef {
    return _overviewCurrentModelType;
  }

  /**
   * Setter for attribute overviewCurrentModelType
   *
   * specifies if the panel is currently selected
   * \par In Python:
   *   obj.overviewCurrentModelType = value
   */
  virtual auto overviewCurrentModelType(const model_ModelRef &value) -> void {
    grt::ValueRef ovalue(_overviewCurrentModelType);
    _overviewCurrentModelType = value;
    member_changed("overviewCurrentModelType", ovalue, value);
  }

  // overviewPanels is owned by studio_Document
  /**
   * Getter for attribute overviewPanels (read-only)
   *
   * the panels that are presented on the overview page
   * \par In Python:
   *    value = obj.overviewPanels
   */
  auto overviewPanels() const -> grt::ListRef<studio_OverviewPanel> {
    return _overviewPanels;
  }

private: // The next attribute is read-only.
  virtual auto overviewPanels(const grt::ListRef<studio_OverviewPanel> &value) -> void {
    grt::ValueRef ovalue(_overviewPanels);

    _overviewPanels = value;
    owned_member_changed("overviewPanels", ovalue, value);
  }

public:
  // physicalModels is owned by studio_Document
  /**
   * Getter for attribute physicalModels (read-only)
   *
   * the physical models
   * \par In Python:
   *    value = obj.physicalModels
   */
  auto physicalModels() const -> grt::ListRef<studio_physical_Model> {
    return _physicalModels;
  }

private: // The next attribute is read-only.
  virtual auto physicalModels(const grt::ListRef<studio_physical_Model> &value) -> void {
    grt::ValueRef ovalue(_physicalModels);

    _physicalModels = value;
    owned_member_changed("physicalModels", ovalue, value);
  }

public:
protected:
  studio_logical_ModelRef _logicalModel; // owned
  model_ModelRef _overviewCurrentModelType;
  grt::ListRef<studio_OverviewPanel> _overviewPanels;  // owned
  grt::ListRef<studio_physical_Model> _physicalModels; // owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_Document());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_Document::create);
    {
      void (studio_Document::*setter)(const studio_logical_ModelRef &) = &studio_Document::logicalModel;
      studio_logical_ModelRef (studio_Document::*getter)() const = &studio_Document::logicalModel;
      meta->bind_member("logicalModel",
                        new grt::MetaClass::Property<studio_Document, studio_logical_ModelRef>(getter, setter));
    }
    {
      void (studio_Document::*setter)(const model_ModelRef &) = &studio_Document::overviewCurrentModelType;
      model_ModelRef (studio_Document::*getter)() const = &studio_Document::overviewCurrentModelType;
      meta->bind_member("overviewCurrentModelType",
                        new grt::MetaClass::Property<studio_Document, model_ModelRef>(getter, setter));
    }
    {
      void (studio_Document::*setter)(const grt::ListRef<studio_OverviewPanel> &) =
        &studio_Document::overviewPanels;
      grt::ListRef<studio_OverviewPanel> (studio_Document::*getter)() const = &studio_Document::overviewPanels;
      meta->bind_member(
        "overviewPanels",
        new grt::MetaClass::Property<studio_Document, grt::ListRef<studio_OverviewPanel>>(getter, setter));
    }
    {
      void (studio_Document::*setter)(const grt::ListRef<studio_physical_Model> &) =
        &studio_Document::physicalModels;
      grt::ListRef<studio_physical_Model> (studio_Document::*getter)() const =
        &studio_Document::physicalModels;
      meta->bind_member(
        "physicalModels",
        new grt::MetaClass::Property<studio_Document, grt::ListRef<studio_physical_Model>>(getter, setter));
    }
  }
};

/** an object to store the studio's data */
class studio_MySqlStudio : public app_Application {
  typedef app_Application super;

public:
  studio_MySqlStudio(grt::MetaClass *meta = nullptr)
    : app_Application(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _docPath(""),
      _sqlEditors(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "studio.MySqlStudio";
  }

  // doc is owned by studio_MySqlStudio
  /**
   * Getter for attribute doc
   *
   * the MySqlStudio document
   * \par In Python:
   *    value = obj.doc
   */
  auto doc() const -> studio_DocumentRef {
    return studio_DocumentRef::cast_from(_doc);
  }

  /**
   * Setter for attribute doc
   *
   * the MySqlStudio document
   * \par In Python:
   *   obj.doc = value
   */
  virtual auto doc(const studio_DocumentRef &value) -> void {
    super::doc(value);
  }

  /**
   * Getter for attribute docPath
   *
   * the MySqlStudio document path
   * \par In Python:
   *    value = obj.docPath
   */
  auto docPath() const -> grt::StringRef {
    return _docPath;
  }

  /**
   * Setter for attribute docPath
   *
   * the MySqlStudio document path
   * \par In Python:
   *   obj.docPath = value
   */
  virtual auto docPath(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_docPath);
    _docPath = value;
    member_changed("docPath", ovalue, value);
  }

  // migration is owned by studio_MySqlStudio
  /**
   * Getter for attribute migration
   *
   * data for Migration Plugin
   * \par In Python:
   *    value = obj.migration
   */
  auto migration() const -> db_migration_MigrationRef {
    return _migration;
  }

  /**
   * Setter for attribute migration
   *
   * data for Migration Plugin
   * \par In Python:
   *   obj.migration = value
   */
  virtual auto migration(const db_migration_MigrationRef &value) -> void {
    grt::ValueRef ovalue(_migration);

    _migration = value;
    owned_member_changed("migration", ovalue, value);
  }

  // rdbmsMgmt is owned by studio_MySqlStudio
  /**
   * Getter for attribute rdbmsMgmt
   *
   * the RDBMS management information
   * \par In Python:
   *    value = obj.rdbmsMgmt
   */
  auto rdbmsMgmt() const -> db_mgmt_ManagementRef {
    return _rdbmsMgmt;
  }

  /**
   * Setter for attribute rdbmsMgmt
   *
   * the RDBMS management information
   * \par In Python:
   *   obj.rdbmsMgmt = value
   */
  virtual auto rdbmsMgmt(const db_mgmt_ManagementRef &value) -> void {
    grt::ValueRef ovalue(_rdbmsMgmt);

    _rdbmsMgmt = value;
    owned_member_changed("rdbmsMgmt", ovalue, value);
  }

  // sqlEditors is owned by studio_MySqlStudio
  /**
   * Getter for attribute sqlEditors (read-only)
   *
   * list of open SQL Editor instances
   * \par In Python:
   *    value = obj.sqlEditors
   */
  auto sqlEditors() const -> grt::ListRef<db_query_Editor> {
    return _sqlEditors;
  }

private: // The next attribute is read-only.
  virtual auto sqlEditors(const grt::ListRef<db_query_Editor> &value) -> void {
    grt::ValueRef ovalue(_sqlEditors);

    _sqlEditors = value;
    owned_member_changed("sqlEditors", ovalue, value);
  }

public:
protected:
  grt::StringRef _docPath;
  db_migration_MigrationRef _migration;      // owned
  db_mgmt_ManagementRef _rdbmsMgmt;          // owned
  grt::ListRef<db_query_Editor> _sqlEditors; // owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_MySqlStudio());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_MySqlStudio::create);
    {
      void (studio_MySqlStudio::*setter)(const studio_DocumentRef &) = 0;
      studio_DocumentRef (studio_MySqlStudio::*getter)() const = 0;
      meta->bind_member("doc",
                        new grt::MetaClass::Property<studio_MySqlStudio, studio_DocumentRef>(getter, setter));
    }
    {
      void (studio_MySqlStudio::*setter)(const grt::StringRef &) = &studio_MySqlStudio::docPath;
      grt::StringRef (studio_MySqlStudio::*getter)() const = &studio_MySqlStudio::docPath;
      meta->bind_member("docPath", new grt::MetaClass::Property<studio_MySqlStudio, grt::StringRef>(getter, setter));
    }
    {
      void (studio_MySqlStudio::*setter)(const db_migration_MigrationRef &) = &studio_MySqlStudio::migration;
      db_migration_MigrationRef (studio_MySqlStudio::*getter)() const = &studio_MySqlStudio::migration;
      meta->bind_member("migration",
                        new grt::MetaClass::Property<studio_MySqlStudio, db_migration_MigrationRef>(getter, setter));
    }
    {
      void (studio_MySqlStudio::*setter)(const db_mgmt_ManagementRef &) = &studio_MySqlStudio::rdbmsMgmt;
      db_mgmt_ManagementRef (studio_MySqlStudio::*getter)() const = &studio_MySqlStudio::rdbmsMgmt;
      meta->bind_member("rdbmsMgmt",
                        new grt::MetaClass::Property<studio_MySqlStudio, db_mgmt_ManagementRef>(getter, setter));
    }
    {
      void (studio_MySqlStudio::*setter)(const grt::ListRef<db_query_Editor> &) = &studio_MySqlStudio::sqlEditors;
      grt::ListRef<db_query_Editor> (studio_MySqlStudio::*getter)() const = &studio_MySqlStudio::sqlEditors;
      meta->bind_member(
        "sqlEditors",
        new grt::MetaClass::Property<studio_MySqlStudio, grt::ListRef<db_query_Editor>>(getter, setter));
    }
  }
};

inline auto register_structs_studio_xml() -> void {
  grt::internal::ClassRegistry::register_class<studio_OverviewPanel>();
  grt::internal::ClassRegistry::register_class<studio_Document>();
  grt::internal::ClassRegistry::register_class<studio_MySqlStudio>();
}

#ifdef AUTO_REGISTER_GRT_CLASSES
static struct _autoreg__structs_studio_xml {
  _autoreg__structs_studio_xml() {
    register_structs_studio_xml();
  }
} __autoreg__structs_studio_xml;
#endif

#ifndef _MSC_VER
  #pragma GCC diagnostic pop
#endif

