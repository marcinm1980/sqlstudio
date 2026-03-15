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

#include "studio/wb_backend_public_interface.h"
#include "base/notifications.h"

#include "wbcanvas/model_model_impl.h"

#include <grts/structs.app.h>
#include <grts/structs.model.h>
#include <grts/structs.studio.h>
#include <grts/structs.ui.h>

#define MODEL_DOCKING_POINT "studio.physical.Model:main"

namespace grt {
  class UndoAction;
};

namespace mforms {
  class View;
  class TabView;
  class TreeView;
  class DockingPoint;
};

class TableTemplatePanel;
class UserDefinedTypeEditor;

namespace wb {
  class WBContextUI;
  class ModelDiagramForm;
  class PhysicalOverviewBE;
  class ModelFile;
  class UserDatatypeList;
  class HistoryTree;

  enum CatalogNodeNotificationType { NodeAddUpdate, NodeDelete, NodeUnmark };

  class MYSQLWBBACKEND_PUBLIC_FUNC WBContextModel : public ModelBridgeDelegate, public base::trackable, base::Observer {
  public:
    WBContextModel();
    virtual ~WBContextModel();

    static auto detect_auto_save_files(const std::string &autosave_dir) -> void;
    static auto auto_save_files() -> std::map<std::string, std::string>;
    auto auto_save_document() -> bool;

    auto shared_secondary_sidebar() -> mforms::View *;

  public:
    auto get_overview() -> PhysicalOverviewBE * {
      return _overview;
    }

    auto create_user_type_list() -> mforms::TreeView *;
    auto show_user_type_editor(studio_physical_ModelRef model) -> void;

    auto get_target_version() -> GrtVersionRef;

    auto create_history_tree() -> mforms::TreeView *;

    auto get_active_model_diagram(bool main_form) -> model_DiagramRef;
    auto get_active_model(bool main_form) -> model_ModelRef;

    // return the named toolbar
    auto model_created(ModelFile *file, studio_DocumentRef doc) -> void;
    auto model_loaded(ModelFile *file, studio_DocumentRef doc) -> void;
    auto model_closed() -> void;

    auto register_diagram_form(ModelDiagramForm *view) -> void;

    auto get_diagram_form_for_diagram_id(const std::string &id) -> ModelDiagramForm * {
      return _model_forms.find(id) == _model_forms.end() ? 0 : _model_forms[id];
    }
    auto get_diagram_form(mdc::CanvasView *view) -> ModelDiagramForm *;

    auto notify_diagram_created(ModelDiagramForm *view) -> void;
    auto notify_diagram_destroyed(ModelDiagramForm *view) -> void;

    auto realize() -> void;
    auto unrealize() -> void;

    auto activate_canvas_object(const model_ObjectRef &object, ssize_t flags) -> void;

    auto update_page_settings() -> void;

    auto export_png(const std::string &path) -> void;
    auto export_pdf(const std::string &path) -> void;
    auto export_ps(const std::string &path) -> void;
    auto export_svg(const std::string &path) -> void;
    auto exportPng(const model_DiagramRef &diagram, const std::string &path) -> void;

    // Diagrams
    auto get_view_with_id(const std::string &id) -> model_DiagramRef;

    auto add_new_diagram(const model_ModelRef &model) -> void;

    auto switch_diagram(const model_DiagramRef &view) -> void;

    auto delete_diagram(const model_DiagramRef &view) -> bool;
    auto delete_object(model_ObjectRef object) -> bool;
    auto remove_figure(model_ObjectRef object) -> bool;

    auto duplicate_object(const db_DatabaseObjectRef &object, grt::CopyContext &copy_context) -> GrtObjectRef;
    void notify_catalog_tree_view(const CatalogNodeNotificationType &notify_type, grt::ValueRef value,
                                  const std::string &diagram_id = "");
    auto refill_catalog_tree() -> void;

  public:
    auto update_plugin_arguments_pool(bec::ArgumentPool &args) -> void;

    auto get_object_list_popup_items(bec::UIForm *form, const std::vector<bec::NodeId> &nodes,
                                    const grt::ListRef<GrtObject> &objects, const std::string &label,
                                    const std::list<std::string> &groups, bec::MenuItemList &items) -> int;

    auto begin_plugin_exec() -> void;
    auto end_plugin_exec() -> void;

  public:
    boost::signals2::signal<void()> _udt_list_changed;

  private:
    // delegate functions from ModelBridgeDelegate
    virtual auto fetch_image(const std::string &file) -> cairo_surface_t *;
    virtual auto attach_image(const std::string &file) -> std::string;
    virtual auto release_image(const std::string &name) -> void;

    virtual auto create_diagram(const model_DiagramRef &view) -> mdc::CanvasView *;
    virtual auto free_canvas_view(mdc::CanvasView *view) -> void;

    auto create_diagram_main(const model_DiagramRef &mview) -> mdc::CanvasView *;

    auto update_current_diagram(bec::UIForm *form) -> void;

    auto diagram_object_changed(const std::string &member, const grt::ValueRef &ovalue, ModelDiagramForm *view) -> void;
    auto diagram_object_list_changed(grt::internal::OwnedList *list, bool added, const grt::ValueRef &value,
                                     ModelDiagramForm *vform) -> void;
    auto option_changed(grt::internal::OwnedDict *, bool, const std::string &) -> void;

    auto has_selected_model() -> bool;
    auto has_selected_schema() -> bool;
    auto has_selected_figures() -> bool;
    auto add_model_schema() -> void;
    auto add_model_table() -> void;
    auto add_model_view() -> void;
    auto add_model_rgroup() -> void;
    auto add_model_diagram() -> void;
    auto remove_figure() -> void;

    auto page_settings_changed(const std::string &field, const grt::ValueRef &value) -> void;

    auto add_object_plugins_to_popup_menu(const grt::ListRef<GrtObject> &objects, const std::list<std::string> &groups,
                                         bec::MenuItemList &items) -> int;

    auto history_changed() -> void;
    auto selection_changed() -> void;

    virtual auto handle_notification(const std::string &name, void *sender, base::NotificationInfo &info) -> void;

    auto setup_secondary_sidebar() -> void;

  private:
    PhysicalOverviewBE *_overview;
    ModelFile *_file;
    UserDefinedTypeEditor *_current_user_type_editor;
    mdc::CanvasView *_locked_view_for_plugin_exec;

    ui_ModelPanelRef _grtmodel_panel;
    mforms::TabView *_secondary_sidebar;
    TableTemplatePanel *_template_panel;
    mforms::DockingPoint *_sidebar_dockpoint;

    studio_DocumentRef _doc;
    boost::signals2::connection _page_settings_conn;

    grt::UndoAction *_auto_save_point;
    mdc::Timestamp _last_auto_save_time;
    int _auto_save_interval;
    bec::GRTManager::Timer *_auto_save_timer;

    std::map<std::string, ModelDiagramForm *> _model_forms;
  };
};
