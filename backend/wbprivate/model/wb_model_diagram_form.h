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

#ifndef _WB_MODEL_DIAGRAM_FORM_H_
#define _WB_MODEL_DIAGRAM_FORM_H_

#include "base/ui_form.h"
#include "base/notifications.h"

#include "studio/wb_command_ui.h"
#include "grt/icon_manager.h"
#include "mini_view.h"

#include "mforms/menu.h"
#include "mforms/toolbar.h"
#include "wb_catalog_tree_view.h"
#include "wb_context_model.h"

namespace mforms {
  class ToolBarItem;
  class TreeView;
}; // namespace mforms

namespace wb {
  class Floater;
  class WBContext;
  class WBComponent;

  enum EditFinishReason { EditCancelled, EditReturnPressed, EditTabPressed, EditShiftTabPressed };

  using ChangeSignal = boost::signals2::signal<void(const std::string &, const grt::ValueRef &)>;

  class ModelDiagramForm;
  class PhysicalModelDiagramFeatures;
  class LayerTree;

  class InlineEditContext {
    boost::signals2::signal<void(std::string, EditFinishReason)> _signal_edit_finished;

  public:
    virtual ~InlineEditContext() {
    }

    virtual auto begin_editing(int x, int y, int width, int height, const std::string &text) -> void = 0;
    virtual auto end_editing() -> void = 0;

    virtual auto set_font_size(float size) -> void = 0;
    virtual auto set_multiline(bool flag) -> void = 0;

    boost::signals2::signal<void(std::string, EditFinishReason)> *signal_edit_finished() {
      return &_signal_edit_finished;
    }
  };

  class UpdateLock;

  class MYSQLWBBACKEND_PUBLIC_FUNC ModelDiagramForm : public bec::UIForm, public base::Observer {
    friend class UpdateLock;

  public:
    ModelDiagramForm(WBComponent *owner, const model_DiagramRef &view);
    virtual ~ModelDiagramForm();

    auto attach_canvas_view(mdc::CanvasView *cview) -> void;

    virtual auto is_main_form() -> bool {
      return true;
    }
    virtual auto get_form_context_name() const -> std::string;

    auto get_view() -> mdc::CanvasView * {
      return _view;
    }
    auto get_model_diagram() -> model_DiagramRef & {
      return _model_diagram;
    }

    auto get_model_options() -> grt::DictRef {
      return _model_diagram->owner()->options();
    }
    auto get_diagram_options() -> grt::DictRef {
      return _model_diagram->options();
    }
    auto get_catalog_tree() -> CatalogTreeView *;
    auto notify_catalog_tree(const wb::CatalogNodeNotificationType &notify_type, grt::ValueRef value) -> void;
    auto refill_catalog_tree() -> void;

    auto set_closed(bool flag) -> void;
    auto is_closed() -> bool;
    virtual auto close() -> void;

    auto get_leaf_item_at(const base::Point &pos) -> mdc::CanvasItem *;

    auto get_clipboard() -> bec::Clipboard *;

    auto get_wb() -> WBContext *;

    virtual auto get_title() -> std::string;

    virtual auto can_undo() -> bool;
    virtual auto can_redo() -> bool;
    virtual auto can_copy() -> bool;
    virtual auto can_paste() -> bool;
    virtual auto can_delete() -> bool;
    virtual auto can_select_all() -> bool;

    virtual auto undo() -> void;
    virtual auto redo() -> void;
    virtual auto cut() -> void;
    virtual auto copy() -> void;
    virtual auto paste() -> void;
    virtual auto delete_selection() -> void;
    virtual auto select_all() -> void;

    auto remove_selection(bool deleteSelection = false) -> void;

    virtual auto get_edit_target_name() -> std::string;

    auto get_diagram_info_text() -> std::string;

    auto get_accepted_drop_types() -> std::vector<std::string>;

    auto get_selection() -> grt::ListRef<model_Object>;
    auto get_copiable_selection() -> grt::ListRef<model_Object>;
    auto has_selection() -> bool;

    auto get_zoom() -> double;
    auto set_zoom(double zoom) -> void;
    auto zoom_in() -> void;
    auto zoom_out() -> void;

    auto set_button_callback(
      const std::function<bool(ModelDiagramForm *, mdc::MouseButton, bool, base::Point, mdc::EventState)> &cb) -> void;
    auto set_motion_callback(const std::function<bool(ModelDiagramForm *, base::Point, mdc::EventState)> &cb) -> void;
    auto set_reset_tool_callback(const std::function<void(ModelDiagramForm *)> &cb) -> void;

    auto get_tool() -> std::string {
      return _tool;
    }
    auto set_tool(std::string tool) -> void;
    auto reset_tool(bool notify) -> void;
    auto set_tool_argument(const std::string &option, const std::string &value) -> void;
    auto get_tool_argument(const std::string &option) -> std::string;

    auto is_visible(const model_ObjectRef &object, bool partially) -> bool;
    auto focus_and_make_visible(const model_ObjectRef &object, bool select) -> void;

    auto search_and_focus_object(const std::string &text) -> bool;

    auto set_cursor(const std::string &cursor) -> void;
    inline auto get_cursor() -> const std::string & {
      return _cursor;
    }

    // sidebar
    auto get_layer_tree() -> mforms::TreeView *;
    auto get_mini_view() -> MiniView * {
      return _mini_view;
    }

    auto setup_mini_view(mdc::CanvasView *view) -> void;
    auto update_mini_view_size(int w, int h) -> void;
    auto setBackgroundColor(base::Color const &color) -> void;

    // events
    auto handle_mouse_move(int x, int y, mdc::EventState state) -> void;
    auto handle_mouse_button(mdc::MouseButton button, bool press, int x, int y, mdc::EventState state) -> void;
    auto handle_mouse_double_click(mdc::MouseButton button, int x, int y, mdc::EventState state) -> void;
    auto handle_mouse_leave(int x, int y, mdc::EventState state) -> void;
    auto handle_key(const mdc::KeyInfo &key, bool press, mdc::EventState state) -> bool;

    auto current_mouse_position(int &x, int &y) -> bool;
    auto current_mouse_position(base::Point &pos) -> bool;

    // drag&drop
    auto accepts_drop(int x, int y, const std::string &type, const std::list<GrtObjectRef> &objects) -> bool;
    auto accepts_drop(int x, int y, const std::string &type, const std::string &text) -> bool;

    auto perform_drop(int x, int y, const std::string &type, const std::list<GrtObjectRef> &objects) -> bool;
    auto perform_drop(int x, int y, const std::string &type, const std::string &text) -> bool;

    auto get_layer_at(const base::Point &pos, base::Point &offset) -> model_LayerRef;
    auto get_layer_bounding(const base::Rect &rect, base::Point &offset) -> model_LayerRef;
    auto get_object_at(const base::Point &pos) -> model_ObjectRef;

    auto get_floater_layer() -> mdc::Layer *;
    auto add_floater(Floater *floater) -> void;

    auto enable_panning(bool flag) -> void;
    auto enable_zoom_click(bool enable, bool zoomin) -> void;

    boost::signals2::signal<void(std::string)> *signal_tool_argument_changed() {
      return &_tool_argument_changed;
    }

    auto get_owner() -> WBComponent * {
      return _owner;
    }

    auto get_highlight_fks() -> bool {
      return _highlight_fks;
    }
    auto set_highlight_fks(bool flag) -> void;

    // inline editing
    auto begin_editing(const base::Rect &rect, const std::string &text, float text_size, bool multiline) -> void;
    auto stop_editing() -> void;
    boost::signals2::signal<void(std::string, EditFinishReason)> *signal_editing_done() {
      return &_signal_editing_done;
    }

    auto set_inline_editor_context(InlineEditContext *context) -> void;

    virtual auto get_toolbar() -> mforms::ToolBar *;
    auto get_tools_toolbar() -> mforms::ToolBar *;
    auto get_options_toolbar() -> mforms::ToolBar *;
    auto update_options_toolbar() -> void;

    virtual auto get_menubar() -> mforms::MenuBar *;
    auto revalidate_menu() -> void;

  protected:
    struct OldPosition {
      base::Point pos;
      std::string layer_id;
    };
    CatalogTreeView *_catalog_tree;
    mdc::CanvasView *_view;
    mdc::Layer *_main_layer;
    mdc::Layer *_floater_layer;
    mdc::Layer *_badge_layer;
    WBComponent *_owner;
    model_DiagramRef _model_diagram;
    int _current_mouse_x;
    int _current_mouse_y;
    std::string _tool;
    std::string _cursor;
    std::map<std::string, std::string> _tool_args;
    std::vector<WBShortcut> _shortcuts;

    LayerTree *_layer_tree;
    MiniView *_mini_view;

    PhysicalModelDiagramFeatures *_features;
    boost::signals2::connection _idle_node_mark;

    std::map<grt::internal::Value *, OldPosition> _old_positions;
    InlineEditContext *_inline_edit_context;
    boost::signals2::signal<void(std::string, EditFinishReason)> _signal_editing_done;

    double _paste_offset;

    mforms::MenuBar *_menu;
    mforms::ToolBar *_toolbar;
    mforms::ToolBar *_tools_toolbar;
    mforms::ToolBar *_options_toolbar;

    boost::signals2::signal<void(std::string)> _tool_argument_changed;

    std::function<bool(ModelDiagramForm *, mdc::MouseButton, bool, base::Point, mdc::EventState)> _handle_button;
    std::function<bool(ModelDiagramForm *, base::Point, mdc::EventState)> _handle_motion;
    std::function<void(ModelDiagramForm *)> _reset_tool;

    bool _drag_panning;
    bool _space_panning;

    bool _highlight_fks;

    // saved state for tmp panning
    std::string _old_tool;
    std::string _old_cursor;
    std::function<void(ModelDiagramForm *)> _old_reset_tool;
    std::function<bool(ModelDiagramForm *, mdc::MouseButton, bool, base::Point, mdc::EventState)> _old_handle_button;
    std::function<bool(ModelDiagramForm *, base::Point, mdc::EventState)> _old_handle_motion;

    auto handle_notification(const std::string &name, void *sender, base::NotificationInfo &info) -> void;
    auto update_toolbar_icons() -> void;
    auto clipboard_changed() -> void;

    auto relocate_figures() -> bool;

    auto begin_selection_drag() -> void;
    auto end_selection_drag() -> void;

    auto diagram_changed(grt::internal::OwnedList *, bool, const grt::ValueRef &) -> void;

    auto mark_catalog_node(grt::ValueRef val, bool mark) -> void;

    auto selection_changed() -> void;

    auto get_dropdown_items(const std::string &name, const std::string &option,
                                                std::string &selected) -> std::vector<std::string>;
    auto select_dropdown_item(const std::string &option, mforms::ToolBarItem *item) -> void;
    auto toggle_checkbox_item(const std::string &name, const std::string &option, bool state) -> void;

    auto activate_catalog_tree_item(const grt::ValueRef &value) -> void;

  private:
    int _update_count; // If > 0 don't refresh depending structures.
    mforms::Menu _context_menu;

    // Local class.
    class UpdateLock {
    private:
      ModelDiagramForm *_form;

    public:
      UpdateLock(ModelDiagramForm *form) {
        _form = form;
        _form->_update_count++;
      };
      ~UpdateLock();
    };
  };
}; // namespace wb

#endif
