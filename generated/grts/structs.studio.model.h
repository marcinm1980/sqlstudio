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
  #ifdef GRT_STRUCTS_MYSQLSTUDIO_MODEL_EXPORT
  #define GRT_STRUCTS_MYSQLSTUDIO_MODEL_PUBLIC __declspec(dllexport)
#else
  #define GRT_STRUCTS_MYSQLSTUDIO_MODEL_PUBLIC __declspec(dllimport)
#endif
#else
  #define GRT_STRUCTS_MYSQLSTUDIO_MODEL_PUBLIC
#endif

#include "grts/structs.h"
#include "grts/structs.model.h"

class studio_model_ImageFigure;
typedef grt::Ref<studio_model_ImageFigure> studio_model_ImageFigureRef;
class studio_model_NoteFigure;
typedef grt::Ref<studio_model_NoteFigure> studio_model_NoteFigureRef;


namespace mforms { 
  class Object;
}; 

namespace grt { 
  class AutoPyObject;
}; 

/** a model figure representing an image */
class GRT_STRUCTS_MYSQLSTUDIO_MODEL_PUBLIC studio_model_ImageFigure : public model_Figure {
  typedef model_Figure super;

public:
  class ImplData;
  friend class ImplData;
  studio_model_ImageFigure(grt::MetaClass *meta = nullptr)
    : model_Figure(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _filename(""),
      _keepAspectRatio(0),
      _data(nullptr) {
  }

  virtual ~studio_model_ImageFigure();

  static auto static_class_name() -> std::string {
    return "studio.model.ImageFigure";
  }

  /**
   * Getter for attribute filename
   *
   * the image file name
   * \par In Python:
   *    value = obj.filename
   */
  auto filename() const -> grt::StringRef { return _filename; }

  /**
   * Setter for attribute filename
   * 
   * the image file name
   * \par In Python:
   *   obj.filename = value
   */
  virtual auto filename(const grt::StringRef &value) -> void {
    grt::ValueRef ovalue(_filename);
    _filename = value;
    member_changed("filename", ovalue, value);
  }

  /**
   * Getter for attribute keepAspectRatio
   *
   * 
   * \par In Python:
   *    value = obj.keepAspectRatio
   */
  auto keepAspectRatio() const -> grt::IntegerRef { return _keepAspectRatio; }

  /**
   * Setter for attribute keepAspectRatio
   * 
   * 
   * \par In Python:
   *   obj.keepAspectRatio = value
   */
  virtual auto keepAspectRatio(const grt::IntegerRef &value) -> void;

  /**
   * Method. 
   * \param name 
   * \return 
   */
  virtual auto setImageFile(const std::string &name) -> grt::StringRef;

  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  grt::StringRef _filename;
  grt::IntegerRef _keepAspectRatio;

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_model_ImageFigure());
  }

  static grt::ValueRef call_setImageFile(grt::internal::Object *self, const grt::BaseListRef &args){ return dynamic_cast<studio_model_ImageFigure*>(self)->setImageFile(grt::StringRef::cast_from(args[0])); }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_model_ImageFigure::create);
    {
      void (studio_model_ImageFigure::*setter)(const grt::StringRef &) = &studio_model_ImageFigure::filename;
      grt::StringRef (studio_model_ImageFigure::*getter)() const = &studio_model_ImageFigure::filename;
      meta->bind_member("filename", new grt::MetaClass::Property<studio_model_ImageFigure,grt::StringRef>(getter, setter));
    }
    {
      void (studio_model_ImageFigure::*setter)(const grt::IntegerRef &) = &studio_model_ImageFigure::keepAspectRatio;
      grt::IntegerRef (studio_model_ImageFigure::*getter)() const = &studio_model_ImageFigure::keepAspectRatio;
      meta->bind_member("keepAspectRatio", new grt::MetaClass::Property<studio_model_ImageFigure,grt::IntegerRef>(getter, setter));
    }
    meta->bind_method("setImageFile", &studio_model_ImageFigure::call_setImageFile);
  }
};

/** a model figure representing a text box */
class GRT_STRUCTS_MYSQLSTUDIO_MODEL_PUBLIC studio_model_NoteFigure : public model_Figure {
  typedef model_Figure super;

public:
  class ImplData;
  friend class ImplData;
  studio_model_NoteFigure(grt::MetaClass *meta = nullptr)
    : model_Figure(meta != nullptr ? meta : grt::GRT::get()->get_metaclass(static_class_name())),
      _font(""),
      _text(""),
      _textColor(""),
      _data(nullptr) {
  }

  virtual ~studio_model_NoteFigure();

  static auto static_class_name() -> std::string {
    return "studio.model.NoteFigure";
  }

  /**
   * Getter for attribute font
   *
   * the font to be used for the note
   * \par In Python:
   *    value = obj.font
   */
  auto font() const -> grt::StringRef { return _font; }

  /**
   * Setter for attribute font
   * 
   * the font to be used for the note
   * \par In Python:
   *   obj.font = value
   */
  virtual auto font(const grt::StringRef &value) -> void;

  /**
   * Getter for attribute text
   *
   * the text contents
   * \par In Python:
   *    value = obj.text
   */
  auto text() const -> grt::StringRef { return _text; }

  /**
   * Setter for attribute text
   * 
   * the text contents
   * \par In Python:
   *   obj.text = value
   */
  virtual auto text(const grt::StringRef &value) -> void;

  /**
   * Getter for attribute textColor
   *
   * the text color
   * \par In Python:
   *    value = obj.textColor
   */
  auto textColor() const -> grt::StringRef { return _textColor; }

  /**
   * Setter for attribute textColor
   * 
   * the text color
   * \par In Python:
   *   obj.textColor = value
   */
  virtual auto textColor(const grt::StringRef &value) -> void;


  auto get_data() const -> ImplData * { return _data; }

  auto set_data(ImplData *data) -> void;
  // default initialization function. auto-called by ObjectRef constructor
  virtual auto init() -> void;

protected:

  grt::StringRef _font;
  grt::StringRef _text;
  grt::StringRef _textColor;

private: // Wrapper methods for use by the grt.
  ImplData *_data;

  static auto create() -> grt::ObjectRef {
    return grt::ObjectRef(new studio_model_NoteFigure());
  }

public:
  static auto grt_register() -> void {
    grt::MetaClass *meta = grt::GRT::get()->get_metaclass(static_class_name());
    if (meta == nullptr)
      throw std::runtime_error("error initializing grt object class, metaclass not found");
    meta->bind_allocator(&studio_model_NoteFigure::create);
    {
      void (studio_model_NoteFigure::*setter)(const grt::StringRef &) = &studio_model_NoteFigure::font;
      grt::StringRef (studio_model_NoteFigure::*getter)() const = &studio_model_NoteFigure::font;
      meta->bind_member("font", new grt::MetaClass::Property<studio_model_NoteFigure,grt::StringRef>(getter, setter));
    }
    {
      void (studio_model_NoteFigure::*setter)(const grt::StringRef &) = &studio_model_NoteFigure::text;
      grt::StringRef (studio_model_NoteFigure::*getter)() const = &studio_model_NoteFigure::text;
      meta->bind_member("text", new grt::MetaClass::Property<studio_model_NoteFigure,grt::StringRef>(getter, setter));
    }
    {
      void (studio_model_NoteFigure::*setter)(const grt::StringRef &) = &studio_model_NoteFigure::textColor;
      grt::StringRef (studio_model_NoteFigure::*getter)() const = &studio_model_NoteFigure::textColor;
      meta->bind_member("textColor", new grt::MetaClass::Property<studio_model_NoteFigure,grt::StringRef>(getter, setter));
    }
  }
};



inline auto register_structs_studio_model_xml() -> void {
  grt::internal::ClassRegistry::register_class<studio_model_ImageFigure>();
  grt::internal::ClassRegistry::register_class<studio_model_NoteFigure>();
}

#ifdef AUTO_REGISTER_GRT_CLASSES
static struct _autoreg__structs_studio_model_xml {
  _autoreg__structs_studio_model_xml() {
    register_structs_studio_model_xml();
  }
} __autoreg__structs_studio_model_xml;
#endif

#ifndef _MSC_VER
  #pragma GCC diagnostic pop
#endif

