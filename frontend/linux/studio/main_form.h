/*
 * Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
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

//!
//!
//! \addtogroup linuxui Linux UI
//! @{
//!

#ifndef __MAIN_FORM_H__
#define __MAIN_FORM_H__

#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/builder.h>

#include "mforms/mforms.h"
#include "mforms/dockingpoint.h"
#include "base/notifications.h"

#include "studio/wb_context.h"

namespace bec {
  class GRTManager;
}

namespace Gtk {
  class Window;
}

class ActiveLabel;
class FormViewBase;
class ModelPanel;
class OverviewPanel;
class ModelDiagramPanel;
class OutputBox;

class FormViewBase;
class PluginEditorBase;

//==============================================================================
//
//==============================================================================
class MainForm : public sigc::trackable, base::Observer, public mforms::DockingPointDelegate {
public:
  enum TabStateInfo { TabClosed, TabOpen, TabOpenActive };

  MainForm();
  ~MainForm();
  auto setup_ui() -> void;
  auto get_mainwindow() const -> Gtk::Window *;

  auto show() -> void;

public:
  typedef sigc::slot<FormViewBase *, std::shared_ptr<bec::UIForm> > FormViewFactory;
  auto register_form_view_factory(const std::string &name, FormViewFactory factory) -> void;

  auto show_status_text_becb(const std::string &text) -> void;
  auto show_progress_becb(const std::string &title, const std::string &status, float pct) -> bool;
  auto open_plugin_becb(grt::Module *module, const std::string &shlib, const std::string &editor_class,
                                grt::BaseListRef args, bec::GUIPluginFlags flags) -> NativeHandle;
  auto show_plugin_becb(NativeHandle handle) -> void;
  auto hide_plugin_becb(NativeHandle handle) -> void;
  auto perform_command_becb(const std::string &command) -> void;
  // Creates diagram view
  auto create_view_becb(const model_DiagramRef &) -> mdc::CanvasView *;
  auto destroy_view_becb(mdc::CanvasView *view) -> void;
  auto switched_view_becb(mdc::CanvasView *view) -> void;
  auto tool_changed_becb(mdc::CanvasView *view) -> void;
  auto refresh_gui_becb(wb::RefreshType type, const std::string &arg_id, NativeHandle arg_ptr) -> void;
  auto lock_gui_becb(bool lock) -> void;
  auto create_main_form_view_becb(const std::string &name, std::shared_ptr<bec::UIForm> form) -> void;
  auto destroy_main_form_view_becb(bec::UIForm *form) -> void;
  auto quit_app_becb() -> bool;

  auto exiting() -> void {
    _exiting = true;
  }

private:
  std::map<mdc::CanvasView *, ModelDiagramPanel *> _diagram_panel_list;

  virtual auto handle_notification(const std::string &name, void *sender, base::NotificationInfo &info) -> void;

  auto register_commands() -> void;
  auto get_panel_for_view(mdc::CanvasView *view) -> ModelDiagramPanel *;

  auto get_active_pane() -> FormViewBase *;

  //  void add_model_pane(ModelDiagramPanel *pane);
  //
  auto add_form_pane(FormViewBase *pane, TabStateInfo tabState) -> void;

  auto add_plugin_form(PluginEditorBase *frame) -> void;

  auto add_plugin_pane(PluginEditorBase *editor) -> void;
  auto bring_plugin_pane(PluginEditorBase *editor) -> void;
  //    void remove_plugin_pane(PluginEditorBase *editor);
  //

  auto close_active_tab() -> void;
  auto close_main_tab() -> void;
  auto close_inner_tab() -> void;

  auto close_tab(Gtk::Notebook *note, Gtk::Widget *widget) -> bool;
  auto append_tab_page(Gtk::Notebook *note, Gtk::Widget *widget, const std::string &title, TabStateInfo tabState,
                       ActiveLabel **title_label_ret = 0) -> void;

  //  void init_tab_menu(mforms::Menu* m);
  auto init_tab_menu(Gtk::Widget *widget) -> mforms::Menu *;
  auto tab_menu_handler(const std::string &action, ActiveLabel *label, Gtk::Widget *widget, Gtk::Notebook *note) -> void;

  auto show_output() -> void;
  auto show_diagram_options() -> void;
  auto show_page_setup() -> void;

  auto handle_model_created() -> void;
  auto handle_model_closed() -> void;

  auto close_window(GdkEventAny *ev) -> bool;
  auto on_focus_widget(Gtk::Widget *focus) -> void;
  auto on_configure_window(GdkEventConfigure *conf) -> void;
  auto on_window_state(GdkEventWindowState *conf) -> void;
  auto is_active_changed() -> void;

  auto prepare_close_document() -> void;

  auto update_timer() -> void;
  auto fire_timer() -> bool;

  // command handlers
  auto reset_layout() -> void;

  auto switch_page(Gtk::Widget *page, guint pagenum) -> void;

  auto get_upper_note() const -> Gtk::Notebook *;

  auto call_find_replace() -> void;
  auto call_find() -> void;
  auto call_undo() -> void;
  auto call_redo() -> void;
  auto call_paste() -> void;
  auto call_cut() -> void;
  auto call_copy() -> void;
  auto call_select_all() -> void;
  auto call_delete() -> void;
  auto call_search() -> void;

  auto validate_find_replace() -> bool;
  auto validate_find() -> bool;
  auto validate_undo() -> bool;
  auto validate_redo() -> bool;
  auto validate_copy() -> bool;
  auto validate_cut() -> bool;
  auto validate_paste() -> bool;
  auto validate_select_all() -> bool;
  auto validate_delete() -> bool;
  auto validate_search() -> bool;

private:
  // mforms integration
  auto setup_mforms_app() -> void;

  virtual auto get_type() -> std::string {
    return "MainWindow";
  }
  virtual auto set_name(const std::string &name) -> void;
  virtual auto dock_view(mforms::AppView *view, const std::string &position, int arg) -> void;
  virtual auto select_view(mforms::AppView *view) -> bool;
  virtual auto undock_view(mforms::AppView *view) -> void;
  virtual auto get_size() -> std::pair<int, int>;
  virtual auto set_view_title(mforms::AppView *view, const std::string &title) -> void;
  virtual auto selected_view() -> mforms::AppView *;
  virtual auto view_count() -> int;
  virtual auto view_at_index(int index) -> mforms::AppView *;

  static auto set_status_text(mforms::App *app, const std::string &text) -> void;

  //@@@  bool find_callback(const std::string &search, const std::string &replace,
  //                     mforms::SearchFlags flags, SqlEditorFE *editor);

  auto decorate_widget(Gtk::Widget *panel, bec::UIForm *form) -> Gtk::Widget *;

private:
  std::map<std::string, FormViewFactory> _form_view_factories;

  ModelPanel *_model_panel;
  OverviewPanel *_model_overview; //!< Overview of the model, see overview_panel.h

  OutputBox *_output_box;

  Glib::RefPtr<Gtk::Builder> _ui; //!< Glade model wrapper of the main window
  const char *_db_glade_file;     //!< File name of the glade model of the model overview part

  bool _gui_locked;
  bool _exiting;
  Gtk::ProgressBar _progress_bar;

  sigc::signal<void, std::string> _signal_close_editor;

  sigc::connection _sig_change_status;
  sigc::connection _sig_flush_idle;
  sigc::connection _sig_set_current_page;
  sigc::connection _sig_close_tab;

  typedef std::vector<sigc::slot_base> Slots;
  Slots _slots;

  template <typename R, typename T>
  sigc::slot<R> make_slot(R (T::*t)()) {
    const sigc::slot<R> slot = sigc::mem_fun(this, t);
    _slots.push_back(slot);
    return slot;
  }
};

#endif
