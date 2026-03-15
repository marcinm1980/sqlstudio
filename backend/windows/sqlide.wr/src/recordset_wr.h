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

#include "var_grid_model_wr.h"

namespace MySQL {
  namespace Grt {
    namespace Db {

    public
      ref class RecordsetWrapper : public MySQL::Grt::VarGridModelWrapper {
      public:
        typedef ManagedRef<::Recordset> ^ Ref;
        RecordsetWrapper(Ref ref);
        RecordsetWrapper(IntPtr nref_ptr);
        auto ref() -> Ref {
          return _ref;
        }
        auto ref_intptr() -> IntPtr {
          return ~_ref;
        }

      private:
        Ref _ref;
        ~RecordsetWrapper();

      public:
        MySQL::Grt::ActionList ^ action_list;
        auto register_edit_actions() -> void;

        auto key() -> long long {
          return _ref->key();
        }
        String ^ caption() { return CppStringToNative(_ref->caption()); } void caption(String ^ value) {
          _ref->caption(NativeToCppString(value));
        }
        auto can_close() -> bool {
          return _ref->can_close();
        }
        auto close() -> bool {
          if ((void*)~_ref != NULL)
            return _ref->close();
          return true;
        }

        String ^ status_text() { return CppStringToNative(_ref->status_text()); }

          auto row_count() -> int {
          return (int)_ref->row_count();
        }

        void pending_changes(int % upd_count, int % ins_count, int % del_count);
        auto has_pending_changes() -> bool {
          return _ref->has_pending_changes();
        }
        auto apply_changes() -> void {
          _ref->apply_changes();
        }
        auto rollback() -> void {
          _ref->rollback();
        }

        auto limit_rows(bool value) -> void {
          _ref->limit_rows(value);
        }
        auto limit_rows() -> bool {
          return _ref->limit_rows();
        }
        auto limit_rows_applicable() -> bool {
          return _ref->limit_rows_applicable();
        }
        auto limit_rows_count() -> int {
          return _ref->limit_rows_count();
        }

        auto real_row_count() -> int {
          return (int)_ref->real_row_count();
        }

        auto sort_by(int column, int direction, bool retaining) -> void {
          _ref->sort_by((::ColumnId)column, direction, retaining);
        }

        bool delete_nodes(List<NodeIdWrapper ^> ^ nodes);

        auto has_column_filters() -> bool {
          return _ref->has_column_filters();
        }
        auto has_column_filter(int column) -> bool {
          return _ref->has_column_filter((::ColumnId)column);
        }
        auto get_column_filter_expr(int column) -> String ^ {
          return CppStringToNative(_ref->get_column_filter_expr((::ColumnId)column));
        } void set_column_filter(int column, System::String ^ filter_expr) {
          _ref->set_column_filter((::ColumnId)column, NativeToCppString(filter_expr));
        }
        auto reset_column_filter(int column) -> void {
          _ref->reset_column_filter((::ColumnId)column);
        }
        auto reset_column_filters() -> void {
          _ref->reset_column_filters();
        }
        auto column_filter_icon_id() -> int {
          return (int)_ref->column_filter_icon_id();
        }

        auto data_search_string() -> String ^ {
          return CppStringToNative(_ref->data_search_string());
        } void set_data_search_string(String ^ value) {
          _ref->set_data_search_string(NativeToCppString(value));
        }
        auto reset_data_search_string() -> void {
          _ref->reset_data_search_string();
        }

        void copy_rows_to_clipboard(List<int> ^ indeces);
        auto copy_field_to_clipboard(int row, int column) -> void {
          _ref->copy_field_to_clipboard(row, column);
        }

        void set_flush_ui_changes_cb(DelegateSlot0<void, void>::ManagedDelegate ^ apply);

        auto inserts_editor() -> bool {
          return _ref->inserts_editor();
        }

        auto getFont() -> String^ {
          return CppStringToNative(_ref->getFont());
        }

        auto getFontSize() -> float {
          return _ref->getFontSize();
        }

      private:
        DelegateSlot0<void, void> ^ _flush_ui_changes;

      public:
        GrtThreadedTaskWrapper ^ task;

        System::Windows::Forms::ContextMenuStrip ^ get_context_menu(List<int> ^ indexes, int clicked_column);

        delegate MySQL::Base::IRecordsetView ^ CreateRecordsetViewForWrapper(RecordsetWrapper ^ wrapper);

        // used by the main program to initialize mforms::RecordGridView
        // we can't create a direct delegate that will create a RecordsetView from a std::shared_ptr<Recordset>, because
        // RecordsetView
        // is in C# and can't pass around std::shared_ptr values, so we do it in 2 stage callback
        static void init_mforms(CreateRecordsetViewForWrapper ^ deleg);

      private:
        static MySQL::Base::IRecordsetView ^
          wrap_and_create_recordset_view(IntPtr /* to a std::shared_ptr<Recordset> ptr */ rset);
        static CreateRecordsetViewForWrapper ^ create_recordset_for_wrapper = nullptr;
      };

    }; // namespace Db
  };   // namespace Grt
};     // namespace MySQL
