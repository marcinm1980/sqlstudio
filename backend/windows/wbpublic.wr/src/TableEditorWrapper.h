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

#include "DBObjectEditorWrapper.h"
#include "GrtTemplates.h"
#include "grtdb/editor_table.h"
#include "recordset_wr.h"

namespace MySQL {
  namespace Grt {
    namespace Db {

      ref class TableEditorWrapper;
      ref class IndexListWrapper;
      ref class FKConstraintListWrapper;

    public
      ref class TableColumnsListWrapper : public MySQL::Grt::ListModelWrapper {
      public:
        enum class ColumnListColumns {
          Name = bec::TableColumnsListBE::Name,
          Type = bec::TableColumnsListBE::Type,
          IsPK = bec::TableColumnsListBE::IsPK,
          IsNotNull = bec::TableColumnsListBE::IsNotNull,
          IsUnique = bec::TableColumnsListBE::IsUnique,
          IsBinary = bec::TableColumnsListBE::IsBinary,
          IsUnsigned = bec::TableColumnsListBE::IsUnsigned,
          IsZerofill = bec::TableColumnsListBE::IsZerofill,
          Flags = bec::TableColumnsListBE::Flags,
          Default = bec::TableColumnsListBE::Default,
          Charset = bec::TableColumnsListBE::Charset,
          Collation = bec::TableColumnsListBE::Collation,
          HasCharset = bec::TableColumnsListBE::HasCharset,
          Comment = bec::TableColumnsListBE::Comment,
          LastColumn = bec::TableColumnsListBE::LastColumn
        };

        TableColumnsListWrapper(bec::TableColumnsListBE *inn);

        auto get_datatype_names() -> List<String ^> ^ { return CppStringListToNative(get_unmanaged_object()->get_datatype_names()); }

          inline auto get_unmanaged_object() -> bec::TableColumnsListBE * {
          return static_cast<bec::TableColumnsListBE *>(inner);
        }

        void reorder_many(List<int> ^ rows, int nindex);
      };

    public
      ref class IndexColumnsListWrapper : public ListModelWrapper {
      public:
        enum class IndexColumnsListColumns {
          Name = bec::IndexColumnsListBE::Name,
          Descending = bec::IndexColumnsListBE::Descending,
          Length = bec::IndexColumnsListBE::Length,
          OrderIndex = bec::IndexColumnsListBE::OrderIndex
        };

        IndexColumnsListWrapper(IndexListWrapper ^ owner);

        IndexColumnsListWrapper(bec::IndexColumnsListBE *inn);

        inline auto get_unmanaged_object() -> bec::IndexColumnsListBE * {
          return static_cast<bec::IndexColumnsListBE *>(inner);
        }

        void set_column_enabled(NodeIdWrapper ^ node, bool flag);
        bool get_column_enabled(NodeIdWrapper ^ node);

        auto get_max_order_index() -> int;
      };

    public
      ref class IndexListWrapper : public ListModelWrapper {
      public:
        enum class IndexListColumns {
          Name = bec::IndexListBE::Name,
          Type = bec::IndexListBE::Type,
          Comment = bec::IndexListBE::Comment
        };

        IndexListWrapper(TableEditorWrapper ^ owner);

        IndexListWrapper(bec::IndexListBE *inn);

        inline auto get_unmanaged_object() -> bec::IndexListBE * {
          return static_cast<bec::IndexListBE *>(inner);
        }

        auto get_columns() -> IndexColumnsListWrapper ^;

        // db_Index get_selected_index();
        void select_index(NodeIdWrapper ^ node);

        // cannot create a wrapper instance here
        // TableEditorWrapper *get_owner() { return _owner; }
      };

    public
      ref class FKConstraintColumnsListWrapper : public ListModelWrapper {
      public:
        enum class FKConstraintColumnsListColumns {
          Enabled = bec::FKConstraintColumnsListBE::Enabled,
          Column = bec::FKConstraintColumnsListBE::Column,
          RefColumn = bec::FKConstraintColumnsListBE::RefColumn
        };

        FKConstraintColumnsListWrapper(FKConstraintListWrapper ^ owner);

        FKConstraintColumnsListWrapper(bec::FKConstraintColumnsListBE *inn);

        inline auto get_unmanaged_object() -> bec::FKConstraintColumnsListBE * {
          return static_cast<bec::FKConstraintColumnsListBE *>(inner);
        }

        List<String ^> ^ get_ref_columns_list(NodeIdWrapper ^ node, bool filtered);

        bool set_column_is_fk(NodeIdWrapper ^ node, bool flag);
        bool get_column_is_fk(NodeIdWrapper ^ node);
      };

    public
      ref class FKConstraintListWrapper : public ListModelWrapper {
      public:
        enum class FKConstraintListColumns {
          Name = bec::FKConstraintListBE::Name,
          OnDelete = bec::FKConstraintListBE::OnDelete,
          OnUpdate = bec::FKConstraintListBE::OnUpdate,
          RefTable = bec::FKConstraintListBE::RefTable,
          Comment = bec::FKConstraintListBE::Comment,
          Index = bec::FKConstraintListBE::Index,
          ModelOnly = bec::FKConstraintListBE::ModelOnly,
        };

        FKConstraintListWrapper(TableEditorWrapper ^ owner);

        FKConstraintListWrapper(bec::FKConstraintListBE *inn);

        inline auto get_unmanaged_object() -> bec::FKConstraintListBE * {
          return static_cast<bec::FKConstraintListBE *>(inner);
        }

        void select_fk(NodeIdWrapper ^ node);

        auto get_columns() -> FKConstraintColumnsListWrapper ^;
      };

    public
      ref class TableEditorWrapper : public DBObjectEditorWrapper {
      protected:
        TableEditorWrapper(bec::TableEditorBE *inn) : DBObjectEditorWrapper(inn) {
        }

      public:
        enum class PartialRefreshes {
          RefreshColumnMoveUp = bec::TableEditorBE::RefreshColumnMoveUp,
          RefreshColumnMoveDown = bec::TableEditorBE::RefreshColumnMoveDown,
          RefreshColumnList = bec::TableEditorBE::RefreshColumnList,
          RefreshColumnCollation = bec::TableEditorBE::RefreshColumnCollation,
        };

        auto get_unmanaged_object() -> bec::TableEditorBE * {
          return static_cast<bec::TableEditorBE *>(inner);
        }

        auto get_indexes() -> IndexListWrapper ^;

        auto get_fks() -> FKConstraintListWrapper ^;

        auto get_inserts_panel() -> Control ^;

        // table options
        //...

        // column editing
        NodeIdWrapper ^ add_column(String ^ name);

        void remove_column(NodeIdWrapper ^ column);

        // db_Column get_column_with_name(const std::string &name);

        // fk editing
        NodeIdWrapper ^ add_fk(String ^ name);

        void remove_fk(NodeIdWrapper ^ fk);

        NodeIdWrapper ^ add_fk_with_columns(List<NodeIdWrapper ^> ^ columns);

        // index editing
        NodeIdWrapper ^ add_index(String ^ name);

        void remove_index(NodeIdWrapper ^ index);

        NodeIdWrapper ^ add_index_with_columns(List<NodeIdWrapper ^> ^ columns);

        auto get_index_types() -> List<String ^> ^;
      };

    } // namespace Db
  }   // namespace Grt
} // namespace MySQL
