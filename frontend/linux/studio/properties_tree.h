/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _PROPERTIES_TREE_H_
#define _PROPERTIES_TREE_H_

#include <gtkmm/eventbox.h>
#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/textview.h>
#include <gtkmm/table.h>
#include <gtkmm/colorselection.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/checkbutton.h>

#include "listmodel_wrapper.h"
#include "studio/wb_context_ui.h"

class PropertyInspector;

//==============================================================================
class PropertyValue : public Gtk::EventBox {
public:
  PropertyValue(PropertyInspector* owner, const bec::NodeId& node);
  virtual ~PropertyValue();

  virtual auto set_text(const std::string& text) -> void;
  virtual auto get_text() const -> std::string {
    return _text.get_text();
  }

  virtual auto get_new_value() const -> std::string = 0;

  auto node() const -> const bec::NodeId& {
    return _prop_node;
  }

  auto stop_edit() -> void;
  auto start_edit() -> void;

  virtual auto type() const -> grt::Type = 0;

protected:
  virtual auto start_editing() -> void = 0;
  virtual auto stop_editing() -> void {
    editor().hide();
  } // For the most editor this is sufficient
    // For more sophisticated ones, the actual
    // implementation should hide editor
  virtual auto editor() -> Gtk::Widget& {
    return _text;
  }
  virtual auto label() -> Gtk::Widget& {
    return _text;
  }

  PropertyInspector* _owner;

  auto on_event(GdkEvent* event) -> bool; //!< fwd clicks to Inspector, handle Enter/Esc and tell Inspector about edit_done
private:
  Gtk::Label _text;
  bec::NodeId _prop_node;
  sigc::connection _conn;
};

//==============================================================================
class PropertyString : public PropertyValue {
public:
  PropertyString(PropertyInspector* owner, const bec::NodeId& node);

  virtual auto get_new_value() const -> std::string;
  virtual auto type() const -> grt::Type {
    return grt::StringType;
  }

protected:
  virtual auto start_editing() -> void;
  virtual auto editor() -> Gtk::Widget&;

private:
  Gtk::Entry _entry;
};

//==============================================================================
class PropertyBool : public PropertyValue {
public:
  PropertyBool(PropertyInspector* owner, const bec::NodeId& node);

  virtual auto set_text(const std::string& text) -> void;
  virtual auto get_text() const -> std::string;

  virtual auto get_new_value() const -> std::string;
  virtual auto type() const -> grt::Type {
    return grt::IntegerType;
  }

protected:
  virtual auto start_editing() -> void;
  virtual auto stop_editing() -> void {};
  virtual auto editor() -> Gtk::Widget&;
  virtual auto label() -> Gtk::Widget&;

private:
  auto on_value_changed() -> void;
  Gtk::CheckButton _button;
  sigc::connection _conn;
};

//==============================================================================
class PropertyColor : public PropertyValue {
public:
  PropertyColor(PropertyInspector* owner, const bec::NodeId& node);

  virtual auto get_new_value() const -> std::string;
  virtual auto type() const -> grt::Type {
    return grt::StringType;
  }

protected:
  virtual auto start_editing() -> void;
  virtual auto editor() -> Gtk::Widget&;

private:
  auto show_dlg() -> void;

  Gtk::Box _hbox;
  Gtk::ColorSelectionDialog _dlg;
  Gtk::Entry _entry;
  Gtk::Button _button;
};

//==============================================================================
class PropertyText : public PropertyValue {
public:
  PropertyText(PropertyInspector* owner, const bec::NodeId& node);

  virtual auto get_new_value() const -> std::string;
  virtual auto type() const -> grt::Type {
    return grt::StringType;
  }

protected:
  virtual auto start_editing() -> void;

  virtual auto stop_editing() -> void;

private:
  auto handle_event(GdkEvent* event) -> bool;

  Gtk::Dialog _wnd;
  Gtk::ScrolledWindow _scroll;
  Gtk::TextView _text;
  bool _ctrl;
};

//==============================================================================
class PropertyInspector : public Gtk::ScrolledWindow {
public:
  PropertyInspector();
  ~PropertyInspector();

  auto clear() -> void;
  auto populate() -> void;
  auto update() -> void; //!< Updates all values in the inspector.

  auto handle_click(PropertyValue* value) -> void;
  auto edit_done(PropertyValue* property, const bool finish = false) -> void;
  auto edit_canceled() -> void;

  typedef sigc::slot<int> properties_count_t;
  auto set_count_slot(const properties_count_t& count_slot) -> void {
    _get_properties_count = count_slot;
  }

  typedef sigc::slot<void, const bec::NodeId&, const std::string&, const grt::Type> set_value_slot_t;
  auto set_value_slot_setter(const set_value_slot_t& slot) -> void {
    _set_value_slot = slot;
  }

  //! std::string - value, NodeId node for which value is requested, bool - is it a prop name or value
  typedef sigc::slot<std::string, const bec::NodeId&, const bool> get_value_slot_t;
  auto set_value_slot_getter(const get_value_slot_t& slot) -> void {
    _get_value_slot = slot;
  }

  //! std::string - value, NodeId node for which value is requested, bool - is it a prop name or value
  typedef sigc::slot<std::string, const bec::NodeId&> get_type_slot_t;
  auto set_type_slot_getter(const get_type_slot_t& slot) -> void {
    _get_type_slot = slot;
  }

private:
  Gtk::Table* _table;
  PropertyValue* _edited_property;

  properties_count_t _get_properties_count;
  set_value_slot_t _set_value_slot;
  get_value_slot_t _get_value_slot;
  get_type_slot_t _get_type_slot;

  std::vector<PropertyValue*> _properties;
  int _updating;
};

//==============================================================================
class PropertiesTree : public Gtk::Box {
  bec::ValueInspectorBE* _inspector;
  PropertyInspector _inspector_view;

  auto get_properties_count() const -> int;
  auto set_value(const bec::NodeId& node, const std::string&, const grt::Type type) -> void;
  auto get_value(const bec::NodeId& node, const bool is_name) const -> std::string;
  auto get_prop_type(const bec::NodeId& node) const -> std::string;

  auto refresh() -> void;

public:
  PropertiesTree();
  virtual ~PropertiesTree();

  auto update() -> void;
};

#endif /* _PROPERTIES_TREE_H_ */
