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

#ifdef __GNUC__
#include <cxxabi.h>
#endif
#include <typeinfo>
#include <list>
#include <map>
#include <unordered_map>

#include <vector>
#include <stdexcept>
#include <boost/function.hpp>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include "base/threading.h"
#include <string>
#include <gmodule.h>

#ifndef _NODLL_
#if defined(_MSC_VER)
#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#ifdef MYSQLGRTLIBRARY_EXPORTS
#define MYSQLGRT_PUBLIC __declspec(dllexport)
#else
#define MYSQLGRT_PUBLIC __declspec(dllimport)
#endif

#endif
#endif

#ifndef MYSQLGRT_PUBLIC
#define MYSQLGRT_PUBLIC
#endif

#include <set>

#define GRT_VERSION "4.1.0"

namespace sql {
  class SQLException;
};

namespace grt {

  //  std::map<std::string, base::any> convert(const grt::DictRef dict)
  //  {
  //    std::map<std::string, base::any> result;
  //    for (auto it = dict.begin(); it != dict.end(); ++it)
  //    {
  //      result.insert({it->first, it->second});
  //    }
  //  }

  class MYSQLGRT_PUBLIC os_error : public std::runtime_error {
  public:
    os_error(const std::string &msg) : std::runtime_error(msg) {
    }
    os_error(int err) : std::runtime_error(std::strerror(err)) {
    }
    os_error(const std::string &msg, int err) : std::runtime_error(msg + ": " + std::strerror(err)) {
    }
  };

  class MYSQLGRT_PUBLIC null_value : public std::logic_error {
  public:
    explicit null_value(const std::string &msg) : std::logic_error(msg) {
    }
    explicit null_value() : std::logic_error("Attempt to operate on a NULL GRT value.") {
    }
  };

  class MYSQLGRT_PUBLIC read_only_item : public std::logic_error {
  public:
    explicit read_only_item(const std::string &value) : std::logic_error(value + " is read-only") {
    }
  };

  class MYSQLGRT_PUBLIC bad_item : public std::logic_error {
  public:
    bad_item(size_t index, size_t count) : std::logic_error("Index out of range") {
    }
    bad_item(const std::string &name) : std::logic_error("Invalid item name '" + name + "'") {
    }
  };

  class MYSQLGRT_PUBLIC bad_class : public std::logic_error {
  public:
    bad_class(const std::string &name) : std::logic_error("Invalid class " + name) {
    }
  };

  class MYSQLGRT_PUBLIC grt_runtime_error : public std::runtime_error {
  public:
    std::string detail;
    bool fatal;

    grt_runtime_error(const grt_runtime_error &other)
      : std::runtime_error(other), detail(other.detail), fatal(other.fatal) {
    }
    grt_runtime_error(const std::string &exc, const std::string &adetail, bool afatal = false)
      : std::runtime_error(exc), detail(adetail), fatal(afatal) {
    }

    virtual ~grt_runtime_error() {
    }
  };

  class MYSQLGRT_PUBLIC module_error : public std::runtime_error {
  public:
    std::string inner;
    module_error(const std::string &exc, const std::string &ainner = "") : std::runtime_error(exc), inner(ainner) {
    }
    virtual ~module_error() {
    }
  };

  class MYSQLGRT_PUBLIC user_cancelled : public std::runtime_error {
  public:
    user_cancelled(const std::string &exc) : std::runtime_error(exc) {
    }
  };

  class MYSQLGRT_PUBLIC server_denied : public std::runtime_error {
  public:
    int errNo;

    server_denied(const server_denied &other) : std::runtime_error(other), errNo(other.errNo) {
    }
    server_denied(const std::string &exc, int err) : std::runtime_error(exc), errNo(err) {
    }
    virtual ~server_denied() {
    }
  };

  class MYSQLGRT_PUBLIC db_error : public std::runtime_error {
    int _error;

  public:
    db_error(const sql::SQLException &exc);
    db_error(const std::string &exc, int error) : std::runtime_error(exc), _error(error) {
    }

    auto error() const -> int {
      return _error;
    };
  };

  class MYSQLGRT_PUBLIC db_not_connected : public std::runtime_error {
  public:
    db_not_connected(const std::string &exc) : std::runtime_error(exc) {
    }
  };

  class MYSQLGRT_PUBLIC db_login_error : public std::runtime_error {
  public:
    db_login_error(const std::string &exc) : std::runtime_error(exc) {
    }
  };

  class MYSQLGRT_PUBLIC db_access_denied : public std::runtime_error {
  public:
    db_access_denied(const std::string &exc) : std::runtime_error(exc) {
    }
  };
}; // namespace grt

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

/** @addtogroup GRT
 * @htmlinclude GRT.html
 *
 */

#include "grtpp_value.h"

namespace grt {

  class GRT;

  namespace internal {
    class Serializer;
    class Unserializer;
  }; // namespace internal

  //------------------------------------------------------------------------------------------------

  /** Base GRT value reference class.
   * The Ref classes act as a smart pointer for GRT values. Because GRT
   * values are allocated on the heap and are reference counted, the Ref
   * classes will wrap around these values and perform automatic reference
   * incrementing or decrementing when statically allocated. In most cases
   * you must not directly reference or allocate a GRT value.
   *
   * They also have methods for accessing meta-information about the value
   * like the type and implement some basic operators like assignment, ==,
   * != and <
   *
   * The comparison operators will work on the actual GRT value wrapped,
   * even when you compare through the ValueRef type. In this case, if the
   * types of the compared values do not match they will also compare the
   * type of each value.
   *
   * @ingroup GRT
   */
  class MYSQLGRT_PUBLIC ValueRef {
  public:
    ValueRef() : _value(0) {
    }

    explicit ValueRef(internal::Value *value) : _value(value) {
      if (_value)
        _value->retain();
    }

    ValueRef(const ValueRef &value) : _value(value._value) {
      if (_value)
        _value->retain();
    }

    virtual ~ValueRef() {
      if (_value)
        _value->release();
    }

    inline void clear() {
      if (_value)
        _value->release();
      _value = 0;
    }

    inline auto is_valid() const -> bool {
      return _value != 0;
    }
    inline auto is_same(const ValueRef &value) const -> bool {
      return valueptr() == value.valueptr();
    }

    inline auto type() const -> Type {
      return _value ? _value->get_type() : UnknownType;
    }

    auto operator=(const ValueRef &other) -> ValueRef & {
      swap(other._value);
      return *this;
    }

    // for non-simple types will only check if its the same object
    inline auto operator==(const ValueRef &other) const -> bool {
      if (_value == other._value)
        return true;
      if (!_value || !other._value)
        return false;
      if (type() != other.type())
        return false;

      return _value->equals(other._value);
    }

    inline auto operator!=(const ValueRef &other) const -> bool {
      return !(operator==(other));
    }

    // for non-simple types will check order of the pointer of the object
    // (ie gives an arbitrary order)
    inline auto operator<(const ValueRef &other) const -> bool {
      if (!_value || !other._value)
        return _value < other._value;
      if (type() != other.type())
        return (type() < other.type());

      return _value->less_than(other._value);
    }

    auto debugDescription(const std::string &indentation = "") const -> std::string {
      return _value ? _value->debugDescription(indentation) : "NULL";
    }
    auto toString() const -> std::string {
      return _value ? _value->toString() : "NULL";
    }

    inline auto valueptr() const -> internal::Value * {
      return _value;
    }

    auto refcount() const -> int {
      return _value->refcount();
    }
    void retain() {
      if (_value)
        _value->retain();
    }
    void release() {
      if (_value)
        _value->release();
    }

    void mark_global() const {
      if (_value)
        _value->mark_global();
    }
    void unmark_global() const {
      if (_value)
        _value->unmark_global();
    }

  protected:
    internal::Value *_value;

    void swap(internal::Value *nvalue) {
      if (nvalue != _value) {
        if (_value)
          _value->release();
        _value = nvalue;
        if (_value)
          _value->retain();
      }
    }
  };

  //----------------------------------------------------------------------
  // Object Refs

  template <class C>
  class Ref;

  using ObjectRef = Ref<internal::Object>;

  /** Holds a reference to a GRT object.
   *
   * Use it as Ref<db_Table> or db_TableRef, which is an alias created along
   * auto-generated classes.
   *
   * To allocate a new object from C++ code:
   * @code
   *   db_TableRef table;
   * @endcode
   *
   * To access members and methods:
   * @code
   *   table->member();
   * @endcode
   *
   * Reference counting is performed automatically.
   *
   * @ingroup GRT
   */

  enum CreateMode { Initialized = true };
  template <class Class>
  class Ref : public ValueRef {
  public:
    using RefType = Class;

    Ref() {
    }

    explicit Ref(CreateMode mode) : ValueRef(new Class) {
      content().init();
    }

    Ref(Class *obj) : ValueRef(obj) {
    }

    Ref(const Ref<Class> &ref) : ValueRef(ref) {
#if defined(WB_DEBUG)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2) // this causes errors in mac, with gcc 4.2
      // just to give an error if Class is not an object
      Class::static_class_name();
#endif
#endif
    }

    template <class Subclass>
    Ref(const Ref<Subclass> &ref) {
#ifdef WB_DEBUG
      // Poor mans compile-time type "check". Compiler will throw an error if Subclass is not derived from Class.
      Class *dummy_variable_just_for_type_check WB_UNUSED = static_cast<Subclass *>(ref.valueptr());
#endif
      _value = ref.valueptr();
      retain();
    }

    static inline auto can_wrap(const ValueRef &value) -> bool {
      return (value.type() == ObjectType) && (!value.is_valid() || dynamic_cast<Class *>(value.valueptr()));
    }

    static inline auto cast_from(const ValueRef &ov) -> Ref<Class> {
      if (ov.is_valid()) {
        Class *obj = dynamic_cast<Class *>(ov.valueptr());
        if (!obj) {
          internal::Object *object = dynamic_cast<internal::Object *>(ov.valueptr());
          if (object)
            throw grt::type_error(Class::static_class_name(), object->class_name());
          else
            throw grt::type_error(Class::static_class_name(), ov.type());
        }
        return Ref<Class>(obj);
      }
      return Ref<Class>();
    }

    auto id() const -> const std::string & {
      return content().id();
    }
    auto class_name() const -> const std::string & {
      return content().class_name();
    }
    auto get_metaclass() const -> MetaClass * {
      return content().get_metaclass();
    }

    auto is_instance(MetaClass *mc) const -> bool {
      return content().is_instance(mc);
    }
    auto is_instance(const std::string &klass) const -> bool {
      return content().is_instance(klass);
    }
    template <class C>
    auto is_instance() const -> bool {
      return C::static_class_name().empty() ? true : content().is_instance(C::static_class_name());
    }

    auto operator=(const Ref<Class> &other) -> Ref<Class> & {
      Ref<Class> tmp(other);
#ifdef __linux__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
      swap(tmp._value);
#ifdef __linux__
#pragma GCC diagnostic pop
#endif
      return *this;
    }

    auto operator==(const ValueRef &other) const -> bool {
      return _value == other.valueptr() || (_value && content().equals(other.valueptr()));
    }
    auto operator!=(const ValueRef &other) const -> bool {
      return !(operator==(other));
    }

    auto operator->() const -> Class * {
      return static_cast<Class *>(_value);
    }

    auto get_member(const std::string &m) const -> ValueRef {
      return content().get_member(m);
    }

    void set_member(const std::string &m, const ValueRef &new_value) {
      content().set_member(m, new_value);
    }

    auto get_string_member(const std::string &member) const -> std::string {
      return content().get_string_member(member);
    }
    auto get_double_member(const std::string &member) const -> internal::Double::storage_type {
      return content().get_double_member(member);
    }
    auto get_integer_member(const std::string &member) const -> internal::Integer::storage_type {
      return content().get_integer_member(member);
    }
    auto has_member(const std::string &member) const -> bool {
      return content().has_member(member);
    }

    auto has_method(const std::string &method) const -> bool {
      return content().has_method(method);
    }
    //    Class *operator*() const { return static_cast<Class*>(_value); }

  public:
    auto content() const -> Class & {
      return *static_cast<Class *>(_value);
    }
  };

#if 0
  //----------------------------------------------------------------------
  // Weak Object Refs

  template<class C> class WeakRef;

  typedef WeakRef<internal::Object> WeakObjectRef;

  /** Holds a weak reference to a GRT object.
   *
   * Use it as WeakRef<db_Table>
   *
   * Weak references will not increment the reference count of the referenced object,
   * but will be able to tell whether it has been deleted. Use it to avoid circular
   * loops and other situations where you don't want a real reference to an object.
   *
   * @ingroup GRT
   */
  template<class Class>
  class WeakRef
    {
    public:
      typedef Class RefType;

      /// unitialized weak reference is invalid
      WeakRef() : _content(0), _valid_flag(false) // unassigned means reference is undefined
      {
      }

      // copy constructor
      WeakRef(const WeakRef<Class> &copy)
      : _content(copy._content), _valid_flag(copy._valid_flag)
      {
      }

      // Create a weak-ref from a normal ref
      WeakRef(const Ref<Class> &object_ref)
      : _content(object_ref.is_valid() ? &object_ref.content() : 0),
        _valid_flag(object_ref.is_valid() ? object_ref->weakref_valid_flag() : internal::ObjectValidFlag(false))
      {
      }

      // gets a real usable reference to the object
      Ref<Class> lock() const
      {
        if (!_valid_flag.valid())
          throw std::logic_error("attempt to access invalid weak-reference");
        return Ref<Class>(_content);
      }

      void reset()
      {
        _content= 0; // pointer was nullified, it's a valid reference to null (ie not undefined)
        _valid_flag= internal::ObjectValidFlag(true);
      }

      //! Returns true if the reference is still valid, false if the referenced object is gone
      bool expired() const
      {
        return _valid_flag.valid();
      }

      WeakRef<Class> &operator= (const WeakRef<Class>& other)
      {
        WeakRef<Class> tmp(other);
        swap(tmp);
        return *this;
      }

      WeakRef<Class> &operator= (const Ref<Class>& other)
      {
        WeakRef<Class> tmp(other);
        swap(tmp);
        return *this;
      }

      void swap(Ref<Class> &other)
      {
        std::swap(_content, other._content);
        _valid_flag.swap(other._valid_flag);
      }

      bool operator == (const WeakRef<Class>& other) const
      {
        return lock() == other.lock();
      }

      bool operator != (const WeakRef<Class>& other) const
      {
        return lock() != other.lock();
      }

    private:
      /* Create a weak reference to a raw object pointer... is not safe, so we just disallow it */
      explicit WeakRef(Class *instance)
      : _content(instance),  // if object is null, the reference is valid, just null
      _valid_flag(instance ? instance->weakref_valid_flag() : internal::ObjectValidFlag(true))
      {
      }

    private:
      Class *_content;
      internal::ObjectValidFlag _valid_flag;
    };
#endif // disabled
  //----------------------------------------------------------------------
  // IntegerRef

  using IntegerRef = Ref<internal::Integer>;

  /** Reference object class for integer GRT values (32 or 64bit, depending on compiler architecture).
   *
   * aka IntegerRef
   *
   * To create an integer value:
   * @code
   *   InegerRef(1234);
   * @endcode
   *
   * An implicit constructor for long/int/size_t is available, so you can assign
   * to a IntegerRef as:
   * @code
   *   IntegerRef i= 1234;
   * @endcode
   *
   * @ingroup GRT
   */
  template <>
  class Ref<internal::Integer> : public ValueRef {
  public:
    using RefType = internal::Integer;
    using storage_type = internal::Integer::storage_type;

    static inline auto cast_from(const ValueRef &svalue) -> Ref<internal::Integer> {
      if (svalue.is_valid() && svalue.type() != IntegerType)
        throw type_error(IntegerType, svalue.type());
      return Ref<internal::Integer>(svalue);
    }

    static inline auto extract_from(const ValueRef &svalue) -> storage_type {
      if (!svalue.is_valid() || svalue.type() != IntegerType)
        throw type_error(IntegerType, svalue.type());
      return *static_cast<internal::Integer *>(svalue.valueptr());
    }

    static inline auto can_wrap(const ValueRef &value) -> bool {
      return (value.type() == internal::Integer::static_type());
    }

    Ref() {
    }

    Ref(const Ref &value) : ValueRef(value) {
    }

    explicit Ref(internal::Integer *ptr) : ValueRef(ptr) {
    }

#ifdef DEFINE_INT_FUNCTIONS
    Ref(int value) : ValueRef(internal::Integer::get(value)) {
    }
#endif

#ifndef DEFINE_INT_FUNCTIONS
    Ref(long int value) : ValueRef(internal::Integer::get(value)) {
    }
#endif

#ifdef DEFINE_SSIZE_T_FUNCTIONS
    Ref(ssize_t value) : ValueRef(internal::Integer::get(value)) {
    }
#endif

    Ref(size_t value) : ValueRef(internal::Integer::get(value)) {
    }

    inline operator storage_type() const {
      return *content();
    }
    inline auto operator*() const -> storage_type {
      return *content();
    }

    auto operator=(const Ref<internal::Integer> &other) -> Ref<internal::Integer> & {
      swap(other._value);
      return *this;
    }

    inline auto operator==(const IntegerRef &o) const -> bool {
      return _value == o._value || (_value && o._value && *content() == *o);
    }
#ifdef DEFINE_INT_FUNCTIONS
    inline auto operator==(int v) const -> bool {
      return _value && (*content() == v);
    }
#endif

#ifndef DEFINE_INT_FUNCTIONS
    inline bool operator==(long int v) const {
      return _value && (*content() == v);
    }
#endif

#ifdef DEFINE_SSIZE_T_FUNCTIONS
    inline auto operator==(ssize_t v) const -> bool {
      return _value && (*content() == v);
    }
#endif

    inline auto operator!=(const IntegerRef &o) const -> bool {
      return !(operator==(o));
    }
#ifdef DEFINE_INT_FUNCTIONS
    inline auto operator!=(int v) const -> bool {
      return _value && (*content() != v);
    }
#endif

#ifndef DEFINE_INT_FUNCTIONS
    inline bool operator!=(long int v) const {
      return _value && (*content() != v);
    }
#endif

#ifdef DEFINE_SSIZE_T_FUNCTIONS
    inline auto operator!=(ssize_t v) const -> bool {
      return _value && (*content() != v);
    }
#endif

  protected:
    explicit Ref(const ValueRef &ivalue) {
      if (ivalue.is_valid() && ivalue.type() != internal::Integer::static_type())
        throw type_error(internal::Integer::static_type(), ivalue.type());
      _value = ivalue.valueptr();
      if (_value)
        _value->retain();
    }

    auto content() const -> internal::Integer & {
      return *static_cast<internal::Integer *>(_value);
    }
  };

  //----------------------------------------------------------------------
  // DoubleRef

  using DoubleRef = Ref<internal::Double>;

  /** Reference object class for double GRT values.
   *
   * aka DoubleRef
   *
   * To create a double value:
   * @code
   *   DoubleRef(12.34);
   * @endcode
   *
   * An implicit constructor for long is available, so you can assign
   * to a DoubleRef as:
   * @code
   *   DoubleRef i= 12.34;
   * @endcode
   *
   * @ingroup GRT
   */
  template <>
  class Ref<internal::Double> : public ValueRef {
  public:
    using RefType = internal::Double;
    using storage_type = internal::Double::storage_type;

    static inline auto cast_from(const ValueRef &svalue) -> Ref<internal::Double> {
      if (svalue.is_valid() && svalue.type() != DoubleType)
        throw type_error(DoubleType, svalue.type());
      return Ref<internal::Double>(svalue);
    }

    static inline auto extract_from(const ValueRef &svalue) -> storage_type {
      if (!svalue.is_valid() || svalue.type() != DoubleType)
        throw type_error(DoubleType, svalue.type());
      return *static_cast<internal::Double *>(svalue.valueptr());
    }

    static inline auto can_wrap(const ValueRef &value) -> bool {
      return (value.type() == internal::Double::static_type());
    }

    Ref() {
    }

    Ref(const Ref &value) : ValueRef(value) {
    }

    explicit Ref(internal::Double *ptr) : ValueRef(ptr) {
    }

    Ref(storage_type value) : ValueRef(internal::Double::get(value)) {
    }

    inline operator storage_type() const {
      return *content();
    }
    inline auto operator*() const -> storage_type {
      return *content();
    }

    auto operator=(const Ref<internal::Double> &other) -> Ref<internal::Double> & {
      swap(other._value);
      return *this;
    }

    inline auto operator==(const DoubleRef &o) const -> bool {
      return _value == o._value || (_value && o._value && (*content() == *o));
    }

    inline auto operator==(storage_type v) const -> bool {
      return _value && (*content() == v);
    }

    inline auto operator!=(storage_type v) const -> bool {
      return _value && (*content() != v);
    }

    inline auto operator!=(const DoubleRef &o) const -> bool {
      return !(operator==(o));
    }

  protected:
    explicit Ref(const ValueRef &ivalue) {
      if (ivalue.is_valid() && ivalue.type() != internal::Double::static_type())
        throw type_error(internal::Double::static_type(), ivalue.type());
      _value = ivalue.valueptr();
      if (_value)
        _value->retain();
    }

    auto content() const -> internal::Double & {
      return *static_cast<internal::Double *>(_value);
    }
  };

  //----------------------------------------------------------------------
  // StringRef

  using StringRef = Ref<internal::String>;

  /** Reference object class for string GRT values.
   *
   * aka StringRef
   *
   * To create a string value:
   * @code
   *   StringRef("foo");
   * @endcode
   *
   * An implicit constructor for string is available, so you can assign
   * to a StringRef as:
   * @code
   *   StringRef s= "foo";
   * @endcode
   *
   * @ingroup GRT
   */
  template <>
  class MYSQLGRT_PUBLIC Ref<internal::String> : public ValueRef {
  public:
    using RefType = internal::String;
    using storage_type = internal::String::storage_type;

    static inline auto cast_from(const ValueRef &svalue) -> Ref<internal::String> {
      if (svalue.is_valid() && svalue.type() != StringType)
        throw type_error(StringType, svalue.type());
      return Ref<internal::String>(svalue);
    }

    static inline auto extract_from(const ValueRef &svalue) -> std::string {
      if (!svalue.is_valid() || svalue.type() != StringType)
        throw type_error(StringType, svalue.type());
      return *static_cast<internal::String *>(svalue.valueptr());
    }

    static inline auto can_wrap(const ValueRef &value) -> bool {
      return (value.type() == internal::String::static_type());
    }

    static auto format(const char *format, ...) -> Ref;

    Ref() {
    }

    Ref(const Ref &value) : ValueRef(value) {
    }

    explicit Ref(internal::String *ptr) : ValueRef(ptr) {
    }

    Ref(const std::string &value) : ValueRef(internal::String::get(value)) {
    }

    Ref(const char *value) : ValueRef(internal::String::get(value)) {
    }

    inline operator storage_type() const {
      return *content();
    }
    inline auto operator*() const -> storage_type {
      return *content();
    }

    auto c_str() const -> const char * {
      return content().c_str();
    }
    auto empty() const -> bool {
      return content().empty();
    }

    auto operator=(const Ref<internal::String> &other) -> Ref<internal::String> & {
      swap(other._value);
      return *this;
    }

    inline auto operator==(const StringRef &v) const -> bool {
      return _value == v._value || (_value && v._value && (*content() == *v));
    }

    inline auto operator==(const storage_type &v) const -> bool {
      return _value && (*content() == v);
    }

    inline auto operator==(const char *v) const -> bool {
      return _value && (strcmp(content().c_str(), v) == 0);
    }

    inline auto operator!=(const StringRef &v) const -> bool {
      return !operator==(v);
    }

    inline auto operator!=(const storage_type &v) const -> bool {
      return !operator==(v);
    }

    inline auto operator!=(const char *v) const -> bool {
      return !operator==(v);
    }

  protected:
    explicit Ref(const ValueRef &ivalue) {
      if (ivalue.is_valid() && ivalue.type() != internal::String::static_type())
        throw type_error(internal::String::static_type(), ivalue.type());
      _value = ivalue.valueptr();
      if (_value)
        _value->retain();
    }

    auto content() const -> internal::String & {
      return *static_cast<internal::String *>(_value);
    }
  };

  //----------------------------------------------------------------------
  // Lists

  template <class C>
  struct TypedListConstIterator {
    using iterator_category = std::random_access_iterator_tag;
    using value_type = C;
    using difference_type = int;
    using pointer = C *;
    using reference = C &;
    using IterType = internal::List::raw_const_iterator;

    IterType iter;

    // TypedListConstIterator() : iter(0) {}

    TypedListConstIterator(const TypedListConstIterator &content) : iter(content.iter) {
    }

    TypedListConstIterator(const IterType &content) : iter(content) {
    }

    inline auto operator<(const TypedListConstIterator &o) const -> bool {
      return iter < o.iter;
    }

    inline auto operator*() const -> Ref<C> {
      return Ref<C>((C *)iter->valueptr());
    }

    inline auto operator==(const TypedListConstIterator &o) const -> bool {
      return iter == o.iter;
    }

    inline auto operator!=(const TypedListConstIterator &o) const -> bool {
      return iter != o.iter;
    }

    inline auto operator++() -> TypedListConstIterator & {
      ++iter;
      return *this;
    }

    inline auto operator++(int) -> TypedListConstIterator {
      TypedListConstIterator temp(*this);
      ++iter;
      return temp;
    }

    inline auto operator-(const TypedListConstIterator &other) const -> size_t {
      return iter - other.iter;
    }
  };

  template <class C>
  struct TypedListConstReverseIterator {
    using iterator_category = std::random_access_iterator_tag;
    using value_type = C;
    using difference_type = int;
    using pointer = C *;
    using reference = C &;
    using IterType = internal::List::raw_const_reverse_iterator;

    IterType iter;

    TypedListConstReverseIterator() {
    }

    TypedListConstReverseIterator(const TypedListConstReverseIterator &content) : iter(content.iter) {
    }

    TypedListConstReverseIterator(const IterType &content) : iter(content) {
    }

    inline auto operator<(const TypedListConstReverseIterator &o) const -> bool {
      return iter < o.iter;
    }

    inline auto operator*() const -> Ref<C> {
      return Ref<C>((C *)iter->valueptr());
    }

    inline auto operator==(const TypedListConstReverseIterator &o) const -> bool {
      return iter == o.iter;
    }

    inline auto operator!=(const TypedListConstReverseIterator &o) const -> bool {
      return iter != o.iter;
    }

    inline auto operator++() -> TypedListConstReverseIterator & {
      ++iter;
      return *this;
    }

    inline auto operator++(int) -> TypedListConstReverseIterator {
      TypedListConstReverseIterator temp(*this);
      ++iter;
      return temp;
    }
  };

  /** Base GRT list reference class.
   *
   * @ingroup GRT
   */
  class MYSQLGRT_PUBLIC BaseListRef : public ValueRef {
  public:
    using RefType = internal::List;
    using raw_const_iterator = internal::List::raw_const_iterator;
    using raw_const_reverse_iterator = internal::List::raw_const_reverse_iterator;

    enum { npos = internal::List::npos };

    BaseListRef() {
    }

    BaseListRef(const BaseListRef &list) : ValueRef(list) {
    }

    BaseListRef(internal::List *list) : ValueRef(list) {
    }

    BaseListRef(bool allow_null) : ValueRef(new internal::List(allow_null)) {
    }

    BaseListRef(Type type, const std::string &class_name = "", internal::Object *owner = 0, bool allow_null = true)
      : ValueRef(owner ? new internal::OwnedList(type, class_name, owner, allow_null)
                       : new internal::List(type, class_name, allow_null)) {
    }

    auto operator=(const BaseListRef &other) -> BaseListRef & = default;

    inline auto content_type() const -> Type {
      return content().content_type();
    };
    inline auto content_class_name() const -> std::string {
      return content().content_class_name();
    }

    static auto can_wrap(const ValueRef &value) -> bool {
      return value.type() == ListType;
    }

    static auto cast_from(const ValueRef &value) -> BaseListRef {
      return BaseListRef(value);
    }

    inline void remove(size_t index) {
      content().remove(index);
    }

    inline void remove_all() {
      while (content().count() > 0)
        content().remove(0);
    }

    inline auto count() const -> size_t {
      return is_valid() ? content().count() : 0;
    }

    inline auto operator[](size_t index) const -> const ValueRef & {
      return content().get(index);
    }

    inline auto get(size_t index) const -> const ValueRef & {
      return content().get(index);
    }

    inline auto begin() const -> raw_const_iterator {
      return content().raw_begin();
    }

    inline auto end() const -> raw_const_iterator {
      return content().raw_end();
    }

    template <typename TPred>
    auto foreach (TPred pred) const -> bool {
      for (internal::List::raw_const_iterator end = content().raw_end(), iter = content().raw_begin(); iter != end;
           ++iter) {
        if (!pred(*iter))
          return false;
      }
      return true;
    }

    inline auto get_index(const ValueRef &value) const -> size_t {
      return content().get_index(value);
    }

    inline void reorder(size_t oindex, size_t nindex) {
      content().reorder(oindex, nindex);
    }

    // methods beginning with g perform type checking at runtime
    inline void gset(size_t index, const ValueRef &value) {
      content().set_checked(index, value);
    }

    inline void ginsert(const ValueRef &value, size_t index = npos) {
      content().insert_checked(value, index);
    }

    inline void gremove_value(const ValueRef &value) {
      content().remove(value);
    }

    inline void gset_unchecked(size_t index, const ValueRef &value) {
      content().set_unchecked(index, value);
    }

    inline void ginsert_unchecked(const ValueRef &value, size_t index = npos) {
      content().insert_unchecked(value, index);
    }

  public:
    inline auto content() const -> internal::List & {
      return *static_cast<internal::List *>(_value);
    }

    // For consistency with other Ref<> templates use -> operator as shortcut for content().
    inline auto operator->() const -> internal::List * {
      return static_cast<internal::List *>(_value);
    }

  protected:
    explicit BaseListRef(const ValueRef &lvalue) {
      if (lvalue.is_valid() && lvalue.type() != ListType)
        throw type_error(ListType, lvalue.type());

      _value = lvalue.valueptr();
      if (_value)
        _value->retain();
    }
  };

  //----------------------------------------------------------------------
  // ListRef<Object>

  /** GRT object list reference class.
   *
   * @ingroup GRT
   */
  template <class O>
  class ListRef : public BaseListRef {
  public:
    using const_iterator = TypedListConstIterator<O>;
    using const_reverse_iterator = TypedListConstReverseIterator<O>;
    using value_type = Ref<O>;

    ListRef() {
    }

    ListRef(bool allow_null) : BaseListRef(ObjectType, O::static_class_name(), 0, allow_null) {
    }

    ListRef(internal::Object *owner, bool allow_null = true)
      : BaseListRef(ObjectType, O::static_class_name(), owner, allow_null) {
    }

    ListRef(CreateMode mode, internal::Object *owner = nullptr, bool allow_null = true)
      : BaseListRef(ObjectType, O::static_class_name(), owner, allow_null) {
    }

    template <class Subclass>
    ListRef(const ListRef<Subclass> &other) : BaseListRef(other) {
      Subclass *x = 0;
      O *tmp WB_UNUSED = x; // Hack so that we get a compile error if Subclass is not a subclass of O.
    }

    static auto cast_from(const ValueRef &value) -> ListRef<O> {
      // check if a list
      if (!value.is_valid() || can_wrap(value))
        return ListRef<O>(value);

      TypeSpec expected;
      expected.base.type = ListType;
      expected.content.type = ObjectType;
      expected.content.object_class = O::static_class_name();

      if (value.type() == ListType) {
        TypeSpec actual;
        actual.base.type = ListType;
        actual.content = BaseListRef::cast_from(value)->content_type_spec();
        throw type_error(expected, actual);
      } else
        throw type_error(ListType, value.type());
    }

    static bool can_wrap(const ValueRef &value);

    inline void insert(const Ref<O> &value, size_t index = npos) {
      content().insert_unchecked(value, index);
    }

    inline void remove_value(const Ref<O> &value) {
      content().remove(value);
    }

    // Return const Ref<> so that list[i]= newvalue; won't be attempted (that wouldnt work as expected)
    inline auto operator[](size_t index) const -> const Ref<O> {
      return get(index);
    }

    inline auto get(size_t index) const -> Ref<O> {
      return Ref<O>::cast_from(content().get(index));
    }

    inline void set(size_t index, const Ref<O> &value) {
      content().set_unchecked(index, value);
    }

    inline auto begin() const -> const_iterator {
      return const_iterator(content().raw_begin());
    }

    inline auto end() const -> const_iterator {
      return const_iterator(content().raw_end());
    }

    inline auto rbegin() const -> const_reverse_iterator {
      return const_reverse_iterator(content().raw_rbegin());
    }

    inline auto rend() const -> const_reverse_iterator {
      return const_reverse_iterator(content().raw_rend());
    }

    template <typename TPred>
    auto foreach (TPred pred) const -> bool {
      for (internal::List::raw_const_iterator end = content().raw_end(), iter = content().raw_begin(); iter != end;
           ++iter) {
        Ref<O> tmp((O *)iter->valueptr());
        if (!pred(tmp))
          return false;
      }
      return true;
    }

  protected:
    explicit ListRef(const ValueRef &lvalue) : BaseListRef(lvalue) {
      if (lvalue.is_valid() && content().content_type() != O::static_type())
        throw type_error(O::static_type(), content().content_type(), ListType);
    }
  };

  using ObjectListRef = ListRef<internal::Object>;

  //----------------------------------------------------------------------
  // ListRef<Integer>

  using IntegerListRef = ListRef<internal::Integer>;

  /** GRT integer list reference class.
   *
   * aka IntegerListRef
   *
   * @ingroup GRT
   */
  template <>
  class MYSQLGRT_PUBLIC ListRef<internal::Integer> : public BaseListRef {
  public:
    ListRef() {
    }

    ListRef(bool allow_null) : BaseListRef(IntegerType, "", 0, allow_null) {
    }

    ListRef(internal::Object *owner, bool allow_null = true) : BaseListRef(IntegerType, "", owner, allow_null) {
    }

    ListRef(CreateMode mode, internal::Object *owner = nullptr, bool allow_null = true)
      : BaseListRef(IntegerType, "", owner, allow_null) {
    }

    static inline auto can_wrap(const ValueRef &value) -> bool {
      if (value.type() != ListType)
        return false;
      if (static_cast<internal::List *>(value.valueptr())->content_type() != IntegerType)
        return false;
      return true;
    }

    static auto cast_from(const ValueRef &value) -> ListRef<internal::Integer> {
      return ListRef<internal::Integer>(value);
    }

    inline void insert(const IntegerRef &value, size_t index = npos) {
      content().insert_unchecked(value, index);
    }

    inline auto operator[](size_t index) const -> IntegerRef {
      return get(index);
    }

    inline auto get(size_t index) const -> IntegerRef {
      return IntegerRef::cast_from(content().get(index));
    }

    inline void set(size_t index, const IntegerRef &value) {
      content().set_unchecked(index, value);
    }

    template <typename TPred>
    auto foreach (TPred pred) const -> bool {
      for (internal::List::raw_const_iterator end = content().raw_end(), iter = content().raw_begin(); iter != end;
           ++iter) {
        if (!pred(*(internal::Integer *)iter->valueptr()))
          return false;
      }
      return true;
    }

    inline void remove_value(const IntegerRef &value) {
      content().remove(value);
    }

  protected:
    explicit ListRef(const ValueRef &lvalue) : BaseListRef(lvalue) {
      if (lvalue.is_valid() && content().content_type() != IntegerType)
        throw type_error(IntegerType, content().content_type(), ListType);
    }
  };

  //----------------------------------------------------------------------
  // ListRef<Double>

  using DoubleListRef = ListRef<internal::Double>;

  /** GRT double number list reference class.
   *
   * aka DoubleListRef
   *
   * @ingroup GRT
   */
  template <>
  class MYSQLGRT_PUBLIC ListRef<internal::Double> : public BaseListRef {
  public:
    ListRef() {
    }

    ListRef(bool allow_null) : BaseListRef(DoubleType, "", 0, allow_null) {
    }

    ListRef(internal::Object *owner, bool allow_null = true) : BaseListRef(DoubleType, "", owner, allow_null) {
    }

    ListRef(CreateMode mode, internal::Object *owner = nullptr, bool allow_null = true)
      : BaseListRef(DoubleType, "", owner, allow_null) {
    }

    static inline auto can_wrap(const ValueRef &value) -> bool {
      if (value.type() != ListType)
        return false;
      if (static_cast<internal::List *>(value.valueptr())->content_type() != DoubleType)
        return false;
      return true;
    }

    static auto cast_from(const ValueRef &value) -> ListRef<internal::Double> {
      return ListRef<internal::Double>(value);
    }

    inline void insert(const DoubleRef &value, size_t index = npos) {
      content().insert_unchecked(value, index);
    }

    inline auto operator[](size_t index) const -> DoubleRef {
      return get(index);
    }

    inline auto get(size_t index) const -> DoubleRef {
      return DoubleRef::cast_from(content().get(index));
    }

    inline void set(size_t index, const DoubleRef &value) {
      content().set_unchecked(index, value);
    }

    template <typename TPred>
    auto foreach (TPred pred) const -> bool {
      for (internal::List::raw_const_iterator end = content().raw_end(), iter = content().raw_begin(); iter != end;
           ++iter) {
        if (!pred(*(internal::Double *)iter->valueptr()))
          return false;
      }
      return true;
    }

    inline void remove_value(const DoubleRef &value) {
      content().remove(value);
    }

  protected:
    explicit ListRef(const ValueRef &lvalue) : BaseListRef(lvalue) {
      if (lvalue.is_valid() && content().content_type() != DoubleType)
        throw type_error(DoubleType, content().content_type(), ListType);
    }
  };

  //----------------------------------------------------------------------
  // ListRef<internal::String>

  using StringListRef = ListRef<internal::String>;

  /** GRT string list reference class.
   *
   * aka StringListRef
   *
   * @ingroup GRT
   */
  template <>
  class MYSQLGRT_PUBLIC ListRef<internal::String> : public BaseListRef {
  public:
    using const_iterator = TypedListConstIterator<internal::String>;
    using const_reverse_iterator = TypedListConstReverseIterator<internal::String>;

    ListRef() {
    }

    ListRef(bool allow_null) : BaseListRef(StringType, "", 0, allow_null) {
    }

    ListRef(internal::Object *owner, bool allow_null = true) : BaseListRef(StringType, "", owner, allow_null) {
    }

    ListRef(CreateMode mode, internal::Object *owner = nullptr, bool allow_null = true)
      : BaseListRef(StringType, "", owner, allow_null) {
    }

    static inline auto can_wrap(const ValueRef &value) -> bool {
      if (value.type() != ListType)
        return false;
      if (static_cast<internal::List *>(value.valueptr())->content_type() != StringType)
        return false;
      return true;
    }

    static auto cast_from(const ValueRef &value) -> ListRef<internal::String> {
      return ListRef<internal::String>(value);
    }

    inline void insert(const StringRef &value, size_t index = npos) {
      content().insert_unchecked(value, index);
    }

    inline auto operator[](size_t index) const -> StringRef {
      return get(index);
    }

    inline auto get(size_t index) const -> StringRef {
      return StringRef::cast_from(content().get(index));
    }

    inline void set(size_t index, const StringRef &value) {
      content().set_unchecked(index, value);
    }

    inline auto begin() const -> const_iterator {
      return const_iterator(content().raw_begin());
    }

    inline auto end() const -> const_iterator {
      return const_iterator(content().raw_end());
    }

    inline auto rbegin() const -> const_reverse_iterator {
      return const_reverse_iterator(content().raw_rbegin());
    }

    inline auto rend() const -> const_reverse_iterator {
      return const_reverse_iterator(content().raw_rend());
    }

    template <typename TPred>
    auto foreach (TPred pred) const -> bool {
      for (internal::List::raw_const_iterator end = content().raw_end(), iter = content().raw_begin(); iter != end;
           ++iter) {
        if (!pred(*(internal::String *)iter->valueptr()))
          return false;
      }
      return true;
    }

    inline void remove_value(const StringRef &value) {
      content().remove(value);
    }

    inline auto get_index(const std::string &str) -> size_t {
      return BaseListRef::get_index(StringRef(str));
    }

  protected:
    explicit ListRef(const ValueRef &lvalue) : BaseListRef(lvalue) {
      if (lvalue.is_valid() && content().content_type() != StringType)
        throw type_error(StringType, content().content_type(), ListType);
    }
  };

  //----------------------------------------------------------------------
  // DictRef.

  /** GRT dictionary reference class.
   *
   * @ingroup GRT
   */
  class MYSQLGRT_PUBLIC DictRef : public ValueRef {
  public:
    using RefType = internal::Dict;
    using const_iterator = internal::Dict::const_iterator;

    DictRef() {
    }

    DictRef(bool allow_null) : ValueRef(new internal::Dict(allow_null)) {
    }

    DictRef(Type type, const std::string &cclass = "", bool allow_null = true)
      : ValueRef(new internal::Dict(type, cclass, allow_null)) {
    }

    DictRef(internal::Dict *dict) : ValueRef(dict) {
    }

    DictRef(internal::Object *owner, bool allow_null = true)
      : ValueRef(new internal::OwnedDict(AnyType, "", owner, allow_null)) {
    }

    DictRef(Type type, const std::string &cclass, internal::Object *owner, bool allow_null = true)
      : ValueRef(new internal::OwnedDict(type, cclass, owner, allow_null)) {
    }

    DictRef(const DictRef &d) = default;

    static auto cast_from(const ValueRef &ivalue) -> DictRef {
      if (ivalue.is_valid() && ivalue.type() != DictType)
        throw type_error(DictType, ivalue.type());
      return DictRef(ivalue);
    }

    static auto can_wrap(const ValueRef &ivalue) -> bool {
      return ivalue.type() == DictType;
    }

    inline auto content_type() const -> Type {
      return content().content_type();
    };
    inline auto content_class_name() const -> std::string {
      return content().content_class_name();
    }

    auto begin() const -> const_iterator {
      return content().begin();
    }
    auto end() const -> const_iterator {
      return content().end();
    }

    template <typename TPred>
    auto foreach (TPred pred) const -> bool {
      for (const_iterator end = content().end(), iter = content().begin(); iter != end; ++iter) {
        if (!pred(iter->first, iter->second))
          return false;
      }
      return true;
    }

    inline auto has_key(const std::string &k) const -> bool {
      return content().has_key(k);
    }

    inline auto operator[](const std::string &k) const -> ValueRef {
      return get(k);
    }

    inline auto operator=(const DictRef &o) -> DictRef & {
      DictRef tmp(o);
      swap(o.valueptr());
      return *this;
    }

    /*
        inline Reference operator[](const std::string &k)
        {
          if (!has_key(k))
            throw bad_item(k);
          return Reference(this, k);
        }
      */
    inline auto count() const -> size_t {
      return content().count();
    }

    inline auto keys() const -> std::vector<std::string> {
      return content().keys();
    }

    inline void remove(const std::string &k) {
      content().remove(k);
    }

    void reset_entries() {
      content().reset_entries();
    }

    inline auto get(const std::string &k) const -> ValueRef {
      return ValueRef(content().get(k));
    }

    inline auto get(const std::string &k, const ValueRef &defvalue) const -> ValueRef {
      // No need to check here if the key exists.
      // If it does not then an invalid ValueRef will be returned.
      ValueRef tmp = content().get(k);
      if (!tmp.is_valid())
        return defvalue;
      return tmp;
    }

    auto get_string(const std::string &k, const std::string &defvalue = "") const -> std::string {
      ValueRef value = get(k);
      if (value.is_valid())
        return StringRef::extract_from(value);
      return defvalue;
    }

    auto get_int(const std::string &k, internal::Integer::storage_type defvalue = 0) const
      -> internal::Integer::storage_type {
      ValueRef value = get(k);
      if (value.is_valid())
        return IntegerRef::extract_from(value);
      return defvalue;
    }

    auto get_double(const std::string &k, internal::Double::storage_type defvalue = 0.0) const
      -> internal::Double::storage_type {
      ValueRef value = get(k);
      if (value.is_valid())
        return DoubleRef::extract_from(value);
      return defvalue;
    }

    inline void set(const std::string &k, const ValueRef &value) {
      content().set(k, value);
    }

    inline void gset(const std::string &k, const std::string &value) {
      content().set(k, StringRef(value));
    }

    inline void gset(const std::string &k, long value) {
      content().set(k, IntegerRef(value));
    }

    inline void gset(const std::string &k, int value) {
      content().set(k, IntegerRef(value));
    }

    inline void gset(const std::string &k, internal::Double::storage_type value) {
      content().set(k, DoubleRef(value));
    }

    inline auto content() const -> internal::Dict & {
      return *static_cast<internal::Dict *>(_value);
    }

  protected:
    explicit DictRef(const ValueRef &dvalue) : ValueRef(dvalue) {
      if (dvalue.is_valid() && dvalue.type() != DictType)
        throw type_error(DictType, dvalue.type());
    }
  };

  //--------------------------------------------------------------------------------------------------

  using DictListRef = ListRef<internal::Dict>;

  /** GRT Dict list reference class.
   *
   * aka DictListRef
   *
   * @ingroup GRT
   */
  template <>
  class MYSQLGRT_PUBLIC ListRef<internal::Dict> : public BaseListRef {
  public:
    ListRef() {
    }

    ListRef(bool allow_null) : BaseListRef(DictType, "", 0, allow_null) {
    }

    ListRef(internal::Object *owner, bool allow_null = true) : BaseListRef(DictType, "", owner, allow_null) {
    }

    ListRef(CreateMode mode, internal::Object *owner = nullptr, bool allow_null = true)
      : BaseListRef(DictType, "", owner, allow_null) {
    }

    static inline auto can_wrap(const ValueRef &value) -> bool {
      if (value.type() != ListType)
        return false;
      if (static_cast<internal::List *>(value.valueptr())->content_type() != DictType)
        return false;
      return true;
    }

    static auto cast_from(const ValueRef &value) -> ListRef<internal::Dict> {
      return ListRef<internal::Dict>(value);
    }

    inline void insert(const DictRef &value, size_t index = npos) {
      content().insert_unchecked(value, index);
    }

    inline auto operator[](size_t index) const -> DictRef {
      return get(index);
    }

    inline auto get(size_t index) const -> DictRef {
      return DictRef::cast_from(content().get(index));
    }

    inline void set(size_t index, const DictRef &value) {
      content().set_unchecked(index, value);
    }

    template <typename TPred>
    auto foreach (TPred pred) const -> bool {
      for (internal::List::raw_const_iterator end = content().raw_end(), iter = content().raw_begin(); iter != end;
           ++iter) {
        if (!pred(*(internal::Dict *)iter->valueptr()))
          return false;
      }
      return true;
    }

    inline void remove_value(const DictRef &value) {
      content().remove(value);
    }

  protected:
    explicit ListRef(const ValueRef &lvalue) : BaseListRef(lvalue) {
      if (lvalue.is_valid() && content().content_type() != DictType)
        throw type_error(DictType, content().content_type(), ListType);
    }
  };

  //--------------------------------------------------------------------------------------------------

  struct MYSQLGRT_PUBLIC ArgSpec {
    std::string name;
    std::string doc;
    TypeSpec type;
  };
  using ArgSpecList = std::vector<ArgSpec>;

  class PropertyBase {
  public:
    virtual ~PropertyBase() {};

    virtual auto has_setter() const -> bool = 0;

    virtual void set(internal::Object *obj, const grt::ValueRef &value) = 0;
    virtual auto get(const internal::Object *obj) const -> grt::ValueRef = 0;
  };

  /** Describes a GRT object member variable.
   * Contains information about a member variable for a GRT metaclass.
   * The meta-information in this is read from the struct*.xml files but
   * the property pointer is set at runtime when each class registers itself
   * with the GRT.
   */
  struct MYSQLGRT_PUBLIC ClassMember {
    std::string name;
    TypeSpec type;
    std::string default_value;
    bool read_only;            //!< member not directly settable (setter still needed for unserializing)
    bool delegate_get;         //!< getter is user implemented
    bool delegate_set;         //!< setter is user implemented
    bool private_;             //!< member has no getter/setter and is not stored
    bool calculated;           //!< whether a instance var should be created for this value
    bool owned_object;         //!< ref object is owned by this one (owned)
    bool overrides;            //!< member overrides another one
    bool null_content_allowed; //!< whether inserting NULL values to a list or dict is allowed (allow-null)

    //! set by class when registering
    PropertyBase *property;
  };

  /** Describes a GRT object method.
   * Contains information about a method for a GRT metaclass.
   * The meta-information in this is read from the struct*.xml files but
   * the method pointer is set at runtime when each class registers itself
   * with the GRT.
   */
  struct MYSQLGRT_PUBLIC ClassMethod {
    using Function = ValueRef (*)(internal::Object *, const BaseListRef &);

    std::string name;
    // for stuff to be re-routed to methods
    std::string module_name;
    std::string module_function;

    TypeSpec ret_type;
    ArgSpecList arg_types;

    bool constructor;
    bool abstract;

    Function function;
  };

  class Validator {
  public:
    using MessageSlot = std::function<void(const ObjectRef &, const std::string &, const int)>;

    using Tag = std::string;

    virtual ~Validator() {};
    virtual auto validate(const Tag &what, const ObjectRef &obj) -> int = 0;
  };

  /** GRT Class descriptor, formerly known as "struct".
   * Contains information about member variables and methods of a GRT class/object.
   * The class descriptions are loaded from XML files, which contain
   * the list of members and methods with information about types and
   * other metadata. This information is used for generating C++ classes
   * that actually implement these classes and also for exposing the
   * interface to these classes to the scripting interface (such as Python).
   *
   * @ingroup GRT
   */
  class MYSQLGRT_PUBLIC MetaClass //
  {
  public:
    template <class C, class T>
    class Property : public PropertyBase {
    protected:
      void (C::*setter)(const T &value);
      T (C::*getter)() const;

    public:
      Property(T (C::*agetter)() const, void (C::*asetter)(const T &value) = 0) : setter(asetter), getter(agetter) {
      }

      virtual ~Property() {
      }

      virtual auto has_setter() const -> bool {
        return setter != 0;
      }

      virtual void set(internal::Object *obj, const grt::ValueRef &value) {
        (((C *)obj)->*setter)(T::cast_from(value));
      }

      virtual auto get(const internal::Object *obj) const -> grt::ValueRef {
        return (((C *)obj)->*getter)();
      }
    };

    using Allocator = ObjectRef (*)();

    using Member = ClassMember;
    using Method = ClassMethod;

    enum SignalArgType { BoolSArg, IntSArg, DoubleSArg, StringSArg, ObjectSArg };

    struct SignalArg {
      std::string name;
      SignalArgType type;
      std::string object_class;
    };

    /** Describes a signal from a GRT object.
     *
     */
    struct MYSQLGRT_PUBLIC Signal {
      std::string name;

      std::vector<SignalArg> arg_types;
    };

    using MemberList = std::map<std::string, Member>;
    using MethodList = std::map<std::string, Method>;
    using SignalList = std::list<Signal>;
    using ValidatorList = std::vector<Validator *>;

  public:
    auto name() const -> const std::string & {
      return _name;
    }
    auto parent() const -> MetaClass * {
      return _parent;
    }

    auto crc32() const -> unsigned int {
      return _crc32;
    }

    /** Calls slot iterating through all members of the metaclass.
     *
     * The slot will be called for each member of the metaclass,
     * including inherited ones. If a member is overridden,
     * it will be called only once, for the "topmost" one.
     * The slot must return true as long as the iteration is
     * to be continued. Returning false will stop it.
     *
     * @return true if iteration was completed and false if it
     * was cancelled by the slot before completion.
     */
    template <typename TPred>
    auto foreach_member(TPred pred) -> bool {
      // set of already seen members (only overridden ones)
      std::set<std::string> seen;
      MetaClass *mc = this;

      do {
        for (MemberList::const_iterator mem = mc->_members.begin(); mem != mc->_members.end(); ++mem) {
          if (seen.find(mem->first) != seen.end())
            continue;
          seen.insert(mem->first);

          if (!pred(&mem->second))
            return false;
        }
        mc = mc->_parent;
      } while (mc != 0);

      return true;
    }

    /** Calls slot iterating through all methods of the metaclass.
     *
     * The slot will be called for each method of the metaclass,
     * including inherited ones. If a method is overridden,
     * it will be called only once, for the "topmost" one.
     * The slot must return true as long as the iteration is
     * to be continued. Returning false will stop it.
     *
     * @return true if iteration was completed and false if it
     * was cancelled by the slot before completion.
     */

    template <typename TPred>
    auto foreach_method(TPred pred) -> bool {
      // set of already seen methods (only overridden ones)
      std::set<std::string> seen;
      MetaClass *mc = this;

      do {
        for (MethodList::const_iterator mem = mc->_methods.begin(); mem != mc->_methods.end(); ++mem) {
          if (seen.find(mem->first) != seen.end())
            continue;
          seen.insert(mem->first);

          if (!pred(&mem->second))
            return false;
        }
        mc = mc->_parent;
      } while (mc != 0);

      return true;
    }

    /** Calls slot iterating through all signals of the metaclass.
     *
     * The slot will be called for each signal of the metaclass,
     * including inherited ones.
     * The slot must return true as long as the iteration is
     * to be continued. Returning false will stop it.
     *
     * @return true if iteration was completed and false if it
     * was cancelled by the slot before completion.
     */
    template <typename TPred>
    auto foreach_signal(TPred pred) -> bool {
      // set of already seen methods (only overridden ones)
      std::set<std::string> seen;
      MetaClass *mc = this;

      do {
        for (SignalList::const_iterator mem = mc->_signals.begin(); mem != mc->_signals.end(); ++mem) {
          if (seen.find(mem->name) != seen.end())
            continue;
          seen.insert(mem->name);

          if (!pred(&*mem))
            return false;
        }
        mc = mc->_parent;
      } while (mc != 0);

      return true;
    }

    /** Applies all validators to obj and tag
     *
     * @return true upon success and false if any validator has failed
     */
    auto foreach_validator(const ObjectRef &obj, const Validator::Tag &tag) -> bool;

    inline auto get_members_partial() -> const MemberList & {
      return _members;
    }
    inline auto get_methods_partial() -> const MethodList & {
      return _methods;
    }
    inline auto get_signals_partial() -> const SignalList & {
      return _signals;
    }

    auto is_a(const std::string &name) const -> bool;
    auto is_a(MetaClass *struc) const -> bool;

    auto has_member(const std::string &member) const -> bool;
    auto has_method(const std::string &method) const -> bool;

    auto get_member_info(const std::string &member) const -> const Member *;
    auto get_method_info(const std::string &method) const -> const Method *;

    auto get_member_type(const std::string &member) const -> TypeSpec;

    auto get_attribute(const std::string &attr, bool search_parents = true) -> std::string;
    auto get_member_attribute(const std::string &member, const std::string &attr, bool search_parents = true)
      -> std::string;

    auto is_abstract() const -> bool;

    void set_member_value(internal::Object *object, const std::string &name, const ValueRef &value);
    auto get_member_value(const internal::Object *object, const std::string &name) -> ValueRef;
    auto get_member_value(const internal::Object *object, const Member *member) -> ValueRef;

    auto call_method(internal::Object *object, const std::string &name, const BaseListRef &args) -> ValueRef;
    auto call_method(internal::Object *object, const Method *method, const BaseListRef &args) -> ValueRef;

    auto allocate() -> ObjectRef;

  public:
    // for use by GRT only
    ~MetaClass();

    static auto create_base_class() -> MetaClass *;
    static auto from_xml(const std::string &source, xmlNodePtr node) -> MetaClass *;
    auto placeholder() const -> bool {
      return _placeholder;
    }
    auto validate() -> bool;
    auto is_bound() const -> bool;
    auto source() -> std::string {
      return _source;
    }

    auto force_impl() const -> bool {
      return _force_impl;
    }
    auto watch_lists() const -> bool {
      return _watch_lists;
    }
    auto watch_dicts() const -> bool {
      return _watch_dicts;
    }
    auto impl_data() const -> bool {
      return _impl_data;
    }

    void set_member_internal(internal::Object *object, const std::string &name, const ValueRef &value, bool force);

  public: // for use by Objects during registration
    void bind_allocator(Allocator alloc);
    void bind_member(const std::string &name, PropertyBase *prop);

    void bind_method(const std::string &name, Method::Function method);
    void add_validator(Validator *v);
    void remove_validator(Validator *v);

  protected:
    friend class Serializer;
    friend class Unserializer;

    MetaClass();
    void load_xml(xmlNodePtr node);
    void load_attribute_list(xmlNodePtr node, const std::string &member = "");

    std::string _name;
    MetaClass *_parent;

    std::string _source;

    Allocator _alloc;

    std::unordered_map<std::string, std::string> _attributes;
    //    std::map<std::string,std::string> _attributes;
    MemberList _members;
    MethodList _methods;
    SignalList _signals;
    ValidatorList _validators;

    unsigned int _crc32;

    bool _bound;
    bool _placeholder;

    bool _watch_lists; //< adds the virtual method that's called when owned lists are changed (watch-lists)
    bool _watch_dicts; //< adds the virtual method that's called when owned dicts are changed (watch-dicts)
    bool _force_impl;
    bool _impl_data; //< needs extra data for the object
  };

  //------------------------------------------------------------------------------------------------
  //------------------------------------------------------------------------------------------------

  class Module;
  class Interface;

  /** Module loader base class.
   *
   * Supports loading and execution of modules composed of functions written in
   * a language. @a CPPModuleLoader is a special module loader to handle modules
   * written in C++.
   *
   * @ingroup GRT
   */
  class MYSQLGRT_PUBLIC ModuleLoader {
  public:
    ModuleLoader() {
    }
    virtual ~ModuleLoader() {
    }

    virtual auto get_loader_name() -> std::string = 0;

    virtual auto init_module(const std::string &path) -> Module * = 0;

    virtual void refresh() = 0;

    virtual auto load_library(const std::string &path) -> bool = 0;
    virtual auto run_script_file(const std::string &path) -> bool = 0;
    virtual auto run_script(const std::string &script) -> bool = 0;
    virtual auto check_file_extension(const std::string &path) -> bool = 0;
  };

  /** A GRT module class.
   *
   * This class may be subclassed to create C++ modules.
   * A C++ module may be called directly through its @a Module
   * object or indirectly, through a @a ModuleWrapper
   *
   * @ingroup GRT
   */
  class MYSQLGRT_PUBLIC Module //
  {
  public:
    struct Function {
      std::string name;
      std::string description;
      TypeSpec ret_type;
      ArgSpecList arg_types;

      std::function<ValueRef(const grt::BaseListRef &)> call;
    };

    auto has_function(const std::string &name) const -> bool;
    virtual auto call_function(const std::string &name, const grt::BaseListRef &args) -> ValueRef;

    Module(ModuleLoader *loader);
    virtual ~Module() {
    }

    virtual void closeModule() noexcept {
    }

    virtual auto getModule() const -> GModule * {
      return nullptr;
    };

    auto name() const -> std::string {
      return _name;
    }
    auto version() const -> std::string {
      return _meta_version;
    }
    auto author() const -> std::string {
      return _meta_author;
    }
    auto description() const -> std::string {
      return _meta_description;
    }
    auto extends() const -> std::string {
      return _extends;
    }
    auto path() const -> std::string {
      return _path;
    }
    auto bundle_path() const -> std::string;
    auto default_icon_path() const -> std::string;

    auto is_bundle() const -> bool {
      return _is_bundle;
    }

    auto get_functions() const -> const std::vector<Function> & {
      return _functions;
    }

    auto get_function(const std::string &name) const -> const Function *;

    using Interfaces = std::vector<std::string>;

    //! Returns list of the interfaces which Module implements
    auto get_interfaces() const -> const Interfaces & {
      return _interfaces;
    }

    auto get_loader() const -> ModuleLoader * {
      return _loader;
    }

    void validate() const;

    void set_global_data(const std::string &key, const std::string &value);
    void set_global_data(const std::string &key, int value);
    auto global_int_data(const std::string &key, int default_value = 0) -> int;
    auto global_string_data(const std::string &key, const std::string &default_value = "") -> std::string;

    void set_document_data(const std::string &key, const std::string &value);
    void set_document_data(const std::string &key, int value);
    auto document_int_data(const std::string &key, int default_value = 0) -> int;
    auto document_string_data(const std::string &key, const std::string &default_value = "") -> std::string;

  protected:
    /** Parse a String defined module function specification.
     *
     * Function parameter list syntax:
     * @code
     * param_list::= param[, param_list]
     * param::= type [ label]
     * label::= a name for the parameter
     * type::= i | r | s | l[<content_spec>] | d[<content_spec>] | o[@struct]
     * content_spec::= i | r | s | o@struct
     * struct::= name of a valid struct
     * @endcode
     *
     * Ex.:
     *  doSomething:s:i count,l<i> poslist,o<db.mysql.Table> table,d args
     */
    virtual auto add_parse_function_spec(const std::string &spec,
                                         const std::function<ValueRef(BaseListRef, Module *, Module::Function)> &caller)
      -> bool;

    void add_function(const Function &func);

    std::string _name;
    std::string _path;

    std::string _meta_version;
    std::string _meta_author;
    std::string _meta_description;

    std::vector<Function> _functions;

    std::string _extends;
    // Module *_extends;
    Interfaces _interfaces;

    bool _is_bundle;

    ModuleLoader *_loader;
  };

  /** Base class for module wrapper classes.
   *
   * This class is inherited by classes automatically generated by the genwrap tool.
   * These wrapper classes expose GRT modules written in any language as a C++ object.
   *
   * @ingroup GRT
   */
  class MYSQLGRT_PUBLIC ModuleWrapper {
  public:
    ModuleWrapper(Module *module) : _module(module) {
    }
    virtual ~ModuleWrapper() {
    }

    auto get_module() const -> Module * {
      return _module;
    }

  protected:
    Module *_module;
  };

  //------------------------------------------------------------------------------------------------
  //------------------------------------------------------------------------------------------------

  enum MessageType {
    ErrorMsg,
    WarningMsg,
    InfoMsg,
    OutputMsg,
    VerboseMsg,
    ProgressMsg = 10,

    ControlMsg = 1000,
    NoErrorMsg = 0x1000 //!< NoErrorMsg is used for live validation. ValidationManager uses this message type to
                        //!< inform listeners that certain UI (list of errors, etc..) should be cleared
  };

  struct MYSQLGRT_PUBLIC Message {
    MessageType type;
    time_t timestamp;
    std::string text;
    std::string detail;
    float progress;

    auto format(bool withtype = false) const -> std::string;
  };

  using MessageSlot = std::function<bool(const Message &, void *)>;
  using StatusQuerySlot = std::function<bool()>;

  struct SlotHolder {
    MessageSlot slot;
    SlotHolder(MessageSlot s) : slot(s) {};
  };

  //-----------------------------------------------------------------------------------------------

  // gcc and msc produce different output for typeid(arg).name()
  // this function is a platform independent wrapper for typeid(arg).name()
  inline auto get_full_type_name(const std::type_info &ti) -> std::string {
#ifdef __GNUC__
    int s;
    char *tmp = __cxxabiv1::__cxa_demangle(ti.name(), NULL, NULL, &s);
    std::string ret(tmp);
    free(tmp);
    return ret;
#elif _MSC_VER
    const char *name = ti.name();
    if (strncmp("class ", name, sizeof("class ") - 1) == 0)
      return std::string(name + sizeof("class ") - 1);
    else if (strncmp("struct ", name, sizeof("struct ") - 1) == 0)
      return std::string(name + sizeof("struct ") - 1);
    else
      return std::string(name);
#else
#error Unknown compiler
#endif
  }

  inline auto get_type_name(const std::type_info &ti) -> std::string {
    std::string name = get_full_type_name(ti);

    // strip namespace::
    std::string::size_type p = name.rfind(':');
    if (p != std::string::npos)
      return name.substr(p + 1);

    return name;
  }

  //------------------------------------------------------------------------------------------------
  //------------------------------------------------------------------------------------------------

  class UndoManager;
  class Shell;
  class ModuleWrapper;
  class CPPModuleLoader;
  class Interface;

  class UndoGroup;

  /** The main GRT context object.
   *
   * @ingroup GRT
   */
  class MYSQLGRT_PUBLIC GRT //
  {
  public:
    static auto get() -> std::shared_ptr<GRT>;
    ~GRT();

    /**
     *
     * @param flag
     *
     * @return
     */
    void set_verbose(bool flag);
    auto verbose() const -> bool {
      return _verbose;
    }

    // Set to true when we are running unit tests.
    void setTesting(bool flag) {
      _testing = flag;
    };
    auto testing() -> bool {
      return _testing;
    };

    // metaclasss

    /**
     * Indicate whenever metaclasses should be registered.
     */
    auto metaclassesNeedRegister() -> bool;

    /** Load metaclasses defined in a XML file.
     *
     * @ref end_loading_metaclasses() must be called once no more metaclasses
     * will be loaded.
     *
     * @param requires list of other XML files required by the loaded one
     */
    void load_metaclasses(const std::string &file, std::list<std::string> *requiresList = 0);

    /**
     * This one should not be used during normal studio run,
     * it's mean to be used only in the test.
     * Causes all metaclasses to be unloaded,
     * and grt to be recreated.
     */
    void reinitialiseForTests();

    /** Scans a directory for metaclass definition files and load them.
     * Looks in the directory for files with the structs*.xml pattern and loads metaclasses
     * defined in them. The list of other struct files pre-required is returned in the optional
     * @a requires parameter.
     *
     * @ref end_loading_metaclasses() must be called once no more metaclasses
     * will be loaded.
     *
     * @param dir directory to look for struct files
     * @param requires optional pointer to a multimap where required XML files for each loaded
     * files is stored
     */
    auto scan_metaclasses_in(const std::string &dir, std::multimap<std::string, std::string> *requiresMap = 0) -> int;
    /** End loading of metaclass definition files.
     * Finishes up loading of metaclass definition XMLs files. This will
     * check that all metaclasses referred by something were loaded. It will
     * also perform initialization of all known implementatin classes by
     * binding properties and method pointers.
     */
    void end_loading_metaclasses(bool check_class_binding = true);

    auto get_metaclasses() const -> const std::list<MetaClass *> & {
      return _metaclasses_list;
    }

    auto get_metaclass(const std::string &name) const -> MetaClass *;

    template <class C>
    auto create_object(const std::string &class_name) const -> Ref<C> {
      MetaClass *mc = get_metaclass(class_name);
      if (!mc)
        throw bad_class(class_name);
      return Ref<C>::cast_from(mc->allocate());
    }

    // serialization
    void serialize(const ValueRef &value, const std::string &path, const std::string &doctype = "",
                   const std::string &version = "", bool list_objects_as_links = false);
    auto unserialize(const std::string &path, std::shared_ptr<grt::internal::Unserializer> unserializer =
                                                std::shared_ptr<grt::internal::Unserializer>()) -> ValueRef;
    auto unserialize(const std::string &path, std::string &doctype_ret, std::string &version_ret) -> ValueRef;
    auto get_unserializer() -> std::shared_ptr<grt::internal::Unserializer>;

    auto load_xml(const std::string &path) -> xmlDocPtr;
    void get_xml_metainfo(xmlDocPtr doc, std::string &doctype_ret, std::string &version_ret);
    auto unserialize_xml(xmlDocPtr doc, const std::string &source_path) -> ValueRef;

    auto serialize_xml_data(const ValueRef &value, const std::string &doctype = "", const std::string &version = "",
                            bool list_objects_as_links = false) -> std::string;
    auto unserialize_xml_data(const std::string &data) -> ValueRef;

    // globals

    inline auto root() const -> ValueRef {
      return _root;
    }
    void set_root(const ValueRef &root);

    auto get(const std::string &path) const -> ValueRef;
    void set(const std::string &path, const ValueRef &value);

    auto find_object_by_id(const std::string &id, const std::string &subpath) -> ObjectRef;

    // modules

    // path in globals tree for modules to store options
    void set_global_module_data_path(const std::string &path) {
      _global_module_options_path = path;
    }
    auto global_module_data_path() -> std::string {
      return _global_module_options_path;
    }

    void set_document_module_data_path(const std::string &path) {
      _document_module_options_path = path;
    }
    auto document_module_data_path() -> std::string {
      return _document_module_options_path;
    }

    void add_module_loader(ModuleLoader *loader);
    auto load_module(const std::string &path, const std::string &basePath, bool refresh) -> bool;
    void end_loading_modules();

    auto get_module_loader(const std::string &name) -> ModuleLoader *;
    auto get_module_loader_for_file(const std::string &path) -> ModuleLoader *;
    void refresh_loaders();

    void register_new_module(Module *module);
    void refresh_module(Module *module);
    void unregister_module(Module *module);

    void register_new_interface(Interface *iface);
    auto get_interfaces() const -> const std::map<std::string, Interface *> & {
      return _interfaces;
    }
    auto get_interface(const std::string &name) -> const Interface *;

    auto scan_modules_in(const std::string &path, const std::string &basePath, const std::list<std::string> &exts,
                         bool reload) -> int;

    auto get_modules() const -> const std::vector<Module *> & {
      return _modules;
    }

    auto call_module_function(const std::string &module, const std::string &function, const grt::BaseListRef &args)
      -> grt::ValueRef;

    auto get_module(const std::string &name) -> Module *;

    // create an instance of the given native module and registers it with the GRT.
    // this should not be used for accessing modules, use the
    // wrapper class for the module you want, instead (with get_module())
    template <class ModuleImplClass>
    auto get_native_module() -> ModuleImplClass * {
      std::string mname = get_type_name(typeid(ModuleImplClass));
      Module *module;

      if (mname.size() > 4 && mname.substr(mname.size() - 4) == "Impl")
        mname = mname.substr(0, mname.size() - 4);

      ModuleImplClass *instance;

      module = get_module(mname);
      if (!module) {
        instance = new ModuleImplClass((CPPModuleLoader *)get_module_loader("cpp"));
        instance->init_module();
        register_new_module(instance);
      } else {
        instance = dynamic_cast<ModuleImplClass *>(module);
        if (!instance)
          return 0;
      }

      return instance;
    }

    // locate an instance of a module. suitable for direct access to modules
    template <class ModuleImplClass>
    auto find_native_module(const char *name) -> ModuleImplClass * {
      Module *module = get_module(name);

      return static_cast<ModuleImplClass *>(module);
    }

    auto find_modules_matching(const std::string &interface_name, const std::string &name_pattern)
      -> std::vector<Module *>;

    template <class InterfaceWrapperClass>
    auto get_implementing_modules() -> std::vector<InterfaceWrapperClass *> {
      std::vector<Module *> modules;
      std::vector<InterfaceWrapperClass *> mlist;

      modules = find_modules_matching(InterfaceWrapperClass::static_get_name(), "");

      for (std::vector<Module *>::const_iterator i = modules.begin(); i != modules.end(); ++i) {
        mlist.push_back(get_module_wrapper<InterfaceWrapperClass>(*i));
      }

      return mlist;
    }

    // create an instance of a wrapper for the given module
    template <class ModuleWrapperClass>
    auto get_module_wrapper(Module *amodule) -> ModuleWrapperClass * {
      ModuleWrapper *bmodule =
        _cached_module_wrapper[std::string(ModuleWrapperClass::static_get_name()).append("/").append(amodule->name())];
      ModuleWrapperClass *wrapper = dynamic_cast<ModuleWrapperClass *>(bmodule);

      if (!wrapper) {
        wrapper = new ModuleWrapperClass(amodule);
        _cached_module_wrapper[std::string(ModuleWrapperClass::static_get_name()).append("/").append(amodule->name())] =
          wrapper;
      }
      return wrapper;
    }

    template <class ModuleWrapperClass>
    auto get_module_wrapper(const std::string &module) -> ModuleWrapperClass * {
      ModuleWrapper *bmodule =
        _cached_module_wrapper[std::string(ModuleWrapperClass::static_get_name()).append("/").append(module)];
      ModuleWrapperClass *wrapper = dynamic_cast<ModuleWrapperClass *>(bmodule);

      if (!wrapper) {
        Module *amodule = get_module(module);
        if (amodule) {
          wrapper = new ModuleWrapperClass(amodule);
          _cached_module_wrapper
            [std::string(ModuleWrapperClass::static_get_name()).append("/").append(amodule->name())] = wrapper;
        }
      }
      return wrapper;
    }

    // context data

    void set_context_data(const std::string &key, void *value, void (*free_value)(void *) = 0);
    void unset_context_data(const std::string &key);
    auto get_context_data(const std::string &key) -> void *;

    // shell
    auto init_shell(const std::string &shell_type) -> bool;
    auto get_shell() -> Shell *;
    auto shell_type() -> std::string;

    // undo tracking
    void push_undo_manager(UndoManager *um);
    auto pop_undo_manager() -> UndoManager *;

    auto get_undo_manager() const -> UndoManager *;
    void start_tracking_changes();
    void stop_tracking_changes();
    auto tracking_changes() const -> bool {
      return _tracking_changes > 0;
    }

    /** Starts tracking undo changes and opens an undo group.
     * Use the AutoUndo class for auto-trackign.
     */
    auto begin_undoable_action(UndoGroup *group = 0) -> UndoGroup *;
    void end_undoable_action(const std::string &group_description);
    void cancel_undoable_action();

    // grt logging/messaging
    auto messageHandlerCount() -> int {
      return (int)_messageSlotStack.size();
    }
    void pushMessageHandler(SlotHolder *slot);
    void popMessageHandler();
    void removeMessageHandler(SlotHolder *slot);

    void push_status_query_handler(const StatusQuerySlot &slot);
    void pop_status_query_handler();
    auto query_status() -> bool;

    void send_error(const std::string &message, const std::string &details = "", void *sender = NULL);
    void send_warning(const std::string &message, const std::string &details = "", void *sender = NULL);
    void send_info(const std::string &message, const std::string &details = "", void *sender = NULL);
    void send_progress(float percentage, const std::string &message, const std::string &details = "",
                       void *sender = NULL);
    void begin_progress_step(float from, float to);
    void end_progress_step();
    void reset_progress_steps();

    void send_verbose(const std::string &message, void *sender = NULL);
    void send_output(const std::string &text, void *sender = NULL);

  protected:
    struct AutoLock {
      AutoLock() {
        grt::GRT::get()->lock();
      }
      ~AutoLock() {
        grt::GRT::get()->unlock();
      }
    };
    void lock() const;
    void unlock() const;

  protected:
    friend class MetaClass;

    std::map<std::string, ObjectRef> _objects_cache;

    std::vector<SlotHolder *> _messageSlotStack;
    std::vector<StatusQuerySlot> _status_query_slot_stack;

    std::vector<std::pair<float, float> > _progress_step_stack;

    base::RecMutex _message_mutex;

    std::list<ModuleLoader *> _loaders;
    std::vector<Module *> _modules;
    std::map<std::string, Interface *> _interfaces;
    std::map<std::string, ModuleWrapper *> _cached_module_wrapper;

    std::map<std::string, std::pair<void *, void (*)(void *)> > _context_data;

    Shell *_shell;

    void add_metaclass(MetaClass *stru);
    auto module_path_in_bundle(const std::string &path) -> std::string;

    auto handle_message(const Message &msg, void *sender) -> bool;

    std::map<std::string, MetaClass *> _metaclasses;
    std::list<MetaClass *> _metaclasses_list;

    ValueRef _root;
    std::list<UndoManager *> _undo_managers;
    UndoManager *_default_undo_manager;
    std::string _global_module_options_path;
    std::string _document_module_options_path;
    int _tracking_changes;
    bool _check_serialized_crc;
    bool _verbose;
    bool _scanning_modules;
    bool _testing;

  private:
    GRT();
    GRT(const GRT &) = delete;
    auto operator=(GRT &) -> GRT & = delete;
  };

  //------------------------------------------------------------------------------------------------

  template <class O>
  inline bool ListRef<O>::can_wrap(const ValueRef &value) {
    if (value.type() != ListType)
      return false;

    if (!value.is_valid())
      return true;

    internal::List *candidate_list = static_cast<internal::List *>(value.valueptr());

    if (candidate_list->content_type() != O::static_type())
      return false;

    // we allow stuff like List<db_Table> = List<db_mysql_Table>
    //
    MetaClass *content_class = grt::GRT::get()->get_metaclass(O::static_class_name());
    if (!content_class && !O::static_class_name().empty())
      throw std::runtime_error(std::string("metaclass without runtime info ").append(O::static_class_name()));

    MetaClass *candidate_class = grt::GRT::get()->get_metaclass(candidate_list->content_class_name());
    if (!candidate_class && !candidate_list->content_class_name().empty())
      throw std::runtime_error(
        std::string("metaclass without runtime info ").append(candidate_list->content_class_name()));

    if (candidate_class == content_class) // classes are the same
      return true;
    if (!content_class) // we're a generic list
      return true;
    if (!candidate_class) // candidate is a generic list and we're not
      return false;
    return candidate_class->is_a(content_class);
  }
}; // namespace grt
