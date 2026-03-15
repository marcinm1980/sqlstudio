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
  #ifdef GRT_STRUCTS_APP_EXPORT
  #define GRT_STRUCTS_APP_PUBLIC __declspec(dllexport)
#else
  #define GRT_STRUCTS_APP_PUBLIC __declspec(dllimport)
#endif
#else
  #define GRT_STRUCTS_APP_PUBLIC
#endif

#include "grts/structs.h"

class app_PluginInputDefinition;
typedef grt::Ref<app_PluginInputDefinition> app_PluginInputDefinitionRef;
class app_PluginObjectInput;
typedef grt::Ref<app_PluginObjectInput> app_PluginObjectInputRef;
class app_PluginFileInput;
typedef grt::Ref<app_PluginFileInput> app_PluginFileInputRef;
class app_PluginSelectionInput;
typedef grt::Ref<app_PluginSelectionInput> app_PluginSelectionInputRef;
class app_Plugin;
typedef grt::Ref<app_Plugin> app_PluginRef;
class app_DocumentPlugin;
typedef grt::Ref<app_DocumentPlugin> app_DocumentPluginRef;
class app_PluginGroup;
typedef grt::Ref<app_PluginGroup> app_PluginGroupRef;
class app_Toolbar;
typedef grt::Ref<app_Toolbar> app_ToolbarRef;
class app_CommandItem;
typedef grt::Ref<app_CommandItem> app_CommandItemRef;
class app_ToolbarItem;
typedef grt::Ref<app_ToolbarItem> app_ToolbarItemRef;
class app_ShortcutItem;
typedef grt::Ref<app_ShortcutItem> app_ShortcutItemRef;
class app_MenuItem;
typedef grt::Ref<app_MenuItem> app_MenuItemRef;
class app_CustomDataField;
typedef grt::Ref<app_CustomDataField> app_CustomDataFieldRef;
class app_PageSettings;
typedef grt::Ref<app_PageSettings> app_PageSettingsRef;
class app_PaperType;
typedef grt::Ref<app_PaperType> app_PaperTypeRef;
class app_Registry;
typedef grt::Ref<app_Registry> app_RegistryRef;
class app_Starter;
typedef grt::Ref<app_Starter> app_StarterRef;
class app_Starters;
typedef grt::Ref<app_Starters> app_StartersRef;
class app_Options;
typedef grt::Ref<app_Options> app_OptionsRef;
class app_DocumentInfo;
typedef grt::Ref<app_DocumentInfo> app_DocumentInfoRef;
class app_Info;
typedef grt::Ref<app_Info> app_InfoRef;
class app_Document;
typedef grt::Ref<app_Document> app_DocumentRef;
class app_Application;
typedef grt::Ref<app_Application> app_ApplicationRef;


namespace mforms { 
  class Object;
}; 

namespace grt { 
  class AutoPyObject;
}; 

class  app_PluginInputDefinition : public GrtObject {
  typedef GrtObject super;

public:
  app_PluginInputDefinition(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())) {
  }

  static auto static_class_name() -> std::string {
    return "app.PluginInputDefinition";
  }

protected:


private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_PluginInputDefinition());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_PluginInputDefinition::create);
  }
};

class  app_PluginObjectInput : public app_PluginInputDefinition {
  typedef app_PluginInputDefinition super;

public:
  app_PluginObjectInput(grt::MetaClass *meta = nullptr)
    : app_PluginInputDefinition(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _objectStructName("") {
  }

  static auto static_class_name() -> std::string {
    return "app.PluginObjectInput";
  }

  /**
   * Getter for attribute objectStructName
   *
   * 
   * \par In Python:
   *    value = obj.objectStructName
   */
  auto objectStructName() const -> grt::StringRef { return _objectStructName; }

  /**
   * Setter for attribute objectStructName
   * 
   * 
   * \par In Python:
   *   obj.objectStructName = value
   */
  virtual auto objectStructName(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_objectStructName);
    _objectStructName = value;
    member_changed("objectStructName", ovalue, value);
  }

protected:

  grt::StringRef _objectStructName;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_PluginObjectInput());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_PluginObjectInput::create);
    {
      void (app_PluginObjectInput::*setter)(const grt::StringRef &) = &app_PluginObjectInput::objectStructName;
      grt::StringRef (app_PluginObjectInput::*getter)() const = &app_PluginObjectInput::objectStructName;
      meta->bind_member("objectStructName", new grt::MetaClass::Property<app_PluginObjectInput,grt::StringRef>(getter, setter));
    }
  }
};

class  app_PluginFileInput : public app_PluginInputDefinition {
  typedef app_PluginInputDefinition super;

public:
  app_PluginFileInput(grt::MetaClass *meta = nullptr)
    : app_PluginInputDefinition(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _dialogTitle(""),
      _dialogType(""),
      _fileExtensions("") {
  }

  static auto static_class_name() -> std::string {
    return "app.PluginFileInput";
  }

  /**
   * Getter for attribute dialogTitle
   *
   * Title to use in file dialog when requesting a file to the user
   * \par In Python:
   *    value = obj.dialogTitle
   */
  auto dialogTitle() const -> grt::StringRef { return _dialogTitle; }

  /**
   * Setter for attribute dialogTitle
   * 
   * Title to use in file dialog when requesting a file to the user
   * \par In Python:
   *   obj.dialogTitle = value
   */
  virtual auto dialogTitle(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_dialogTitle);
    _dialogTitle = value;
    member_changed("dialogTitle", ovalue, value);
  }

  /**
   * Getter for attribute dialogType
   *
   * Type of file dialog (save, open)
   * \par In Python:
   *    value = obj.dialogType
   */
  auto dialogType() const -> grt::StringRef { return _dialogType; }

  /**
   * Setter for attribute dialogType
   * 
   * Type of file dialog (save, open)
   * \par In Python:
   *   obj.dialogType = value
   */
  virtual auto dialogType(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_dialogType);
    _dialogType = value;
    member_changed("dialogType", ovalue, value);
  }

  /**
   * Getter for attribute fileExtensions
   *
   * Accepted file extensions, starting with the default one  (without the .)
   * \par In Python:
   *    value = obj.fileExtensions
   */
  auto fileExtensions() const -> grt::StringRef { return _fileExtensions; }

  /**
   * Setter for attribute fileExtensions
   * 
   * Accepted file extensions, starting with the default one  (without the .)
   * \par In Python:
   *   obj.fileExtensions = value
   */
  virtual auto fileExtensions(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_fileExtensions);
    _fileExtensions = value;
    member_changed("fileExtensions", ovalue, value);
  }

protected:

  grt::StringRef _dialogTitle;
  grt::StringRef _dialogType;
  grt::StringRef _fileExtensions;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_PluginFileInput());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_PluginFileInput::create);
    {
      void (app_PluginFileInput::*setter)(const grt::StringRef &) = &app_PluginFileInput::dialogTitle;
      grt::StringRef (app_PluginFileInput::*getter)() const = &app_PluginFileInput::dialogTitle;
      meta->bind_member("dialogTitle", new grt::MetaClass::Property<app_PluginFileInput,grt::StringRef>(getter, setter));
    }
    {
      void (app_PluginFileInput::*setter)(const grt::StringRef &) = &app_PluginFileInput::dialogType;
      grt::StringRef (app_PluginFileInput::*getter)() const = &app_PluginFileInput::dialogType;
      meta->bind_member("dialogType", new grt::MetaClass::Property<app_PluginFileInput,grt::StringRef>(getter, setter));
    }
    {
      void (app_PluginFileInput::*setter)(const grt::StringRef &) = &app_PluginFileInput::fileExtensions;
      grt::StringRef (app_PluginFileInput::*getter)() const = &app_PluginFileInput::fileExtensions;
      meta->bind_member("fileExtensions", new grt::MetaClass::Property<app_PluginFileInput,grt::StringRef>(getter, setter));
    }
  }
};

/** input is a list of objects taken from the source given in name (eg activeDiagram) */
class  app_PluginSelectionInput : public app_PluginInputDefinition {
  typedef app_PluginInputDefinition super;

public:
  app_PluginSelectionInput(grt::MetaClass *meta = nullptr)
    : app_PluginInputDefinition(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _argumentCardinality(""),
      _objectStructNames(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "app.PluginSelectionInput";
  }

  /**
   * Getter for attribute argumentCardinality
   *
   * defines the number of objects the plugin requires: 1 for exactly 1, ? for 0 or 1, + for 1 or more and * for 0 or more
   * \par In Python:
   *    value = obj.argumentCardinality
   */
  auto argumentCardinality() const -> grt::StringRef { return _argumentCardinality; }

  /**
   * Setter for attribute argumentCardinality
   * 
   * defines the number of objects the plugin requires: 1 for exactly 1, ? for 0 or 1, + for 1 or more and * for 0 or more
   * \par In Python:
   *   obj.argumentCardinality = value
   */
  virtual auto argumentCardinality(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_argumentCardinality);
    _argumentCardinality = value;
    member_changed("argumentCardinality", ovalue, value);
  }

  /**
   * Getter for attribute objectStructNames (read-only)
   *
   * the types of objects that can be handled by this plugin
   * \par In Python:
   *    value = obj.objectStructNames
   */
  auto objectStructNames() const -> grt::StringListRef { return _objectStructNames; }


private: // The next attribute is read-only.
  virtual auto objectStructNames(const grt::StringListRef &value) -> void {
    grt::ValueRef ovalue(_objectStructNames);
    _objectStructNames = value;
    member_changed("objectStructNames", ovalue, value);
  }
public:

protected:

  grt::StringRef _argumentCardinality;
  grt::StringListRef _objectStructNames;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_PluginSelectionInput());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_PluginSelectionInput::create);
    {
      void (app_PluginSelectionInput::*setter)(const grt::StringRef &) = &app_PluginSelectionInput::argumentCardinality;
      grt::StringRef (app_PluginSelectionInput::*getter)() const = &app_PluginSelectionInput::argumentCardinality;
      meta->bind_member("argumentCardinality", new grt::MetaClass::Property<app_PluginSelectionInput,grt::StringRef>(getter, setter));
    }
    {
      void (app_PluginSelectionInput::*setter)(const grt::StringListRef &) = &app_PluginSelectionInput::objectStructNames;
      grt::StringListRef (app_PluginSelectionInput::*getter)() const = &app_PluginSelectionInput::objectStructNames;
      meta->bind_member("objectStructNames", new grt::MetaClass::Property<app_PluginSelectionInput,grt::StringListRef>(getter, setter));
    }
  }
};

/** a plugin that can be registered */
class  app_Plugin : public GrtObject {
  typedef GrtObject super;

public:
  app_Plugin(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _accessibilityName(""),
      _attributes(this, false),
      _caption(""),
      _description(""),
      _documentStructNames(this, false),
      _groups(this, false),
      _inputValues(this, false),
      _moduleFunctionName(""),
      _moduleName(""),
      _pluginType(""),
      _rating(0),
      _showProgress(0) {
  }

  static auto static_class_name() -> std::string {
    return "app.Plugin";
  }

  /**
   * Getter for attribute accessibilityName
   *
   * the plugin accessible name
   * \par In Python:
   *    value = obj.accessibilityName
   */
  auto accessibilityName() const -> grt::StringRef { return _accessibilityName; }

  /**
   * Setter for attribute accessibilityName
   * 
   * the plugin accessible name
   * \par In Python:
   *   obj.accessibilityName = value
   */
  virtual auto accessibilityName(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_accessibilityName);
    _accessibilityName = value;
    member_changed("accessibilityName", ovalue, value);
  }

  /**
   * Getter for attribute attributes (read-only)
   *
   * additional application specific attributes
   * \par In Python:
   *    value = obj.attributes
   */
  auto attributes() const -> grt::DictRef { return _attributes; }


private: // The next attribute is read-only.
  virtual auto attributes(const grt::DictRef &value) -> void {
    grt::ValueRef ovalue(_attributes);
    _attributes = value;
    member_changed("attributes", ovalue, value);
  }
public:

  /**
   * Getter for attribute caption
   *
   * the plugin caption
   * \par In Python:
   *    value = obj.caption
   */
  auto caption() const -> grt::StringRef { return _caption; }

  /**
   * Setter for attribute caption
   * 
   * the plugin caption
   * \par In Python:
   *   obj.caption = value
   */
  virtual auto caption(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_caption);
    _caption = value;
    member_changed("caption", ovalue, value);
  }

  /**
   * Getter for attribute description
   *
   * the plugin description
   * \par In Python:
   *    value = obj.description
   */
  auto description() const -> grt::StringRef { return _description; }

  /**
   * Setter for attribute description
   * 
   * the plugin description
   * \par In Python:
   *   obj.description = value
   */
  virtual auto description(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_description);
    _description = value;
    member_changed("description", ovalue, value);
  }

  /**
   * Getter for attribute documentStructNames (read-only)
   *
   * the types of documents that can be handled by this plugin
   * \par In Python:
   *    value = obj.documentStructNames
   */
  auto documentStructNames() const -> grt::StringListRef { return _documentStructNames; }


private: // The next attribute is read-only.
  virtual auto documentStructNames(const grt::StringListRef &value) -> void {
    grt::ValueRef ovalue(_documentStructNames);
    _documentStructNames = value;
    member_changed("documentStructNames", ovalue, value);
  }
public:

  /**
   * Getter for attribute groups (read-only)
   *
   * list of group names the plugin belongs to
   * \par In Python:
   *    value = obj.groups
   */
  auto groups() const -> grt::StringListRef { return _groups; }


private: // The next attribute is read-only.
  virtual auto groups(const grt::StringListRef &value) -> void {
    grt::ValueRef ovalue(_groups);
    _groups = value;
    member_changed("groups", ovalue, value);
  }
public:

  // inputValues is owned by app_Plugin
  /**
   * Getter for attribute inputValues (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.inputValues
   */
  auto inputValues() const -> grt::ListRef<app_PluginInputDefinition> { return _inputValues; }


private: // The next attribute is read-only.
  virtual auto inputValues(const grt::ListRef<app_PluginInputDefinition> &value) -> void {
    grt::ValueRef ovalue(_inputValues);

    _inputValues = value;
    owned_member_changed("inputValues", ovalue, value);
  }
public:

  /**
   * Getter for attribute moduleFunctionName
   *
   * the module function that implements the editor (for dll plugins, the dll function name)
   * \par In Python:
   *    value = obj.moduleFunctionName
   */
  auto moduleFunctionName() const -> grt::StringRef { return _moduleFunctionName; }

  /**
   * Setter for attribute moduleFunctionName
   * 
   * the module function that implements the editor (for dll plugins, the dll function name)
   * \par In Python:
   *   obj.moduleFunctionName = value
   */
  virtual auto moduleFunctionName(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_moduleFunctionName);
    _moduleFunctionName = value;
    member_changed("moduleFunctionName", ovalue, value);
  }

  /**
   * Getter for attribute moduleName
   *
   * the module that implements the editor (for dll plugins, it will be the dll name)
   * \par In Python:
   *    value = obj.moduleName
   */
  auto moduleName() const -> grt::StringRef { return _moduleName; }

  /**
   * Setter for attribute moduleName
   * 
   * the module that implements the editor (for dll plugins, it will be the dll name)
   * \par In Python:
   *   obj.moduleName = value
   */
  virtual auto moduleName(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_moduleName);
    _moduleName = value;
    member_changed("moduleName", ovalue, value);
  }

  /**
   * Getter for attribute pluginType
   *
   * one of (normal, gui, standalone). Type of plugin.
   * \par In Python:
   *    value = obj.pluginType
   */
  auto pluginType() const -> grt::StringRef { return _pluginType; }

  /**
   * Setter for attribute pluginType
   * 
   * one of (normal, gui, standalone). Type of plugin.
   * \par In Python:
   *   obj.pluginType = value
   */
  virtual auto pluginType(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_pluginType);
    _pluginType = value;
    member_changed("pluginType", ovalue, value);
  }

  /**
   * Getter for attribute rating
   *
   * the rating of this plugin. The plugin with the highest rating will be choosen, if some kind of matching is used
   * \par In Python:
   *    value = obj.rating
   */
  auto rating() const -> grt::IntegerRef { return _rating; }

  /**
   * Setter for attribute rating
   * 
   * the rating of this plugin. The plugin with the highest rating will be choosen, if some kind of matching is used
   * \par In Python:
   *   obj.rating = value
   */
  virtual auto rating(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_rating);
    _rating = value;
    member_changed("rating", ovalue, value);
  }

  /**
   * Getter for attribute showProgress
   *
   * DEPRECATED. set to 1 to show a progress bar during execution, 2 if the progress is indeterminate
   * \par In Python:
   *    value = obj.showProgress
   */
  auto showProgress() const -> grt::IntegerRef { return _showProgress; }

  /**
   * Setter for attribute showProgress
   * 
   * DEPRECATED. set to 1 to show a progress bar during execution, 2 if the progress is indeterminate
   * \par In Python:
   *   obj.showProgress = value
   */
  virtual auto showProgress(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_showProgress);
    _showProgress = value;
    member_changed("showProgress", ovalue, value);
  }

protected:

  grt::StringRef _accessibilityName;
  grt::DictRef _attributes;
  grt::StringRef _caption;
  grt::StringRef _description;
  grt::StringListRef _documentStructNames;
  grt::StringListRef _groups;
  grt::ListRef<app_PluginInputDefinition> _inputValues;// owned
  grt::StringRef _moduleFunctionName;
  grt::StringRef _moduleName;
  grt::StringRef _pluginType;
  grt::IntegerRef _rating;
  grt::IntegerRef _showProgress;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_Plugin());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_Plugin::create);
    {
      void (app_Plugin::*setter)(const grt::StringRef &) = &app_Plugin::accessibilityName;
      grt::StringRef (app_Plugin::*getter)() const = &app_Plugin::accessibilityName;
      meta->bind_member("accessibilityName", new grt::MetaClass::Property<app_Plugin,grt::StringRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::DictRef &) = &app_Plugin::attributes;
      grt::DictRef (app_Plugin::*getter)() const = &app_Plugin::attributes;
      meta->bind_member("attributes", new grt::MetaClass::Property<app_Plugin,grt::DictRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::StringRef &) = &app_Plugin::caption;
      grt::StringRef (app_Plugin::*getter)() const = &app_Plugin::caption;
      meta->bind_member("caption", new grt::MetaClass::Property<app_Plugin,grt::StringRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::StringRef &) = &app_Plugin::description;
      grt::StringRef (app_Plugin::*getter)() const = &app_Plugin::description;
      meta->bind_member("description", new grt::MetaClass::Property<app_Plugin,grt::StringRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::StringListRef &) = &app_Plugin::documentStructNames;
      grt::StringListRef (app_Plugin::*getter)() const = &app_Plugin::documentStructNames;
      meta->bind_member("documentStructNames", new grt::MetaClass::Property<app_Plugin,grt::StringListRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::StringListRef &) = &app_Plugin::groups;
      grt::StringListRef (app_Plugin::*getter)() const = &app_Plugin::groups;
      meta->bind_member("groups", new grt::MetaClass::Property<app_Plugin,grt::StringListRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::ListRef<app_PluginInputDefinition> &) = &app_Plugin::inputValues;
      grt::ListRef<app_PluginInputDefinition> (app_Plugin::*getter)() const = &app_Plugin::inputValues;
      meta->bind_member("inputValues", new grt::MetaClass::Property<app_Plugin,grt::ListRef<app_PluginInputDefinition>>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::StringRef &) = &app_Plugin::moduleFunctionName;
      grt::StringRef (app_Plugin::*getter)() const = &app_Plugin::moduleFunctionName;
      meta->bind_member("moduleFunctionName", new grt::MetaClass::Property<app_Plugin,grt::StringRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::StringRef &) = &app_Plugin::moduleName;
      grt::StringRef (app_Plugin::*getter)() const = &app_Plugin::moduleName;
      meta->bind_member("moduleName", new grt::MetaClass::Property<app_Plugin,grt::StringRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::StringRef &) = &app_Plugin::pluginType;
      grt::StringRef (app_Plugin::*getter)() const = &app_Plugin::pluginType;
      meta->bind_member("pluginType", new grt::MetaClass::Property<app_Plugin,grt::StringRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::IntegerRef &) = &app_Plugin::rating;
      grt::IntegerRef (app_Plugin::*getter)() const = &app_Plugin::rating;
      meta->bind_member("rating", new grt::MetaClass::Property<app_Plugin,grt::IntegerRef>(getter, setter));
    }
    {
      void (app_Plugin::*setter)(const grt::IntegerRef &) = &app_Plugin::showProgress;
      grt::IntegerRef (app_Plugin::*getter)() const = &app_Plugin::showProgress;
      meta->bind_member("showProgress", new grt::MetaClass::Property<app_Plugin,grt::IntegerRef>(getter, setter));
    }
  }
};

class  app_DocumentPlugin : public app_Plugin {
  typedef app_Plugin super;

public:
  app_DocumentPlugin(grt::MetaClass *meta = nullptr)
    : app_Plugin(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())) {
    _documentStructNames.content().__retype(grt::ObjectType, "");
  }

  static auto static_class_name() -> std::string {
    return "app.DocumentPlugin";
  }

  /**
   * Getter for attribute documentStructNames (read-only)
   *
   * type of document that can be handled
   * \par In Python:
   *    value = obj.documentStructNames
   */


private: // The next attribute is read-only.
public:

protected:


private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_DocumentPlugin());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_DocumentPlugin::create);
    {
      void (app_DocumentPlugin::*setter)(const grt::StringListRef &) = 0;
      grt::StringListRef (app_DocumentPlugin::*getter)() const = 0;
      meta->bind_member("documentStructNames", new grt::MetaClass::Property<app_DocumentPlugin,grt::StringListRef>(getter, setter));
    }
  }
};

/** groups a number of plugins together */
class  app_PluginGroup : public GrtObject {
  typedef GrtObject super;

public:
  app_PluginGroup(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _accessibilityName(""),
      _category(""),
      _plugins(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "app.PluginGroup";
  }

  /**
   * Getter for attribute accessibilityName
   *
   * the plugin group accessible name
   * \par In Python:
   *    value = obj.accessibilityName
   */
  auto accessibilityName() const -> grt::StringRef { return _accessibilityName; }

  /**
   * Setter for attribute accessibilityName
   * 
   * the plugin group accessible name
   * \par In Python:
   *   obj.accessibilityName = value
   */
  virtual auto accessibilityName(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_accessibilityName);
    _accessibilityName = value;
    member_changed("accessibilityName", ovalue, value);
  }

  /**
   * Getter for attribute category
   *
   * the category this group belongs to
   * \par In Python:
   *    value = obj.category
   */
  auto category() const -> grt::StringRef { return _category; }

  /**
   * Setter for attribute category
   * 
   * the category this group belongs to
   * \par In Python:
   *   obj.category = value
   */
  virtual auto category(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_category);
    _category = value;
    member_changed("category", ovalue, value);
  }

  // plugins is owned by app_PluginGroup
  /**
   * Getter for attribute plugins (read-only)
   *
   * the list of plugins in this group
   * \par In Python:
   *    value = obj.plugins
   */
  auto plugins() const -> grt::ListRef<app_Plugin> { return _plugins; }


private: // The next attribute is read-only.
  virtual auto plugins(const grt::ListRef<app_Plugin> &value) -> void {
    grt::ValueRef ovalue(_plugins);

    _plugins = value;
    owned_member_changed("plugins", ovalue, value);
  }
public:

protected:

  grt::StringRef _accessibilityName;
  grt::StringRef _category;
  grt::ListRef<app_Plugin> _plugins;// owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_PluginGroup());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_PluginGroup::create);
    {
      void (app_PluginGroup::*setter)(const grt::StringRef &) = &app_PluginGroup::accessibilityName;
      grt::StringRef (app_PluginGroup::*getter)() const = &app_PluginGroup::accessibilityName;
      meta->bind_member("accessibilityName", new grt::MetaClass::Property<app_PluginGroup,grt::StringRef>(getter, setter));
    }
    {
      void (app_PluginGroup::*setter)(const grt::StringRef &) = &app_PluginGroup::category;
      grt::StringRef (app_PluginGroup::*getter)() const = &app_PluginGroup::category;
      meta->bind_member("category", new grt::MetaClass::Property<app_PluginGroup,grt::StringRef>(getter, setter));
    }
    {
      void (app_PluginGroup::*setter)(const grt::ListRef<app_Plugin> &) = &app_PluginGroup::plugins;
      grt::ListRef<app_Plugin> (app_PluginGroup::*getter)() const = &app_PluginGroup::plugins;
      meta->bind_member("plugins", new grt::MetaClass::Property<app_PluginGroup,grt::ListRef<app_Plugin>>(getter, setter));
    }
  }
};

class  app_Toolbar : public GrtObject {
  typedef GrtObject super;

public:
  app_Toolbar(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _accessibilityName(""),
      _items(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "app.Toolbar";
  }

  /**
   * Getter for attribute accessibilityName
   *
   * 
   * \par In Python:
   *    value = obj.accessibilityName
   */
  auto accessibilityName() const -> grt::StringRef { return _accessibilityName; }

  /**
   * Setter for attribute accessibilityName
   * 
   * 
   * \par In Python:
   *   obj.accessibilityName = value
   */
  virtual auto accessibilityName(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_accessibilityName);
    _accessibilityName = value;
    member_changed("accessibilityName", ovalue, value);
  }

  // items is owned by app_Toolbar
  /**
   * Getter for attribute items (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.items
   */
  auto items() const -> grt::ListRef<app_ToolbarItem> { return _items; }


private: // The next attribute is read-only.
  virtual auto items(const grt::ListRef<app_ToolbarItem> &value) -> void {
    grt::ValueRef ovalue(_items);

    _items = value;
    owned_member_changed("items", ovalue, value);
  }
public:

protected:

  grt::StringRef _accessibilityName;
  grt::ListRef<app_ToolbarItem> _items;// owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_Toolbar());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_Toolbar::create);
    {
      void (app_Toolbar::*setter)(const grt::StringRef &) = &app_Toolbar::accessibilityName;
      grt::StringRef (app_Toolbar::*getter)() const = &app_Toolbar::accessibilityName;
      meta->bind_member("accessibilityName", new grt::MetaClass::Property<app_Toolbar,grt::StringRef>(getter, setter));
    }
    {
      void (app_Toolbar::*setter)(const grt::ListRef<app_ToolbarItem> &) = &app_Toolbar::items;
      grt::ListRef<app_ToolbarItem> (app_Toolbar::*getter)() const = &app_Toolbar::items;
      meta->bind_member("items", new grt::MetaClass::Property<app_Toolbar,grt::ListRef<app_ToolbarItem>>(getter, setter));
    }
  }
};

class  app_CommandItem : public GrtObject {
  typedef GrtObject super;

public:
  app_CommandItem(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _command(""),
      _context(""),
      _platform("") {
  }

  static auto static_class_name() -> std::string {
    return "app.CommandItem";
  }

  /**
   * Getter for attribute command
   *
   * command name if builtin, or plugin name
   * \par In Python:
   *    value = obj.command
   */
  auto command() const -> grt::StringRef { return _command; }

  /**
   * Setter for attribute command
   * 
   * command name if builtin, or plugin name
   * \par In Python:
   *   obj.command = value
   */
  virtual auto command(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_command);
    _command = value;
    member_changed("command", ovalue, value);
  }

  /**
   * Getter for attribute context
   *
   * application context where the item is valid (eg global, model etc)
   * \par In Python:
   *    value = obj.context
   */
  auto context() const -> grt::StringRef { return _context; }

  /**
   * Setter for attribute context
   * 
   * application context where the item is valid (eg global, model etc)
   * \par In Python:
   *   obj.context = value
   */
  virtual auto context(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_context);
    _context = value;
    member_changed("context", ovalue, value);
  }

  /**
   * Getter for attribute platform
   *
   * windows, linux, macosx
   * \par In Python:
   *    value = obj.platform
   */
  auto platform() const -> grt::StringRef { return _platform; }

  /**
   * Setter for attribute platform
   * 
   * windows, linux, macosx
   * \par In Python:
   *   obj.platform = value
   */
  virtual auto platform(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_platform);
    _platform = value;
    member_changed("platform", ovalue, value);
  }

protected:

  grt::StringRef _command;
  grt::StringRef _context;
  grt::StringRef _platform;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_CommandItem());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_CommandItem::create);
    {
      void (app_CommandItem::*setter)(const grt::StringRef &) = &app_CommandItem::command;
      grt::StringRef (app_CommandItem::*getter)() const = &app_CommandItem::command;
      meta->bind_member("command", new grt::MetaClass::Property<app_CommandItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_CommandItem::*setter)(const grt::StringRef &) = &app_CommandItem::context;
      grt::StringRef (app_CommandItem::*getter)() const = &app_CommandItem::context;
      meta->bind_member("context", new grt::MetaClass::Property<app_CommandItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_CommandItem::*setter)(const grt::StringRef &) = &app_CommandItem::platform;
      grt::StringRef (app_CommandItem::*getter)() const = &app_CommandItem::platform;
      meta->bind_member("platform", new grt::MetaClass::Property<app_CommandItem,grt::StringRef>(getter, setter));
    }
  }
};

class  app_ToolbarItem : public app_CommandItem {
  typedef app_CommandItem super;

public:
  app_ToolbarItem(grt::MetaClass *meta = nullptr)
    : app_CommandItem(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _accessibilityName(""),
      _altIcon(""),
      _darkIcon(""),
      _icon(""),
      _initialState(0),
      _itemType(""),
      _tooltip("") {
  }

  static auto static_class_name() -> std::string {
    return "app.ToolbarItem";
  }

  /**
   * Getter for attribute accessibilityName
   *
   * 
   * \par In Python:
   *    value = obj.accessibilityName
   */
  auto accessibilityName() const -> grt::StringRef { return _accessibilityName; }

  /**
   * Setter for attribute accessibilityName
   * 
   * 
   * \par In Python:
   *   obj.accessibilityName = value
   */
  virtual auto accessibilityName(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_accessibilityName);
    _accessibilityName = value;
    member_changed("accessibilityName", ovalue, value);
  }

  /**
   * Getter for attribute altIcon
   *
   * 
   * \par In Python:
   *    value = obj.altIcon
   */
  auto altIcon() const -> grt::StringRef { return _altIcon; }

  /**
   * Setter for attribute altIcon
   * 
   * 
   * \par In Python:
   *   obj.altIcon = value
   */
  virtual auto altIcon(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_altIcon);
    _altIcon = value;
    member_changed("altIcon", ovalue, value);
  }

  /**
   * Getter for attribute darkIcon
   *
   * 
   * \par In Python:
   *    value = obj.darkIcon
   */
  auto darkIcon() const -> grt::StringRef { return _darkIcon; }

  /**
   * Setter for attribute darkIcon
   * 
   * 
   * \par In Python:
   *   obj.darkIcon = value
   */
  virtual auto darkIcon(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_darkIcon);
    _darkIcon = value;
    member_changed("darkIcon", ovalue, value);
  }

  /**
   * Getter for attribute icon
   *
   * 
   * \par In Python:
   *    value = obj.icon
   */
  auto icon() const -> grt::StringRef { return _icon; }

  /**
   * Setter for attribute icon
   * 
   * 
   * \par In Python:
   *   obj.icon = value
   */
  virtual auto icon(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_icon);
    _icon = value;
    member_changed("icon", ovalue, value);
  }

  /**
   * Getter for attribute initialState
   *
   * For (segmented) toggle only: is the item checked initially?
   * \par In Python:
   *    value = obj.initialState
   */
  auto initialState() const -> grt::IntegerRef { return _initialState; }

  /**
   * Setter for attribute initialState
   * 
   * For (segmented) toggle only: is the item checked initially?
   * \par In Python:
   *   obj.initialState = value
   */
  virtual auto initialState(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_initialState);
    _initialState = value;
    member_changed("initialState", ovalue, value);
  }

  /**
   * Getter for attribute itemType
   *
   * type of button (action, separator, toggle, segmentedToggle, radio, label, dropdown)
   * \par In Python:
   *    value = obj.itemType
   */
  auto itemType() const -> grt::StringRef { return _itemType; }

  /**
   * Setter for attribute itemType
   * 
   * type of button (action, separator, toggle, segmentedToggle, radio, label, dropdown)
   * \par In Python:
   *   obj.itemType = value
   */
  virtual auto itemType(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_itemType);
    _itemType = value;
    member_changed("itemType", ovalue, value);
  }

  /**
   * Getter for attribute tooltip
   *
   * 
   * \par In Python:
   *    value = obj.tooltip
   */
  auto tooltip() const -> grt::StringRef { return _tooltip; }

  /**
   * Setter for attribute tooltip
   * 
   * 
   * \par In Python:
   *   obj.tooltip = value
   */
  virtual auto tooltip(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_tooltip);
    _tooltip = value;
    member_changed("tooltip", ovalue, value);
  }

protected:

  grt::StringRef _accessibilityName;
  grt::StringRef _altIcon;
  grt::StringRef _darkIcon;
  grt::StringRef _icon;
  grt::IntegerRef _initialState;
  grt::StringRef _itemType;
  grt::StringRef _tooltip;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_ToolbarItem());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_ToolbarItem::create);
    {
      void (app_ToolbarItem::*setter)(const grt::StringRef &) = &app_ToolbarItem::accessibilityName;
      grt::StringRef (app_ToolbarItem::*getter)() const = &app_ToolbarItem::accessibilityName;
      meta->bind_member("accessibilityName", new grt::MetaClass::Property<app_ToolbarItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_ToolbarItem::*setter)(const grt::StringRef &) = &app_ToolbarItem::altIcon;
      grt::StringRef (app_ToolbarItem::*getter)() const = &app_ToolbarItem::altIcon;
      meta->bind_member("altIcon", new grt::MetaClass::Property<app_ToolbarItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_ToolbarItem::*setter)(const grt::StringRef &) = &app_ToolbarItem::darkIcon;
      grt::StringRef (app_ToolbarItem::*getter)() const = &app_ToolbarItem::darkIcon;
      meta->bind_member("darkIcon", new grt::MetaClass::Property<app_ToolbarItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_ToolbarItem::*setter)(const grt::StringRef &) = &app_ToolbarItem::icon;
      grt::StringRef (app_ToolbarItem::*getter)() const = &app_ToolbarItem::icon;
      meta->bind_member("icon", new grt::MetaClass::Property<app_ToolbarItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_ToolbarItem::*setter)(const grt::IntegerRef &) = &app_ToolbarItem::initialState;
      grt::IntegerRef (app_ToolbarItem::*getter)() const = &app_ToolbarItem::initialState;
      meta->bind_member("initialState", new grt::MetaClass::Property<app_ToolbarItem,grt::IntegerRef>(getter, setter));
    }
    {
      void (app_ToolbarItem::*setter)(const grt::StringRef &) = &app_ToolbarItem::itemType;
      grt::StringRef (app_ToolbarItem::*getter)() const = &app_ToolbarItem::itemType;
      meta->bind_member("itemType", new grt::MetaClass::Property<app_ToolbarItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_ToolbarItem::*setter)(const grt::StringRef &) = &app_ToolbarItem::tooltip;
      grt::StringRef (app_ToolbarItem::*getter)() const = &app_ToolbarItem::tooltip;
      meta->bind_member("tooltip", new grt::MetaClass::Property<app_ToolbarItem,grt::StringRef>(getter, setter));
    }
  }
};

class  app_ShortcutItem : public app_CommandItem {
  typedef app_CommandItem super;

public:
  app_ShortcutItem(grt::MetaClass *meta = nullptr)
    : app_CommandItem(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _shortcut("") {
  }

  static auto static_class_name() -> std::string {
    return "app.ShortcutItem";
  }

  /**
   * Getter for attribute shortcut
   *
   * 
   * \par In Python:
   *    value = obj.shortcut
   */
  auto shortcut() const -> grt::StringRef { return _shortcut; }

  /**
   * Setter for attribute shortcut
   * 
   * 
   * \par In Python:
   *   obj.shortcut = value
   */
  virtual auto shortcut(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_shortcut);
    _shortcut = value;
    member_changed("shortcut", ovalue, value);
  }

protected:

  grt::StringRef _shortcut;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_ShortcutItem());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_ShortcutItem::create);
    {
      void (app_ShortcutItem::*setter)(const grt::StringRef &) = &app_ShortcutItem::shortcut;
      grt::StringRef (app_ShortcutItem::*getter)() const = &app_ShortcutItem::shortcut;
      meta->bind_member("shortcut", new grt::MetaClass::Property<app_ShortcutItem,grt::StringRef>(getter, setter));
    }
  }
};

class  app_MenuItem : public app_CommandItem {
  typedef app_CommandItem super;

public:
  app_MenuItem(grt::MetaClass *meta = nullptr)
    : app_CommandItem(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _accessibilityName(""),
      _caption(""),
      _itemType(""),
      _shortcut(""),
      _subItems(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "app.MenuItem";
  }

  /**
   * Getter for attribute accessibilityName
   *
   * 
   * \par In Python:
   *    value = obj.accessibilityName
   */
  auto accessibilityName() const -> grt::StringRef { return _accessibilityName; }

  /**
   * Setter for attribute accessibilityName
   * 
   * 
   * \par In Python:
   *   obj.accessibilityName = value
   */
  virtual auto accessibilityName(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_accessibilityName);
    _accessibilityName = value;
    member_changed("accessibilityName", ovalue, value);
  }

  /**
   * Getter for attribute caption
   *
   * 
   * \par In Python:
   *    value = obj.caption
   */
  auto caption() const -> grt::StringRef { return _caption; }

  /**
   * Setter for attribute caption
   * 
   * 
   * \par In Python:
   *   obj.caption = value
   */
  virtual auto caption(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_caption);
    _caption = value;
    member_changed("caption", ovalue, value);
  }

  /**
   * Getter for attribute itemType
   *
   * type of item (action, separator, cascade, check, radio)
   * \par In Python:
   *    value = obj.itemType
   */
  auto itemType() const -> grt::StringRef { return _itemType; }

  /**
   * Setter for attribute itemType
   * 
   * type of item (action, separator, cascade, check, radio)
   * \par In Python:
   *   obj.itemType = value
   */
  virtual auto itemType(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_itemType);
    _itemType = value;
    member_changed("itemType", ovalue, value);
  }

  /**
   * Getter for attribute shortcut
   *
   * optional shortcut (eg: control+s)
   * \par In Python:
   *    value = obj.shortcut
   */
  auto shortcut() const -> grt::StringRef { return _shortcut; }

  /**
   * Setter for attribute shortcut
   * 
   * optional shortcut (eg: control+s)
   * \par In Python:
   *   obj.shortcut = value
   */
  virtual auto shortcut(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_shortcut);
    _shortcut = value;
    member_changed("shortcut", ovalue, value);
  }

  // subItems is owned by app_MenuItem
  /**
   * Getter for attribute subItems (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.subItems
   */
  auto subItems() const -> grt::ListRef<app_MenuItem> { return _subItems; }


private: // The next attribute is read-only.
  virtual auto subItems(const grt::ListRef<app_MenuItem> &value) -> void {
    grt::ValueRef ovalue(_subItems);

    _subItems = value;
    owned_member_changed("subItems", ovalue, value);
  }
public:

protected:

  grt::StringRef _accessibilityName;
  grt::StringRef _caption;
  grt::StringRef _itemType;
  grt::StringRef _shortcut;
  grt::ListRef<app_MenuItem> _subItems;// owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_MenuItem());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_MenuItem::create);
    {
      void (app_MenuItem::*setter)(const grt::StringRef &) = &app_MenuItem::accessibilityName;
      grt::StringRef (app_MenuItem::*getter)() const = &app_MenuItem::accessibilityName;
      meta->bind_member("accessibilityName", new grt::MetaClass::Property<app_MenuItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_MenuItem::*setter)(const grt::StringRef &) = &app_MenuItem::caption;
      grt::StringRef (app_MenuItem::*getter)() const = &app_MenuItem::caption;
      meta->bind_member("caption", new grt::MetaClass::Property<app_MenuItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_MenuItem::*setter)(const grt::StringRef &) = &app_MenuItem::itemType;
      grt::StringRef (app_MenuItem::*getter)() const = &app_MenuItem::itemType;
      meta->bind_member("itemType", new grt::MetaClass::Property<app_MenuItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_MenuItem::*setter)(const grt::StringRef &) = &app_MenuItem::shortcut;
      grt::StringRef (app_MenuItem::*getter)() const = &app_MenuItem::shortcut;
      meta->bind_member("shortcut", new grt::MetaClass::Property<app_MenuItem,grt::StringRef>(getter, setter));
    }
    {
      void (app_MenuItem::*setter)(const grt::ListRef<app_MenuItem> &) = &app_MenuItem::subItems;
      grt::ListRef<app_MenuItem> (app_MenuItem::*getter)() const = &app_MenuItem::subItems;
      meta->bind_member("subItems", new grt::MetaClass::Property<app_MenuItem,grt::ListRef<app_MenuItem>>(getter, setter));
    }
  }
};

class  app_CustomDataField : public GrtObject {
  typedef GrtObject super;

public:
  app_CustomDataField(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _defaultValue(""),
      _description(""),
      _objectStruct(""),
      _type("") {
  }

  static auto static_class_name() -> std::string {
    return "app.CustomDataField";
  }

  /**
   * Getter for attribute defaultValue
   *
   * default value for the field
   * \par In Python:
   *    value = obj.defaultValue
   */
  auto defaultValue() const -> grt::StringRef { return _defaultValue; }

  /**
   * Setter for attribute defaultValue
   * 
   * default value for the field
   * \par In Python:
   *   obj.defaultValue = value
   */
  virtual auto defaultValue(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_defaultValue);
    _defaultValue = value;
    member_changed("defaultValue", ovalue, value);
  }

  /**
   * Getter for attribute description
   *
   * description of the field
   * \par In Python:
   *    value = obj.description
   */
  auto description() const -> grt::StringRef { return _description; }

  /**
   * Setter for attribute description
   * 
   * description of the field
   * \par In Python:
   *   obj.description = value
   */
  virtual auto description(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_description);
    _description = value;
    member_changed("description", ovalue, value);
  }

  /**
   * Getter for attribute objectStruct
   *
   * object struct names that this applies to
   * \par In Python:
   *    value = obj.objectStruct
   */
  auto objectStruct() const -> grt::StringRef { return _objectStruct; }

  /**
   * Setter for attribute objectStruct
   * 
   * object struct names that this applies to
   * \par In Python:
   *   obj.objectStruct = value
   */
  virtual auto objectStruct(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_objectStruct);
    _objectStruct = value;
    member_changed("objectStruct", ovalue, value);
  }

  /**
   * Getter for attribute type
   *
   * type of the field (int, string, double, dict, object, list)
   * \par In Python:
   *    value = obj.type
   */
  auto type() const -> grt::StringRef { return _type; }

  /**
   * Setter for attribute type
   * 
   * type of the field (int, string, double, dict, object, list)
   * \par In Python:
   *   obj.type = value
   */
  virtual auto type(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_type);
    _type = value;
    member_changed("type", ovalue, value);
  }

protected:

  grt::StringRef _defaultValue;
  grt::StringRef _description;
  grt::StringRef _objectStruct;
  grt::StringRef _type;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_CustomDataField());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_CustomDataField::create);
    {
      void (app_CustomDataField::*setter)(const grt::StringRef &) = &app_CustomDataField::defaultValue;
      grt::StringRef (app_CustomDataField::*getter)() const = &app_CustomDataField::defaultValue;
      meta->bind_member("defaultValue", new grt::MetaClass::Property<app_CustomDataField,grt::StringRef>(getter, setter));
    }
    {
      void (app_CustomDataField::*setter)(const grt::StringRef &) = &app_CustomDataField::description;
      grt::StringRef (app_CustomDataField::*getter)() const = &app_CustomDataField::description;
      meta->bind_member("description", new grt::MetaClass::Property<app_CustomDataField,grt::StringRef>(getter, setter));
    }
    {
      void (app_CustomDataField::*setter)(const grt::StringRef &) = &app_CustomDataField::objectStruct;
      grt::StringRef (app_CustomDataField::*getter)() const = &app_CustomDataField::objectStruct;
      meta->bind_member("objectStruct", new grt::MetaClass::Property<app_CustomDataField,grt::StringRef>(getter, setter));
    }
    {
      void (app_CustomDataField::*setter)(const grt::StringRef &) = &app_CustomDataField::type;
      grt::StringRef (app_CustomDataField::*getter)() const = &app_CustomDataField::type;
      meta->bind_member("type", new grt::MetaClass::Property<app_CustomDataField,grt::StringRef>(getter, setter));
    }
  }
};

class  app_PageSettings : public GrtObject {
  typedef GrtObject super;

public:
  app_PageSettings(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _marginBottom(0.0),
      _marginLeft(0.0),
      _marginRight(0.0),
      _marginTop(0.0),
      _orientation(""),
      _scale(5) {
  }

  static auto static_class_name() -> std::string {
    return "app.PageSettings";
  }

  /**
   * Getter for attribute marginBottom
   *
   * 
   * \par In Python:
   *    value = obj.marginBottom
   */
  auto marginBottom() const -> grt::DoubleRef { return _marginBottom; }

  /**
   * Setter for attribute marginBottom
   * 
   * 
   * \par In Python:
   *   obj.marginBottom = value
   */
  virtual auto marginBottom(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_marginBottom);
    _marginBottom = value;
    member_changed("marginBottom", ovalue, value);
  }

  /**
   * Getter for attribute marginLeft
   *
   * 
   * \par In Python:
   *    value = obj.marginLeft
   */
  auto marginLeft() const -> grt::DoubleRef { return _marginLeft; }

  /**
   * Setter for attribute marginLeft
   * 
   * 
   * \par In Python:
   *   obj.marginLeft = value
   */
  virtual auto marginLeft(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_marginLeft);
    _marginLeft = value;
    member_changed("marginLeft", ovalue, value);
  }

  /**
   * Getter for attribute marginRight
   *
   * 
   * \par In Python:
   *    value = obj.marginRight
   */
  auto marginRight() const -> grt::DoubleRef { return _marginRight; }

  /**
   * Setter for attribute marginRight
   * 
   * 
   * \par In Python:
   *   obj.marginRight = value
   */
  virtual auto marginRight(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_marginRight);
    _marginRight = value;
    member_changed("marginRight", ovalue, value);
  }

  /**
   * Getter for attribute marginTop
   *
   * 
   * \par In Python:
   *    value = obj.marginTop
   */
  auto marginTop() const -> grt::DoubleRef { return _marginTop; }

  /**
   * Setter for attribute marginTop
   * 
   * 
   * \par In Python:
   *   obj.marginTop = value
   */
  virtual auto marginTop(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_marginTop);
    _marginTop = value;
    member_changed("marginTop", ovalue, value);
  }

  /**
   * Getter for attribute orientation
   *
   * landscape or portrait
   * \par In Python:
   *    value = obj.orientation
   */
  auto orientation() const -> grt::StringRef { return _orientation; }

  /**
   * Setter for attribute orientation
   * 
   * landscape or portrait
   * \par In Python:
   *   obj.orientation = value
   */
  virtual auto orientation(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_orientation);
    _orientation = value;
    member_changed("orientation", ovalue, value);
  }

  /**
   * Getter for attribute paperType
   *
   * type of paper size (A4, letter etc)
   * \par In Python:
   *    value = obj.paperType
   */
  auto paperType() const -> app_PaperTypeRef { return _paperType; }

  /**
   * Setter for attribute paperType
   * 
   * type of paper size (A4, letter etc)
   * \par In Python:
   *   obj.paperType = value
   */
  virtual auto paperType(const app_PaperTypeRef &value) -> void {
    grt::ValueRef ovalue(_paperType);
    _paperType = value;
    member_changed("paperType", ovalue, value);
  }

  /**
   * Getter for attribute scale
   *
   * 
   * \par In Python:
   *    value = obj.scale
   */
  auto scale() const -> grt::DoubleRef { return _scale; }

  /**
   * Setter for attribute scale
   * 
   * 
   * \par In Python:
   *   obj.scale = value
   */
  virtual auto scale(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_scale);
    _scale = value;
    member_changed("scale", ovalue, value);
  }

protected:

  grt::DoubleRef _marginBottom;
  grt::DoubleRef _marginLeft;
  grt::DoubleRef _marginRight;
  grt::DoubleRef _marginTop;
  grt::StringRef _orientation;
  app_PaperTypeRef _paperType;
  grt::DoubleRef _scale;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_PageSettings());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_PageSettings::create);
    {
      void (app_PageSettings::*setter)(const grt::DoubleRef &) = &app_PageSettings::marginBottom;
      grt::DoubleRef (app_PageSettings::*getter)() const = &app_PageSettings::marginBottom;
      meta->bind_member("marginBottom", new grt::MetaClass::Property<app_PageSettings,grt::DoubleRef>(getter, setter));
    }
    {
      void (app_PageSettings::*setter)(const grt::DoubleRef &) = &app_PageSettings::marginLeft;
      grt::DoubleRef (app_PageSettings::*getter)() const = &app_PageSettings::marginLeft;
      meta->bind_member("marginLeft", new grt::MetaClass::Property<app_PageSettings,grt::DoubleRef>(getter, setter));
    }
    {
      void (app_PageSettings::*setter)(const grt::DoubleRef &) = &app_PageSettings::marginRight;
      grt::DoubleRef (app_PageSettings::*getter)() const = &app_PageSettings::marginRight;
      meta->bind_member("marginRight", new grt::MetaClass::Property<app_PageSettings,grt::DoubleRef>(getter, setter));
    }
    {
      void (app_PageSettings::*setter)(const grt::DoubleRef &) = &app_PageSettings::marginTop;
      grt::DoubleRef (app_PageSettings::*getter)() const = &app_PageSettings::marginTop;
      meta->bind_member("marginTop", new grt::MetaClass::Property<app_PageSettings,grt::DoubleRef>(getter, setter));
    }
    {
      void (app_PageSettings::*setter)(const grt::StringRef &) = &app_PageSettings::orientation;
      grt::StringRef (app_PageSettings::*getter)() const = &app_PageSettings::orientation;
      meta->bind_member("orientation", new grt::MetaClass::Property<app_PageSettings,grt::StringRef>(getter, setter));
    }
    {
      void (app_PageSettings::*setter)(const app_PaperTypeRef &) = &app_PageSettings::paperType;
      app_PaperTypeRef (app_PageSettings::*getter)() const = &app_PageSettings::paperType;
      meta->bind_member("paperType", new grt::MetaClass::Property<app_PageSettings,app_PaperTypeRef>(getter, setter));
    }
    {
      void (app_PageSettings::*setter)(const grt::DoubleRef &) = &app_PageSettings::scale;
      grt::DoubleRef (app_PageSettings::*getter)() const = &app_PageSettings::scale;
      meta->bind_member("scale", new grt::MetaClass::Property<app_PageSettings,grt::DoubleRef>(getter, setter));
    }
  }
};

class  app_PaperType : public GrtObject {
  typedef GrtObject super;

public:
  app_PaperType(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _caption(""),
      _height(0.0),
      _marginBottom(0.0),
      _marginLeft(0.0),
      _marginRight(0.0),
      _marginTop(0.0),
      _marginsSet(0),
      _width(0.0) {
  }

  static auto static_class_name() -> std::string {
    return "app.PaperType";
  }

  /**
   * Getter for attribute caption
   *
   * 
   * \par In Python:
   *    value = obj.caption
   */
  auto caption() const -> grt::StringRef { return _caption; }

  /**
   * Setter for attribute caption
   * 
   * 
   * \par In Python:
   *   obj.caption = value
   */
  virtual auto caption(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_caption);
    _caption = value;
    member_changed("caption", ovalue, value);
  }

  /**
   * Getter for attribute height
   *
   * 
   * \par In Python:
   *    value = obj.height
   */
  auto height() const -> grt::DoubleRef { return _height; }

  /**
   * Setter for attribute height
   * 
   * 
   * \par In Python:
   *   obj.height = value
   */
  virtual auto height(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_height);
    _height = value;
    member_changed("height", ovalue, value);
  }

  /**
   * Getter for attribute marginBottom
   *
   * 
   * \par In Python:
   *    value = obj.marginBottom
   */
  auto marginBottom() const -> grt::DoubleRef { return _marginBottom; }

  /**
   * Setter for attribute marginBottom
   * 
   * 
   * \par In Python:
   *   obj.marginBottom = value
   */
  virtual auto marginBottom(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_marginBottom);
    _marginBottom = value;
    member_changed("marginBottom", ovalue, value);
  }

  /**
   * Getter for attribute marginLeft
   *
   * 
   * \par In Python:
   *    value = obj.marginLeft
   */
  auto marginLeft() const -> grt::DoubleRef { return _marginLeft; }

  /**
   * Setter for attribute marginLeft
   * 
   * 
   * \par In Python:
   *   obj.marginLeft = value
   */
  virtual auto marginLeft(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_marginLeft);
    _marginLeft = value;
    member_changed("marginLeft", ovalue, value);
  }

  /**
   * Getter for attribute marginRight
   *
   * 
   * \par In Python:
   *    value = obj.marginRight
   */
  auto marginRight() const -> grt::DoubleRef { return _marginRight; }

  /**
   * Setter for attribute marginRight
   * 
   * 
   * \par In Python:
   *   obj.marginRight = value
   */
  virtual auto marginRight(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_marginRight);
    _marginRight = value;
    member_changed("marginRight", ovalue, value);
  }

  /**
   * Getter for attribute marginTop
   *
   * 
   * \par In Python:
   *    value = obj.marginTop
   */
  auto marginTop() const -> grt::DoubleRef { return _marginTop; }

  /**
   * Setter for attribute marginTop
   * 
   * 
   * \par In Python:
   *   obj.marginTop = value
   */
  virtual auto marginTop(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_marginTop);
    _marginTop = value;
    member_changed("marginTop", ovalue, value);
  }

  /**
   * Getter for attribute marginsSet
   *
   * 
   * \par In Python:
   *    value = obj.marginsSet
   */
  auto marginsSet() const -> grt::IntegerRef { return _marginsSet; }

  /**
   * Setter for attribute marginsSet
   * 
   * 
   * \par In Python:
   *   obj.marginsSet = value
   */
  virtual auto marginsSet(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_marginsSet);
    _marginsSet = value;
    member_changed("marginsSet", ovalue, value);
  }

  /**
   * Getter for attribute width
   *
   * 
   * \par In Python:
   *    value = obj.width
   */
  auto width() const -> grt::DoubleRef { return _width; }

  /**
   * Setter for attribute width
   * 
   * 
   * \par In Python:
   *   obj.width = value
   */
  virtual auto width(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_width);
    _width = value;
    member_changed("width", ovalue, value);
  }

protected:

  grt::StringRef _caption;
  grt::DoubleRef _height;
  grt::DoubleRef _marginBottom;
  grt::DoubleRef _marginLeft;
  grt::DoubleRef _marginRight;
  grt::DoubleRef _marginTop;
  grt::IntegerRef _marginsSet;
  grt::DoubleRef _width;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_PaperType());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_PaperType::create);
    {
      void (app_PaperType::*setter)(const grt::StringRef &) = &app_PaperType::caption;
      grt::StringRef (app_PaperType::*getter)() const = &app_PaperType::caption;
      meta->bind_member("caption", new grt::MetaClass::Property<app_PaperType,grt::StringRef>(getter, setter));
    }
    {
      void (app_PaperType::*setter)(const grt::DoubleRef &) = &app_PaperType::height;
      grt::DoubleRef (app_PaperType::*getter)() const = &app_PaperType::height;
      meta->bind_member("height", new grt::MetaClass::Property<app_PaperType,grt::DoubleRef>(getter, setter));
    }
    {
      void (app_PaperType::*setter)(const grt::DoubleRef &) = &app_PaperType::marginBottom;
      grt::DoubleRef (app_PaperType::*getter)() const = &app_PaperType::marginBottom;
      meta->bind_member("marginBottom", new grt::MetaClass::Property<app_PaperType,grt::DoubleRef>(getter, setter));
    }
    {
      void (app_PaperType::*setter)(const grt::DoubleRef &) = &app_PaperType::marginLeft;
      grt::DoubleRef (app_PaperType::*getter)() const = &app_PaperType::marginLeft;
      meta->bind_member("marginLeft", new grt::MetaClass::Property<app_PaperType,grt::DoubleRef>(getter, setter));
    }
    {
      void (app_PaperType::*setter)(const grt::DoubleRef &) = &app_PaperType::marginRight;
      grt::DoubleRef (app_PaperType::*getter)() const = &app_PaperType::marginRight;
      meta->bind_member("marginRight", new grt::MetaClass::Property<app_PaperType,grt::DoubleRef>(getter, setter));
    }
    {
      void (app_PaperType::*setter)(const grt::DoubleRef &) = &app_PaperType::marginTop;
      grt::DoubleRef (app_PaperType::*getter)() const = &app_PaperType::marginTop;
      meta->bind_member("marginTop", new grt::MetaClass::Property<app_PaperType,grt::DoubleRef>(getter, setter));
    }
    {
      void (app_PaperType::*setter)(const grt::IntegerRef &) = &app_PaperType::marginsSet;
      grt::IntegerRef (app_PaperType::*getter)() const = &app_PaperType::marginsSet;
      meta->bind_member("marginsSet", new grt::MetaClass::Property<app_PaperType,grt::IntegerRef>(getter, setter));
    }
    {
      void (app_PaperType::*setter)(const grt::DoubleRef &) = &app_PaperType::width;
      grt::DoubleRef (app_PaperType::*getter)() const = &app_PaperType::width;
      meta->bind_member("width", new grt::MetaClass::Property<app_PaperType,grt::DoubleRef>(getter, setter));
    }
  }
};

/** registry that keeps dynamic information used by the application */
class  app_Registry : public GrtObject {
  typedef GrtObject super;

public:
  app_Registry(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _appDataDirectory(""),
      _appExecutablePath(""),
      _customDataFields(this, false),
      _pluginGroups(this, false),
      _plugins(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "app.Registry";
  }

  /**
   * Getter for attribute appDataDirectory
   *
   * 
   * \par In Python:
   *    value = obj.appDataDirectory
   */
  auto appDataDirectory() const -> grt::StringRef { return _appDataDirectory; }

  /**
   * Setter for attribute appDataDirectory
   * 
   * 
   * \par In Python:
   *   obj.appDataDirectory = value
   */
  virtual auto appDataDirectory(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_appDataDirectory);
    _appDataDirectory = value;
    member_changed("appDataDirectory", ovalue, value);
  }

  /**
   * Getter for attribute appExecutablePath
   *
   * 
   * \par In Python:
   *    value = obj.appExecutablePath
   */
  auto appExecutablePath() const -> grt::StringRef { return _appExecutablePath; }

  /**
   * Setter for attribute appExecutablePath
   * 
   * 
   * \par In Python:
   *   obj.appExecutablePath = value
   */
  virtual auto appExecutablePath(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_appExecutablePath);
    _appExecutablePath = value;
    member_changed("appExecutablePath", ovalue, value);
  }

  // customDataFields is owned by app_Registry
  /**
   * Getter for attribute customDataFields (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.customDataFields
   */
  auto customDataFields() const -> grt::ListRef<app_CustomDataField> { return _customDataFields; }


private: // The next attribute is read-only.
  virtual auto customDataFields(const grt::ListRef<app_CustomDataField> &value) -> void {
    grt::ValueRef ovalue(_customDataFields);

    _customDataFields = value;
    owned_member_changed("customDataFields", ovalue, value);
  }
public:

  // pluginGroups is owned by app_Registry
  /**
   * Getter for attribute pluginGroups (read-only)
   *
   * the list of available plugin groups
   * \par In Python:
   *    value = obj.pluginGroups
   */
  auto pluginGroups() const -> grt::ListRef<app_PluginGroup> { return _pluginGroups; }


private: // The next attribute is read-only.
  virtual auto pluginGroups(const grt::ListRef<app_PluginGroup> &value) -> void {
    grt::ValueRef ovalue(_pluginGroups);

    _pluginGroups = value;
    owned_member_changed("pluginGroups", ovalue, value);
  }
public:

  // plugins is owned by app_Registry
  /**
   * Getter for attribute plugins (read-only)
   *
   * the list of available plugins
   * \par In Python:
   *    value = obj.plugins
   */
  auto plugins() const -> grt::ListRef<app_Plugin> { return _plugins; }


private: // The next attribute is read-only.
  virtual auto plugins(const grt::ListRef<app_Plugin> &value) -> void {
    grt::ValueRef ovalue(_plugins);

    _plugins = value;
    owned_member_changed("plugins", ovalue, value);
  }
public:

protected:

  grt::StringRef _appDataDirectory;
  grt::StringRef _appExecutablePath;
  grt::ListRef<app_CustomDataField> _customDataFields;// owned
  grt::ListRef<app_PluginGroup> _pluginGroups;// owned
  grt::ListRef<app_Plugin> _plugins;// owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_Registry());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_Registry::create);
    {
      void (app_Registry::*setter)(const grt::StringRef &) = &app_Registry::appDataDirectory;
      grt::StringRef (app_Registry::*getter)() const = &app_Registry::appDataDirectory;
      meta->bind_member("appDataDirectory", new grt::MetaClass::Property<app_Registry,grt::StringRef>(getter, setter));
    }
    {
      void (app_Registry::*setter)(const grt::StringRef &) = &app_Registry::appExecutablePath;
      grt::StringRef (app_Registry::*getter)() const = &app_Registry::appExecutablePath;
      meta->bind_member("appExecutablePath", new grt::MetaClass::Property<app_Registry,grt::StringRef>(getter, setter));
    }
    {
      void (app_Registry::*setter)(const grt::ListRef<app_CustomDataField> &) = &app_Registry::customDataFields;
      grt::ListRef<app_CustomDataField> (app_Registry::*getter)() const = &app_Registry::customDataFields;
      meta->bind_member("customDataFields", new grt::MetaClass::Property<app_Registry,grt::ListRef<app_CustomDataField>>(getter, setter));
    }
    {
      void (app_Registry::*setter)(const grt::ListRef<app_PluginGroup> &) = &app_Registry::pluginGroups;
      grt::ListRef<app_PluginGroup> (app_Registry::*getter)() const = &app_Registry::pluginGroups;
      meta->bind_member("pluginGroups", new grt::MetaClass::Property<app_Registry,grt::ListRef<app_PluginGroup>>(getter, setter));
    }
    {
      void (app_Registry::*setter)(const grt::ListRef<app_Plugin> &) = &app_Registry::plugins;
      grt::ListRef<app_Plugin> (app_Registry::*getter)() const = &app_Registry::plugins;
      meta->bind_member("plugins", new grt::MetaClass::Property<app_Registry,grt::ListRef<app_Plugin>>(getter, setter));
    }
  }
};

class  app_Starter : public GrtObject {
  typedef GrtObject super;

public:
  app_Starter(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _authorHome(""),
      _command(""),
      _description(""),
      _edition(""),
      _introduction(""),
      _largeIcon(""),
      _publisher(""),
      _smallIcon("0"),
      _title(""),
      _type("") {
  }

  static auto static_class_name() -> std::string {
    return "app.Starter";
  }

  /**
   * Getter for attribute authorHome
   *
   * 
   * \par In Python:
   *    value = obj.authorHome
   */
  auto authorHome() const -> grt::StringRef { return _authorHome; }

  /**
   * Setter for attribute authorHome
   * 
   * 
   * \par In Python:
   *   obj.authorHome = value
   */
  virtual auto authorHome(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_authorHome);
    _authorHome = value;
    member_changed("authorHome", ovalue, value);
  }

  /**
   * Getter for attribute command
   *
   * 
   * \par In Python:
   *    value = obj.command
   */
  auto command() const -> grt::StringRef { return _command; }

  /**
   * Setter for attribute command
   * 
   * 
   * \par In Python:
   *   obj.command = value
   */
  virtual auto command(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_command);
    _command = value;
    member_changed("command", ovalue, value);
  }

  /**
   * Getter for attribute description
   *
   * 
   * \par In Python:
   *    value = obj.description
   */
  auto description() const -> grt::StringRef { return _description; }

  /**
   * Setter for attribute description
   * 
   * 
   * \par In Python:
   *   obj.description = value
   */
  virtual auto description(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_description);
    _description = value;
    member_changed("description", ovalue, value);
  }

  /**
   * Getter for attribute edition
   *
   * 
   * \par In Python:
   *    value = obj.edition
   */
  auto edition() const -> grt::StringRef { return _edition; }

  /**
   * Setter for attribute edition
   * 
   * 
   * \par In Python:
   *   obj.edition = value
   */
  virtual auto edition(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_edition);
    _edition = value;
    member_changed("edition", ovalue, value);
  }

  /**
   * Getter for attribute introduction
   *
   * 
   * \par In Python:
   *    value = obj.introduction
   */
  auto introduction() const -> grt::StringRef { return _introduction; }

  /**
   * Setter for attribute introduction
   * 
   * 
   * \par In Python:
   *   obj.introduction = value
   */
  virtual auto introduction(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_introduction);
    _introduction = value;
    member_changed("introduction", ovalue, value);
  }

  /**
   * Getter for attribute largeIcon
   *
   * 
   * \par In Python:
   *    value = obj.largeIcon
   */
  auto largeIcon() const -> grt::StringRef { return _largeIcon; }

  /**
   * Setter for attribute largeIcon
   * 
   * 
   * \par In Python:
   *   obj.largeIcon = value
   */
  virtual auto largeIcon(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_largeIcon);
    _largeIcon = value;
    member_changed("largeIcon", ovalue, value);
  }

  /**
   * Getter for attribute publisher
   *
   * 
   * \par In Python:
   *    value = obj.publisher
   */
  auto publisher() const -> grt::StringRef { return _publisher; }

  /**
   * Setter for attribute publisher
   * 
   * 
   * \par In Python:
   *   obj.publisher = value
   */
  virtual auto publisher(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_publisher);
    _publisher = value;
    member_changed("publisher", ovalue, value);
  }

  /**
   * Getter for attribute smallIcon
   *
   * 
   * \par In Python:
   *    value = obj.smallIcon
   */
  auto smallIcon() const -> grt::StringRef { return _smallIcon; }

  /**
   * Setter for attribute smallIcon
   * 
   * 
   * \par In Python:
   *   obj.smallIcon = value
   */
  virtual auto smallIcon(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_smallIcon);
    _smallIcon = value;
    member_changed("smallIcon", ovalue, value);
  }

  /**
   * Getter for attribute title
   *
   * 
   * \par In Python:
   *    value = obj.title
   */
  auto title() const -> grt::StringRef { return _title; }

  /**
   * Setter for attribute title
   * 
   * 
   * \par In Python:
   *   obj.title = value
   */
  virtual auto title(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_title);
    _title = value;
    member_changed("title", ovalue, value);
  }

  /**
   * Getter for attribute type
   *
   * 
   * \par In Python:
   *    value = obj.type
   */
  auto type() const -> grt::StringRef { return _type; }

  /**
   * Setter for attribute type
   * 
   * 
   * \par In Python:
   *   obj.type = value
   */
  virtual auto type(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_type);
    _type = value;
    member_changed("type", ovalue, value);
  }

protected:

  grt::StringRef _authorHome;
  grt::StringRef _command;
  grt::StringRef _description;
  grt::StringRef _edition;
  grt::StringRef _introduction;
  grt::StringRef _largeIcon;
  grt::StringRef _publisher;
  grt::StringRef _smallIcon;
  grt::StringRef _title;
  grt::StringRef _type;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_Starter());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_Starter::create);
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::authorHome;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::authorHome;
      meta->bind_member("authorHome", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::command;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::command;
      meta->bind_member("command", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::description;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::description;
      meta->bind_member("description", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::edition;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::edition;
      meta->bind_member("edition", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::introduction;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::introduction;
      meta->bind_member("introduction", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::largeIcon;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::largeIcon;
      meta->bind_member("largeIcon", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::publisher;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::publisher;
      meta->bind_member("publisher", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::smallIcon;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::smallIcon;
      meta->bind_member("smallIcon", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::title;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::title;
      meta->bind_member("title", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
    {
      void (app_Starter::*setter)(const grt::StringRef &) = &app_Starter::type;
      grt::StringRef (app_Starter::*getter)() const = &app_Starter::type;
      meta->bind_member("type", new grt::MetaClass::Property<app_Starter,grt::StringRef>(getter, setter));
    }
  }
};

/** Stores all defined home screen starters. */
class  app_Starters : public GrtObject {
  typedef GrtObject super;

public:
  app_Starters(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _custom(this, false),
      _displayList(this, false),
      _predefined(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "app.Starters";
  }

  // custom is owned by app_Starters
  /**
   * Getter for attribute custom (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.custom
   */
  auto custom() const -> grt::ListRef<app_Starter> { return _custom; }


private: // The next attribute is read-only.
  virtual auto custom(const grt::ListRef<app_Starter> &value) -> void {
    grt::ValueRef ovalue(_custom);

    _custom = value;
    owned_member_changed("custom", ovalue, value);
  }
public:

  /**
   * Getter for attribute displayList (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.displayList
   */
  auto displayList() const -> grt::ListRef<app_Starter> { return _displayList; }


private: // The next attribute is read-only.
  virtual auto displayList(const grt::ListRef<app_Starter> &value) -> void {
    grt::ValueRef ovalue(_displayList);
    _displayList = value;
    member_changed("displayList", ovalue, value);
  }
public:

  // predefined is owned by app_Starters
  /**
   * Getter for attribute predefined (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.predefined
   */
  auto predefined() const -> grt::ListRef<app_Starter> { return _predefined; }


private: // The next attribute is read-only.
  virtual auto predefined(const grt::ListRef<app_Starter> &value) -> void {
    grt::ValueRef ovalue(_predefined);

    _predefined = value;
    owned_member_changed("predefined", ovalue, value);
  }
public:

protected:

  grt::ListRef<app_Starter> _custom;// owned
  grt::ListRef<app_Starter> _displayList;
  grt::ListRef<app_Starter> _predefined;// owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_Starters());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_Starters::create);
    {
      void (app_Starters::*setter)(const grt::ListRef<app_Starter> &) = &app_Starters::custom;
      grt::ListRef<app_Starter> (app_Starters::*getter)() const = &app_Starters::custom;
      meta->bind_member("custom", new grt::MetaClass::Property<app_Starters,grt::ListRef<app_Starter>>(getter, setter));
    }
    {
      void (app_Starters::*setter)(const grt::ListRef<app_Starter> &) = &app_Starters::displayList;
      grt::ListRef<app_Starter> (app_Starters::*getter)() const = &app_Starters::displayList;
      meta->bind_member("displayList", new grt::MetaClass::Property<app_Starters,grt::ListRef<app_Starter>>(getter, setter));
    }
    {
      void (app_Starters::*setter)(const grt::ListRef<app_Starter> &) = &app_Starters::predefined;
      grt::ListRef<app_Starter> (app_Starters::*getter)() const = &app_Starters::predefined;
      meta->bind_member("predefined", new grt::MetaClass::Property<app_Starters,grt::ListRef<app_Starter>>(getter, setter));
    }
  }
};

/** stores the application's options */
class  app_Options : public GrtObject {
  typedef GrtObject super;

public:
  app_Options(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _commonOptions(this, false),
      _disabledPlugins(this, false),
      _options(this, false),
      _paperTypes(this, false),
      _recentFiles(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "app.Options";
  }

  /**
   * Getter for attribute commonOptions (read-only)
   *
   * stores options that are shared between applications
   * \par In Python:
   *    value = obj.commonOptions
   */
  auto commonOptions() const -> grt::DictRef { return _commonOptions; }


private: // The next attribute is read-only.
  virtual auto commonOptions(const grt::DictRef &value) -> void {
    grt::ValueRef ovalue(_commonOptions);
    _commonOptions = value;
    member_changed("commonOptions", ovalue, value);
  }
public:

  /**
   * Getter for attribute disabledPlugins (read-only)
   *
   * list of plugin names that are disabled
   * \par In Python:
   *    value = obj.disabledPlugins
   */
  auto disabledPlugins() const -> grt::StringListRef { return _disabledPlugins; }


private: // The next attribute is read-only.
  virtual auto disabledPlugins(const grt::StringListRef &value) -> void {
    grt::ValueRef ovalue(_disabledPlugins);
    _disabledPlugins = value;
    member_changed("disabledPlugins", ovalue, value);
  }
public:

  /**
   * Getter for attribute options (read-only)
   *
   * stores application specific options
   * \par In Python:
   *    value = obj.options
   */
  auto options() const -> grt::DictRef { return _options; }


private: // The next attribute is read-only.
  virtual auto options(const grt::DictRef &value) -> void {
    grt::ValueRef ovalue(_options);
    _options = value;
    member_changed("options", ovalue, value);
  }
public:

  // paperTypes is owned by app_Options
  /**
   * Getter for attribute paperTypes (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.paperTypes
   */
  auto paperTypes() const -> grt::ListRef<app_PaperType> { return _paperTypes; }


private: // The next attribute is read-only.
  virtual auto paperTypes(const grt::ListRef<app_PaperType> &value) -> void {
    grt::ValueRef ovalue(_paperTypes);

    _paperTypes = value;
    owned_member_changed("paperTypes", ovalue, value);
  }
public:

  /**
   * Getter for attribute recentFiles (read-only)
   *
   * recently opened files
   * \par In Python:
   *    value = obj.recentFiles
   */
  auto recentFiles() const -> grt::StringListRef { return _recentFiles; }


private: // The next attribute is read-only.
  virtual auto recentFiles(const grt::StringListRef &value) -> void {
    grt::ValueRef ovalue(_recentFiles);
    _recentFiles = value;
    member_changed("recentFiles", ovalue, value);
  }
public:

protected:

  grt::DictRef _commonOptions;
  grt::StringListRef _disabledPlugins;
  grt::DictRef _options;
  grt::ListRef<app_PaperType> _paperTypes;// owned
  grt::StringListRef _recentFiles;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_Options());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_Options::create);
    {
      void (app_Options::*setter)(const grt::DictRef &) = &app_Options::commonOptions;
      grt::DictRef (app_Options::*getter)() const = &app_Options::commonOptions;
      meta->bind_member("commonOptions", new grt::MetaClass::Property<app_Options,grt::DictRef>(getter, setter));
    }
    {
      void (app_Options::*setter)(const grt::StringListRef &) = &app_Options::disabledPlugins;
      grt::StringListRef (app_Options::*getter)() const = &app_Options::disabledPlugins;
      meta->bind_member("disabledPlugins", new grt::MetaClass::Property<app_Options,grt::StringListRef>(getter, setter));
    }
    {
      void (app_Options::*setter)(const grt::DictRef &) = &app_Options::options;
      grt::DictRef (app_Options::*getter)() const = &app_Options::options;
      meta->bind_member("options", new grt::MetaClass::Property<app_Options,grt::DictRef>(getter, setter));
    }
    {
      void (app_Options::*setter)(const grt::ListRef<app_PaperType> &) = &app_Options::paperTypes;
      grt::ListRef<app_PaperType> (app_Options::*getter)() const = &app_Options::paperTypes;
      meta->bind_member("paperTypes", new grt::MetaClass::Property<app_Options,grt::ListRef<app_PaperType>>(getter, setter));
    }
    {
      void (app_Options::*setter)(const grt::StringListRef &) = &app_Options::recentFiles;
      grt::StringListRef (app_Options::*getter)() const = &app_Options::recentFiles;
      meta->bind_member("recentFiles", new grt::MetaClass::Property<app_Options,grt::StringListRef>(getter, setter));
    }
  }
};

/** information about the document */
class  app_DocumentInfo : public GrtObject {
  typedef GrtObject super;

public:
  app_DocumentInfo(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _author(""),
      _caption(""),
      _dateChanged(""),
      _dateCreated(""),
      _description(""),
      _project(""),
      _version("") {
  }

  static auto static_class_name() -> std::string {
    return "app.DocumentInfo";
  }

  /**
   * Getter for attribute author
   *
   * Author of the document
   * \par In Python:
   *    value = obj.author
   */
  auto author() const -> grt::StringRef { return _author; }

  /**
   * Setter for attribute author
   * 
   * Author of the document
   * \par In Python:
   *   obj.author = value
   */
  virtual auto author(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_author);
    _author = value;
    member_changed("author", ovalue, value);
  }

  /**
   * Getter for attribute caption
   *
   * Caption of the document
   * \par In Python:
   *    value = obj.caption
   */
  auto caption() const -> grt::StringRef { return _caption; }

  /**
   * Setter for attribute caption
   * 
   * Caption of the document
   * \par In Python:
   *   obj.caption = value
   */
  virtual auto caption(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_caption);
    _caption = value;
    member_changed("caption", ovalue, value);
  }

  /**
   * Getter for attribute dateChanged
   *
   * Date of last modification of the document
   * \par In Python:
   *    value = obj.dateChanged
   */
  auto dateChanged() const -> grt::StringRef { return _dateChanged; }

  /**
   * Setter for attribute dateChanged
   * 
   * Date of last modification of the document
   * \par In Python:
   *   obj.dateChanged = value
   */
  virtual auto dateChanged(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_dateChanged);
    _dateChanged = value;
    member_changed("dateChanged", ovalue, value);
  }

  /**
   * Getter for attribute dateCreated
   *
   * Date of creation of the document
   * \par In Python:
   *    value = obj.dateCreated
   */
  auto dateCreated() const -> grt::StringRef { return _dateCreated; }

  /**
   * Setter for attribute dateCreated
   * 
   * Date of creation of the document
   * \par In Python:
   *   obj.dateCreated = value
   */
  virtual auto dateCreated(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_dateCreated);
    _dateCreated = value;
    member_changed("dateCreated", ovalue, value);
  }

  /**
   * Getter for attribute description
   *
   * Description/comments for the document
   * \par In Python:
   *    value = obj.description
   */
  auto description() const -> grt::StringRef { return _description; }

  /**
   * Setter for attribute description
   * 
   * Description/comments for the document
   * \par In Python:
   *   obj.description = value
   */
  virtual auto description(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_description);
    _description = value;
    member_changed("description", ovalue, value);
  }

  /**
   * Getter for attribute project
   *
   * Name of the project
   * \par In Python:
   *    value = obj.project
   */
  auto project() const -> grt::StringRef { return _project; }

  /**
   * Setter for attribute project
   * 
   * Name of the project
   * \par In Python:
   *   obj.project = value
   */
  virtual auto project(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_project);
    _project = value;
    member_changed("project", ovalue, value);
  }

  /**
   * Getter for attribute version
   *
   * Version of the document
   * \par In Python:
   *    value = obj.version
   */
  auto version() const -> grt::StringRef { return _version; }

  /**
   * Setter for attribute version
   * 
   * Version of the document
   * \par In Python:
   *   obj.version = value
   */
  virtual auto version(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_version);
    _version = value;
    member_changed("version", ovalue, value);
  }

protected:

  grt::StringRef _author;
  grt::StringRef _caption;
  grt::StringRef _dateChanged;
  grt::StringRef _dateCreated;
  grt::StringRef _description;
  grt::StringRef _project;
  grt::StringRef _version;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_DocumentInfo());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_DocumentInfo::create);
    {
      void (app_DocumentInfo::*setter)(const grt::StringRef &) = &app_DocumentInfo::author;
      grt::StringRef (app_DocumentInfo::*getter)() const = &app_DocumentInfo::author;
      meta->bind_member("author", new grt::MetaClass::Property<app_DocumentInfo,grt::StringRef>(getter, setter));
    }
    {
      void (app_DocumentInfo::*setter)(const grt::StringRef &) = &app_DocumentInfo::caption;
      grt::StringRef (app_DocumentInfo::*getter)() const = &app_DocumentInfo::caption;
      meta->bind_member("caption", new grt::MetaClass::Property<app_DocumentInfo,grt::StringRef>(getter, setter));
    }
    {
      void (app_DocumentInfo::*setter)(const grt::StringRef &) = &app_DocumentInfo::dateChanged;
      grt::StringRef (app_DocumentInfo::*getter)() const = &app_DocumentInfo::dateChanged;
      meta->bind_member("dateChanged", new grt::MetaClass::Property<app_DocumentInfo,grt::StringRef>(getter, setter));
    }
    {
      void (app_DocumentInfo::*setter)(const grt::StringRef &) = &app_DocumentInfo::dateCreated;
      grt::StringRef (app_DocumentInfo::*getter)() const = &app_DocumentInfo::dateCreated;
      meta->bind_member("dateCreated", new grt::MetaClass::Property<app_DocumentInfo,grt::StringRef>(getter, setter));
    }
    {
      void (app_DocumentInfo::*setter)(const grt::StringRef &) = &app_DocumentInfo::description;
      grt::StringRef (app_DocumentInfo::*getter)() const = &app_DocumentInfo::description;
      meta->bind_member("description", new grt::MetaClass::Property<app_DocumentInfo,grt::StringRef>(getter, setter));
    }
    {
      void (app_DocumentInfo::*setter)(const grt::StringRef &) = &app_DocumentInfo::project;
      grt::StringRef (app_DocumentInfo::*getter)() const = &app_DocumentInfo::project;
      meta->bind_member("project", new grt::MetaClass::Property<app_DocumentInfo,grt::StringRef>(getter, setter));
    }
    {
      void (app_DocumentInfo::*setter)(const grt::StringRef &) = &app_DocumentInfo::version;
      grt::StringRef (app_DocumentInfo::*getter)() const = &app_DocumentInfo::version;
      meta->bind_member("version", new grt::MetaClass::Property<app_DocumentInfo,grt::StringRef>(getter, setter));
    }
  }
};

/** information about the application */
class  app_Info : public GrtObject {
  typedef GrtObject super;

public:
  app_Info(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _caption(""),
      _copyright(""),
      _description(""),
      _edition(""),
      _license("") {
  }

  static auto static_class_name() -> std::string {
    return "app.Info";
  }

  /**
   * Getter for attribute caption
   *
   * the application's caption
   * \par In Python:
   *    value = obj.caption
   */
  auto caption() const -> grt::StringRef { return _caption; }

  /**
   * Setter for attribute caption
   * 
   * the application's caption
   * \par In Python:
   *   obj.caption = value
   */
  virtual auto caption(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_caption);
    _caption = value;
    member_changed("caption", ovalue, value);
  }

  /**
   * Getter for attribute copyright
   *
   * the copyright message
   * \par In Python:
   *    value = obj.copyright
   */
  auto copyright() const -> grt::StringRef { return _copyright; }

  /**
   * Setter for attribute copyright
   * 
   * the copyright message
   * \par In Python:
   *   obj.copyright = value
   */
  virtual auto copyright(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_copyright);
    _copyright = value;
    member_changed("copyright", ovalue, value);
  }

  /**
   * Getter for attribute description
   *
   * a short description of the application
   * \par In Python:
   *    value = obj.description
   */
  auto description() const -> grt::StringRef { return _description; }

  /**
   * Setter for attribute description
   * 
   * a short description of the application
   * \par In Python:
   *   obj.description = value
   */
  virtual auto description(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_description);
    _description = value;
    member_changed("description", ovalue, value);
  }

  /**
   * Getter for attribute edition
   *
   * the edition name
   * \par In Python:
   *    value = obj.edition
   */
  auto edition() const -> grt::StringRef { return _edition; }

  /**
   * Setter for attribute edition
   * 
   * the edition name
   * \par In Python:
   *   obj.edition = value
   */
  virtual auto edition(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_edition);
    _edition = value;
    member_changed("edition", ovalue, value);
  }

  /**
   * Getter for attribute license
   *
   * the license message
   * \par In Python:
   *    value = obj.license
   */
  auto license() const -> grt::StringRef { return _license; }

  /**
   * Setter for attribute license
   * 
   * the license message
   * \par In Python:
   *   obj.license = value
   */
  virtual auto license(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_license);
    _license = value;
    member_changed("license", ovalue, value);
  }

  // version is owned by app_Info
  /**
   * Getter for attribute version
   *
   * the version of the application
   * \par In Python:
   *    value = obj.version
   */
  auto version() const -> GrtVersionRef { return _version; }

  /**
   * Setter for attribute version
   * 
   * the version of the application
   * \par In Python:
   *   obj.version = value
   */
  virtual auto version(const GrtVersionRef &value) -> void {
    grt::ValueRef ovalue(_version);

    _version = value;
    owned_member_changed("version", ovalue, value);
  }

protected:

  grt::StringRef _caption;
  grt::StringRef _copyright;
  grt::StringRef _description;
  grt::StringRef _edition;
  grt::StringRef _license;
  GrtVersionRef _version;// owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_Info());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_Info::create);
    {
      void (app_Info::*setter)(const grt::StringRef &) = &app_Info::caption;
      grt::StringRef (app_Info::*getter)() const = &app_Info::caption;
      meta->bind_member("caption", new grt::MetaClass::Property<app_Info,grt::StringRef>(getter, setter));
    }
    {
      void (app_Info::*setter)(const grt::StringRef &) = &app_Info::copyright;
      grt::StringRef (app_Info::*getter)() const = &app_Info::copyright;
      meta->bind_member("copyright", new grt::MetaClass::Property<app_Info,grt::StringRef>(getter, setter));
    }
    {
      void (app_Info::*setter)(const grt::StringRef &) = &app_Info::description;
      grt::StringRef (app_Info::*getter)() const = &app_Info::description;
      meta->bind_member("description", new grt::MetaClass::Property<app_Info,grt::StringRef>(getter, setter));
    }
    {
      void (app_Info::*setter)(const grt::StringRef &) = &app_Info::edition;
      grt::StringRef (app_Info::*getter)() const = &app_Info::edition;
      meta->bind_member("edition", new grt::MetaClass::Property<app_Info,grt::StringRef>(getter, setter));
    }
    {
      void (app_Info::*setter)(const grt::StringRef &) = &app_Info::license;
      grt::StringRef (app_Info::*getter)() const = &app_Info::license;
      meta->bind_member("license", new grt::MetaClass::Property<app_Info,grt::StringRef>(getter, setter));
    }
    {
      void (app_Info::*setter)(const GrtVersionRef &) = &app_Info::version;
      GrtVersionRef (app_Info::*getter)() const = &app_Info::version;
      meta->bind_member("version", new grt::MetaClass::Property<app_Info,GrtVersionRef>(getter, setter));
    }
  }
};

/** information about the application */
class  app_Document : public GrtObject {
  typedef GrtObject super;

public:
  app_Document(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _customData(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "app.Document";
  }

  /**
   * Getter for attribute customData (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.customData
   */
  auto customData() const -> grt::DictRef { return _customData; }


private: // The next attribute is read-only.
  virtual auto customData(const grt::DictRef &value) -> void {
    grt::ValueRef ovalue(_customData);
    _customData = value;
    member_changed("customData", ovalue, value);
  }
public:

  // info is owned by app_Document
  /**
   * Getter for attribute info
   *
   * user supplied info about the document
   * \par In Python:
   *    value = obj.info
   */
  auto info() const -> app_DocumentInfoRef { return _info; }

  /**
   * Setter for attribute info
   * 
   * user supplied info about the document
   * \par In Python:
   *   obj.info = value
   */
  virtual auto info(const app_DocumentInfoRef &value) -> void {
    grt::ValueRef ovalue(_info);

    _info = value;
    owned_member_changed("info", ovalue, value);
  }

  // pageSettings is owned by app_Document
  /**
   * Getter for attribute pageSettings
   *
   * 
   * \par In Python:
   *    value = obj.pageSettings
   */
  auto pageSettings() const -> app_PageSettingsRef { return _pageSettings; }

  /**
   * Setter for attribute pageSettings
   * 
   * 
   * \par In Python:
   *   obj.pageSettings = value
   */
  virtual auto pageSettings(const app_PageSettingsRef &value) -> void {
    grt::ValueRef ovalue(_pageSettings);

    _pageSettings = value;
    owned_member_changed("pageSettings", ovalue, value);
  }

protected:

  grt::DictRef _customData;
  app_DocumentInfoRef _info;// owned
  app_PageSettingsRef _pageSettings;// owned

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_Document());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_Document::create);
    {
      void (app_Document::*setter)(const grt::DictRef &) = &app_Document::customData;
      grt::DictRef (app_Document::*getter)() const = &app_Document::customData;
      meta->bind_member("customData", new grt::MetaClass::Property<app_Document,grt::DictRef>(getter, setter));
    }
    {
      void (app_Document::*setter)(const app_DocumentInfoRef &) = &app_Document::info;
      app_DocumentInfoRef (app_Document::*getter)() const = &app_Document::info;
      meta->bind_member("info", new grt::MetaClass::Property<app_Document,app_DocumentInfoRef>(getter, setter));
    }
    {
      void (app_Document::*setter)(const app_PageSettingsRef &) = &app_Document::pageSettings;
      app_PageSettingsRef (app_Document::*getter)() const = &app_Document::pageSettings;
      meta->bind_member("pageSettings", new grt::MetaClass::Property<app_Document,app_PageSettingsRef>(getter, setter));
    }
  }
};

/** a GRT application object */
class  app_Application : public GrtObject {
  typedef GrtObject super;

public:
  app_Application(grt::MetaClass *meta = nullptr)
    : GrtObject(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _customData(this, false),
      _state(this, false) {
  }

  static auto static_class_name() -> std::string {
    return "app.Application";
  }

  /**
   * Getter for attribute customData (read-only)
   *
   * a generic dictionary to hold additional information used by e.g. plugins
   * \par In Python:
   *    value = obj.customData
   */
  auto customData() const -> grt::DictRef { return _customData; }


private: // The next attribute is read-only.
  virtual auto customData(const grt::DictRef &value) -> void {
    grt::ValueRef ovalue(_customData);
    _customData = value;
    member_changed("customData", ovalue, value);
  }
public:

  // doc is owned by app_Application
  /**
   * Getter for attribute doc
   *
   * the document the application is working with
   * \par In Python:
   *    value = obj.doc
   */
  auto doc() const -> app_DocumentRef { return _doc; }

  /**
   * Setter for attribute doc
   * 
   * the document the application is working with
   * \par In Python:
   *   obj.doc = value
   */
  virtual auto doc(const app_DocumentRef &value) -> void {
    grt::ValueRef ovalue(_doc);

    _doc = value;
    owned_member_changed("doc", ovalue, value);
  }

  /**
   * Getter for attribute info
   *
   * information about the application
   * \par In Python:
   *    value = obj.info
   */
  auto info() const -> app_InfoRef { return _info; }

  /**
   * Setter for attribute info
   * 
   * information about the application
   * \par In Python:
   *   obj.info = value
   */
  virtual auto info(const app_InfoRef &value) -> void {
    grt::ValueRef ovalue(_info);
    _info = value;
    member_changed("info", ovalue, value);
  }

  /**
   * Getter for attribute options
   *
   * application options
   * \par In Python:
   *    value = obj.options
   */
  auto options() const -> app_OptionsRef { return _options; }

  /**
   * Setter for attribute options
   * 
   * application options
   * \par In Python:
   *   obj.options = value
   */
  virtual auto options(const app_OptionsRef &value) -> void {
    grt::ValueRef ovalue(_options);
    _options = value;
    member_changed("options", ovalue, value);
  }

  /**
   * Getter for attribute registry
   *
   * information about the application
   * \par In Python:
   *    value = obj.registry
   */
  auto registry() const -> app_RegistryRef { return _registry; }

  /**
   * Setter for attribute registry
   * 
   * information about the application
   * \par In Python:
   *   obj.registry = value
   */
  virtual auto registry(const app_RegistryRef &value) -> void {
    grt::ValueRef ovalue(_registry);
    _registry = value;
    member_changed("registry", ovalue, value);
  }

  /**
   * Getter for attribute starters
   *
   * Application starters
   * \par In Python:
   *    value = obj.starters
   */
  auto starters() const -> app_StartersRef { return _starters; }

  /**
   * Setter for attribute starters
   * 
   * Application starters
   * \par In Python:
   *   obj.starters = value
   */
  virtual auto starters(const app_StartersRef &value) -> void {
    grt::ValueRef ovalue(_starters);
    _starters = value;
    member_changed("starters", ovalue, value);
  }

  /**
   * Getter for attribute state (read-only)
   *
   * application state info, keys in format domain:option
   * \par In Python:
   *    value = obj.state
   */
  auto state() const -> grt::DictRef { return _state; }


private: // The next attribute is read-only.
  virtual auto state(const grt::DictRef &value) -> void {
    grt::ValueRef ovalue(_state);
    _state = value;
    member_changed("state", ovalue, value);
  }
public:

protected:

  grt::DictRef _customData;
  app_DocumentRef _doc;// owned
  app_InfoRef _info;
  app_OptionsRef _options;
  app_RegistryRef _registry;
  app_StartersRef _starters;
  grt::DictRef _state;

private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new app_Application());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&app_Application::create);
    {
      void (app_Application::*setter)(const grt::DictRef &) = &app_Application::customData;
      grt::DictRef (app_Application::*getter)() const = &app_Application::customData;
      meta->bind_member("customData", new grt::MetaClass::Property<app_Application,grt::DictRef>(getter, setter));
    }
    {
      void (app_Application::*setter)(const app_DocumentRef &) = &app_Application::doc;
      app_DocumentRef (app_Application::*getter)() const = &app_Application::doc;
      meta->bind_member("doc", new grt::MetaClass::Property<app_Application,app_DocumentRef>(getter, setter));
    }
    {
      void (app_Application::*setter)(const app_InfoRef &) = &app_Application::info;
      app_InfoRef (app_Application::*getter)() const = &app_Application::info;
      meta->bind_member("info", new grt::MetaClass::Property<app_Application,app_InfoRef>(getter, setter));
    }
    {
      void (app_Application::*setter)(const app_OptionsRef &) = &app_Application::options;
      app_OptionsRef (app_Application::*getter)() const = &app_Application::options;
      meta->bind_member("options", new grt::MetaClass::Property<app_Application,app_OptionsRef>(getter, setter));
    }
    {
      void (app_Application::*setter)(const app_RegistryRef &) = &app_Application::registry;
      app_RegistryRef (app_Application::*getter)() const = &app_Application::registry;
      meta->bind_member("registry", new grt::MetaClass::Property<app_Application,app_RegistryRef>(getter, setter));
    }
    {
      void (app_Application::*setter)(const app_StartersRef &) = &app_Application::starters;
      app_StartersRef (app_Application::*getter)() const = &app_Application::starters;
      meta->bind_member("starters", new grt::MetaClass::Property<app_Application,app_StartersRef>(getter, setter));
    }
    {
      void (app_Application::*setter)(const grt::DictRef &) = &app_Application::state;
      grt::DictRef (app_Application::*getter)() const = &app_Application::state;
      meta->bind_member("state", new grt::MetaClass::Property<app_Application,grt::DictRef>(getter, setter));
    }
  }
};



inline auto register_structs_app_xml() -> void {
  grt::internal::ClassRegistry::register_class<app_PluginInputDefinition>();
  grt::internal::ClassRegistry::register_class<app_PluginObjectInput>();
  grt::internal::ClassRegistry::register_class<app_PluginFileInput>();
  grt::internal::ClassRegistry::register_class<app_PluginSelectionInput>();
  grt::internal::ClassRegistry::register_class<app_Plugin>();
  grt::internal::ClassRegistry::register_class<app_DocumentPlugin>();
  grt::internal::ClassRegistry::register_class<app_PluginGroup>();
  grt::internal::ClassRegistry::register_class<app_Toolbar>();
  grt::internal::ClassRegistry::register_class<app_CommandItem>();
  grt::internal::ClassRegistry::register_class<app_ToolbarItem>();
  grt::internal::ClassRegistry::register_class<app_ShortcutItem>();
  grt::internal::ClassRegistry::register_class<app_MenuItem>();
  grt::internal::ClassRegistry::register_class<app_CustomDataField>();
  grt::internal::ClassRegistry::register_class<app_PageSettings>();
  grt::internal::ClassRegistry::register_class<app_PaperType>();
  grt::internal::ClassRegistry::register_class<app_Registry>();
  grt::internal::ClassRegistry::register_class<app_Starter>();
  grt::internal::ClassRegistry::register_class<app_Starters>();
  grt::internal::ClassRegistry::register_class<app_Options>();
  grt::internal::ClassRegistry::register_class<app_DocumentInfo>();
  grt::internal::ClassRegistry::register_class<app_Info>();
  grt::internal::ClassRegistry::register_class<app_Document>();
  grt::internal::ClassRegistry::register_class<app_Application>();
}

#ifdef AUTO_REGISTER_GRT_CLASSES
static struct _autoreg__structs_app_xml {
  _autoreg__structs_app_xml() {
    register_structs_app_xml();
  }
} __autoreg__structs_app_xml;
#endif

#ifndef _MSC_VER
  #pragma GCC diagnostic pop
#endif

