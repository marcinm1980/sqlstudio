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

#include "mdc.h"
#include "grt.h"

#include "grts/structs.model.h"

#include "wbpublic_public_interface.h"

#include "base_bridge.h"

class WBPUBLICBACKEND_PUBLIC_FUNC ModelBridgeDelegate {
public:
  virtual ~ModelBridgeDelegate() {};
  virtual auto create_diagram(const model_DiagramRef &mview) -> mdc::CanvasView * = 0;
  virtual auto free_canvas_view(mdc::CanvasView *view) -> void = 0;

  virtual auto fetch_image(const std::string &file) -> cairo_surface_t * = 0;
  virtual auto attach_image(const std::string &name) -> std::string = 0;
  virtual auto release_image(const std::string &name) -> void = 0;
};

class WBPUBLICBACKEND_PUBLIC_FUNC model_Model::ImplData : public BridgeBase {
  using super = BridgeBase;

protected:
  model_Model *_owner;

  ModelBridgeDelegate *_delegate;
  boost::signals2::signal<void(std::string)> _options_changed_signal;
  bool _reset_pending;
  bool _options_signal_installed;

  auto member_changed(const std::string &name) -> void;

  auto get_app_options_dict() -> grt::DictRef;

  auto option_changed(grt::internal::OwnedDict *dict, bool added, const std::string &option) -> void;

  auto list_changed(grt::internal::OwnedList *list, bool added, const grt::ValueRef &value) -> void;

  virtual auto get_object() -> GrtObject * {
    return _owner;
  }

public:
  ImplData(model_Model *owner);

  //  void add_diagram(const model_DiagramRef &view);
  auto remove_diagram(const model_DiagramRef &view) -> void;

  virtual auto realize() -> bool;
  virtual auto unrealize() -> void;

  auto reset_connections() -> void;
  auto reset_figures() -> void;
  auto reset_layers() -> void;

  auto common_color_for_db_object(const grt::ObjectRef &object, const std::string &member) -> std::string;

  auto update_object_color_in_all_diagrams(const std::string &color, const std::string &object_member,
                                           const std::string &object_id) -> void;

public:
  auto set_delegate(ModelBridgeDelegate *delegate) -> void {
    _delegate = delegate;
  }
  auto get_delegate() -> ModelBridgeDelegate * {
    return _delegate;
  }

  auto get_page_settings() -> app_PageSettingsRef;

  auto get_string_option(const std::string &name, const std::string &defvalue) -> std::string;
  auto get_int_option(const std::string &name, int defvalue) -> int;

  boost::signals2::signal<void(std::string)> *signal_options_changed() {
    return &_options_changed_signal;
  }
};
