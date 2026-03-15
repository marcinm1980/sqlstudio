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

#include "base/string_utilities.h"
#include "wf_base.h"
#include "wf_view.h"
#include "wf_native.h"
#include "wf_menubar.h"
#include "wf_gridview.h"

using namespace MySQL;
using namespace MySQL::Controls;
using namespace MySQL::Forms;
using namespace MySQL::Base;

using namespace System::Drawing;

class ConcreteGridView : public mforms::GridView {
  gcroot<IRecordsetView ^> _view;
  bool _resizing;

public:
  ConcreteGridView(IRecordsetView ^ rset) {
    _resizing = false;
    _view = rset;
  }

  auto view() -> IRecordsetView ^ { return _view; }

    auto control() -> Control ^ { return _view->control(); }

    auto is_resizing() -> bool {
    return _resizing;
  }

  virtual auto get_column_count() -> int {
    return _view->get_column_count();
  }

  virtual auto get_column_width(int column) -> int {
    return _view->get_column_width(column);
  }

  virtual auto set_column_width(int column, int width) -> void {
    _view->set_column_width(column, width);
  }

  virtual auto update_columns() -> void {
    _view->update_columns();
  }

  virtual auto current_cell(size_t &row, int &column) -> bool {
    row = _view->current_cell_row();
    column = _view->current_cell_column();
    if (_view->current_cell_row() < 0)
      return false;
    return true;
  }

  virtual auto set_current_cell(size_t row, int column) -> void {
    _view->set_current_cell((int)row, column);
  }

  virtual auto set_column_header_indicator(int column, mforms::ColumnHeaderIndicator order) -> void {
    _view->set_column_header_indicator(column, (IRecordsetView::ColumnHeaderIndicator)order);
  }

  virtual auto set_header_menu(mforms::ContextMenu *header_menu) -> void {
    System::Windows::Forms::ContextMenuStrip ^ menu =
      MenuBarWrapper::GetManagedObject<System::Windows::Forms::ContextMenuStrip>(header_menu);
    if (Conversions::UseWin8Drawing())
      menu->Renderer = gcnew Win8MenuStripRenderer();
    else
      menu->Renderer = gcnew TransparentMenuStripRenderer();

    mforms::GridView::set_header_menu(header_menu);
  }
};

public
ref class MySQL::Forms::ColumnCallbackWrapper {
  ConcreteGridView *backend;

public:
  ColumnCallbackWrapper(ConcreteGridView *be) : backend(be) {
  }

  auto resized(int column) -> void {
    if (!backend->is_resizing())
      (*backend->signal_column_resized())(column);
  }

  auto header_right_click(int column) -> System::Windows::Forms::ContextMenuStrip ^ {
    backend->clicked_header_column(column);
    if (backend->header_menu() != NULL) {
      backend->header_menu()->will_show();

      return MenuBarWrapper::GetManagedObject<System::Windows::Forms::ContextMenuStrip>(backend->header_menu());
    }
    return nullptr;
  }
};

gcroot<CreateGridViewDelegate ^> GridViewWrapper::factory = nullptr;

GridViewWrapper::GridViewWrapper(mforms::GridView *backend) : NativeWrapper(backend) {
}

GridViewWrapper::~GridViewWrapper() {
}

auto GridViewWrapper::create(std::shared_ptr<Recordset> rset) -> mforms::GridView * {
  CreateGridViewDelegate ^ create = factory;
  ConcreteGridView *backend = new ConcreteGridView(create(IntPtr(&rset)));
  GridViewWrapper *wrapper = new GridViewWrapper(backend);
  NativeWrapper::ConnectParts(backend, wrapper, backend->control());

  wrapper->column_callback_delegate = gcnew ColumnCallbackWrapper(backend);
  backend->view()->set_column_resize_callback(
    gcnew IRecordsetView::ColumnResizeCallback(wrapper->column_callback_delegate, &ColumnCallbackWrapper::resized));
  backend->view()->set_column_header_right_click_callback(gcnew IRecordsetView::ColumnHeaderRightClickCallback(
    wrapper->column_callback_delegate, &ColumnCallbackWrapper::header_right_click));
  return backend;
}

void GridViewWrapper::init(CreateGridViewDelegate ^ creator) {
  factory = creator;
  mforms::GridView::register_factory(&GridViewWrapper::create);
}
