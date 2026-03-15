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

#ifndef __TABLE_FIGURE_WB_H__
#define __TABLE_FIGURE_WB_H__

#include "table_figure.h"

namespace wbbridge {
  namespace physical {
    class TableFigure;
  };
};

namespace wbfig {

  class WBPUBLICBACKEND_PUBLIC_FUNC WBTable : public Table {
    typedef Table super;

  public:
    WBTable(mdc::Layer *layer, FigureEventHub *hub, const model_ObjectRef &self);
    virtual ~WBTable();

    virtual auto set_color(const base::Color &color) -> void;
    virtual auto set_dependant(bool flag) -> void;

    virtual auto set_allow_manual_resizing(bool flag) -> void;

    virtual auto begin_columns_sync() -> ItemList::iterator;
    virtual auto sync_next_column(ItemList::iterator iter, const std::string &id, ColumnFlags type,
                                                const std::string &text) -> ItemList::iterator;
    virtual auto end_columns_sync(ItemList::iterator iter) -> void;

    virtual auto begin_indexes_sync() -> ItemList::iterator;
    virtual auto sync_next_index(ItemList::iterator iter, const std::string &id, const std::string &text) -> ItemList::iterator;
    virtual auto end_indexes_sync(ItemList::iterator iter) -> void;

    virtual auto begin_triggers_sync() -> ItemList::iterator;
    virtual auto sync_next_trigger(ItemList::iterator iter, const std::string &id,
                                                 const std::string &text) -> ItemList::iterator;
    virtual auto end_triggers_sync(ItemList::iterator iter) -> void;

    virtual auto get_index_title() -> Titlebar * {
      return &_index_title;
    }
    virtual auto get_trigger_title() -> Titlebar * {
      return &_trigger_title;
    }

    auto hide_indices() -> void;
    auto hide_triggers() -> void;

    auto hide_columns() -> void;
    virtual auto get_columns() -> ItemList * {
      return &_columns;
    }
    virtual auto get_indexes() -> ItemList * {
      return &_indexes;
    }

    virtual auto set_max_columns_shown(int count) -> void;

    virtual auto set_content_font(const mdc::FontSpec &font) -> void;

  protected:
    mdc::Box _content_box;

    ShrinkableBox _column_box;
    ItemList _columns;

    Titlebar _index_title;
    mdc::Box _index_box;
    ItemList _indexes;

    Titlebar _trigger_title;
    mdc::Box _trigger_box;
    ItemList _triggers;

    Titlebar _footer;

    virtual auto get_expanded() -> bool;
    virtual auto toggle(bool flag) -> void;

    virtual auto get_indexes_expanded() -> bool;
    virtual auto get_triggers_expanded() -> bool;
    virtual auto toggle_indexes(bool flag) -> void;
    virtual auto toggle_triggers(bool flag) -> void;

    auto create_truncated_item(mdc::Layer *layer, wbfig::FigureEventHub *hub) -> wbfig::FigureItem *;
  };
};

#endif
