/*
 * Copyright (c) 2010, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "wbpublic_public_interface.h"

#include "mforms/box.h"
#include "mforms/form.h"
#include "mforms/tabview.h"
#include "mforms/label.h"
#include "mforms/button.h"
#include "mforms/jsonview.h"

namespace bec {
  class GRTManager;
};

class BinaryDataEditor;

class WBPUBLICBACKEND_PUBLIC_FUNC BinaryDataViewer : public mforms::Box {
public:
  BinaryDataViewer(BinaryDataEditor *owner);

  virtual auto data_changed() -> void = 0;

protected:
  BinaryDataEditor *_owner;
};

class WBPUBLICBACKEND_PUBLIC_FUNC BinaryDataEditor : public mforms::Form {
public:
  BinaryDataEditor(const char *data, size_t length, bool read_only = true);
  BinaryDataEditor(const char *data, size_t length, const std::string &text_encoding, const std::string &data_type,
                   bool read_only = true);
  virtual ~BinaryDataEditor();

  auto data() const -> const char * {
    return _data;
  }
  auto length() const -> size_t {
    return _length;
  }

  // when user clicks Save
  boost::signals2::signal<void()> signal_saved;

public:
  auto add_viewer(BinaryDataViewer *viewer, const std::string &title) -> void;
  auto add_json_viewer(bool read_only, const std::string &text_encoding, const std::string &title) -> void;

  auto assign_data(const char *data, size_t length, bool steal_pointer = false) -> void;
  auto notify_edit() -> void;

  auto read_only() -> bool {
    return _read_only;
  }
  auto isJson() -> bool {
    return _type == "JSON";
  }

protected:
  char *_data;
  size_t _length;
  std::string _type;

  std::vector<BinaryDataViewer *> _viewers;
  std::set<BinaryDataViewer *> _pendingUpdates;
  bool _updating;

  mforms::Box _box;
  mforms::Box _hbox;
  mforms::TabView _tab_view;
  mforms::Label _length_text;
  mforms::Button _save;
  mforms::Button _close;
  mforms::Button _export;
  mforms::Button _import;

  bool _read_only;

  auto setup() -> void;
  auto save() -> void;
  auto tab_changed() -> void;

  auto import_value() -> void;
  auto export_value() -> void;
};
