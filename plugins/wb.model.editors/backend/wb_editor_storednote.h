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

#pragma once

#include "wb_editor_backend_public_interface.h"
#include "grt/editor_base.h"
#include "sqlide/sql_editor_be.h"
#include "grts/structs.studio.model.h"
#include <memory>

namespace mforms {
  class ToolBarItem;
};

class WBEDITOR_BACKEND_PUBLIC_FUNC StoredNoteEditorBE : public bec::BaseEditor {
  GrtStoredNoteRef _note;

public:
  StoredNoteEditorBE(const GrtStoredNoteRef &note);

  auto is_script() -> bool;

  virtual auto get_sql_editor() -> MySQLEditor::Ref;

  auto set_name(const std::string &name) -> void;
  auto get_name() -> std::string;

  virtual auto get_title() -> std::string;

  auto load_text() -> void;
  virtual auto commit_changes() -> void;
  virtual auto has_editor() -> bool {
    return true;
  }

protected:
  MySQLEditor::Ref _sql_editor;

  auto set_text(grt::StringRef ext) -> void;
  auto get_text(bool &isutf8) -> grt::StringRef;

  auto changed_selector(mforms::ToolBarItem *item) -> void;
};
