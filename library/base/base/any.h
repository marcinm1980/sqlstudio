/*
 * Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include <type_traits>
#include <utility>
#include <typeinfo>

namespace base {

  template <class T>
  using StorageType = typename std::decay<T>::type;

  struct any {
    auto isNull() const -> bool {
      return !ptr;
    }

    template <typename U>
    any(U&& value, typename std::enable_if<!std::is_same<typename std::decay<U>::type, any>::value>::type* = nullptr)
      : ptr(new Derived<StorageType<U>>(std::forward<U>(value))) {
    }

    template <class U>
    auto is() const -> bool {
      using T = StorageType<U>;

      auto derived = dynamic_cast<Derived<T>*>(ptr);
      return derived != nullptr;
    }

    template <class U>
    auto as() -> StorageType<U>& {
      using T = StorageType<U>;

      auto derived = dynamic_cast<Derived<T>*>(ptr);

      if (!derived)
        throw std::bad_cast();

      return derived->value;
    }

    template <class U>
    auto as() const -> StorageType<U>& {
      using T = StorageType<U>;

      auto derived = dynamic_cast<Derived<T>*>(ptr);

      if (!derived)
        throw std::bad_cast();

      return derived->value;
    }

    template <class U>
    operator U() {
      return as<StorageType<U>>();
    }

    template <class U>
    operator U() const {
      return as<StorageType<U>>();
    }

    any() : ptr(nullptr) {
    }

    any(any&& that) : ptr(that.ptr) {
      that.ptr = nullptr;
    }

    any(const any& that) : ptr(that.clone()) {
    }

    auto operator=(const any& a) -> any& {
      if (ptr == a.ptr)
        return *this;

      auto old_ptr = ptr;

      ptr = a.clone();

      delete old_ptr;

      return *this;
    }

    auto operator=(any&& a) -> any& {
      if (ptr == a.ptr)
        return *this;

      std::swap(ptr, a.ptr);

      return *this;
    }

    ~any() {
      delete ptr;
    }

  private:
    struct Base {
      virtual ~Base() {
      }

      virtual auto clone() const -> Base* = 0;
    };

    template <typename T>
    struct Derived : Base {
      template <typename U>
      Derived(U&& value) : value(std::forward<U>(value)) {
      }

      T value;

      auto clone() const -> Base* {
        return new Derived<T>(value);
      }
    };

    auto clone() const -> Base* {
      if (ptr)
        return ptr->clone();
      else
        return nullptr;
    }

    Base* ptr;
  };

} /* namespace base */
