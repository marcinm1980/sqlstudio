/*
 * Copyright (c) 2014, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "mforms/native.h"

#ifndef SWIG
class Recordset;
#endif

namespace mforms {
  class ContextMenu;

  enum ColumnHeaderIndicator {
    NoIndicator = 0,
    SortDescIndicator = -1,
    SortAscIndicator = 1,
  };

  // This is a skeleton/proxy/facade class for a Recordset grid view
  // The implementation of the view itself is in frontend/platform specific code
  // The main use for this class is to provide an interface for SWIG
  // and to allow the frontend to register a factory function to allocate the
  // concrete implementation.
  class MFORMS_EXPORT GridView : public NativeContainer {
  public:
    virtual auto get_column_count() -> int = 0;
    virtual auto get_column_width(int column) -> int = 0;
    virtual auto set_column_width(int column, int width) -> void = 0;

    virtual auto update_columns() -> void = 0;

    virtual auto set_column_header_indicator(int column, ColumnHeaderIndicator order) -> void = 0;

    virtual auto current_cell(size_t &row, int &column) -> bool = 0;
    virtual auto set_current_cell(size_t row, int column) -> void = 0;

    virtual auto set_header_menu(ContextMenu *menu) -> void;
    auto get_clicked_header_column() -> int {
      return _clicked_header_column;
    }

#ifndef SWIG
    static auto create(std::shared_ptr<Recordset> rset) -> GridView *;

    static auto register_factory(GridView *(*create)(std::shared_ptr<Recordset> rset)) -> void;
#endif

    // TODO must be emited from Windows
    boost::signals2::signal<void(int)> *signal_column_resized() {
      return &_signal_column_resized;
    }
    boost::signals2::signal<void(const std::vector<int>)> *signal_columns_resized() {
      return &_signal_columns_resized;
    }

    auto header_menu() -> ContextMenu * {
      return _header_menu;
    }

#ifndef SWIG
    auto clicked_header_column(int column) -> void;
#endif
  protected:
    GridView();

  private:
    boost::signals2::signal<void(int)> _signal_column_resized;
    boost::signals2::signal<void(const std::vector<int>)> _signal_columns_resized;
    ContextMenu *_header_menu;
    int _clicked_header_column;
  };
};
