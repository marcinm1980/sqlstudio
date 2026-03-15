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

#include <deque>
#include <boost/signals2.hpp>
#include <ostream>

namespace grt {

  class UndoManager;

  class MYSQLGRT_PUBLIC UndoAction {
    std::string _description;

  public:
    virtual ~UndoAction() {};

    virtual auto set_description(const std::string &description) -> void;

    virtual auto undo(UndoManager *owner) -> void = 0;
    virtual auto description() const -> std::string {
      return _description;
    }

    virtual auto dump(std::ostream &out, int indent = 0) const -> void = 0;
  };

  class MYSQLGRT_PUBLIC SimpleUndoAction : public UndoAction {
    std::string _description;

    std::function<void()> _undo_slot;

  public:
    SimpleUndoAction(const std::function<void()> &undoslot) : _undo_slot(undoslot) {};

    virtual auto dump(std::ostream &out, int indent = 0) const -> void;

    virtual auto undo(UndoManager *owner) -> void {
      _undo_slot();
    }
  };

  class MYSQLGRT_PUBLIC UndoObjectChangeAction : public UndoAction {
  protected:
    ObjectRef _object;
    std::string _member;
    ValueRef _value;

  public:
    UndoObjectChangeAction(const ObjectRef &object, const std::string &member);
    UndoObjectChangeAction(const ObjectRef &object, const std::string &member, const ValueRef &value);

    virtual auto undo(UndoManager *owner) -> void;

    auto get_object() const -> const ObjectRef & {
      return _object;
    }
    auto get_member() const -> const std::string & {
      return _member;
    }

    virtual auto dump(std::ostream &out, int indent = 0) const -> void;
  };

  class MYSQLGRT_PUBLIC UndoListInsertAction : public UndoAction {
    BaseListRef _list;
    size_t _index;

  public:
    UndoListInsertAction(const BaseListRef &list, size_t index = BaseListRef::npos);

    virtual auto undo(UndoManager *owner) -> void;

    virtual auto dump(std::ostream &out, int indent = 0) const -> void;
  };

  class MYSQLGRT_PUBLIC UndoListSetAction : public UndoAction {
    BaseListRef _list;
    size_t _index;
    ValueRef _value;

  public:
    UndoListSetAction(const BaseListRef &list, size_t index);

    virtual auto undo(UndoManager *owner) -> void;

    virtual auto dump(std::ostream &out, int indent = 0) const -> void;
  };

  class MYSQLGRT_PUBLIC UndoListReorderAction : public UndoAction {
    BaseListRef _list;
    size_t _oindex;
    size_t _nindex;

  public:
    UndoListReorderAction(const BaseListRef &list, size_t oindex, size_t nindex);

    virtual auto undo(UndoManager *owner) -> void;
    virtual auto dump(std::ostream &out, int indent = 0) const -> void;
  };

  class MYSQLGRT_PUBLIC UndoListRemoveAction : public UndoAction {
    BaseListRef _list;
    ValueRef _value;
    size_t _index;

  public:
    UndoListRemoveAction(const BaseListRef &list, const ValueRef &value);
    UndoListRemoveAction(const BaseListRef &list, size_t index);

    virtual auto undo(UndoManager *owner) -> void;
    virtual auto dump(std::ostream &out, int indent = 0) const -> void;
  };

  class MYSQLGRT_PUBLIC UndoDictSetAction : public UndoAction {
    DictRef _dict;
    std::string _key;
    ValueRef _value;
    bool _had_value;

  public:
    UndoDictSetAction(const DictRef &dict, const std::string &key);

    virtual auto undo(UndoManager *owner) -> void;
    virtual auto dump(std::ostream &out, int indent = 0) const -> void;
  };

  class MYSQLGRT_PUBLIC UndoDictRemoveAction : public UndoAction {
    DictRef _dict;
    std::string _key;
    ValueRef _value;
    bool _had_value;

  public:
    UndoDictRemoveAction(const DictRef &dict, const std::string &key);

    virtual auto undo(UndoManager *owner) -> void;
    virtual auto dump(std::ostream &out, int indent = 0) const -> void;
  };

  class MYSQLGRT_PUBLIC UndoGroup : public UndoAction {
    std::list<UndoAction *> _actions;
    bool _is_open;

  public:
    UndoGroup();
    virtual ~UndoGroup();

    auto trim() -> void;
    auto close() -> void;
    inline auto is_open() -> bool {
      return _is_open;
    }

    virtual auto set_description(const std::string &description) -> void;
    virtual auto description() const -> std::string;

    virtual auto undo(UndoManager *owner) -> void;

    virtual auto dump(std::ostream &out, int indent = 0) const -> void;

    auto add(UndoAction *op) -> void;
    auto empty() const -> bool;

    virtual auto matches_group(UndoGroup *group) const -> bool {
      return false;
    }

    auto get_deepest_open_subgroup(UndoGroup **parent = 0) -> UndoGroup *;

    auto get_actions() -> std::list<UndoAction *> & {
      return _actions;
    }
  };

  //----------------------------------------------------------------------

  class MYSQLGRT_PUBLIC UndoManager {
  public:
    using UndoSignal = boost::signals2::signal<void(UndoAction *)>;
    using RedoSignal = boost::signals2::signal<void(UndoAction *)>;

    UndoManager();
    virtual ~UndoManager();

    auto enable_logging_to(std::ostream *stream) -> void;

    auto can_undo() const -> bool;
    auto can_redo() const -> bool;
    auto undo_description() const -> std::string;
    auto redo_description() const -> std::string;

    auto set_undo_limit(size_t limit) -> void;
    auto get_undo_limit() const -> size_t {
      return _undo_limit;
    }

    auto disable() -> void;
    auto enable() -> void;
    auto is_enabled() const -> bool {
      return _blocks == 0;
    }

    auto reset() -> void;
    auto empty() const -> bool;

    auto is_undoing() const -> bool {
      return _is_undoing;
    }
    auto is_redoing() const -> bool {
      return _is_redoing;
    }

    virtual auto undo() -> void;
    virtual auto redo() -> void;

    // the optional group to be used will become owned by the undo manager
    auto begin_undo_group(UndoGroup *group = 0) -> UndoGroup *;
    bool end_undo_group(const std::string &description = "", bool trim = false);
    auto cancel_undo_group() -> void;

    virtual auto add_undo(UndoAction *cmd) -> void;
    virtual auto add_simple_undo(const std::function<void()> &slot) -> void;
    auto set_action_description(const std::string &descr) -> void;
    auto get_action_description() const -> std::string;

    auto get_latest_undo_action() const -> UndoAction *;
    auto get_latest_closed_undo_action() const -> UndoAction *;

    auto get_running_action_description() const -> std::string;

    auto signal_undo() -> UndoSignal * {
      return &_undo_signal;
    };
    auto signal_redo() -> RedoSignal * {
      return &_redo_signal;
    };

    boost::signals2::signal<void()> *signal_changed() {
      return &_changed_signal;
    }

    auto dump_undo_stack() -> void;
    auto dump_redo_stack() -> void;

  public:
    auto get_undo_stack() -> std::deque<UndoAction *> & {
      return _undo_stack;
    }
    auto get_redo_stack() -> std::deque<UndoAction *> & {
      return _redo_stack;
    }
    auto lock() const -> void;
    auto unlock() const -> void;

  protected:
    mutable base::RecMutex _mutex;
    std::ostream *_undo_log;

    std::deque<UndoAction *> _undo_stack;
    std::deque<UndoAction *> _redo_stack;

    size_t _undo_limit;

    int _blocks;
    bool _is_undoing;
    bool _is_redoing;

    UndoSignal _undo_signal;
    RedoSignal _redo_signal;
    boost::signals2::signal<void()> _changed_signal;

    auto trim_undo_stack() -> void;
  };

  struct MYSQLGRT_PUBLIC AutoUndo {
  public:
    UndoGroup *group;

    AutoUndo(bool noop = false);
    AutoUndo(UndoGroup *use_group, bool noop = false);

    ~AutoUndo();

    auto set_description_for_last_action(const std::string &s) -> void;
    auto cancel() -> void;
    auto end_or_cancel_if_empty(const std::string &descr) -> void;
    auto end(const std::string &descr) -> void;

  private:
    bool _valid;
  };
}; // namespace grt
