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

#pragma once

#include <iostream>

#include "../grt.h"

namespace grt {

  //
  enum ChangeType {
    SimpleValue,
    ValueAdded,
    ValueRemoved,

    ObjectModified,
    ObjectAttrModified,

    ListModified,

    ListItemAdded,
    ListItemModified,
    ListItemRemoved,
    ListItemOrderChanged,

    DictModified,

    DictItemAdded,
    DictItemModified,
    DictItemRemoved,
  };

  class DiffChange;

  typedef std::vector<std::shared_ptr<DiffChange> > ChangeList;

  struct MYSQLGRT_PUBLIC ChangeSet {
    typedef ChangeList::const_iterator const_iterator;
    typedef ChangeList::const_reverse_iterator const_reverse_iterator;

    ChangeList changes;

    inline auto begin() const -> const_iterator {
      return changes.begin();
    }
    inline auto end() const -> const_iterator {
      return changes.end();
    }

    inline auto rbegin() const -> const_reverse_iterator {
      return changes.rbegin();
    }
    inline auto rend() const -> const_reverse_iterator {
      return changes.rend();
    }

    inline auto append(std::shared_ptr<DiffChange> change) -> void {
      if (change.get())
        changes.push_back(change);
    }

    inline auto empty() const -> bool {
      return changes.empty();
    }
  };

  // Base class for any specific change in grt instances
  class MYSQLGRT_PUBLIC DiffChange {
    friend class std::shared_ptr<DiffChange>;

  protected:
    // Parent change, is plain pointer instead of shared ptr to avoid cylic references
    // of shared pointers
    // It also initialized later since changes are created in bottom to top and thus
    // children created prior to parents
    DiffChange* _parent;

    DiffChange(ChangeType atype) : _parent(NULL), type(atype) {
    }

    ChangeType type;
    virtual ~DiffChange() {
    }

  public:
    auto set_parent(DiffChange* parent) -> void {
      _parent = parent;
    }
    auto parent() const -> DiffChange* {
      return _parent;
    }

    virtual auto subchanges() const -> const ChangeSet* {
      return NULL;
    }

    // dumps to stdout text for of change tree
    virtual auto dump_log(int level) const -> void {
      std::cout << std::string(level, ' ');
      std::cout << get_type_name() << std::endl;
    }

    auto get_change_type() const -> ChangeType;

    // string representation of ChangeType
    auto get_type_name() const -> std::string {
#define CASE(v) \
  case v:       \
    return #v;
      switch (type) {
        CASE(SimpleValue);
        CASE(ValueAdded);
        CASE(ValueRemoved);
        CASE(ObjectModified);
        CASE(ObjectAttrModified);
        CASE(ListModified);
        CASE(ListItemAdded);
        CASE(ListItemModified);
        CASE(ListItemRemoved);
        CASE(ListItemOrderChanged);
        CASE(DictModified);
        CASE(DictItemAdded);
        CASE(DictItemModified);
        CASE(DictItemRemoved);
      }
      return "unknown";
    }
  };

  //////////////////////////////////////////////////////////////
  // @class MultiChange
  //////////////////////////////////////////////////////////////
  //
  // @brief Contains set of nested changes
  //
  //////////////////////////////////////////////////////////////
  class MYSQLGRT_PUBLIC MultiChange : public DiffChange {
  protected:
    ChangeSet _changes;

  public:
    MultiChange(ChangeType atype, ChangeSet& changes) : DiffChange(atype), _changes(changes) {
      for (ChangeSet::const_iterator iter = _changes.begin(); iter != _changes.end(); ++iter) {
        (*iter)->set_parent(this);
      }
    }
    virtual ~MultiChange() { /*TODO release changes*/
    }

    virtual auto subchanges() const -> const ChangeSet* {
      return &_changes;
    }

    auto dump_log(int level) const -> void {
      std::cout << std::string(level, ' ');
      std::cout << get_type_name() << std::endl;
      for (ChangeSet::const_iterator iter = _changes.begin(); iter != _changes.end(); ++iter) {
        (*iter)->dump_log(level + 1);
      }
    }
  };
}
