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

#ifndef __DELEGATEWRAPPER_H__
#define __DELEGATEWRAPPER_H__

#include "sqlide/recordset_data_storage.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

namespace MySQL {
  namespace Grt {

    inline auto nativeToManaged(int input) -> int {
      return input;
    }

    inline auto nativeToManaged(long long input) -> long long {
      return input;
    }

    inline auto nativeToManaged(float input) -> float {
      return input;
    }

    inline auto nativeToManaged(bool input) -> bool {
      return input;
    }

    inline auto nativeToManaged(const char *input) -> String ^ { return CppStringToNative(input); }

      inline auto nativeToManaged(const std::string &input) -> String ^ { return CppStringToNative(input); }

      inline auto nativeToManaged(Recordset::Ref input) -> long {
      return (long)input->key();
    }

    inline auto nativeToManaged(const std::vector<int> &input) -> List<int> ^ { return CppVectorToNativeList<int, int>(input); }

      inline auto managedToNative(int input) -> int {
      return input;
    }

    inline auto managedToNative(bool input) -> bool {
      return input;
    }

    inline std::string managedToNative(String ^ input) {
      return NativeToCppString(input);
    }

    inline std::vector<int> managedToNative(List<int> ^ input) {
      return NativeListToCppVector<int, int>(input);
    }

    // ----------------------------------------------------------------------------
    // RunWrappedDelegate0
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM, typename MD>
    public ref class RunWrappedDelegate0 {
    public:
      RunWrappedDelegate0(MD ^ md) : managed_delegate(md) {
      }
      RN native_callback() {
        RM ret = managed_delegate();
        return managedToNative(ret);
      }

    private:
      MD ^ managed_delegate;
    };
    template <typename MD>
    public ref class RunWrappedDelegate0<void, void, MD> {
    public:
      RunWrappedDelegate0(MD ^ md) : managed_delegate(md) {
      }
      auto native_callback() -> void {
        managed_delegate();
      }

    private:
      MD ^ managed_delegate;
    };

    // ----------------------------------------------------------------------------
    // RunWrappedDelegate1
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM, typename A1N, typename A1M, typename MD>
    public ref class RunWrappedDelegate1 {
    public:
      RunWrappedDelegate1(MD ^ md) : managed_delegate(md) {
      }
      RN native_callback(const A1N &a1) {
        RM ret = managed_delegate(nativeToManaged(a1));
        return managedToNative(ret);
      }

    private:
      MD ^ managed_delegate;
    };
    template <typename A1N, typename A1M, typename MD>
    public ref class RunWrappedDelegate1<void, void, A1N, A1M, MD> {
    public:
      RunWrappedDelegate1(MD ^ md) : managed_delegate(md) {
      }
      auto native_callback(const A1N &a1) -> void {
        managed_delegate(nativeToManaged(a1));
      }

    private:
      MD ^ managed_delegate;
    };

    // ----------------------------------------------------------------------------
    // RunWrappedDelegate2
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM, typename A1N, typename A1M, typename A2N, typename A2M, typename MD>
    public ref class RunWrappedDelegate2 {
    public:
      RunWrappedDelegate2(MD ^ md) : managed_delegate(md) {
      }
      RN native_callback(const A1N &a1, const A2N &a2) {
        RM ret = managed_delegate(nativeToManaged(a1), nativeToManaged(a2));
        return managedToNative(ret);
      }

    private:
      MD ^ managed_delegate;
    };
    template <typename A1N, typename A1M, typename A2N, typename A2M, typename MD>
    public ref class RunWrappedDelegate2<void, void, A1N, A1M, A2N, A2M, MD> {
    public:
      RunWrappedDelegate2(MD ^ md) : managed_delegate(md) {
      }
      auto native_callback(const A1N &a1, const A2N &a2) -> void {
        managed_delegate(nativeToManaged(a1), nativeToManaged(a2));
      }

    private:
      MD ^ managed_delegate;
    };

    // ----------------------------------------------------------------------------
    // RunWrappedDelegate3
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM, typename A1N, typename A1M, typename A2N, typename A2M, typename A3N,
              typename A3M, typename MD>
    public ref class RunWrappedDelegate3 {
    public:
      RunWrappedDelegate3(MD ^ md) : managed_delegate(md) {
      }
      RN native_callback(const A1N &a1, const A2N &a2, const A3N &a3) {
        RM ret = managed_delegate(nativeToManaged(a1), nativeToManaged(a2), nativeToManaged(a3));
        return managedToNative(ret);
      }

    private:
      MD ^ managed_delegate;
    };
    template <typename A1N, typename A1M, typename A2N, typename A2M, typename A3N, typename A3M, typename MD>
    public ref class RunWrappedDelegate3<void, void, A1N, A1M, A2N, A2M, A3N, A3M, MD> {
    public:
      RunWrappedDelegate3(MD ^ md) : managed_delegate(md) {
      }
      auto native_callback(const A1N &a1, const A2N &a2, const A3N &a3) -> void {
        managed_delegate(nativeToManaged(a1), nativeToManaged(a2), nativeToManaged(a3));
      }

    private:
      MD ^ managed_delegate;
    };

    // ----------------------------------------------------------------------------
    // RunWrappedDelegate4
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM, typename A1N, typename A1M, typename A2N, typename A2M, typename A3N,
              typename A3M, typename A4N, typename A4M, typename MD>
    public ref class RunWrappedDelegate4 {
    public:
      RunWrappedDelegate4(MD ^ md) : managed_delegate(md) {
      }
      RN native_callback(const A1N &a1, const A2N &a2, const A3N &a3, const A4N &a4) {
        RM ret = managed_delegate(nativeToManaged(a1), nativeToManaged(a2), nativeToManaged(a3), nativeToManaged(a4));
        return managedToNative(ret);
      }

    private:
      MD ^ managed_delegate;
    };
    template <typename A1N, typename A1M, typename A2N, typename A2M, typename A3N, typename A3M, typename A4N,
              typename A4M, typename MD>
    public ref class RunWrappedDelegate4<void, void, A1N, A1M, A2N, A2M, A3N, A3M, A4N, A4M, MD> {
    public:
      RunWrappedDelegate4(MD ^ md) : managed_delegate(md) {
      }
      auto native_callback(const A1N &a1, const A2N &a2, const A3N &a3, const A4N &a4) -> void {
        managed_delegate(nativeToManaged(a1), nativeToManaged(a2), nativeToManaged(a3), nativeToManaged(a4));
      }

    private:
      MD ^ managed_delegate;
    };

    // ----------------------------------------------------------------------------
    // DelegateSlotWrapper_
    // ----------------------------------------------------------------------------
    template <typename MD, typename WD, typename NCB, typename NS, typename DR>
    public ref class DelegateSlotWrapper_ {
    public:
      DelegateSlotWrapper_(MD ^ md) : managed_delegate(md), native_slot(NULL) {
        run_wrapped_delegate = gcnew DR(md);
      }
      auto get_slot() -> NS * {
        if (native_slot)
          return native_slot;
        if (nullptr == wrapped_delegate)
          wrapped_delegate = gcnew WD(run_wrapped_delegate, &DR::native_callback);
        IntPtr ip = Marshal::GetFunctionPointerForDelegate(wrapped_delegate);
        NCB cb = static_cast<NCB>(ip.ToPointer());
        native_slot = new NS(cb);
        return native_slot;
      }
      auto destroy_slot() -> void {
        if (!native_slot)
          return;
        delete native_slot;
        native_slot = NULL;
      }

      bool wraps_delegate(MD ^ md) {
        return managed_delegate == md;
      }

    private:
      MD ^ managed_delegate;
      WD ^ wrapped_delegate;
      DR ^ run_wrapped_delegate;
      NS *native_slot;
    };

    // ----------------------------------------------------------------------------
    // DelegateSlot0
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM>
    public ref class DelegateSlot0 {
    public:
      auto ManagedDelegate() -> delegate RM;
      auto WrapperDelegate() -> delegate RN;
      typedef RN (*NativeCBType)();
      typedef boost::function<RN()> NativeSlot;
      typedef RunWrappedDelegate0<RN, RM, ManagedDelegate> RunWrappedDelegate;
      typedef DelegateSlotWrapper_<ManagedDelegate, WrapperDelegate, NativeCBType, NativeSlot, RunWrappedDelegate>
        DelegateSlotWrapper;

    private:
      DelegateSlotWrapper ^ delegate_slot_wrapper;

    public:
      DelegateSlot0(ManagedDelegate ^ deleg) : delegate_slot_wrapper(gcnew DelegateSlotWrapper(deleg)) {
      }
      const boost::function<RN()> &get_slot() {
        return *delegate_slot_wrapper->get_slot();
      }
      bool wraps_delegate(ManagedDelegate ^ deleg) {
        return delegate_slot_wrapper->wraps_delegate(deleg);
      }

    private:
      ~DelegateSlot0() {
        reset();
      }
      !DelegateSlot0() {
        reset();
      }
      auto reset() -> void {
        delegate_slot_wrapper->destroy_slot();
      }
    };

    // ----------------------------------------------------------------------------
    // DelegateSlot1
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM, typename A1N, typename A1M>
    public ref class DelegateSlot1 {
    public:
      delegate RM ManagedDelegate(A1M);
      auto WrapperDelegate(const A1N &) -> delegate RN;
      typedef boost::function<RN(const A1N &)> NativeSlot;
      typedef RN (*NativeCBType)(const A1N &);
      typedef RunWrappedDelegate1<RN, RM, A1N, A1M, ManagedDelegate> RunWrappedDelegate;
      typedef DelegateSlotWrapper_<ManagedDelegate, WrapperDelegate, NativeCBType, NativeSlot, RunWrappedDelegate>
        DelegateSlotWrapper;

    private:
      DelegateSlotWrapper ^ delegate_slot_wrapper;

    public:
      DelegateSlot1(ManagedDelegate ^ deleg) : delegate_slot_wrapper(gcnew DelegateSlotWrapper(deleg)) {
      }
      auto get_slot() -> const NativeSlot & {
        return *delegate_slot_wrapper->get_slot();
      }
      bool wraps_delegate(ManagedDelegate ^ deleg) {
        return delegate_slot_wrapper->wraps_delegate(deleg);
      }

    private:
      ~DelegateSlot1() {
        reset();
      }
      !DelegateSlot1() {
        reset();
      }
      auto reset() -> void {
        delegate_slot_wrapper->destroy_slot();
      }
    };

    // ----------------------------------------------------------------------------
    // DelegateSlot2
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM, typename A1N, typename A1M, typename A2N, typename A2M>
    public ref class DelegateSlot2 {
    public:
      delegate RM ManagedDelegate(A1M, A2M);
      auto WrapperDelegate(const A1N &, const A2N &) -> delegate RN;
      typedef RN (*NativeCBType)(const A1N &, const A2N &);
      typedef boost::function<RN(const A1N &, const A2N &)> NativeSlot;
      typedef RunWrappedDelegate2<RN, RM, A1N, A1M, A2N, A2M, ManagedDelegate> RunWrappedDelegate;
      typedef DelegateSlotWrapper_<ManagedDelegate, WrapperDelegate, NativeCBType, NativeSlot, RunWrappedDelegate>
        DelegateSlotWrapper;

    private:
      DelegateSlotWrapper ^ delegate_slot_wrapper;

    public:
      DelegateSlot2(ManagedDelegate ^ deleg) : delegate_slot_wrapper(gcnew DelegateSlotWrapper(deleg)) {
      }
      auto get_slot() -> const NativeSlot & {
        return *delegate_slot_wrapper->get_slot();
      }
      bool wraps_delegate(ManagedDelegate ^ deleg) {
        return delegate_slot_wrapper->wraps_delegate(deleg);
      }

    private:
      ~DelegateSlot2() {
        reset();
      }
      !DelegateSlot2() {
        reset();
      }
      auto reset() -> void {
        delegate_slot_wrapper->destroy_slot();
      }
    };

    // ----------------------------------------------------------------------------
    // DelegateSlot3
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM, typename A1N, typename A1M, typename A2N, typename A2M, typename A3N,
              typename A3M>
    public ref class DelegateSlot3 {
    public:
      delegate RM ManagedDelegate(A1M, A2M, A3M);
      auto WrapperDelegate(const A1N &, const A2N &, const A3N &) -> delegate RN;
      typedef RN (*NativeCBType)(const A1N &, const A2N &, const A3N &);
      typedef boost::function<RN(const A1N &, const A2N &, const A3N &)> NativeSlot;
      typedef RunWrappedDelegate3<RN, RM, A1N, A1M, A2N, A2M, A3N, A3M, ManagedDelegate> RunWrappedDelegate;
      typedef DelegateSlotWrapper_<ManagedDelegate, WrapperDelegate, NativeCBType, NativeSlot, RunWrappedDelegate>
        DelegateSlotWrapper;

    private:
      DelegateSlotWrapper ^ delegate_slot_wrapper;

    public:
      DelegateSlot3(ManagedDelegate ^ deleg) : delegate_slot_wrapper(gcnew DelegateSlotWrapper(deleg)) {
      }
      auto get_slot() -> const NativeSlot & {
        return *delegate_slot_wrapper->get_slot();
      }
      bool wraps_delegate(ManagedDelegate ^ deleg) {
        return delegate_slot_wrapper->wraps_delegate(deleg);
      }

    private:
      ~DelegateSlot3() {
        reset();
      }
      !DelegateSlot3() {
        reset();
      }
      auto reset() -> void {
        delegate_slot_wrapper->destroy_slot();
      }
    };

    // ----------------------------------------------------------------------------
    // DelegateSlot4
    // ----------------------------------------------------------------------------
    template <typename RN, typename RM, typename A1N, typename A1M, typename A2N, typename A2M, typename A3N,
              typename A3M, typename A4N, typename A4M>
    public ref class DelegateSlot4 {
    public:
      delegate RM ManagedDelegate(A1M, A2M, A3M, A4M);
      auto WrapperDelegate(const A1N &, const A2N &, const A3N &, const A4N &) -> delegate RN;
      typedef RN (*NativeCBType)(const A1N &, const A2N &, const A3N &, const A4N &);
      typedef boost::function<RN(const A1N &, const A2N &, const A3N &, const A4N &)> NativeSlot;
      typedef RunWrappedDelegate4<RN, RM, A1N, A1M, A2N, A2M, A3N, A3M, A4N, A4M, ManagedDelegate> RunWrappedDelegate;
      typedef DelegateSlotWrapper_<ManagedDelegate, WrapperDelegate, NativeCBType, NativeSlot, RunWrappedDelegate>
        DelegateSlotWrapper;

    private:
      DelegateSlotWrapper ^ delegate_slot_wrapper;

    public:
      DelegateSlot4(ManagedDelegate ^ deleg) : delegate_slot_wrapper(gcnew DelegateSlotWrapper(deleg)) {
      }
      auto get_slot() -> const NativeSlot & {
        return *delegate_slot_wrapper->get_slot();
      }
      bool wraps_delegate(ManagedDelegate ^ deleg) {
        return delegate_slot_wrapper->wraps_delegate(deleg);
      }

    private:
      ~DelegateSlot4() {
        reset();
      }
      !DelegateSlot4() {
        reset();
      }
      auto reset() -> void {
        delegate_slot_wrapper->destroy_slot();
      }
    };
  }
};

#endif // __DELEGATEWRAPPER_H__