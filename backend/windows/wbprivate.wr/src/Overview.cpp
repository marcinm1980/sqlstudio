/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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
#include "studio/wb_overview.h"
#include "ConvUtils.h"
#include "GrtTemplates.h"
#include "Overview.h"

namespace MySQL {
  namespace MySqlStudio {

    MySQL::Base::UIForm ^ Overview::get_uiform() {
      return uiform;
    }

    System::Collections::Generic::List<::MySQL::Base::ToolbarItem ^> ^
      Overview::get_toolbar_items(MySQL::Grt::NodeIdWrapper ^ node) {
      bec::ToolbarItemList items = get_unmanaged_object()->get_toolbar_items(*node->get_unmanaged_object());
      return MySQL::Grt::CppVectorToObjectList<::bec::ToolbarItem, ::MySQL::Base::ToolbarItem>(items);
    }

  } // namespace MySqlStudio
} // namespace MySQL
