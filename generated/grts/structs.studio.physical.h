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
  #ifdef GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_EXPORT
  #define GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_PUBLIC __declspec(dllexport)
#else
  #define GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_PUBLIC __declspec(dllimport)
#endif
#else
  #define GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_PUBLIC
#endif

#include "grts/structs.h"
#include "grts/structs.model.h"
#include "grts/structs.meta.h"
#include "grts/structs.db.h"
#include "grts/structs.db.mgmt.h"

class studio_physical_Layer;
typedef grt::Ref<studio_physical_Layer> studio_physical_LayerRef;
class studio_physical_Connection;
typedef grt::Ref<studio_physical_Connection> studio_physical_ConnectionRef;
class studio_physical_RoutineGroupFigure;
typedef grt::Ref<studio_physical_RoutineGroupFigure> studio_physical_RoutineGroupFigureRef;
class studio_physical_ViewFigure;
typedef grt::Ref<studio_physical_ViewFigure> studio_physical_ViewFigureRef;
class studio_physical_TableFigure;
typedef grt::Ref<studio_physical_TableFigure> studio_physical_TableFigureRef;
class studio_physical_Diagram;
typedef grt::Ref<studio_physical_Diagram> studio_physical_DiagramRef;
class studio_physical_Model;
typedef grt::Ref<studio_physical_Model> studio_physical_ModelRef;


namespace mforms { 
  class Object;
}; 

namespace grt { 
  class AutoPyObject;
}; 

class  studio_physical_Layer : public model_Layer {
  typedef model_Layer super;

public:
  class ImplData;
  friend class ImplData;
  studio_physical_Layer(grt::MetaClass *meta = nullptr)
    : model_Layer(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())) {
  }

  static auto static_class_name() -> std::string {
    return "studio.physical.Layer";
  }

protected:


private: // Wrapper methods for use by the grt.
  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_physical_Layer());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_physical_Layer::create);
  }
};

/** a model connection */
class GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_PUBLIC studio_physical_Connection : public model_Connection {
  typedef model_Connection super;

public:
  class ImplData;
  friend class ImplData;
  studio_physical_Connection(grt::MetaClass *meta = nullptr)
    : model_Connection(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _caption(""),
      _captionXOffs(0.0),
      _captionYOffs(0.0),
      _comment(""),
      _endCaptionXOffs(0.0),
      _endCaptionYOffs(0.0),
      _extraCaption(""),
      _extraCaptionXOffs(0.0),
      _extraCaptionYOffs(0.0),
      _middleSegmentOffset(0.0),
      _startCaptionXOffs(0.0),
      _startCaptionYOffs(0.0),
      _data(nullptr) {
  }

  virtual ~studio_physical_Connection();

  static auto static_class_name() -> std::string {
    return "studio.physical.Connection";
  }

  /**
   * Getter for attribute caption
   *
   * center caption
   * \par In Python:
   *    value = obj.caption
   */
  auto caption() const -> grt::StringRef { return _caption; }

  /**
   * Setter for attribute caption
   * 
   * center caption
   * \par In Python:
   *   obj.caption = value
   */
  virtual auto caption(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_caption);
    _caption = value;
    member_changed("caption", ovalue, value);
  }

  /**
   * Getter for attribute captionXOffs
   *
   * X offset of the caption
   * \par In Python:
   *    value = obj.captionXOffs
   */
  auto captionXOffs() const -> grt::DoubleRef { return _captionXOffs; }

  /**
   * Setter for attribute captionXOffs
   * 
   * X offset of the caption
   * \par In Python:
   *   obj.captionXOffs = value
   */
  virtual auto captionXOffs(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_captionXOffs);
    _captionXOffs = value;
    member_changed("captionXOffs", ovalue, value);
  }

  /**
   * Getter for attribute captionYOffs
   *
   * Y offset of the caption
   * \par In Python:
   *    value = obj.captionYOffs
   */
  auto captionYOffs() const -> grt::DoubleRef { return _captionYOffs; }

  /**
   * Setter for attribute captionYOffs
   * 
   * Y offset of the caption
   * \par In Python:
   *   obj.captionYOffs = value
   */
  virtual auto captionYOffs(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_captionYOffs);
    _captionYOffs = value;
    member_changed("captionYOffs", ovalue, value);
  }

  /**
   * Getter for attribute comment
   *
   * a comment about the relationship
   * \par In Python:
   *    value = obj.comment
   */
  auto comment() const -> grt::StringRef { return _comment; }

  /**
   * Setter for attribute comment
   * 
   * a comment about the relationship
   * \par In Python:
   *   obj.comment = value
   */
  virtual auto comment(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_comment);
    _comment = value;
    member_changed("comment", ovalue, value);
  }

  /**
   * Getter for attribute endCaptionXOffs
   *
   * X offset of the end caption
   * \par In Python:
   *    value = obj.endCaptionXOffs
   */
  auto endCaptionXOffs() const -> grt::DoubleRef { return _endCaptionXOffs; }

  /**
   * Setter for attribute endCaptionXOffs
   * 
   * X offset of the end caption
   * \par In Python:
   *   obj.endCaptionXOffs = value
   */
  virtual auto endCaptionXOffs(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_endCaptionXOffs);
    _endCaptionXOffs = value;
    member_changed("endCaptionXOffs", ovalue, value);
  }

  /**
   * Getter for attribute endCaptionYOffs
   *
   * Y offset of the end caption
   * \par In Python:
   *    value = obj.endCaptionYOffs
   */
  auto endCaptionYOffs() const -> grt::DoubleRef { return _endCaptionYOffs; }

  /**
   * Setter for attribute endCaptionYOffs
   * 
   * Y offset of the end caption
   * \par In Python:
   *   obj.endCaptionYOffs = value
   */
  virtual auto endCaptionYOffs(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_endCaptionYOffs);
    _endCaptionYOffs = value;
    member_changed("endCaptionYOffs", ovalue, value);
  }

  /**
   * Getter for attribute extraCaption
   *
   * additional center caption
   * \par In Python:
   *    value = obj.extraCaption
   */
  auto extraCaption() const -> grt::StringRef { return _extraCaption; }

  /**
   * Setter for attribute extraCaption
   * 
   * additional center caption
   * \par In Python:
   *   obj.extraCaption = value
   */
  virtual auto extraCaption(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_extraCaption);
    _extraCaption = value;
    member_changed("extraCaption", ovalue, value);
  }

  /**
   * Getter for attribute extraCaptionXOffs
   *
   * X offset of the caption
   * \par In Python:
   *    value = obj.extraCaptionXOffs
   */
  auto extraCaptionXOffs() const -> grt::DoubleRef { return _extraCaptionXOffs; }

  /**
   * Setter for attribute extraCaptionXOffs
   * 
   * X offset of the caption
   * \par In Python:
   *   obj.extraCaptionXOffs = value
   */
  virtual auto extraCaptionXOffs(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_extraCaptionXOffs);
    _extraCaptionXOffs = value;
    member_changed("extraCaptionXOffs", ovalue, value);
  }

  /**
   * Getter for attribute extraCaptionYOffs
   *
   * Y offset of the caption
   * \par In Python:
   *    value = obj.extraCaptionYOffs
   */
  auto extraCaptionYOffs() const -> grt::DoubleRef { return _extraCaptionYOffs; }

  /**
   * Setter for attribute extraCaptionYOffs
   * 
   * Y offset of the caption
   * \par In Python:
   *   obj.extraCaptionYOffs = value
   */
  virtual auto extraCaptionYOffs(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_extraCaptionYOffs);
    _extraCaptionYOffs = value;
    member_changed("extraCaptionYOffs", ovalue, value);
  }

  /**
   * Getter for attribute foreignKey
   *
   * the foreign key this corresponds to
   * \par In Python:
   *    value = obj.foreignKey
   */
  auto foreignKey() const -> db_ForeignKeyRef { return _foreignKey; }

  /**
   * Setter for attribute foreignKey
   * 
   * the foreign key this corresponds to
   * \par In Python:
   *   obj.foreignKey = value
   */
  virtual auto foreignKey(const db_ForeignKeyRef &value) -> void;

  /**
   * Getter for attribute middleSegmentOffset
   *
   * offset of the middle segment of the line, if applicable
   * \par In Python:
   *    value = obj.middleSegmentOffset
   */
  auto middleSegmentOffset() const -> grt::DoubleRef { return _middleSegmentOffset; }

  /**
   * Setter for attribute middleSegmentOffset
   * 
   * offset of the middle segment of the line, if applicable
   * \par In Python:
   *   obj.middleSegmentOffset = value
   */
  virtual auto middleSegmentOffset(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_middleSegmentOffset);
    _middleSegmentOffset = value;
    member_changed("middleSegmentOffset", ovalue, value);
  }

  /**
   * Getter for attribute startCaptionXOffs
   *
   * X offset of the start caption
   * \par In Python:
   *    value = obj.startCaptionXOffs
   */
  auto startCaptionXOffs() const -> grt::DoubleRef { return _startCaptionXOffs; }

  /**
   * Setter for attribute startCaptionXOffs
   * 
   * X offset of the start caption
   * \par In Python:
   *   obj.startCaptionXOffs = value
   */
  virtual auto startCaptionXOffs(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_startCaptionXOffs);
    _startCaptionXOffs = value;
    member_changed("startCaptionXOffs", ovalue, value);
  }

  /**
   * Getter for attribute startCaptionYOffs
   *
   * Y offset of the start caption
   * \par In Python:
   *    value = obj.startCaptionYOffs
   */
  auto startCaptionYOffs() const -> grt::DoubleRef { return _startCaptionYOffs; }

  /**
   * Setter for attribute startCaptionYOffs
   * 
   * Y offset of the start caption
   * \par In Python:
   *   obj.startCaptionYOffs = value
   */
  virtual auto startCaptionYOffs(const grt::DoubleRef &value) -> void {
    grt::ValueRef ovalue(_startCaptionYOffs);
    _startCaptionYOffs = value;
    member_changed("startCaptionYOffs", ovalue, value);
  }


  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  grt::StringRef _caption;
  grt::DoubleRef _captionXOffs;
  grt::DoubleRef _captionYOffs;
  grt::StringRef _comment;
  grt::DoubleRef _endCaptionXOffs;
  grt::DoubleRef _endCaptionYOffs;
  grt::StringRef _extraCaption;
  grt::DoubleRef _extraCaptionXOffs;
  grt::DoubleRef _extraCaptionYOffs;
  db_ForeignKeyRef _foreignKey;
  grt::DoubleRef _middleSegmentOffset;
  grt::DoubleRef _startCaptionXOffs;
  grt::DoubleRef _startCaptionYOffs;

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_physical_Connection());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_physical_Connection::create);
    {
      void (studio_physical_Connection::*setter)(const grt::StringRef &) = &studio_physical_Connection::caption;
      grt::StringRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::caption;
      meta->bind_member("caption", new grt::MetaClass::Property<studio_physical_Connection,grt::StringRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::DoubleRef &) = &studio_physical_Connection::captionXOffs;
      grt::DoubleRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::captionXOffs;
      meta->bind_member("captionXOffs", new grt::MetaClass::Property<studio_physical_Connection,grt::DoubleRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::DoubleRef &) = &studio_physical_Connection::captionYOffs;
      grt::DoubleRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::captionYOffs;
      meta->bind_member("captionYOffs", new grt::MetaClass::Property<studio_physical_Connection,grt::DoubleRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::StringRef &) = &studio_physical_Connection::comment;
      grt::StringRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::comment;
      meta->bind_member("comment", new grt::MetaClass::Property<studio_physical_Connection,grt::StringRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::DoubleRef &) = &studio_physical_Connection::endCaptionXOffs;
      grt::DoubleRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::endCaptionXOffs;
      meta->bind_member("endCaptionXOffs", new grt::MetaClass::Property<studio_physical_Connection,grt::DoubleRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::DoubleRef &) = &studio_physical_Connection::endCaptionYOffs;
      grt::DoubleRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::endCaptionYOffs;
      meta->bind_member("endCaptionYOffs", new grt::MetaClass::Property<studio_physical_Connection,grt::DoubleRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::StringRef &) = &studio_physical_Connection::extraCaption;
      grt::StringRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::extraCaption;
      meta->bind_member("extraCaption", new grt::MetaClass::Property<studio_physical_Connection,grt::StringRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::DoubleRef &) = &studio_physical_Connection::extraCaptionXOffs;
      grt::DoubleRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::extraCaptionXOffs;
      meta->bind_member("extraCaptionXOffs", new grt::MetaClass::Property<studio_physical_Connection,grt::DoubleRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::DoubleRef &) = &studio_physical_Connection::extraCaptionYOffs;
      grt::DoubleRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::extraCaptionYOffs;
      meta->bind_member("extraCaptionYOffs", new grt::MetaClass::Property<studio_physical_Connection,grt::DoubleRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const db_ForeignKeyRef &) = &studio_physical_Connection::foreignKey;
      db_ForeignKeyRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::foreignKey;
      meta->bind_member("foreignKey", new grt::MetaClass::Property<studio_physical_Connection,db_ForeignKeyRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::DoubleRef &) = &studio_physical_Connection::middleSegmentOffset;
      grt::DoubleRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::middleSegmentOffset;
      meta->bind_member("middleSegmentOffset", new grt::MetaClass::Property<studio_physical_Connection,grt::DoubleRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::DoubleRef &) = &studio_physical_Connection::startCaptionXOffs;
      grt::DoubleRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::startCaptionXOffs;
      meta->bind_member("startCaptionXOffs", new grt::MetaClass::Property<studio_physical_Connection,grt::DoubleRef>(getter, setter));
    }
    {
      void (studio_physical_Connection::*setter)(const grt::DoubleRef &) = &studio_physical_Connection::startCaptionYOffs;
      grt::DoubleRef (studio_physical_Connection::*getter)() const = &studio_physical_Connection::startCaptionYOffs;
      meta->bind_member("startCaptionYOffs", new grt::MetaClass::Property<studio_physical_Connection,grt::DoubleRef>(getter, setter));
    }
  }
};

/** a model figure representing a collection of routines */
class GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_PUBLIC studio_physical_RoutineGroupFigure : public model_Figure {
  typedef model_Figure super;

public:
  class ImplData;
  friend class ImplData;
  studio_physical_RoutineGroupFigure(grt::MetaClass *meta = nullptr)
    : model_Figure(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _data(nullptr) {
  }

  virtual ~studio_physical_RoutineGroupFigure();

  static auto static_class_name() -> std::string {
    return "studio.physical.RoutineGroupFigure";
  }

  /**
   * Getter for attribute routineGroup
   *
   * the routine group this figure represents
   * \par In Python:
   *    value = obj.routineGroup
   */
  auto routineGroup() const -> db_RoutineGroupRef { return _routineGroup; }

  /**
   * Setter for attribute routineGroup
   * 
   * the routine group this figure represents
   * \par In Python:
   *   obj.routineGroup = value
   */
  virtual auto routineGroup(const db_RoutineGroupRef &value) -> void;


  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  db_RoutineGroupRef _routineGroup;

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_physical_RoutineGroupFigure());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_physical_RoutineGroupFigure::create);
    {
      void (studio_physical_RoutineGroupFigure::*setter)(const db_RoutineGroupRef &) = &studio_physical_RoutineGroupFigure::routineGroup;
      db_RoutineGroupRef (studio_physical_RoutineGroupFigure::*getter)() const = &studio_physical_RoutineGroupFigure::routineGroup;
      meta->bind_member("routineGroup", new grt::MetaClass::Property<studio_physical_RoutineGroupFigure,db_RoutineGroupRef>(getter, setter));
    }
  }
};

/** a model figure representing a view */
class GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_PUBLIC studio_physical_ViewFigure : public model_Figure {
  typedef model_Figure super;

public:
  class ImplData;
  friend class ImplData;
  studio_physical_ViewFigure(grt::MetaClass *meta = nullptr)
    : model_Figure(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _data(nullptr) {
  }

  virtual ~studio_physical_ViewFigure();

  static auto static_class_name() -> std::string {
    return "studio.physical.ViewFigure";
  }

  /**
   * Getter for attribute view
   *
   * the view this figure represents
   * \par In Python:
   *    value = obj.view
   */
  auto view() const -> db_ViewRef { return _view; }

  /**
   * Setter for attribute view
   * 
   * the view this figure represents
   * \par In Python:
   *   obj.view = value
   */
  virtual auto view(const db_ViewRef &value) -> void;


  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  db_ViewRef _view;

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_physical_ViewFigure());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_physical_ViewFigure::create);
    {
      void (studio_physical_ViewFigure::*setter)(const db_ViewRef &) = &studio_physical_ViewFigure::view;
      db_ViewRef (studio_physical_ViewFigure::*getter)() const = &studio_physical_ViewFigure::view;
      meta->bind_member("view", new grt::MetaClass::Property<studio_physical_ViewFigure,db_ViewRef>(getter, setter));
    }
  }
};

/** a model figure representing a table */
class GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_PUBLIC studio_physical_TableFigure : public model_Figure {
  typedef model_Figure super;

public:
  class ImplData;
  friend class ImplData;
  studio_physical_TableFigure(grt::MetaClass *meta = nullptr)
    : model_Figure(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _columnsExpanded(1),
      _foreignKeysExpanded(0),
      _indicesExpanded(0),
      _summarizeDisplay(-1),
      _triggersExpanded(0),
      _data(nullptr) {
  }

  virtual ~studio_physical_TableFigure();

  static auto static_class_name() -> std::string {
    return "studio.physical.TableFigure";
  }

  /**
   * Getter for attribute columnsExpanded
   *
   * indicates whether the columns list is expanded
   * \par In Python:
   *    value = obj.columnsExpanded
   */
  auto columnsExpanded() const -> grt::IntegerRef { return _columnsExpanded; }

  /**
   * Setter for attribute columnsExpanded
   * 
   * indicates whether the columns list is expanded
   * \par In Python:
   *   obj.columnsExpanded = value
   */
  virtual auto columnsExpanded(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_columnsExpanded);
    _columnsExpanded = value;
    member_changed("columnsExpanded", ovalue, value);
  }

  /**
   * Getter for attribute foreignKeysExpanded
   *
   * indicates whether the foreign keys list is expanded
   * \par In Python:
   *    value = obj.foreignKeysExpanded
   */
  auto foreignKeysExpanded() const -> grt::IntegerRef { return _foreignKeysExpanded; }

  /**
   * Setter for attribute foreignKeysExpanded
   * 
   * indicates whether the foreign keys list is expanded
   * \par In Python:
   *   obj.foreignKeysExpanded = value
   */
  virtual auto foreignKeysExpanded(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_foreignKeysExpanded);
    _foreignKeysExpanded = value;
    member_changed("foreignKeysExpanded", ovalue, value);
  }

  /**
   * Getter for attribute indicesExpanded
   *
   * indicates whether the indices list is expanded
   * \par In Python:
   *    value = obj.indicesExpanded
   */
  auto indicesExpanded() const -> grt::IntegerRef { return _indicesExpanded; }

  /**
   * Setter for attribute indicesExpanded
   * 
   * indicates whether the indices list is expanded
   * \par In Python:
   *   obj.indicesExpanded = value
   */
  virtual auto indicesExpanded(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_indicesExpanded);
    _indicesExpanded = value;
    member_changed("indicesExpanded", ovalue, value);
  }

  /**
   * Getter for attribute summarizeDisplay
   *
   * set to -1 for showing table in summarized view mode if there's too many columns, 0 to show all columns and 1 to force summary view
   * \par In Python:
   *    value = obj.summarizeDisplay
   */
  auto summarizeDisplay() const -> grt::IntegerRef { return _summarizeDisplay; }

  /**
   * Setter for attribute summarizeDisplay
   * 
   * set to -1 for showing table in summarized view mode if there's too many columns, 0 to show all columns and 1 to force summary view
   * \par In Python:
   *   obj.summarizeDisplay = value
   */
  virtual auto summarizeDisplay(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_summarizeDisplay);
    _summarizeDisplay = value;
    member_changed("summarizeDisplay", ovalue, value);
  }

  /**
   * Getter for attribute table
   *
   * the table this figure represents
   * \par In Python:
   *    value = obj.table
   */
  auto table() const -> db_TableRef { return _table; }

  /**
   * Setter for attribute table
   * 
   * the table this figure represents
   * \par In Python:
   *   obj.table = value
   */
  virtual auto table(const db_TableRef &value) -> void;

  /**
   * Getter for attribute triggersExpanded
   *
   * indicates whether the triggers list is expanded
   * \par In Python:
   *    value = obj.triggersExpanded
   */
  auto triggersExpanded() const -> grt::IntegerRef { return _triggersExpanded; }

  /**
   * Setter for attribute triggersExpanded
   * 
   * indicates whether the triggers list is expanded
   * \par In Python:
   *   obj.triggersExpanded = value
   */
  virtual auto triggersExpanded(const grt::IntegerRef &value) -> void {
    grt::ValueRef ovalue(_triggersExpanded);
    _triggersExpanded = value;
    member_changed("triggersExpanded", ovalue, value);
  }


  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  grt::IntegerRef _columnsExpanded;
  grt::IntegerRef _foreignKeysExpanded;
  grt::IntegerRef _indicesExpanded;
  grt::IntegerRef _summarizeDisplay;
  db_TableRef _table;
  grt::IntegerRef _triggersExpanded;

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_physical_TableFigure());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_physical_TableFigure::create);
    {
      void (studio_physical_TableFigure::*setter)(const grt::IntegerRef &) = &studio_physical_TableFigure::columnsExpanded;
      grt::IntegerRef (studio_physical_TableFigure::*getter)() const = &studio_physical_TableFigure::columnsExpanded;
      meta->bind_member("columnsExpanded", new grt::MetaClass::Property<studio_physical_TableFigure,grt::IntegerRef>(getter, setter));
    }
    {
      void (studio_physical_TableFigure::*setter)(const grt::IntegerRef &) = &studio_physical_TableFigure::foreignKeysExpanded;
      grt::IntegerRef (studio_physical_TableFigure::*getter)() const = &studio_physical_TableFigure::foreignKeysExpanded;
      meta->bind_member("foreignKeysExpanded", new grt::MetaClass::Property<studio_physical_TableFigure,grt::IntegerRef>(getter, setter));
    }
    {
      void (studio_physical_TableFigure::*setter)(const grt::IntegerRef &) = &studio_physical_TableFigure::indicesExpanded;
      grt::IntegerRef (studio_physical_TableFigure::*getter)() const = &studio_physical_TableFigure::indicesExpanded;
      meta->bind_member("indicesExpanded", new grt::MetaClass::Property<studio_physical_TableFigure,grt::IntegerRef>(getter, setter));
    }
    {
      void (studio_physical_TableFigure::*setter)(const grt::IntegerRef &) = &studio_physical_TableFigure::summarizeDisplay;
      grt::IntegerRef (studio_physical_TableFigure::*getter)() const = &studio_physical_TableFigure::summarizeDisplay;
      meta->bind_member("summarizeDisplay", new grt::MetaClass::Property<studio_physical_TableFigure,grt::IntegerRef>(getter, setter));
    }
    {
      void (studio_physical_TableFigure::*setter)(const db_TableRef &) = &studio_physical_TableFigure::table;
      db_TableRef (studio_physical_TableFigure::*getter)() const = &studio_physical_TableFigure::table;
      meta->bind_member("table", new grt::MetaClass::Property<studio_physical_TableFigure,db_TableRef>(getter, setter));
    }
    {
      void (studio_physical_TableFigure::*setter)(const grt::IntegerRef &) = &studio_physical_TableFigure::triggersExpanded;
      grt::IntegerRef (studio_physical_TableFigure::*getter)() const = &studio_physical_TableFigure::triggersExpanded;
      meta->bind_member("triggersExpanded", new grt::MetaClass::Property<studio_physical_TableFigure,grt::IntegerRef>(getter, setter));
    }
  }
};

/** a model diagram holding layers and figures */
class GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_PUBLIC studio_physical_Diagram : public model_Diagram {
  typedef model_Diagram super;

public:
  class ImplData;
  friend class ImplData;
  studio_physical_Diagram(grt::MetaClass *meta = nullptr)
    : model_Diagram(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _data(nullptr) {
  }

  virtual ~studio_physical_Diagram();

  static auto static_class_name() -> std::string {
    return "studio.physical.Diagram";
  }

  /**
   * Method. 
   * \param objects 
   * \return 
   */
  virtual auto autoPlaceDBObjects(const grt::ListRef<db_DatabaseObject> &objects) -> void;
  /**
   * Method. 
   * \param fk 
   * \return 
   */
  virtual auto createConnectionForForeignKey(const db_ForeignKeyRef &fk) -> studio_physical_ConnectionRef;
  /**
   * Method. 
   * \param table 
   * \return 
   */
  virtual auto createConnectionsForTable(const db_TableRef &table) -> grt::IntegerRef;
  /**
   * Method. 
   * \param table 
   * \return 
   */
  virtual auto deleteConnectionsForTable(const db_TableRef &table) -> void;
  /**
   * Method. 
   * \param fk 
   * \return 
   */
  virtual auto getConnectionForForeignKey(const db_ForeignKeyRef &fk) -> studio_physical_ConnectionRef;
  /**
   * Method. 
   * \param object 
   * \return 
   */
  virtual auto getFigureForDBObject(const db_DatabaseObjectRef &object) -> model_FigureRef;
  /**
   * Method. 
   * \param x 
   * \param y 
   * \param width 
   * \param height 
   * \param name 
   * \return 
   */
  virtual auto placeNewLayer(double x, double y, double width, double height, const std::string &name) -> model_LayerRef;
  /**
   * Method. 
   * \param routineGroup 
   * \param x 
   * \param y 
   * \return 
   */
  virtual auto placeRoutineGroup(const db_RoutineGroupRef &routineGroup, double x, double y) -> studio_physical_RoutineGroupFigureRef;
  /**
   * Method. 
   * \param table 
   * \param x 
   * \param y 
   * \return 
   */
  virtual auto placeTable(const db_TableRef &table, double x, double y) -> studio_physical_TableFigureRef;
  /**
   * Method. 
   * \param view 
   * \param x 
   * \param y 
   * \return 
   */
  virtual auto placeView(const db_ViewRef &view, double x, double y) -> studio_physical_ViewFigureRef;

  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:


private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_physical_Diagram());
  }

  static grt::ValueRef call_autoPlaceDBObjects(grt::internal::Object *self, const grt::BaseListRef &args){ dynamic_cast<studio_physical_Diagram*>(self)->autoPlaceDBObjects(grt::ListRef<db_DatabaseObject>::cast_from(args[0])); return grt::ValueRef(); }

  static grt::ValueRef call_createConnectionForForeignKey(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_physical_Diagram*>(self)->createConnectionForForeignKey(db_ForeignKeyRef::cast_from(args[0])); }

  static grt::ValueRef call_createConnectionsForTable(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_physical_Diagram*>(self)->createConnectionsForTable(db_TableRef::cast_from(args[0])); }

  static grt::ValueRef call_deleteConnectionsForTable(grt::internal::Object *self, const grt::BaseListRef &args){ dynamic_cast<studio_physical_Diagram*>(self)->deleteConnectionsForTable(db_TableRef::cast_from(args[0])); return grt::ValueRef(); }

  static grt::ValueRef call_getConnectionForForeignKey(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_physical_Diagram*>(self)->getConnectionForForeignKey(db_ForeignKeyRef::cast_from(args[0])); }

  static grt::ValueRef call_getFigureForDBObject(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_physical_Diagram*>(self)->getFigureForDBObject(db_DatabaseObjectRef::cast_from(args[0])); }

  static grt::ValueRef call_placeNewLayer(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_physical_Diagram*>(self)->placeNewLayer(grt::DoubleRef::cast_from(args[0]), grt::DoubleRef::cast_from(args[1]), grt::DoubleRef::cast_from(args[2]), grt::DoubleRef::cast_from(args[3]), grt::StringRef::cast_from(args[4])); }

  static grt::ValueRef call_placeRoutineGroup(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_physical_Diagram*>(self)->placeRoutineGroup(db_RoutineGroupRef::cast_from(args[0]), grt::DoubleRef::cast_from(args[1]), grt::DoubleRef::cast_from(args[2])); }

  static grt::ValueRef call_placeTable(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_physical_Diagram*>(self)->placeTable(db_TableRef::cast_from(args[0]), grt::DoubleRef::cast_from(args[1]), grt::DoubleRef::cast_from(args[2])); }

  static grt::ValueRef call_placeView(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_physical_Diagram*>(self)->placeView(db_ViewRef::cast_from(args[0]), grt::DoubleRef::cast_from(args[1]), grt::DoubleRef::cast_from(args[2])); }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_physical_Diagram::create);
    meta->bind_method("autoPlaceDBObjects", &studio_physical_Diagram::call_autoPlaceDBObjects);
    meta->bind_method("createConnectionForForeignKey", &studio_physical_Diagram::call_createConnectionForForeignKey);
    meta->bind_method("createConnectionsForTable", &studio_physical_Diagram::call_createConnectionsForTable);
    meta->bind_method("deleteConnectionsForTable", &studio_physical_Diagram::call_deleteConnectionsForTable);
    meta->bind_method("getConnectionForForeignKey", &studio_physical_Diagram::call_getConnectionForForeignKey);
    meta->bind_method("getFigureForDBObject", &studio_physical_Diagram::call_getFigureForDBObject);
    meta->bind_method("placeNewLayer", &studio_physical_Diagram::call_placeNewLayer);
    meta->bind_method("placeRoutineGroup", &studio_physical_Diagram::call_placeRoutineGroup);
    meta->bind_method("placeTable", &studio_physical_Diagram::call_placeTable);
    meta->bind_method("placeView", &studio_physical_Diagram::call_placeView);
  }
};

/** a physical model holding diagrams */
class GRT_STRUCTS_MYSQLSTUDIO_PHYSICAL_PUBLIC studio_physical_Model : public model_Model {
  typedef model_Model super;

public:
  class ImplData;
  friend class ImplData;
  studio_physical_Model(grt::MetaClass *meta = nullptr)
    : model_Model(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _connectionNotation(""),
      _connections(this, false),
      _figureNotation(""),
      _notes(this, false),
      _scripts(this, false),
      _syncProfiles(this, false),
      _tagCategories(this, false),
      _tags(this, false),
      _data(nullptr) {
    _diagrams.content().__retype(grt::ObjectType, "studio.physical.Diagram");
  }

  virtual ~studio_physical_Model();

  static auto static_class_name() -> std::string {
    return "studio.physical.Model";
  }

  // catalog is owned by studio_physical_Model
  /**
   * Getter for attribute catalog
   *
   * 
   * \par In Python:
   *    value = obj.catalog
   */
  auto catalog() const -> db_CatalogRef { return _catalog; }

  /**
   * Setter for attribute catalog
   * 
   * 
   * \par In Python:
   *   obj.catalog = value
   */
  virtual auto catalog(const db_CatalogRef &value) -> void {
    grt::ValueRef ovalue(_catalog);

    _catalog = value;
    owned_member_changed("catalog", ovalue, value);
  }

  /**
   * Getter for attribute connectionNotation
   *
   * 
   * \par In Python:
   *    value = obj.connectionNotation
   */
  auto connectionNotation() const -> grt::StringRef { return _connectionNotation; }

  /**
   * Setter for attribute connectionNotation
   * 
   * 
   * \par In Python:
   *   obj.connectionNotation = value
   */
  virtual auto connectionNotation(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_connectionNotation);
    _connectionNotation = value;
    member_changed("connectionNotation", ovalue, value);
  }

  // connections is owned by studio_physical_Model
  /**
   * Getter for attribute connections (read-only)
   *
   * all connections that should be used for a full synchronisation
   * \par In Python:
   *    value = obj.connections
   */
  auto connections() const -> grt::ListRef<db_mgmt_Connection> { return _connections; }


private: // The next attribute is read-only.
  virtual auto connections(const grt::ListRef<db_mgmt_Connection> &value) -> void {
    grt::ValueRef ovalue(_connections);

    _connections = value;
    owned_member_changed("connections", ovalue, value);
  }
public:

  /**
   * Getter for attribute currentConnection
   *
   * the connection used for reverse engineering and synchronisation
   * \par In Python:
   *    value = obj.currentConnection
   */
  auto currentConnection() const -> db_mgmt_ConnectionRef { return _currentConnection; }

  /**
   * Setter for attribute currentConnection
   * 
   * the connection used for reverse engineering and synchronisation
   * \par In Python:
   *   obj.currentConnection = value
   */
  virtual auto currentConnection(const db_mgmt_ConnectionRef &value) -> void {
    grt::ValueRef ovalue(_currentConnection);
    _currentConnection = value;
    member_changed("currentConnection", ovalue, value);
  }

  // diagrams is owned by studio_physical_Model
  /**
   * Getter for attribute diagrams (read-only)
   *
   * the list of all available diagrams
   * \par In Python:
   *    value = obj.diagrams
   */
  auto diagrams() const -> grt::ListRef<studio_physical_Diagram> { return grt::ListRef<studio_physical_Diagram>::cast_from(_diagrams); }


private: // The next attribute is read-only.
public:

  /**
   * Getter for attribute figureNotation
   *
   * 
   * \par In Python:
   *    value = obj.figureNotation
   */
  auto figureNotation() const -> grt::StringRef { return _figureNotation; }

  /**
   * Setter for attribute figureNotation
   * 
   * 
   * \par In Python:
   *   obj.figureNotation = value
   */
  virtual auto figureNotation(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_figureNotation);
    _figureNotation = value;
    member_changed("figureNotation", ovalue, value);
  }

  // notes is owned by studio_physical_Model
  /**
   * Getter for attribute notes (read-only)
   *
   * a list of notes that are stored with the model
   * \par In Python:
   *    value = obj.notes
   */
  auto notes() const -> grt::ListRef<GrtStoredNote> { return _notes; }


private: // The next attribute is read-only.
  virtual auto notes(const grt::ListRef<GrtStoredNote> &value) -> void {
    grt::ValueRef ovalue(_notes);

    _notes = value;
    owned_member_changed("notes", ovalue, value);
  }
public:

  /**
   * Getter for attribute rdbms
   *
   * the rdbms used for the document
   * \par In Python:
   *    value = obj.rdbms
   */
  auto rdbms() const -> db_mgmt_RdbmsRef { return _rdbms; }

  /**
   * Setter for attribute rdbms
   * 
   * the rdbms used for the document
   * \par In Python:
   *   obj.rdbms = value
   */
  virtual auto rdbms(const db_mgmt_RdbmsRef &value) -> void {
    grt::ValueRef ovalue(_rdbms);
    _rdbms = value;
    member_changed("rdbms", ovalue, value);
  }

  // scripts is owned by studio_physical_Model
  /**
   * Getter for attribute scripts (read-only)
   *
   * a list of scripts that are stored with the model
   * \par In Python:
   *    value = obj.scripts
   */
  auto scripts() const -> grt::ListRef<db_Script> { return _scripts; }


private: // The next attribute is read-only.
  virtual auto scripts(const grt::ListRef<db_Script> &value) -> void {
    grt::ValueRef ovalue(_scripts);

    _scripts = value;
    owned_member_changed("scripts", ovalue, value);
  }
public:

  /**
   * Getter for attribute syncProfiles (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.syncProfiles
   */
  auto syncProfiles() const -> grt::DictRef { return _syncProfiles; }


private: // The next attribute is read-only.
  virtual auto syncProfiles(const grt::DictRef &value) -> void {
    grt::ValueRef ovalue(_syncProfiles);
    _syncProfiles = value;
    member_changed("syncProfiles", ovalue, value);
  }
public:

  // tagCategories is owned by studio_physical_Model
  /**
   * Getter for attribute tagCategories (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.tagCategories
   */
  auto tagCategories() const -> grt::ListRef<GrtObject> { return _tagCategories; }


private: // The next attribute is read-only.
  virtual auto tagCategories(const grt::ListRef<GrtObject> &value) -> void {
    grt::ValueRef ovalue(_tagCategories);

    _tagCategories = value;
    owned_member_changed("tagCategories", ovalue, value);
  }
public:

  // tags is owned by studio_physical_Model
  /**
   * Getter for attribute tags (read-only)
   *
   * 
   * \par In Python:
   *    value = obj.tags
   */
  auto tags() const -> grt::ListRef<meta_Tag> { return _tags; }


private: // The next attribute is read-only.
  virtual auto tags(const grt::ListRef<meta_Tag> &value) -> void {
    grt::ValueRef ovalue(_tags);

    _tags = value;
    owned_member_changed("tags", ovalue, value);
  }
public:

  /**
   * Method. 
   * \param deferRealize 
   * \return 
   */
  virtual auto addNewDiagram(ssize_t deferRealize) -> model_DiagramRef;

  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  db_CatalogRef _catalog;// owned
  grt::StringRef _connectionNotation;
  grt::ListRef<db_mgmt_Connection> _connections;// owned
  db_mgmt_ConnectionRef _currentConnection;
  grt::StringRef _figureNotation;
  grt::ListRef<GrtStoredNote> _notes;// owned
  db_mgmt_RdbmsRef _rdbms;
  grt::ListRef<db_Script> _scripts;// owned
  grt::DictRef _syncProfiles;
  grt::ListRef<GrtObject> _tagCategories;// owned
  grt::ListRef<meta_Tag> _tags;// owned

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_physical_Model());
  }

  static grt::ValueRef call_addNewDiagram(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_physical_Model*>(self)->addNewDiagram(grt::IntegerRef::cast_from(args[0])); }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_physical_Model::create);
    {
      void (studio_physical_Model::*setter)(const db_CatalogRef &) = &studio_physical_Model::catalog;
      db_CatalogRef (studio_physical_Model::*getter)() const = &studio_physical_Model::catalog;
      meta->bind_member("catalog", new grt::MetaClass::Property<studio_physical_Model,db_CatalogRef>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const grt::StringRef &) = &studio_physical_Model::connectionNotation;
      grt::StringRef (studio_physical_Model::*getter)() const = &studio_physical_Model::connectionNotation;
      meta->bind_member("connectionNotation", new grt::MetaClass::Property<studio_physical_Model,grt::StringRef>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const grt::ListRef<db_mgmt_Connection> &) = &studio_physical_Model::connections;
      grt::ListRef<db_mgmt_Connection> (studio_physical_Model::*getter)() const = &studio_physical_Model::connections;
      meta->bind_member("connections", new grt::MetaClass::Property<studio_physical_Model,grt::ListRef<db_mgmt_Connection>>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const db_mgmt_ConnectionRef &) = &studio_physical_Model::currentConnection;
      db_mgmt_ConnectionRef (studio_physical_Model::*getter)() const = &studio_physical_Model::currentConnection;
      meta->bind_member("currentConnection", new grt::MetaClass::Property<studio_physical_Model,db_mgmt_ConnectionRef>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const grt::ListRef<studio_physical_Diagram> &) = 0;
      grt::ListRef<studio_physical_Diagram> (studio_physical_Model::*getter)() const = 0;
      meta->bind_member("diagrams", new grt::MetaClass::Property<studio_physical_Model,grt::ListRef<studio_physical_Diagram>>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const grt::StringRef &) = &studio_physical_Model::figureNotation;
      grt::StringRef (studio_physical_Model::*getter)() const = &studio_physical_Model::figureNotation;
      meta->bind_member("figureNotation", new grt::MetaClass::Property<studio_physical_Model,grt::StringRef>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const grt::ListRef<GrtStoredNote> &) = &studio_physical_Model::notes;
      grt::ListRef<GrtStoredNote> (studio_physical_Model::*getter)() const = &studio_physical_Model::notes;
      meta->bind_member("notes", new grt::MetaClass::Property<studio_physical_Model,grt::ListRef<GrtStoredNote>>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const db_mgmt_RdbmsRef &) = &studio_physical_Model::rdbms;
      db_mgmt_RdbmsRef (studio_physical_Model::*getter)() const = &studio_physical_Model::rdbms;
      meta->bind_member("rdbms", new grt::MetaClass::Property<studio_physical_Model,db_mgmt_RdbmsRef>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const grt::ListRef<db_Script> &) = &studio_physical_Model::scripts;
      grt::ListRef<db_Script> (studio_physical_Model::*getter)() const = &studio_physical_Model::scripts;
      meta->bind_member("scripts", new grt::MetaClass::Property<studio_physical_Model,grt::ListRef<db_Script>>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const grt::DictRef &) = &studio_physical_Model::syncProfiles;
      grt::DictRef (studio_physical_Model::*getter)() const = &studio_physical_Model::syncProfiles;
      meta->bind_member("syncProfiles", new grt::MetaClass::Property<studio_physical_Model,grt::DictRef>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const grt::ListRef<GrtObject> &) = &studio_physical_Model::tagCategories;
      grt::ListRef<GrtObject> (studio_physical_Model::*getter)() const = &studio_physical_Model::tagCategories;
      meta->bind_member("tagCategories", new grt::MetaClass::Property<studio_physical_Model,grt::ListRef<GrtObject>>(getter, setter));
    }
    {
      void (studio_physical_Model::*setter)(const grt::ListRef<meta_Tag> &) = &studio_physical_Model::tags;
      grt::ListRef<meta_Tag> (studio_physical_Model::*getter)() const = &studio_physical_Model::tags;
      meta->bind_member("tags", new grt::MetaClass::Property<studio_physical_Model,grt::ListRef<meta_Tag>>(getter, setter));
    }
    meta->bind_method("addNewDiagram", &studio_physical_Model::call_addNewDiagram);
  }
};



inline auto register_structs_studio_physical_xml() -> void {
  grt::internal::ClassRegistry::register_class<studio_physical_Layer>();
  grt::internal::ClassRegistry::register_class<studio_physical_Connection>();
  grt::internal::ClassRegistry::register_class<studio_physical_RoutineGroupFigure>();
  grt::internal::ClassRegistry::register_class<studio_physical_ViewFigure>();
  grt::internal::ClassRegistry::register_class<studio_physical_TableFigure>();
  grt::internal::ClassRegistry::register_class<studio_physical_Diagram>();
  grt::internal::ClassRegistry::register_class<studio_physical_Model>();
}

#ifdef AUTO_REGISTER_GRT_CLASSES
static struct _autoreg__structs_studio_physical_xml {
  _autoreg__structs_studio_physical_xml() {
    register_structs_studio_physical_xml();
  }
} __autoreg__structs_studio_physical_xml;
#endif

#ifndef _MSC_VER
  #pragma GCC diagnostic pop
#endif

