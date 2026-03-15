/*
 * Copyright (c) 2007, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "base/string_utilities.h"
#include "base/threading.h"

#include "grt.h"
#include "grtpp_util.h"
#include "grtpp_undo_manager.h"

#include <glib.h>

using namespace grt;
using namespace grt::internal;
using namespace base;

static auto register_base_class() -> void {
  MetaClass* mc = grt::GRT::get()->get_metaclass(Object::static_class_name());

  mc->bind_allocator(0);

  // nothing in the base class
}

ClassRegistry::ClassRegistry() {
  // register the root class
  classes[Object::static_class_name()] = &register_base_class;
}

auto ClassRegistry::register_all() -> void {
  for (std::map<std::string, ClassRegistrationFunction>::const_iterator iter = classes.begin(); iter != classes.end();
       ++iter) {
    // Register classes only for loaded meta classes.
    if (!grt::GRT::get()->get_metaclass(iter->first)) {
      if (grt::GRT::get()->verbose())
        grt::GRT::get()->send_warning("MetaClass " + iter->first + " is registered but was not loaded from a XML");
      continue;
    }
    (*iter->second)();
  }
}

auto ClassRegistry::cleanUp() -> void {
  classes.clear();
  // register the root class
  classes[Object::static_class_name()] = &register_base_class;
}

auto ClassRegistry::get_instance() -> ClassRegistry* {
  static ClassRegistry* instance = new ClassRegistry();
  return instance;
}

//--------------------------------------------------------------------------------------------------

auto ClassRegistry::isEmpty() -> bool {
  return classes.empty();
}

//--------------------------------------------------------------------------------------------------

auto Integer::debugDescription(const std::string& indentation) const -> std::string {
  // Simple values don't use indentation as they are always on a RHS.
  return toString();
}

auto Integer::toString() const -> std::string {
  return std::to_string(_value);
}

Integer::Integer(storage_type value) : _value(value) {
}

auto Integer::get(storage_type value) -> Integer* {
  static Integer* one = (Integer*)((new Integer(1))->retain());
  static Integer* zero = (Integer*)((new Integer(0))->retain());

  if (value == 1)
    return one;
  if (value == 0)
    return zero;

  return new Integer(value);
}

auto Integer::equals(const Value* o) const -> bool {
  return _value == dynamic_cast<const Integer*>(o)->_value;
}

auto Integer::less_than(const Value* o) const -> bool {
  return _value < dynamic_cast<const Integer*>(o)->_value;
}

//--------------------------------------------------------------------------------------------------

auto Double::debugDescription(const std::string& indentation) const -> std::string {
  return toString();
}

auto Double::toString() const -> std::string {
  return std::to_string(_value);
}

Double::Double(storage_type value) : _value(value) {
}

auto Double::get(storage_type value) -> Double* {
  static Double* one = (Double*)((new Double(1.0))->retain());
  static Double* zero = (Double*)((new Double(0.0))->retain());

  if (value == 1.0)
    return one;
  if (value == 0.0)
    return zero;

  return new Double(value);
}

auto Double::equals(const Value* o) const -> bool {
  return _value == dynamic_cast<const Double*>(o)->_value;
}

auto Double::less_than(const Value* o) const -> bool {
  return _value < dynamic_cast<const Double*>(o)->_value;
}

//--------------------------------------------------------------------------------------------------

auto String::debugDescription(const std::string& indentation) const -> std::string {
  return "'" + _value + "'";
}

auto String::toString() const -> std::string {
  return _value;
}

String::String(const storage_type& value) : _value(value) {
}

auto String::get(const storage_type& value) -> String* {
  static String* empty = (String*)((new String(""))->retain());

  if (value.empty())
    return empty;

  return new String(value);
}

auto String::equals(const Value* o) const -> bool {
  return _value == dynamic_cast<const String*>(o)->_value;
}

auto String::less_than(const Value* o) const -> bool {
  return _value < dynamic_cast<const String*>(o)->_value;
}

//--------------------------------------------------------------------------------------------------

auto List::debugDescription(const std::string& indentation) const -> std::string {
  std::string s;

  s.append("[\n"); // Not indented (RHS value).
  for (raw_const_iterator iter = raw_begin(); iter != raw_end(); ++iter)
    s.append(indentation + "  " + iter->debugDescription(indentation + "  ") + "\n");

  s.append(indentation + "]");
  return s;
}

auto List::toString() const -> std::string {
  std::string s;
  bool first = true;

  s.append("[");
  for (raw_const_iterator iter = raw_begin(); iter != raw_end(); ++iter) {
    if (!first)
      s.append(", ");
    first = false;
    s.append(iter->toString());
  }

  s.append("]");
  return s;
}

List::List(bool allow_null) : _allow_null(allow_null) {
  _is_global = 0;
}

List::List(Type content_type, const std::string& content_class, bool allow_null) : _allow_null(allow_null) {
  _content_type.type = content_type;
  _content_type.object_class = content_class;

  _is_global = 0;
}

List::~List() {
}

auto List::set_unchecked(size_t index, const ValueRef& value) -> void {
  if (index >= count())
    throw bad_item(index, count());

  if (index == count()) {
    insert_unchecked(value, index);
    return;
  }

  //  if (_content[index].valueptr() != value.valueptr())
  {
    if (_is_global > 0 && grt::GRT::get()->tracking_changes())
      grt::GRT::get()->get_undo_manager()->add_undo(new UndoListSetAction(this, index));

    if (_is_global > 0 && _content[index].is_valid()) {
      _content[index].unmark_global();
    }

    if (_is_global > 0 && value.is_valid()) {
      value.mark_global();
    }

    _content[index] = value;
  }
}

auto List::insert_unchecked(const ValueRef& value, size_t index) -> void {
  if (_is_global > 0 && value.is_valid())
    value.mark_global();

  if (index == npos) {
    if (_is_global > 0 && grt::GRT::get()->tracking_changes())
      grt::GRT::get()->get_undo_manager()->add_undo(new UndoListInsertAction(this, index));

    _content.push_back(value);
  } else if (index > _content.size())
    throw grt::bad_item(index, _content.size());
  else {
    if (_is_global > 0 && grt::GRT::get()->tracking_changes())
      grt::GRT::get()->get_undo_manager()->add_undo(new UndoListInsertAction(this, index));

    _content.insert(_content.begin() + index, value);
  }
}

auto List::remove(const ValueRef& value) -> void {
  size_t i = _content.size();
  while (i-- > 0) {
    if (_content[i] == value) {
      if (_is_global > 0 && _content[i].is_valid())
        _content[i].unmark_global();

      if (_is_global > 0 && grt::GRT::get()->tracking_changes())
        grt::GRT::get()->get_undo_manager()->add_undo(new UndoListRemoveAction(this, i));

      _content.erase(_content.begin() + i);
    }
  }
}

auto List::remove(size_t index) -> void {
  if (index >= count())
    throw grt::bad_item(index, count());

  if (_is_global > 0 && _content[index].is_valid())
    _content[index].unmark_global();

  if (_is_global > 0 && grt::GRT::get()->tracking_changes())
    grt::GRT::get()->get_undo_manager()->add_undo(new UndoListRemoveAction(this, index));

  _content.erase(_content.begin() + index);
}

auto List::reorder(size_t oi, size_t ni) -> void {
  if (oi == ni)
    return;

  if (_is_global > 0 && grt::GRT::get()->tracking_changes())
    grt::GRT::get()->get_undo_manager()->add_undo(new UndoListReorderAction(this, oi, ni));

  ValueRef tmp(_content[oi]);
  _content.erase(_content.begin() + oi);
  if (ni >= _content.size())
    _content.insert(_content.end(), tmp);
  else
    _content.insert(_content.begin() + ni, tmp);
}

auto List::get_index(const ValueRef& value) -> size_t {
  size_t i = 0;
  for (std::vector<ValueRef>::const_iterator iter = _content.begin(); iter != _content.end(); ++iter, ++i) {
    if (*iter == value)
      return i;
  }
  return npos;
}

auto List::check_assignable(const ValueRef& value) const -> bool {
  if (value.is_valid()) {
    Type vtype = value.type();

    if (content_type() != vtype) {
      if (content_type() == AnyType)
        return true;

      return false;
    }

    if (vtype == ObjectType) {
      ObjectRef obj(ObjectRef::cast_from(value));

      return obj.is_instance(content_class_name());
    }

    return true;
  }
  return _allow_null;
}

auto List::set_checked(size_t index, const ValueRef& value) -> void {
  if (check_assignable(value))
    set_unchecked(index, value);
  else {
    if (value.is_valid())
      throw std::invalid_argument("attempt to insert invalid value to list");
    else
      throw grt::null_value("inserting null value to not null list");
  }
}

auto List::insert_checked(const ValueRef& value, size_t index) -> void {
  if (check_assignable(value))
    insert_unchecked(value, index);
  else {
    if (value.is_valid()) {
      if (_content_type.type != value.type())
        throw grt::type_error(_content_type.type, value.type());
      else {
        ObjectRef object(ObjectRef::cast_from(value));
        throw grt::type_error(_content_type.object_class, object.class_name());
      }
    } else
      throw grt::null_value("inserting null value to not null list");
  }
}

auto List::mark_global() const -> void {
  if (_is_global == 0) {
    if (_content_type.type == AnyType || is_container_type(_content_type.type)) {
      for (storage_type::const_iterator iter = _content.begin(); iter != _content.end(); ++iter) {
        if (iter->is_valid())
          iter->mark_global();
      }
    }
  }
  _is_global++;
}

auto List::unmark_global() const -> void {
  _is_global--;
  if (_is_global == 0) {
    if (_content_type.type == AnyType || is_container_type(_content_type.type)) {
      for (storage_type::const_iterator iter = _content.begin(); iter != _content.end(); ++iter) {
        if (iter->is_valid())
          iter->unmark_global();
      }
    }
  }
}

auto List::reset_references() -> void {
  const int max_index = (int)count();
  grt::ValueRef value;
  for (int i = 0; i < max_index; ++i) {
    // g_log("grt", G_LOG_LEVEL_DEBUG, "List::reset_references: '%i'", i);

    value = _content[i];
    if (value.is_valid())
      value.valueptr()->reset_references();
  }
}

auto List::equals(const Value* o) const -> bool {
  return this == o;
}

auto List::less_than(const Value* o) const -> bool {
  return this < o;
}

auto List::__retype(Type type, const std::string& content_class) -> void {
  _content_type.type = type;
  _content_type.object_class = content_class;
}

//--------------------------------------------------------------------------------------------------

OwnedList::OwnedList(Type type, const std::string& content_class, Object* owner, bool allow_null)
  : List(type, content_class, allow_null), _owner(owner) {
  if (!owner)
    throw std::invalid_argument("owner cannot be NULL");
}

auto OwnedList::set_unchecked(size_t index, const ValueRef& value) -> void {
  ValueRef item;

  if (index < _content.size())
    item = _content[index];
  else
    throw grt::bad_item(index, _content.size());

  List::set_unchecked(index, value);

  if (item.is_valid())
    _owner->owned_list_item_removed(this, item);
  if (value.is_valid())
    _owner->owned_list_item_added(this, value);
}

auto OwnedList::insert_unchecked(const ValueRef& value, size_t index) -> void {
  List::insert_unchecked(value, index);

  _owner->owned_list_item_added(this, value);
}

auto OwnedList::remove(const ValueRef& value) -> void {
  List::remove(value);

  _owner->owned_list_item_removed(this, value);
}

auto OwnedList::remove(size_t index) -> void {
  ValueRef item(_content[index]);

  List::remove(index);

  _owner->owned_list_item_removed(this, item);
}

//--------------------------------------------------------------------------------------------------

auto Dict::debugDescription(const std::string& indentation) const -> std::string {
  std::string s;

  s.append("{\n");
  for (const_iterator iter = begin(); iter != end(); ++iter) {
    s.append(indentation + "  " + iter->first);
    s.append(" = ");
    s.append(iter->second.debugDescription(indentation + "  ") + "\n");
  }
  s.append(indentation + "}");
  return s;
}

auto Dict::toString() const -> std::string {
  std::string s;
  bool first = true;

  s.append("{");
  for (const_iterator iter = begin(); iter != end(); ++iter) {
    if (!first)
      s.append(", ");
    first = false;
    s.append(iter->first);
    s.append(" = ");
    s.append(iter->second.toString());
  }
  s.append("}");
  return s;
}

Dict::Dict(bool allow_null) : _allow_null(allow_null) {
  _content_type.type = AnyType;
  _is_global = 0;
}

Dict::Dict(Type content_type, const std::string& content_class, bool allow_null) : _allow_null(allow_null) {
  _content_type.type = content_type;
  _content_type.object_class = content_class;
  _is_global = 0;
}

auto Dict::has_key(const std::string& key) const -> bool {
  return _content.find(key) != _content.end();
}

ValueRef Dict::operator[](const std::string& key) const {
  const_iterator iter;
  if ((iter = _content.find(key)) == _content.end())
    return ValueRef();
  return iter->second;
}

auto Dict::begin() const -> Dict::const_iterator {
  return _content.begin();
}

auto Dict::end() const -> Dict::const_iterator {
  return _content.end();
}

auto Dict::keys() const -> std::vector<std::string> {
  std::vector<std::string> r;
  for (storage_type::const_iterator i = _content.begin(); i != _content.end(); ++i)
    r.push_back(i->first);
  return r;
}

auto Dict::get(const std::string& key) const -> ValueRef {
  const_iterator iter;
  if ((iter = _content.find(key)) == _content.end())
    return ValueRef();
  return iter->second;
}

auto Dict::set(const std::string& key, const ValueRef& value) -> void {
  if (!value.is_valid() && !_allow_null)
    throw std::invalid_argument("inserting null value to not null dict");

  storage_type::iterator iter = _content.find(key);

  if (_is_global > 0) {
    if (grt::GRT::get()->tracking_changes())
      grt::GRT::get()->get_undo_manager()->add_undo(new UndoDictSetAction(this, key));

    if (iter != _content.end() && iter->second.is_valid())
      iter->second.unmark_global();

    if (value.is_valid())
      value.mark_global();
  }

  _content[key] = value;
}

auto Dict::remove(const std::string& key) -> void {
  storage_type::iterator iter = _content.find(key);
  if (iter != _content.end()) {
    if (_is_global > 0) {
      if (grt::GRT::get()->tracking_changes())
        grt::GRT::get()->get_undo_manager()->add_undo(new UndoDictRemoveAction(this, key));

      if (iter->second.is_valid())
        iter->second.unmark_global();
    }

    _content.erase(iter);
  }
}

//--------------------------------------------------------------------------------------------------

/**
 * Removes all entries from this dict. The global marker is updated properly, but
 * no undo record is created (in fact, what has a high level feature like undo/redo to do
 * in such a low level storage container?).
 */
auto Dict::reset_entries() -> void {
  if (_is_global > 0) {
    if (_content_type.type == AnyType || is_container_type(_content_type.type)) {
      for (storage_type::const_iterator iter = _content.begin(); iter != _content.end(); ++iter) {
        if (iter->second.is_valid())
          iter->second.unmark_global();
      }
    }
  }
  _content.clear();
}

//--------------------------------------------------------------------------------------------------

auto Dict::mark_global() const -> void {
  if (_is_global == 0) {
    if (_content_type.type == AnyType || is_container_type(_content_type.type)) {
      for (storage_type::const_iterator iter = _content.begin(); iter != _content.end(); ++iter) {
        if (iter->second.is_valid())
          iter->second.mark_global();
      }
    }
  }
  _is_global++;
}

auto Dict::unmark_global() const -> void {
  _is_global--;

  if (_is_global == 0) {
    if (_content_type.type == AnyType || is_container_type(_content_type.type)) {
      for (storage_type::const_iterator iter = _content.begin(); iter != _content.end(); ++iter) {
        if (iter->second.is_valid())
          iter->second.unmark_global();
      }
    }
  }
}

auto Dict::reset_references() -> void {
  storage_type::iterator it = _content.begin();
  const storage_type::const_iterator last = _content.end();

  for (; last != it; ++it) {
    // g_log("grt", G_LOG_LEVEL_DEBUG, "Dict::reset_references: '%s'", it->first.c_str());
    if (it->second.is_valid())
      it->second.valueptr()->reset_references();
  }
}

auto Dict::equals(const Value* o) const -> bool {
  return this == o;
}

auto Dict::less_than(const Value* o) const -> bool {
  return this < o;
}

//--------------------------------------------------------------------------------------------------

OwnedDict::OwnedDict(Type type, const std::string& content_class, Object* owner, bool allow_null)
  : Dict(type, content_class, allow_null), _owner(owner) {
}

auto OwnedDict::set(const std::string& key, const ValueRef& value) -> void {
  Dict::set(key, value);

  _owner->owned_dict_item_set(this, key);
}

auto OwnedDict::remove(const std::string& key) -> void {
  Dict::remove(key);

  _owner->owned_dict_item_removed(this, key);
}

//--------------------------------------------------------------------------------------------------

/**
 * Removes all entries from this dict and sends a notification for each removal.
 */
auto OwnedDict::reset_entries() -> void {
  for (storage_type::const_iterator iter = _content.begin(); iter != _content.end(); ++iter) {
    _owner->owned_dict_item_removed(this, iter->first);
  }
  Dict::reset_entries();
}

//--------------------------------------------------------------------------------------------------

Object::Object(MetaClass* metaclass) : _metaclass(metaclass) {
  if (!_metaclass)
    throw std::runtime_error("GRT object allocated without a metaclass (make sure metaclass data was loaded)");

  _id = get_guid();
  _is_global = 0;
}

Object::~Object() {
}

auto Object::id() const -> const std::string& {
  return _id;
}

auto Object::get_metaclass() const -> MetaClass* {
  return _metaclass;
}

auto Object::class_name() const -> const std::string& {
  return _metaclass->name();
}

auto Object::debugDescription(const std::string& indentation) const -> std::string {
  std::string s;
  bool first = true;

  s = strfmt("{<%s> (%s)\n", _metaclass->name().c_str(), id().c_str());

  MetaClass* mc = _metaclass;

  do {
    for (MetaClass::MemberList::const_iterator iter = mc->get_members_partial().begin();
         iter != mc->get_members_partial().end(); ++iter) {
      if (iter->second.overrides)
        continue;

      if (!first)
        s.append(", ");
      first = false;

      s.append(iter->first);
      s.append(" = ");

      if (iter->second.type.base.type == ObjectType) {
        ObjectRef obj(ObjectRef::cast_from(get_member(iter->first)));
        if (obj.is_valid()) {
          if (obj.has_member("name"))
            s.append(indentation + strfmt("  %s: %s  (%s)", obj.get_string_member("name").c_str(),
                                          obj.get_metaclass()->name().c_str(), obj.id().c_str()));
          else
            s.append(indentation + strfmt("  %s (%s)", obj.get_metaclass()->name().c_str(), obj.id().c_str()));
        } else
          s.append(indentation + strfmt("  %s: null", iter->first.c_str()));
      } else
        s.append(get_member(iter->first).debugDescription(indentation + "  "));
    }

    mc = mc->parent();
  } while (mc != 0);

  s.append(indentation + "}\n");

  return s;
}

auto Object::toString() const -> std::string {
  std::string s;
  bool first = true;

  s = strfmt("{<%s> (%s)\n", _metaclass->name().c_str(), id().c_str());

  MetaClass* mc = _metaclass;

  do {
    for (MetaClass::MemberList::const_iterator iter = mc->get_members_partial().begin();
         iter != mc->get_members_partial().end(); ++iter) {
      if (iter->second.overrides)
        continue;

      if (!first)
        s.append(", ");
      first = false;

      s.append(iter->first);
      s.append(" = ");

      if (iter->second.type.base.type == ObjectType) {
        ObjectRef obj(ObjectRef::cast_from(get_member(iter->first)));
        if (obj.is_valid()) {
          if (obj.has_member("name"))
            s.append(strfmt("%s: %s  (%s)", obj.get_string_member("name").c_str(), obj.get_metaclass()->name().c_str(),
                            obj.id().c_str()));
          else
            s.append(strfmt("%s (%s)", obj.get_metaclass()->name().c_str(), obj.id().c_str()));
        } else
          s.append(strfmt("%s: null", iter->first.c_str()));
      } else
        s.append(get_member(iter->first).toString());
    }

    mc = mc->parent();
  } while (mc != 0);

  s.append("}");

  return s;
}

auto Object::is_instance(MetaClass* metaclass) const -> bool {
  return _metaclass->is_a(metaclass);
}

auto Object::is_instance(const std::string& name) const -> bool {
  return _metaclass->is_a(grt::GRT::get()->get_metaclass(name));
}

auto Object::set_member(const std::string& member, const ValueRef& value) -> void {
  _metaclass->set_member_value(this, member, value);
}

auto Object::get_member(const std::string& member) const -> ValueRef {
  return _metaclass->get_member_value(this, member);
}

auto Object::has_member(const std::string& member) const -> bool {
  return _metaclass->has_member(member);
}

auto Object::has_method(const std::string& method) const -> bool {
  return _metaclass->has_method(method);
}

auto Object::get_string_member(const std::string& member) const -> std::string {
  return StringRef::extract_from(_metaclass->get_member_value(this, member));
}

auto Object::get_double_member(const std::string& member) const -> Double::storage_type {
  return DoubleRef::extract_from(_metaclass->get_member_value(this, member));
}

auto Object::get_integer_member(const std::string& member) const -> Integer::storage_type {
  return IntegerRef::extract_from(_metaclass->get_member_value(this, member));
}

auto Object::call(const std::string& method, const BaseListRef& args) -> ValueRef {
  return _metaclass->call_method(this, method, args);
}

/** Evil function to set ID of an object, use only if you know what you're doing.
 */
auto Object::__set_id(const std::string& id) -> void {
  _id = id;
}

auto process_reset_references_for_member(const MetaClass::Member* m, Object* obj) -> bool {
  if (m && !m->calculated && !grt::is_simple_type(m->type.base.type)) {
    // g_log("grt", G_LOG_LEVEL_DEBUG, "\tprocess_reset_references_for_member'%s':'%s':'%s'", obj->class_name().c_str(),
    // obj->id().c_str(), m->name.c_str());
    grt::ValueRef member_value = obj->get_member(m->name);
    if (member_value.is_valid()) {
      // if the member is owned, then recursively reset references in it
      if (m->owned_object)
        member_value.valueptr()->reset_references();

      obj->signal_changed()->disconnect_all_slots();
      // set the member value to null
      obj->get_metaclass()->set_member_internal(obj, m->name, grt::ValueRef(), true);
    }
  }
  return true;
}

auto Object::reset_references() -> void {
  // g_log("grt", G_LOG_LEVEL_DEBUG, "Object::reset_references for '%s':'%s'", class_name().c_str(), id().c_str());
  _metaclass->foreach_member(std::bind(&process_reset_references_for_member, std::placeholders::_1, this));
}

auto Object::init() -> void {
}

static auto mark_global_(const MetaClass::Member* member, const Object* obj) -> bool {
  if (is_container_type(member->type.base.type)) {
    ValueRef value(obj->get_member(member->name));
    if (value.is_valid())
      value.mark_global();
  }
  return true;
}

auto Object::mark_global() const -> void {
  _is_global++;
  if (_is_global == 1)
    _metaclass->foreach_member(std::bind(&mark_global_, std::placeholders::_1, this));
}

static auto unmark_global_(const MetaClass::Member* member, const Object* obj) -> bool {
  if (is_container_type(member->type.base.type)) {
    ValueRef value(obj->get_member(member->name));
    if (value.is_valid())
      value.unmark_global();
  }
  return true;
}

auto Object::unmark_global() const -> void {
  _is_global--;
  if (_is_global == 0)
    _metaclass->foreach_member(std::bind(&unmark_global_, std::placeholders::_1, this));
}

auto Object::equals(const Value* o) const -> bool {
  return this == o;
}

auto Object::less_than(const Value* o) const -> bool {
  return this < o;
}

auto Object::owned_member_changed(const std::string& name, const grt::ValueRef& ovalue, const grt::ValueRef& nvalue) -> void {
  if (_is_global) {
    if (ovalue != nvalue) {
      if (ovalue.is_valid())
        ovalue.unmark_global();
      if (nvalue.is_valid())
        nvalue.mark_global();
    }
    if (grt::GRT::get()->tracking_changes())
      grt::GRT::get()->get_undo_manager()->add_undo(new UndoObjectChangeAction(this, name, ovalue));
  }
  _changed_signal(name, ovalue);
}

auto Object::member_changed(const std::string& name, const grt::ValueRef& ovalue, const grt::ValueRef& nvalue) -> void {
  if (_is_global && grt::GRT::get()->tracking_changes())
    grt::GRT::get()->get_undo_manager()->add_undo(new UndoObjectChangeAction(this, name, ovalue));
  _changed_signal(name, ovalue);
}

auto Object::owned_list_item_added(OwnedList* list, const grt::ValueRef& value) -> void {
  _list_changed_signal(list, true, value);
}

auto Object::owned_list_item_removed(OwnedList* list, const grt::ValueRef& value) -> void {
  _list_changed_signal(list, false, value);
}

auto Object::owned_dict_item_set(OwnedDict* dict, const std::string& key) -> void {
  _dict_changed_signal(dict, true, key);
}

auto Object::owned_dict_item_removed(OwnedDict* dict, const std::string& key) -> void {
  _dict_changed_signal(dict, false, key);
}

#ifdef USE_EXPRERIMENTAL_REFS
namespace {
  static const int max_ref_size = 32;
  /*
  This is compile time check to be sure that sizeof (ValueRef) won't grow too big without notice
  If you get an error here this means that it grown bigger than planned and set in max_ref_size
  If you are sure that ValueRef needs to grow bigger just increase max_ref_size
  */
  void check_ref_size(const char[max_ref_size - sizeof(ValueRef)]);
};

class CountedTypeHandler : public TypeHandler {
  static auto get_ptr(const TypeHandle& handle) -> internal::Value* {
    return static_cast<internal::Value*>(handle.refcounted_value.ptr_value);
  };
  virtual auto release(TypeHandle& handle) const -> void {
    get_ptr(handle)->release();
  };

  virtual auto retain(TypeHandle& handle) const -> void {
    get_ptr(handle)->retain();
  };
  virtual auto equals(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (type(handle) != other.type())
      return false;
    return get_ptr(handle)->equals(get_ptr(other.get_data()));
  };

  virtual auto less_than(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (get_ptr(handle)->get_type() != other.type())
      return get_ptr(handle)->get_type() < other.type();
    return get_ptr(handle)->less_than(get_ptr(other.get_data()));
  };
  virtual auto reset_references(TypeHandle& handle) const -> void {
    get_ptr(handle)->reset_references();
  };
  virtual auto clear(TypeHandle& handle) const -> void {
    release(handle);
  };
  virtual inline auto is_same(const TypeHandle& handle, const ValueRef& value) const -> bool {
    return get_ptr(handle) == value.valueptr();
  }
  virtual inline auto type(const TypeHandle& handle) const -> Type {
    return get_ptr(handle) ? get_ptr(handle)->get_type() : UnknownType;
  }
  auto debugDescription(const TypeHandle& handle) const -> std::string {
    return get_ptr(handle)->debugDescription();
  };
  auto toString(const TypeHandle& handle) const -> std::string {
    return get_ptr(handle)->toString();
  };
  virtual auto mark_global(const TypeHandle& handle) const -> void {
    get_ptr(handle)->mark_global();
  };
  virtual auto unmark_global(const TypeHandle& handle) const -> void {
    get_ptr(handle)->unmark_global();
  };
  virtual auto valueptr(const TypeHandle& handle) const -> internal::Value* {
    return get_ptr(handle);
  };
  virtual auto refcount(const TypeHandle& handle) const -> int {
    return get_ptr(handle)->refcount();
  };
};

static CountedTypeHandler counted_handler;

class NonCountedTypeHandler : public TypeHandler {
public:
  virtual auto release(TypeHandle& handle) const -> void {};
  virtual auto retain(TypeHandle& handle) const -> void {};
  virtual auto reset_references(TypeHandle& handle) const -> void {};
  virtual auto clear(TypeHandle& handle) const -> void {};
  virtual auto mark_global(const TypeHandle& handle) const -> void {};
  virtual auto unmark_global(const TypeHandle& handle) const -> void {};
  // Type isn't counted thus there is no refcount
  virtual auto refcount(const TypeHandle& handle) const -> int {
    return 1;
  };
  virtual auto valueptr(const TypeHandle& handle) const -> internal::Value* { // kinda hack shouldn't exist at all
    return static_cast<internal::Value*>(handle.refcounted_value.ptr_value);
  };
};

class DefaultTypeHandler : public NonCountedTypeHandler {
public:
  virtual auto type(const TypeHandle& handle) const -> Type {
    return UnknownType;
  }

  // all empty valuerefs are equal so compare only type
  virtual auto equals(const TypeHandle& handle, const ValueRef& other) const -> bool {
    return other.type() == UnknownType;
  };

  virtual auto less_than(const TypeHandle& handle, const ValueRef& other) const -> bool {
    return other.type() != UnknownType;
  };

  virtual inline auto is_same(const TypeHandle& handle, const ValueRef& value) const -> bool {
    return value.type() == UnknownType;
  }

  auto debugDescription(const TypeHandle& handle) const -> std::string {
    return "NULL";
  };

  auto toString(const TypeHandle& handle) const -> std::string {
    return "NULL";
  };
};

DefaultTypeHandler default_handler;
TypeHandler* ValueRef::_defalut_handler = &default_handler;

class DoubleTypeHandler : public NonCountedTypeHandler {
public:
  virtual auto type(const TypeHandle& handle) const -> Type {
    return DoubleType;
  }

  virtual auto equals(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (type(handle) != other.type())
      return false;
    return handle.double_value == other.get_data().double_value;
  };

  virtual auto less_than(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (type(handle) != other.type())
      return type(handle) < other.type();

    return handle.double_value < other.get_data().double_value;
  };

  virtual inline auto is_same(const TypeHandle& handle, const ValueRef& value) const -> bool {
    return handle.double_value == value.get_data().double_value;
  }

  auto debugDescription(const TypeHandle& handle) const -> std::string {
    return toString(handle);
  };

  auto toString(const TypeHandle& handle) const -> std::string {
    return std::to_string(handle.double_value);
  };
};

static DoubleTypeHandler double_handler;

class IntegerTypeHandler : public NonCountedTypeHandler {
public:
  virtual auto type(const TypeHandle& handle) const -> Type {
    return IntegerType;
  }

  virtual auto equals(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (type(handle) != other.type())
      return false;
    return handle.int_value == other.get_data().int_value;
  };

  virtual auto less_than(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (type(handle) != other.type())
      return type(handle) < other.type();

    return handle.int_value < other.get_data().int_value;
  };

  virtual inline auto is_same(const TypeHandle& handle, const ValueRef& value) const -> bool {
    return handle.int_value == value.get_data().int_value;
  }

  auto debugDescription(const TypeHandle& handle) const -> std::string {
    return toString(handle);
  };

  auto toString(const TypeHandle& handle) const -> std::string {
    return std::to_string(handle.int_value);
  };
};

static IntegerTypeHandler int_handler;

class StringTypeHandler : public TypeHandler {
public:
  static const int small_string_size = 36;
  static auto get_ptr(const TypeHandle& handle) -> StringRef::storage_type {
    return handle.string_ptr;
  };
  virtual auto equals(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (type(handle) != other.type())
      return false;
    return !strcmp(get_ptr(handle), get_ptr(other.get_data())); // get_ptr(handle)->equals(get_ptr(other.get_data()));
  };

  virtual auto less_than(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (type(handle) != other.type())
      return type(handle) < other.type();
    return strcmp(get_ptr(handle), get_ptr(other.get_data())) < 0;
  };

  virtual auto reset_references(TypeHandle& handle) const -> void {};

  virtual auto clear(TypeHandle& handle) const -> void {
    release(handle);
  };

  virtual inline auto is_same(const TypeHandle& handle, const ValueRef& value) const -> bool {
    return get_ptr(handle) == value.get_data().string_ptr;
  }
  virtual inline auto type(const TypeHandle& handle) const -> Type {
    return StringType;
  }
  auto debugDescription(const TypeHandle& handle) const -> std::string {
    return get_ptr(handle);
  };
  auto toString(const TypeHandle& handle) const -> std::string {
    return get_ptr(handle);
  };
  virtual auto mark_global(const TypeHandle& handle) const -> void {};
  virtual auto unmark_global(const TypeHandle& handle) const -> void {};
  virtual auto valueptr(const TypeHandle& handle) const -> internal::Value* {
    return NULL;
  };
  virtual auto refcount(const TypeHandle& handle) const -> int {
    return 1;
  };
};

struct CountedStringDataStruct {
  int refcounter;
  //    size_t size;
};

#ifdef _DEBUG
//#define STRING_ALLOCATION_LOGGER_ENABLED
#endif

#ifdef STRING_ALLOCATION_LOGGER_ENABLED
#include <iostream>
struct stringlogger {
  static const char delim = '\t';
  struct string_log_data {
    int alloc_count;
    int dealloc_count;
    int retain_count;
    int release_count;
  };
  std::map<std::string, string_log_data> log;
  auto log_string_alloc(const char* str) -> void {
    log[str].alloc_count++;
  };

  auto log_string_dealloc(const char* str) -> void {
    log[str].dealloc_count++;
  };

  auto log_string_retain(const char* str) -> void {
    log[str].retain_count++;
  };

  auto log_string_release(const char* str) -> void {
    log[str].release_count++;
  };

  ~stringlogger() {
    for (std::map<std::string, string_log_data>::iterator It = log.begin(); It != log.end(); ++It) {
      if (It->first.size() > 36)
        continue;
      std::cerr << It->first << delim << It->second.retain_count << delim << It->second.alloc_count << delim
                << It->first.size() << std::endl;
    }
  }
};
stringlogger logger;

#endif

static char empty_string = 0;
class EmptyStringTypeHandler : public StringTypeHandler {
public:
  auto allocate(const char* srcstring, const size_t len) const -> StringRef::storage_type {
    return len ? NULL : &empty_string;
  };

  virtual auto equals(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (type(handle) != other.type())
      return false;
    return get_ptr(other.get_data())[0] == 0;
  };

  virtual auto release(TypeHandle& handle) const -> void {};
  virtual auto retain(TypeHandle& handle) const -> void {};
};

EmptyStringTypeHandler empty_string_handler;

const char* fixed_strins[] = { "Application/MySqlStudio",
                               "standalone",
                               "string",
                               "MySqlStudio",
                               "normal",
                               "action",
                               "internal",
                               "db.Catalog",
                               "Menu/SQL/Utilities",
                               ".",
                               "gui",
                               "Menu/Utilities",
                               "activeDiagram",
                               "file",
                               "keychain",
                               "model.Diagram",
                               "Catalog/Utilities",
                               "Menu/Text",
                               "Password:",
                               "database/Database",
                               "Username:",
                               "userName",
                               "schema",
                               "root",
                               "pyodbc",
                               "Menu/Objects",
                               "catalog/Editors",
                               "SQLIDEUtils",
                               "Home",
                               "boolean",
                               "*model",
                               "db.query.QueryEditor",
                               "activeQueryEditor",
                               "activeCatalog",
                               "WbUtils",
                               "Menu/SQL/Resultset",
                               "Menu/Catalog",
                               "Filter",
                               "Name of the user to connect with.",
                               "separator",
                               "Database",
                               "MySQLDbModule",
                               "http://www.mysql.com",
                               "FALSE",
                               "127.0.0.1",
                               "activeModel",
                               ".\\db.mysql.editors.wbp.fe.dll",
                               "Others/Menu/Ungrouped",
                               "Database:",
                               "Oracle Corp.",
                               "Validation",
                               "save",
                               "Home/Connections",
                               "Home/Instances",
                               "Home/ModelFiles",
                               "Export",
                               "Menu/Model",
                               "Editors",
                               "WbModel",
                               "Menu/Database",
                               "Menu/SQL/Script",
                               "Menu/SQL/Catalog",
                               "Menu/SQL/Editor",
                               "model/Editors",
                               "hostName",
                               "port",
                               "int",
                               "enum",
                               "sa",
                               "model.Model",
                               "%userName%::Mysql@%hostName%:%port%",
                               "SQLEditor",
                               "Menu/Ungrouped",
                               "TRUE",
                               "WbAdmin",
                               "PyWbUtils",
                               "studio.physical.Model",
                               "open",
                               ".\\wb.model.editors.wbp.fe.dll",
                               "Menu/Administrator",
                               "WB Plugin",
                               "Model",
                               "localhost",
                               "Website",
                               "SSL Key File:",
                               "sslCA",
                               "Path to Key file for SSL.",
                               "Default Schema:",
                               "SSL CA File:",
                               "SSL CERT File:",
                               "SSL Cipher:",
                               "Use SSL if available.",
                               "tristate",
                               "sslKey",
                               "sslCipher",
                               "getSchemata",
                               "useAnsiQuotes",
                               "sslCert",
                               "Path to Certificate file for SSL.",
                               "mysqlcppconn",
                               "useSSL",
                               "ReverseEngineeringMysql",
                               "TCP/IP port.",
                               "Hostname:",
                               "Port:",
                               "Pre-configured ODBC data source.",
                               "driver",
                               "Driver:",
                               "DSN:",
                               "dsn",
                               "DbMssqlRE",
                               "getDataSourceNames",
                               "INT",
                               "windows,linux",
                               "Open Model",
                               "db.mysql.Table",
                               "Model/Printing",
                               "BINARY",
                               "*query",
                               "binary" };

struct str_less {
  bool operator()(const char* str1, const char* str2) const {
    return strcmp(str1, str2) < 0;
  };
};
str_less str_less_pred;

class FixedStringTypeHandler : public StringTypeHandler {
public:
  FixedStringTypeHandler() {
    std::sort(fixed_strins, fixed_strins + sizeof(fixed_strins) / sizeof(fixed_strins[0]), str_less_pred);
  }

  auto allocate(const char* srcstring, const size_t len) const -> StringRef::storage_type {
    if (len >= small_string_size)
      return NULL;
    const char** found = std::lower_bound(fixed_strins, fixed_strins + sizeof(fixed_strins) / sizeof(fixed_strins[0]),
                                          srcstring, str_less_pred);
    if (found != fixed_strins + sizeof(fixed_strins) / sizeof(fixed_strins[0]) && !strcmp(*found, srcstring))
      return *found;
    return NULL;
  };

  virtual auto equals(const TypeHandle& handle, const ValueRef& other) const -> bool {
    if (type(handle) != other.type())
      return false;
    return get_ptr(handle) == get_ptr(other.get_data());
  };

  virtual auto release(TypeHandle& handle) const -> void {};
  virtual auto retain(TypeHandle& handle) const -> void {};
};

FixedStringTypeHandler fixed_string_handler;

class ShortStringTypeHandler : public StringTypeHandler {
protected:
  struct SmallStringBlock {
    CountedStringDataStruct string_data;
    char buffer[small_string_size];
  };

  // NODO: implement dynamic buffers allocation
  static const int smallstrings_buffer_size = 10240;
  SmallStringBlock smallstrings[smallstrings_buffer_size];
  mutable std::vector<SmallStringBlock*> small_strings_stack;
  static GStaticMutex _allocation_mutex;

public:
  ShortStringTypeHandler() {
    small_strings_stack.reserve(smallstrings_buffer_size);
    for (int i = 0; i < smallstrings_buffer_size; ++i)
      small_strings_stack.push_back(&smallstrings[i]);
  };

  ~ShortStringTypeHandler() {
    g_static_mutex_free(&_allocation_mutex);
  }

  auto allocate(const char* srcstring, const size_t len) -> StringRef::storage_type {
    base::GStaticMutexLock lock(_allocation_mutex);

    if ((len >= small_string_size) || small_strings_stack.empty())
      return NULL;

    SmallStringBlock* block_ptr = small_strings_stack.back();
    small_strings_stack.pop_back();
    block_ptr->string_data.refcounter = 0;
    memcpy(block_ptr->buffer, srcstring, len + 1);
    return block_ptr->buffer;
  };

  virtual auto release(TypeHandle& handle) const -> void {
#ifdef STRING_ALLOCATION_LOGGER_ENABLED
    logger.log_string_release(get_ptr(handle));
#endif
    CountedStringDataStruct* ptr = (CountedStringDataStruct*)(handle.string_ptr) - 1;
    ptr->refcounter--;
    if (ptr->refcounter <= 0) {
      base::GStaticMutexLock lock(_allocation_mutex);
      small_strings_stack.push_back(reinterpret_cast<SmallStringBlock*>(ptr));
    }
  };

  virtual auto retain(TypeHandle& handle) const -> void {
#ifdef STRING_ALLOCATION_LOGGER_ENABLED
    logger.log_string_retain(get_ptr(handle));
#endif

    CountedStringDataStruct* ptr = (CountedStringDataStruct*)(handle.string_ptr) - 1;
    ptr->refcounter++;
  };
};

GStaticMutex ShortStringTypeHandler::_allocation_mutex = G_STATIC_MUTEX_INIT;

ShortStringTypeHandler short_string_handler;

class LongStringTypeHandler : public StringTypeHandler {
public:
  auto allocate(const char* srcstring, const size_t len) const -> StringRef::storage_type {
    size_t size = len + 1 + sizeof(CountedStringDataStruct);
    void* buffer = new char[size];
    memset(buffer, 0, sizeof(CountedStringDataStruct));
    char* string_buffer = static_cast<char*>(buffer) + sizeof(CountedStringDataStruct);
    memcpy(string_buffer, srcstring, len + 1);
    return string_buffer;
  };

  virtual auto release(TypeHandle& handle) const -> void {
#ifdef STRING_ALLOCATION_LOGGER_ENABLED
    logger.log_string_release(get_ptr(handle));
#endif

    CountedStringDataStruct* ptr = (CountedStringDataStruct*)(handle.string_ptr) - 1;
    ptr->refcounter--;
    if (ptr->refcounter <= 0)
      delete[](ptr);
  };

  virtual auto retain(TypeHandle& handle) const -> void {
#ifdef STRING_ALLOCATION_LOGGER_ENABLED
    logger.log_string_retain(get_ptr(handle));
#endif
    CountedStringDataStruct* ptr = (CountedStringDataStruct*)(handle.string_ptr) - 1;
    ptr->refcounter++;
  };
};

static LongStringTypeHandler long_string_handler;

auto grt::get_string_type_handler(TypeHandler::TypeHandle& handle, const char* srcstring, const size_t len) -> TypeHandler* {
#ifdef STRING_ALLOCATION_LOGGER_ENABLED
  logger.log_string_alloc(srcstring);
#endif

  if ((handle.string_ptr = empty_string_handler.allocate(srcstring, len)))
    return &empty_string_handler;
  if ((handle.string_ptr = fixed_string_handler.allocate(srcstring, len)))
    return &fixed_string_handler;
  else if ((handle.string_ptr = short_string_handler.allocate(srcstring, len)))
    return &short_string_handler;
  else if ((handle.string_ptr = long_string_handler.allocate(srcstring, len)))
    return &long_string_handler;
  else
    throw std::runtime_error("String Allocation Failed");
}

auto grt::get_int_type_handler() -> TypeHandler* {
  return &int_handler;
}

auto grt::get_double_type_handler() -> TypeHandler* {
  return &double_handler;
}

auto grt::get_object_type_handler() -> TypeHandler* {
  return &counted_handler;
};
#endif
