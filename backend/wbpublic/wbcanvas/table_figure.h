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

#ifndef __TABLE_FIGURE_H__
#define __TABLE_FIGURE_H__

#include "figure_common.h"
#include <set>
#include "wbpublic_public_interface.h"

namespace wbbridge {
  namespace physical {
    class TableFigure;
  };
}; // namespace wbbridge

namespace wbfig {

  class ItemMagnet : public mdc::Magnet {
    virtual auto constrain_angle(double angle) const -> double;
    virtual auto owner_bounds_changed(const base::Rect &obounds) -> void;
    virtual auto owner_parent_bounds_changed(mdc::CanvasItem *item, const base::Rect &obounds) -> void;

  public:
    ItemMagnet(mdc::CanvasItem *owner);
  };

  enum ColumnFlags {
    ColumnPK = (1 << 0),
    ColumnFK = (1 << 1),
    ColumnNotNull = (1 << 2),
    ColumnAutoIncrement = (1 << 3),
    ColumnUnsigned = (1 << 4),

    ColumnListTruncated = (1 << 5)
  };

  class Table;

  class TableColumnItem : public FigureItem {
    ItemMagnet *_magnet;
    ColumnFlags _flags;

    auto check_column_connection(mdc::Connector *connector) -> bool;

    virtual auto calc_min_size() -> base::Size;
    virtual auto draw_contents(mdc::CairoCtx *cr) -> void;

  public:
    auto get_item_magnet() -> mdc::Magnet * {
      return _magnet;
    }
    TableColumnItem(mdc::Layer *layer, FigureEventHub *hub, Table *owner);

    auto set_column_flags(ColumnFlags flags) -> void;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC Table : public BaseFigure {
    using super = BaseFigure;

  public:
    Table(mdc::Layer *layer, FigureEventHub *hub, const model_ObjectRef &self, bool collapsible);

    auto get_title() -> Titlebar * {
      return &_title;
    }
    virtual auto get_index_title() -> Titlebar * {
      return 0;
    }
    virtual auto get_trigger_title() -> Titlebar * {
      return 0;
    }

    auto set_show_flags(bool flag) -> void;

    virtual auto set_dependant(bool flag) -> void = 0;

    virtual auto get_columns() -> ItemList * = 0;

    virtual auto begin_columns_sync() -> ItemList::iterator = 0;
    virtual auto sync_next_column(ItemList::iterator iter, const std::string &id, ColumnFlags type,
                                                const std::string &text) -> ItemList::iterator = 0;
    virtual auto end_columns_sync(ItemList::iterator iter) -> void = 0;

    virtual auto get_indexes() -> ItemList * {
      return 0;
    }

    virtual auto begin_indexes_sync() -> ItemList::iterator = 0;
    virtual auto sync_next_index(ItemList::iterator iter, const std::string &id,
                                               const std::string &text) -> ItemList::iterator = 0;
    virtual auto end_indexes_sync(ItemList::iterator iter) -> void = 0;

    virtual auto begin_triggers_sync() -> ItemList::iterator = 0;
    virtual auto sync_next_trigger(ItemList::iterator iter, const std::string &id,
                                                 const std::string &text) -> ItemList::iterator = 0;
    virtual auto end_triggers_sync(ItemList::iterator iter) -> void = 0;

    virtual auto highlight(const base::Color *color = 0) -> void {
      _background.set_highlight_color(color);
      _background.set_highlighted(true);
      set_highlight_color(color);
      set_highlighted(true);
      set_needs_render();
    }

    virtual auto unhighlight() -> void {
      _background.set_highlighted(false);
      set_highlighted(false);
      set_needs_render();
    }

    virtual auto set_title_font(const mdc::FontSpec &font) -> void;
    virtual auto set_section_font(const mdc::FontSpec &font) -> void;
    virtual auto set_content_font(const mdc::FontSpec &font) -> void;

    auto columns_hidden() -> bool {
      return _hide_columns;
    }
    auto indexes_hidden() -> bool {
      return _hide_indexes;
    }
    auto triggers_hidden() -> bool {
      return _hide_triggers;
    }

    auto get_sides_magnet() -> mdc::BoxSideMagnet * {
      return _sides_magnet;
    }

    virtual auto toggle(bool flag) -> void {
    }
    virtual auto toggle_indexes(bool flag) -> void {
    }
    virtual auto toggle_triggers(bool flag) -> void {
    }

    virtual auto set_max_columns_shown(int count) -> void {
    }

  protected:
    mdc::RectangleFigure _background;
    boost::signals2::signal<void(int, bool)> _signal_index_crossed;

    mdc::BoxSideMagnet *_sides_magnet;

    Titlebar _title;
    double _original_column_box_height;

    bool _hide_columns;
    bool _hide_indexes;
    bool _hide_triggers;

    bool _show_flags;

    auto create_column_item(mdc::Layer *layer, wbfig::FigureEventHub *hub) -> wbfig::FigureItem *;
    auto update_column_item(wbfig::FigureItem *item, ColumnFlags flags) -> void;

    auto create_index_item(mdc::Layer *layer, wbfig::FigureEventHub *hub) -> wbfig::FigureItem *;

    auto compare_connection_position(mdc::Connector *a, mdc::Connector *b, mdc::BoxSideMagnet::Side vertical) -> bool;

    virtual auto get_expanded() -> bool {
      return true;
    }

    virtual auto get_indexes_expanded() -> bool {
      return true;
    }
    virtual auto get_triggers_expanded() -> bool {
      return true;
    }
  };
}; // namespace wbfig

#endif
