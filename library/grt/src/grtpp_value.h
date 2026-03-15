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

#pragma once

#ifdef _MSC_VER
#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef int ssize_t;
#endif
#endif

#include <boost/signals2.hpp>
#include "base/threading.h"

namespace grt {
  class GRT;
  class MetaClass;

  namespace internal {
    class Serializer;
    class Unserializer;
  }; // namespace internal

  //------------------------------------------------------------------------------------------------

  enum Type { UnknownType, AnyType = UnknownType, IntegerType, DoubleType, StringType, ListType, DictType, ObjectType };

  struct MYSQLGRT_PUBLIC SimpleTypeSpec {
    Type type;
    std::string object_class;

    SimpleTypeSpec() : type(UnknownType) {
    }

    SimpleTypeSpec(const SimpleTypeSpec &o) : type(o.type), object_class(o.object_class) {
    }

    inline auto operator=(const SimpleTypeSpec &o) -> SimpleTypeSpec & {
      type = o.type;
      object_class = o.object_class;
      return *this;
    }

    inline auto operator==(const SimpleTypeSpec &o) const -> bool {
      return o.type == type && o.object_class == object_class;
    }

    inline auto operator!=(const SimpleTypeSpec &o) const -> bool {
      return o.type != type || o.object_class != object_class;
    }
  };

  /*!
    \brief Type definition for GRT objects
    Describes type if a member defined by TypeSpec is of type: Integer, Double, String, etc
    Describes class if a member defined by TypeSpec is of Object type
  */
  struct MYSQLGRT_PUBLIC TypeSpec {
    SimpleTypeSpec base;    //!< Type of the object itself
    SimpleTypeSpec content; //!< Type of the stored items in case when object is of type of ListRef<T> or Dict
    auto operator==(const TypeSpec &t) const -> bool {
      return (base == t.base && content == t.content);
    }
  };

  class ValueRef;
  class BaseListRef;

  //------------------------------------------------------------------------------------------------

  class MYSQLGRT_PUBLIC type_error : public std::logic_error {
  public:
    type_error(Type expected, Type actual);
    type_error(TypeSpec expected, TypeSpec actual);
    type_error(Type expected, Type actual, Type container);
    type_error(const std::string &expected, const std::string &actual);
    type_error(const std::string &expected, const std::string &actual, Type container);
    type_error(const std::string &expected, Type actual);
    type_error(const std::string &msg) : std::logic_error(msg) {
    }
  };

  //------------------------------------------------------------------------------------------------

  namespace internal {

    class Object;

    class MYSQLGRT_PUBLIC Value {
    public:
      virtual ~Value() {
      }

      virtual auto get_type() const -> Type = 0;

      auto retain() -> Value *;
      void release();

      virtual auto debugDescription(const std::string &indentation = "") const -> std::string = 0;
      virtual auto toString() const -> std::string = 0;

      auto refcount() const -> base::refcount_t;

      virtual void mark_global() const {
      }
      virtual void unmark_global() const {
      }

      virtual auto equals(const Value *) const -> bool = 0;
      virtual auto less_than(const Value *) const -> bool = 0;

      // This method helps to free memory allocated by Value.
      // It is overridden in Object, List and Dict.
      virtual void reset_references() {
      }

    protected:
      Value() : _refcount(0) {
      }

    private:
      Value(const Value &) {
      }

      volatile mutable base::refcount_t _refcount;
    };

    // 32 bit or 64 bit integer type.
    class MYSQLGRT_PUBLIC Integer : public Value {
    public:
      using storage_type = ssize_t;

    public:
      Integer(storage_type value);
      static auto get(storage_type value) -> Integer *;

      static auto static_type() -> Type {
        return IntegerType;
      }
      virtual auto get_type() const -> Type {
        return IntegerType;
      }
      virtual auto debugDescription(const std::string &indentation = "") const -> std::string;
      virtual auto toString() const -> std::string;

      inline operator storage_type() const {
        return _value;
      }
      inline auto operator*() const -> storage_type {
        return _value;
      }

      virtual auto equals(const Value *) const -> bool;
      virtual auto less_than(const Value *) const -> bool;

    protected:
      storage_type _value;
    };

    //------------------------------------------------------------------------------------------------

    class MYSQLGRT_PUBLIC Double : public Value {
    public:
      using storage_type = double;

    public:
      Double(storage_type value);
      static auto get(storage_type value) -> Double *;

      static auto static_type() -> Type {
        return DoubleType;
      }
      virtual auto get_type() const -> Type {
        return DoubleType;
      }
      virtual auto debugDescription(const std::string &indentation = "") const -> std::string;
      virtual auto toString() const -> std::string;

      inline operator storage_type() const {
        return _value;
      }
      inline auto operator*() const -> storage_type {
        return _value;
      }

      virtual auto equals(const Value *) const -> bool;
      virtual auto less_than(const Value *) const -> bool;

    protected:
      storage_type _value;
    };

    //------------------------------------------------------------------------------------------------

    class MYSQLGRT_PUBLIC String : public Value {
    public:
      using storage_type = std::string;

    public:
      String(const storage_type &value);
      static auto get(const storage_type &value) -> String *;

      static auto static_type() -> Type {
        return StringType;
      }
      virtual auto get_type() const -> Type {
        return StringType;
      }
      virtual auto debugDescription(const std::string &indentation = "") const -> std::string;
      virtual auto toString() const -> std::string;

      inline operator storage_type() const {
        return _value;
      }
      inline auto operator*() const -> const storage_type & {
        return _value;
      }
      inline auto c_str() const -> const char * {
        return _value.c_str();
      }
      inline auto empty() const -> bool {
        return _value.empty();
      }

      virtual auto equals(const Value *) const -> bool;
      virtual auto less_than(const Value *) const -> bool;

    protected:
      storage_type _value;
    };

    //------------------------------------------------------------------------------------------------

    class MYSQLGRT_PUBLIC List : public Value {
    public:
      using storage_type = std::vector<ValueRef>;
      enum { npos = 0xffffffff };

      using raw_const_iterator = std::vector<ValueRef>::const_iterator;
      using raw_const_reverse_iterator = std::vector<ValueRef>::const_reverse_iterator;
      using raw_iterator = std::vector<ValueRef>::iterator;

    public:
      List(bool allow_null);
      List(Type type, const std::string &content_class, bool allow_null);

      static auto static_type() -> Type {
        return ListType;
      }
      virtual auto get_type() const -> Type {
        return ListType;
      }
      inline auto content_type_spec() const -> const SimpleTypeSpec & {
        return _content_type;
      }
      inline auto content_type() const -> Type {
        return _content_type.type;
      }
      inline auto content_class_name() const -> const std::string & {
        return _content_type.object_class;
      }
      virtual auto debugDescription(const std::string &indentation = "") const -> std::string;
      virtual auto toString() const -> std::string;

      inline auto get(size_t index) const -> const ValueRef & {
        if (index >= count())
          throw bad_item(index, count());
        return _content[index];
      }

      virtual void set_unchecked(size_t index, const ValueRef &value);
      virtual void insert_unchecked(const ValueRef &value, size_t index = npos);

      void set_checked(size_t index, const ValueRef &value);
      void insert_checked(const ValueRef &value, size_t index = npos);

      auto check_assignable(const ValueRef &value) const -> bool;
      auto null_allowed() const -> bool {
        return _allow_null;
      }

      inline auto count() const -> size_t {
        return _content.size();
      }

      virtual void remove(const ValueRef &value);
      virtual void remove(size_t index);
      void reorder(size_t oi, size_t ni);

      auto get_index(const ValueRef &value) -> size_t;

      inline auto operator[](size_t i) const -> const ValueRef & {
        return get(i);
      }

      virtual auto equals(const Value *) const -> bool;
      virtual auto less_than(const Value *) const -> bool;

      auto raw_begin() const -> raw_const_iterator {
        return _content.begin();
      }
      auto raw_end() const -> raw_const_iterator {
        return _content.end();
      }

      auto raw_rbegin() const -> raw_const_reverse_iterator {
        return _content.rbegin();
      }
      auto raw_rend() const -> raw_const_reverse_iterator {
        return _content.rend();
      }

      auto raw_begin() -> raw_iterator {
        return _content.begin();
      }
      auto raw_end() -> raw_iterator {
        return _content.end();
      }

    public:
      void __retype(Type type, const std::string &content_class);

      virtual void mark_global() const;
      virtual void unmark_global() const;

      virtual void reset_references();

    protected:
      friend class ::grt::GRT;
      friend class internal::Serializer;
      friend class internal::Unserializer;

      virtual ~List();

      storage_type _content;
      SimpleTypeSpec _content_type;
      bool _allow_null;

      mutable short _is_global;
    };

    class MYSQLGRT_PUBLIC OwnedList : public List {
    public:
      OwnedList(Type type, const std::string &content_class, Object *owner, bool allow_null);

      virtual void set_unchecked(size_t index, const ValueRef &value);
      virtual void insert_unchecked(const ValueRef &value, size_t index = npos);

      virtual void remove(const ValueRef &value);
      virtual void remove(size_t index);

      auto owner_of_owned_list() const -> Object * {
        return _owner;
      }

    protected:
      Object *_owner; // internal: set if it belongs to an object
    };

    //------------------------------------------------------------------------------------------------

    class MYSQLGRT_PUBLIC Dict : public Value {
    public:
      using storage_type = std::map<std::string, ValueRef>;
      using const_iterator = storage_type::const_iterator;
      using iterator = storage_type::const_iterator;

    public:
      Dict(bool allow_null);
      Dict(Type type, const std::string &content_class, bool allow_null);

      static auto static_type() -> Type {
        return DictType;
      }
      virtual auto get_type() const -> Type {
        return DictType;
      }
      virtual auto debugDescription(const std::string &indentation = "") const -> std::string;
      virtual auto toString() const -> std::string;

      auto content_type() const -> Type {
        return _content_type.type;
      }
      auto content_class_name() const -> const std::string & {
        return _content_type.object_class;
      }

      auto operator[](const std::string &key) const -> ValueRef;

      auto begin() const -> const_iterator;
      auto end() const -> const_iterator;

      auto has_key(const std::string &key) const -> bool;
      auto get(const std::string &key) const -> ValueRef;
      virtual void set(const std::string &key, const ValueRef &value);
      virtual void remove(const std::string &key);
      virtual void reset_entries();
      auto count() const -> size_t {
        return _content.size();
      }

      auto keys() const -> std::vector<std::string>;

      virtual auto equals(const Value *) const -> bool;
      virtual auto less_than(const Value *) const -> bool;

      virtual void mark_global() const;
      virtual void unmark_global() const;

      virtual void reset_references();

    protected:
      friend class ::grt::GRT;

      storage_type _content;
      SimpleTypeSpec _content_type;
      bool _allow_null;

      mutable short _is_global; // whether value is attached to the global GRT tree
    };

    class MYSQLGRT_PUBLIC OwnedDict : public Dict {
    public:
      OwnedDict(Type type, const std::string &content_class, Object *owner, bool allow_null);

      virtual void set(const std::string &key, const ValueRef &value);
      virtual void remove(const std::string &key);
      virtual void reset_entries();

      auto owner_of_owned_dict() const -> Object * {
        return _owner;
      }

    protected:
      Object *_owner; // internal: set if it belongs to an object
    };

    //------------------------------------------------------------------------------------------------

    /** Base GRT Object class.
     *
     * This is subclassed by automatically generated GRT object classes.
     *
     * @ingroup GRT
     */
    class MYSQLGRT_PUBLIC Object : public Value {
    public:
      static auto static_class_name() -> std::string {
        return "Object";
      }

      virtual ~Object();

      auto id() const -> const std::string &;
      auto get_metaclass() const -> MetaClass *;
      auto class_name() const -> const std::string &;

      static auto static_type() -> Type {
        return ObjectType;
      }
      virtual auto get_type() const -> Type {
        return ObjectType;
      }
      virtual auto debugDescription(const std::string &indentation = "") const -> std::string;
      virtual auto toString() const -> std::string;

      auto is_instance(MetaClass *gclass) const -> bool;
      auto is_instance(const std::string &name) const -> bool;

      void set_member(const std::string &member, const ValueRef &value);
      auto get_member(const std::string &member) const -> ValueRef;
      auto get_string_member(const std::string &member) const -> std::string;
      auto get_double_member(const std::string &member) const -> Double::storage_type;
      auto get_integer_member(const std::string &member) const -> Integer::storage_type;
      auto has_member(const std::string &member) const -> bool;

      auto has_method(const std::string &method) const -> bool;

      virtual auto equals(const Value *) const -> bool;
      virtual auto less_than(const Value *) const -> bool;

      virtual auto call(const std::string &method, const BaseListRef &args) -> ValueRef;

      auto is_global() const -> bool {
        return _is_global != 0;
      }

      auto signal_changed() -> boost::signals2::signal<void(const std::string &, const ValueRef &)> * {
        return &_changed_signal;
      }
      auto signal_list_changed() -> boost::signals2::signal<void(OwnedList *, bool, const grt::ValueRef &)> * {
        return &_list_changed_signal;
      }
      auto signal_dict_changed() -> boost::signals2::signal<void(OwnedDict *, bool, const std::string &)> * {
        return &_dict_changed_signal;
      }

      virtual void reset_references();

    public:
      virtual void init();

      void __set_id(const std::string &id);

      virtual void mark_global() const;
      virtual void unmark_global() const;

    protected:
      friend class OwnedList;
      friend class OwnedDict;
      friend class ::grt::GRT;
      friend class internal::Unserializer;

      explicit Object(MetaClass *gclass);

      void owned_member_changed(const std::string &name, const grt::ValueRef &ovalue, const grt::ValueRef &nvalue);
      void member_changed(const std::string &name, const grt::ValueRef &ovalue, const grt::ValueRef &nvalue);

      virtual void owned_list_item_added(OwnedList *list, const grt::ValueRef &value);
      virtual void owned_list_item_removed(OwnedList *list, const grt::ValueRef &value);

      virtual void owned_dict_item_set(OwnedDict *dict, const std::string &key);
      virtual void owned_dict_item_removed(OwnedDict *dict, const std::string &key);

      MetaClass *_metaclass;
      std::string _id;
      boost::signals2::signal<void(const std::string &, const grt::ValueRef &)> _changed_signal;
      boost::signals2::signal<void(OwnedList *, bool, const grt::ValueRef &)> _list_changed_signal;
      boost::signals2::signal<void(OwnedDict *, bool, const std::string &)> _dict_changed_signal;

      // ObjectValidFlag _valid_flag;

      mutable short _is_global; // whether object is attached to the global GRT tree

      //    public:
      //      const ObjectValidFlag &weakref_valid_flag() const { return _valid_flag; }
    };

    //----------------------------------------------------------------------------------------------------

    /** Registry for GRT object classes.
     *
     * This object is a singleton used to globally store the list of all
     * GRT object implementing C++ classes available. Each class, such
     * as db_Table, must call the registration function once. Whenever a
     * GRT instance is created, it will look at the list of classes and
     * register them with itself (so that allocation functions, member and
     * method pointers can be properly initialized).
     *
     * @ingroup GRTInternal
     */
    using ClassRegistrationFunction = void (*)();

    struct MYSQLGRT_PUBLIC ClassRegistry {
    private:
      friend class ::grt::GRT;

      std::map<std::string, ClassRegistrationFunction> classes;

      ClassRegistry();

      /** Register all known classes in the given GRT context.
       */
      void register_all();

      /**
       * This one is neede dfor testing purposes only
       */
      void cleanUp();

      static auto get_instance() -> ClassRegistry *;

      auto isEmpty() -> bool;

    public:
      /** Template function to globally register a GRT class.
       */
      template <class C>
      static void register_class() {
        get_instance()->classes[C::static_class_name()] = &C::grt_register;
      }
    };

  }; // namespace internal
}; // namespace grt
