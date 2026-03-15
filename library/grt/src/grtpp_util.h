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

#include <set>

#include "base/any.h"
#include "base/string_utilities.h"

#define GRTLIST_FOREACH(type, list, iter) \
  for (grt::ListRef<type>::const_iterator iter##end = list.end(), iter = list.begin(); iter != iter##end; ++iter)

#define GRTLIST_REVERSE_FOREACH(type, list, iter)                                                                   \
  for (grt::ListRef<type>::const_reverse_iterator iter##end = list.rend(), iter = list.rbegin(); iter != iter##end; \
       ++iter)

namespace grt {
  auto MYSQLGRT_PUBLIC convert(const grt::DictRef dict) -> std::map<std::string, base::any>;

  auto MYSQLGRT_PUBLIC type_to_str(Type type) -> std::string;
  auto MYSQLGRT_PUBLIC str_to_type(const std::string &str) -> Type;

  auto MYSQLGRT_PUBLIC fmt_simple_type_spec(const SimpleTypeSpec &type) -> std::string;
  auto MYSQLGRT_PUBLIC fmt_type_spec(const TypeSpec &type) -> std::string;
  auto MYSQLGRT_PUBLIC fmt_arg_spec_list(const ArgSpecList &args) -> std::string;

  auto MYSQLGRT_PUBLIC get_value_by_path(const ValueRef &root, const std::string &path) -> ValueRef;
  auto MYSQLGRT_PUBLIC set_value_by_path(const ValueRef &value, const std::string &path, const ValueRef &new_value)
    -> bool;

  inline auto is_container_type(Type type) -> bool {
    if (type == ListType || type == DictType || type == ObjectType)
      return true;
    return false;
  }

  inline auto is_simple_type(Type type) -> bool {
    if (type == IntegerType || type == DoubleType || type == StringType)
      return true;
    return false;
  }

  auto MYSQLGRT_PUBLIC get_guid() -> std::string;

  inline auto path_base(const std::string &path) -> std::string {
    std::string::size_type p = path.rfind('/');
    if (p != std::string::npos)
      return path.substr(0, p);
    return "";
  }

  inline auto path_last(const std::string &path) -> std::string {
    std::string::size_type p = path.rfind('/');
    if (p != std::string::npos)
      return path.substr(p);
    return "";
  }

  template <class O>
  inline auto find_named_object_in_list(const ListRef<O> &list, const std::string &value, bool case_sensitive = true,
                                        const std::string &name = "name") -> Ref<O> {
    for (size_t i = 0; i < list.count(); i++) {
      Ref<O> tmp = list[i];

      if (tmp.is_valid() && base::same_string(tmp->get_string_member(name), value, case_sensitive))
        return tmp;
    }
    return Ref<O>();
  }

  template <class O>
  inline auto find_object_in_list(const ListRef<O> &list, const std::string &id) -> Ref<O> {
    size_t i, c = list.count();
    for (i = 0; i < c; i++) {
      Ref<O> value = list[i];

      if (value.is_valid() && value->id() == id)
        return value;
    }
    return Ref<O>();
  }

  template <class O>
  inline auto find_object_index_in_list(ListRef<O> list, const std::string &id) -> size_t {
    size_t i, c = list.count();
    for (i = 0; i < c; i++) {
      Ref<O> value = list.get(i);

      if (value.is_valid() && value.id() == id)
        return i;
    }
    return -1;
  }

  template <typename TPredicate>
  auto get_name_suggestion(TPredicate duplicate_found_pred, const std::string &prefix, const bool serial)
    -> std::string {
    char buffer[30] = "";
    int x = 1;
    std::string name;

    if (serial)
      g_snprintf(buffer, sizeof(buffer), "%i", x);
    name = prefix + buffer;
    while (duplicate_found_pred(name)) {
      g_snprintf(buffer, sizeof(buffer), "%i", x++);
      name = prefix + buffer;
    }
    return name;
  }

  MYSQLGRT_PUBLIC auto get_name_suggestion_for_list_object(const BaseListRef &objlist, const std::string &prefix,
                                                           bool serial = true) -> std::string;

  MYSQLGRT_PUBLIC auto find_child_object(const DictRef &dict, const std::string &id, bool recursive = true)
    -> ObjectRef;
  MYSQLGRT_PUBLIC auto find_child_object(const BaseListRef &list, const std::string &id, bool recursive = true)
    -> ObjectRef;
  MYSQLGRT_PUBLIC auto find_child_object(const ObjectRef &object, const std::string &id, bool recursive = true)
    -> ObjectRef;

  MYSQLGRT_PUBLIC auto update_ids(ObjectRef object,
                                  const std::set<std::string> &skip_members = std::set<std::string>()) -> void;
  // the following merge functions are not recursive
  MYSQLGRT_PUBLIC auto append_contents(BaseListRef target, BaseListRef source) -> void;
  MYSQLGRT_PUBLIC auto replace_contents(BaseListRef target, BaseListRef source) -> void;
  MYSQLGRT_PUBLIC auto merge_contents_by_name(ObjectListRef target, ObjectListRef source, bool replace_matching) -> void;
  MYSQLGRT_PUBLIC auto merge_contents_by_id(ObjectListRef target, ObjectListRef source, bool replace_matching) -> void;

  MYSQLGRT_PUBLIC auto replace_contents(DictRef target, DictRef source) -> void;
  MYSQLGRT_PUBLIC auto merge_contents(DictRef target, DictRef source, bool overwrite) -> void;
  MYSQLGRT_PUBLIC auto merge_contents(ObjectRef target, ObjectRef source) -> void;

  MYSQLGRT_PUBLIC auto compare_list_contents(const ObjectListRef &list1, const ObjectListRef &list2) -> bool;

  MYSQLGRT_PUBLIC auto join_string_list(const StringListRef &list, const std::string &separator) -> std::string;

  MYSQLGRT_PUBLIC auto remove_list_items_matching(ObjectListRef list,
                                                  const std::function<bool(grt::ObjectRef)> &matcher) -> void;

  // XXX don't use this for objects, use CopyContext::copy() instead
  MYSQLGRT_PUBLIC auto copy_value(ValueRef value, bool deep) -> ValueRef;

  struct MYSQLGRT_PUBLIC CopyContext {
    std::map<std::string, ValueRef> object_copies;
    std::list<ObjectRef> copies;

    CopyContext() {
    }

    auto copy(const ObjectRef &object, std::set<std::string> skip_members = std::set<std::string>()) -> ObjectRef;
    auto shallow_copy(const ObjectRef &object) -> ObjectRef;
    auto finish() -> void {
      update_references();
    }
    auto update_references() -> void;

    auto copy_for_object(ValueRef object) -> ValueRef;

  private:
    auto duplicate_object(ObjectRef object, std::set<std::string> skip_members, bool dontfollow) -> ObjectRef;
    auto copy_list(BaseListRef &list, const BaseListRef &source, bool dontfollow) -> void;
    auto copy_dict(DictRef &dict, const DictRef &source, bool dontfollow) -> void;
  };

  template <typename OType>
  auto copy_object(const OType &object, std::set<std::string> skip_members = std::set<std::string>()) -> OType {
    CopyContext copier;
    OType copy;

    copy = OType::cast_from(copier.copy(object, skip_members));
    copier.finish();

    return copy;
  }

  template <typename OType>
  auto shallow_copy_object(const OType &object) -> OType {
    CopyContext copier;
    OType copy;

    copy = OType::cast_from(copier.shallow_copy(object));
    // this is a shallow copy so changing the references in it's sub-values would mean modifying
    // their originals
    // copier.finish();

    return copy;
  }

  MYSQLGRT_PUBLIC auto dump_value(const grt::ValueRef &value) -> void;

  // temporary code
  MYSQLGRT_PUBLIC auto init_python_support(const std::string &python_module_path) -> bool;
  MYSQLGRT_PUBLIC auto add_python_module_dir(const std::string &python_module_path) -> void;

  // diffing

  class DiffChange;
  using TSlotNormalizerSlot = std::function<bool(ValueRef, ValueRef, std::string)>;

  struct MYSQLGRT_PUBLIC Omf {
    TSlotNormalizerSlot normalizer;
    bool case_sensitive;
    bool skip_routine_definer;
    // some structure members needs to be skipped only when diffing model vs db but not when diffing model
    //_dontdiff_mask will hold mask to allow selective bypass of ceratin fields
    // 1 always diff, 2 diff only vs db, 4 diff only vs live object
    unsigned int dontdiff_mask;
    Omf() : case_sensitive(true), skip_routine_definer(false), dontdiff_mask(1) {};
    virtual ~Omf() {};
    virtual auto less(const ValueRef &, const ValueRef &) const -> bool = 0;
    virtual auto equal(const ValueRef &, const ValueRef &) const -> bool = 0;
  };

  struct default_omf : public Omf {
    auto peq(const ValueRef &l, const ValueRef &r) const -> bool {
      if ((l.type() == r.type() && l.type() == ObjectType) && ObjectRef::can_wrap(l) && ObjectRef::can_wrap(r)) {
        ObjectRef left = ObjectRef::cast_from(l);
        ObjectRef right = ObjectRef::cast_from(r);
        if (left->has_member("name"))
          return left->get_string_member("name") == right->get_string_member("name");
      }
      return l == r;
    }

    auto pless(const ValueRef &l, const ValueRef &r) const -> bool {
      if ((l.type() == r.type() && l.type() == ObjectType) && ObjectRef::can_wrap(l) && ObjectRef::can_wrap(r)) {
        ObjectRef left = ObjectRef::cast_from(l);
        ObjectRef right = ObjectRef::cast_from(r);
        if (left->has_member("name"))
          return left->get_string_member("name") < right->get_string_member("name");
      }
      return l < r;
    }

    virtual auto less(const ValueRef &l, const ValueRef &r) const -> bool {
      return pless(l, r);
    };
    virtual auto equal(const ValueRef &l, const ValueRef &r) const -> bool {
      return peq(l, r);
    };
  };

  MYSQLGRT_PUBLIC
  auto diff_make(const ValueRef &source, const ValueRef &target, const Omf *omf, bool dont_clone_values = false)
    -> std::shared_ptr<DiffChange>;
}; // namespace grt
