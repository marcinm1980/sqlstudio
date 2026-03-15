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

#include "studio/wb_backend_public_interface.h"
#include "base/trackable.h"
#include "grt.h"
#include "grtpp_util.h"
#include "grts/structs.model.h"
#include "base/geometry.h"

namespace mdc {
  class CanvasItem;
};

namespace wb {
  class WBContext;
  struct WBOptions;

  class ModelDiagramForm;

  class WBComponent : public base::trackable {
  public:
    WBComponent(WBContext *context);
    virtual ~WBComponent(){};

    inline auto get_wb() -> WBContext * {
      return _wb;
    }

    virtual auto get_name() -> std::string = 0;
    virtual auto get_diagram_class_name() -> std::string {
      return "";
    }

    virtual auto setup_context_grt(WBOptions *options) -> void {
    }
    virtual auto load_app_options(bool update) -> void {
    }
    virtual auto save_app_options() -> void {
    }

    virtual auto close_document() -> void {
    }
    virtual auto reset_document() -> void {
    }
    virtual auto document_loaded() -> void {
    }
    virtual auto block_model_notifications() -> void {
    }
    virtual auto unblock_model_notifications() -> void {
    }

    virtual auto handles_figure(const model_ObjectRef &figure) -> bool = 0;
    virtual auto get_object_for_figure(const model_ObjectRef &figure) -> GrtObjectRef {
      return GrtObjectRef();
    }

    virtual auto can_paste_object(const grt::ObjectRef &object) -> bool {
      return false;
    }
    virtual auto paste_object(ModelDiagramForm *view, const grt::ObjectRef &object,
                                         grt::CopyContext &copy_context) -> model_ObjectRef {
      throw std::logic_error("not implemented");
      return model_ObjectRef();
    }
    virtual auto copy_object_to_clipboard(const grt::ObjectRef &object, grt::CopyContext &copy_context) -> void {
      throw std::logic_error("not implemented");
    }

    // toolbar/menubar handling
    virtual auto get_shortcut_items() -> grt::ListRef<app_ShortcutItem> {
      return grt::ListRef<app_ShortcutItem>();
    }
    virtual auto get_tools_toolbar() -> app_ToolbarRef {
      return app_ToolbarRef();
    }
    virtual auto get_tool_options(const std::string &tool) -> app_ToolbarRef {
      return app_ToolbarRef();
    }

    virtual auto get_command_dropdown_items(const std::string &option) -> std::vector<std::string> {
      return std::vector<std::string>();
    }

    virtual auto get_command_option_value(const std::string &option) -> std::string;
    virtual auto set_command_option_value(const std::string &option, const std::string &item) -> void;

    virtual auto get_object_tooltip(const model_ObjectRef &object, mdc::CanvasItem *item) -> std::string {
      return "";
    }

    // tool handling
    virtual auto setup_canvas_tool(ModelDiagramForm *view, const std::string &tool) -> void = 0;

    virtual auto delete_model_object(const model_ObjectRef &object, bool figure_only) -> bool = 0;

    // drag&
    virtual auto accepts_drop(ModelDiagramForm *view, int x, int y, const std::string &type,
                              const std::list<GrtObjectRef> &objects) -> bool {
      return false;
    }
    virtual auto accepts_drop(ModelDiagramForm *view, int x, int y, const std::string &type, const std::string &text) -> bool {
      return false;
    }

    virtual auto perform_drop(ModelDiagramForm *view, int x, int y, const std::string &type,
                              const std::list<GrtObjectRef> &objects) -> bool {
      return false;
    }
    virtual auto perform_drop(ModelDiagramForm *view, int x, int y, const std::string &type, const std::string &text) -> bool {
      return false;
    }

  protected:
    auto place_object(ModelDiagramForm *view, const base::Point &pos, const std::string &object_struct,
                               const grt::DictRef &args = grt::DictRef()) -> grt::ValueRef;

  public:
    virtual auto activate_canvas_object(const model_ObjectRef &figure, bool newwindow) -> void = 0;

  protected:
    WBContext *_wb;
  };
};
