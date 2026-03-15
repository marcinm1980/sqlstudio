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

#include "studio/wb_context_ui.h"
#include "model/wb_model_diagram_form.h"
#include "model/wb_context_model.h"
#include "model/wb_history_tree.h"
#include "model/wb_diagram_options.h"
#include "model/wb_user_datatypes.h"
#include "model/wb_overview_physical.h"

#include "common/preferences_form.h"
#include "common/document_properties_form.h"

#include "GrtTemplates.h"
#include "WbCallbacks.h"
#include "DelegateWrapper.h"
#include "Overview.h"
#include "ModelDiagramFormWrapper.h"

namespace MySQL {
  namespace MySqlStudio {

  public
    enum class Msg_type {
      MT_warning = grt::WarningMsg,
      MT_error = grt::ErrorMsg,
      MT_info = grt::InfoMsg,
      MT_progress = grt::ProgressMsg,
    };

    //--------------------------------------------------------------------------------------------------

  public
    ref class PageSettings {
    public:
      PageSettings()
        : paper_type(nullptr), margin_top(0), margin_bottom(0), margin_left(0), margin_right(0), orientation(nullptr) {
      }

      PageSettings(const app_PageSettingsRef& settings)
        : paper_type(CppStringToNative(settings->paperType().is_valid() ? settings->paperType()->name() : "")),
          margin_top(settings->marginTop()),
          margin_bottom(settings->marginBottom()),
          margin_left(settings->marginLeft()),
          margin_right(settings->marginRight()),
          orientation(CppStringToNative(settings->orientation())) {
      }

      auto update_object(grt::ListRef<app_PaperType> paperTypes, app_PageSettingsRef settings) -> void {
        settings->paperType(grt::find_named_object_in_list(paperTypes, NativeToCppString(paper_type)));

        settings->marginTop(grt::DoubleRef(margin_top));
        settings->marginBottom(grt::DoubleRef(margin_bottom));
        settings->marginLeft(grt::DoubleRef(margin_left));
        settings->marginRight(grt::DoubleRef(margin_right));

        settings->orientation(NativeToCppString(orientation));
      }

      String ^ paper_type;
      double margin_top;
      double margin_bottom;
      double margin_left;
      double margin_right;
      String ^ orientation;
    };

  public
    ref class PaperSize {
    public:
      PaperSize(const wb::WBPaperSize& paperSize)
        : name(CppStringToNative(paperSize.name)),
          caption(CppStringToNative(paperSize.caption)),
          margins_set(paperSize.margins_set),
          margin_top(paperSize.margin_top),
          margin_bottom(paperSize.margin_bottom),
          margin_left(paperSize.margin_left),
          margin_right(paperSize.margin_right),
          width(paperSize.width),
          height(paperSize.height),
          description(CppStringToNative(paperSize.description)) {
      }

      String ^ name;
      String ^ caption;
      double width;
      double height;
      bool margins_set;
      double margin_top;
      double margin_bottom;
      double margin_left;
      double margin_right;
      String ^ description;
    };

    // ----------------------------------------------------------------------------

  public
    ref class WbOptions {
    private:
      wb::WBOptions* inner;

    public:
      WbOptions(String ^ baseDir, String ^ userDir, bool full_init);

      auto get_unmanaged_object() -> wb::WBOptions* {
        return inner;
      };
      bool parse_args(array<String ^> ^ args, String ^ app_path);

      auto analyzeCommandLineArguments() -> void;

      auto get() -> property bool Verbose { bool {
          return !inner->verbose;
        }
      }

      auto get() -> property String ^ OpenAtStartup { String ^; }

        auto get() -> property String ^ OpenAtStartupType { String ^; }
    };

    class WbContextUiHolder {
      std::shared_ptr<wb::WBContextUI> _wbCtxUi;

    public:
      WbContextUiHolder() : _wbCtxUi(wb::WBContextUI::get()) {
      }
    };

  public
    ref class WbContext {
      WbContextUiHolder* _wbContextUi;

      MySQL::Grt::GrtManager ^ manager;
      MySQL::MySqlStudio::Overview ^ physical_overview;

      System::Collections::ArrayList open_editor_slot_wrappers;

      DelegateSlot0<void, void> ^ undo_delegate;
      DelegateSlot0<bool, bool> ^ can_undo_delegate;
      DelegateSlot0<void, void> ^ redo_delegate;
      DelegateSlot0<bool, bool> ^ can_redo_delegate;
      DelegateSlot0<void, void> ^ copy_delegate;
      DelegateSlot0<bool, bool> ^ can_copy_delegate;
      DelegateSlot0<void, void> ^ cut_delegate;
      DelegateSlot0<bool, bool> ^ can_cut_delegate;
      DelegateSlot0<void, void> ^ paste_delegate;
      DelegateSlot0<bool, bool> ^ can_paste_delegate;
      DelegateSlot0<void, void> ^ select_all_delegate;
      DelegateSlot0<bool, bool> ^ can_select_all_delegate;
      DelegateSlot0<void, void> ^ delete_delegate;
      DelegateSlot0<bool, bool> ^ can_delete_delegate;
      DelegateSlot0<void, void> ^ find_delegate;
      DelegateSlot0<bool, bool> ^ can_find_delegate;
      DelegateSlot0<void, void> ^ find_replace_delegate;
      DelegateSlot0<bool, bool> ^ can_find_replace_delegate;

    public:
      delegate void VoidStrUIFormDelegate(String ^ str1, MySQL::Base::UIForm ^ form);

    private:
      // void (string, bec::UIFrom)
      [UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)] delegate void VoidStrUIFormWrapperDelegate(
        const std::string& str1, std::shared_ptr<bec::UIForm> form);
      typedef void (*WbContext::VOID_STR_UIFORM_CB)(const std::string& str1, std::shared_ptr<bec::UIForm> form);

      // TODO: implement differently, simpler!
      // Creating these views needs a lot of specialized knowledge, which requires to include many heavy-weight
      // classes (e.g. SQL editor). This should not be handled by a general callback but by the classes
      // that need to create a view (can't the wrapper create the frontend view class?).
      VoidStrUIFormDelegate ^ create_main_form_view_delegate;
      VoidStrUIFormWrapperDelegate ^ create_main_form_view_wrapper_delegate;
      void set_create_main_form_view(MySQL::MySqlStudio::WbFrontendCallbacks ^ cbacks, VoidStrUIFormDelegate ^ dt);
      auto create_main_form_view_wrapper(const std::string& view_name, std::shared_ptr<bec::UIForm> form_be) -> void;

    public:
      WbContext(bool verbose);
      ~WbContext();

      auto is_commercial() -> bool {
        return wb::WBContextUI::get()->get_wb()->is_commercial();
      };

      bool init(MySQL::MySqlStudio::WbFrontendCallbacks ^ callbacks, WbOptions ^ options,
                VoidStrUIFormDelegate ^ create_main_form_view);

      auto opengl_rendering_enforced() -> bool {
        return wb::WBContextUI::get()->get_wb()->opengl_rendering_enforced();
      }
      auto software_rendering_enforced() -> bool {
        return wb::WBContextUI::get()->get_wb()->software_rendering_enforced();
      }
      auto is_busy() -> bool {
        return bec::GRTManager::get()->get_dispatcher()->get_busy();
      }
      auto request_quit() -> bool {
        return wb::WBContextUI::get()->request_quit();
      }
      auto perform_quit() -> void {
        wb::WBContextUI::get()->perform_quit();
      }
      auto is_quitting() -> bool {
        return wb::WBContextUI::get()->is_quitting();
      }
      auto finalize() -> void {
        wb::WBContextUI::get()->finalize();
      }

      auto get_grt_manager() -> GrtManager ^;

      System::Windows::Forms::MenuStrip ^ menu_for_form(MySQL::Base::UIForm ^ form);
      System::Windows::Forms::MenuStrip ^ menu_for_appview(MySQL::Forms::AppViewDockContent ^ content);
      void validate_menu_for_form(MySQL::Base::UIForm ^ form);
      System::Windows::Forms::ToolStrip ^ toolbar_for_form(MySQL::Base::UIForm ^ form);
      auto shared_secondary_sidebar() -> System::Windows::Forms::Control ^;

      void focus_search_box(MySQL::Base::UIForm ^ form);
      String ^ get_search_string(MySQL::Base::UIForm ^ form);
      auto get_title() -> String ^;
      auto has_unsaved_changes() -> bool;
      void open_document(String ^ file);
      auto save_changes() -> bool;
      auto flush_idle_tasks(bool force) -> void;
      auto delay_for_next_timer() -> double;
      auto flush_timers() -> void;

      // ----- Edit menu handling
      auto validate_edit_menu() -> void {
        wb::WBContextUI::get()->get_command_ui()->revalidate_edit_menu_items();
      }

      auto edit_undo() -> void {
        if (wb::WBContextUI::get()->get_active_main_form())
          wb::WBContextUI::get()->get_active_main_form()->undo();
      }

      auto edit_can_undo() -> bool {
        if (wb::WBContextUI::get()->get_active_main_form() &&
            wb::WBContextUI::get()->get_active_main_form()->can_undo())
          return true;
        return false;
      }

      auto edit_redo() -> void {
        if (wb::WBContextUI::get()->get_active_main_form())
          wb::WBContextUI::get()->get_active_main_form()->redo();
      }

      auto edit_can_redo() -> bool {
        if (wb::WBContextUI::get()->get_active_main_form() &&
            wb::WBContextUI::get()->get_active_main_form()->can_redo())
          return true;
        return false;
      }

      auto edit_copy() -> void {
        if (wb::WBContextUI::get()->get_active_form())
          wb::WBContextUI::get()->get_active_form()->copy();
      }

      auto edit_can_copy() -> bool {
        if (wb::WBContextUI::get()->get_active_form())
          return wb::WBContextUI::get()->get_active_form()->can_copy();
        return false;
      }

      auto edit_cut() -> void {
        if (wb::WBContextUI::get()->get_active_form())
          wb::WBContextUI::get()->get_active_form()->cut();
      }

      auto edit_can_cut() -> bool {
        if (wb::WBContextUI::get()->get_active_form() && wb::WBContextUI::get()->get_active_form()->can_cut())
          return true;
        return false;
      }

      auto edit_paste() -> void {
        if (wb::WBContextUI::get()->get_active_form())
          wb::WBContextUI::get()->get_active_form()->paste();
      }

      auto edit_can_paste() -> bool {
        if (wb::WBContextUI::get()->get_active_form())
          return wb::WBContextUI::get()->get_active_form()->can_paste();
        return false;
      }

      auto edit_select_all() -> void {
        if (wb::WBContextUI::get()->get_active_form())
          wb::WBContextUI::get()->get_active_form()->select_all();
      }

      auto edit_can_select_all() -> bool {
        if (wb::WBContextUI::get()->get_active_form() && wb::WBContextUI::get()->get_active_form()->can_select_all())
          return true;
        return false;
      }

      auto edit_delete() -> void {
        if (wb::WBContextUI::get()->get_active_form())
          wb::WBContextUI::get()->get_active_form()->delete_selection();
      }

      auto edit_can_delete() -> bool {
        if (wb::WBContextUI::get()->get_active_form() && wb::WBContextUI::get()->get_active_form()->can_delete())
          return true;
        return false;
      }

      bool try_searching_diagram(String ^ text) {
        bec::UIForm* form = wb::WBContextUI::get()->get_active_main_form();
        if (form && dynamic_cast<wb::ModelDiagramForm*>(form)) {
          dynamic_cast<wb::ModelDiagramForm*>(form)->search_and_focus_object(NativeToCppString(text));
          return true;
        }
        return false;
      }

      typedef DelegateSlot0<void, void> CommandActionDelegate;
      typedef DelegateSlot0<bool, bool> CommandValidateDelegate;

      void set_edit_menu_delegates(CommandActionDelegate::ManagedDelegate ^ undo_delegate,
                                   CommandValidateDelegate::ManagedDelegate ^ can_undo_delegate,
                                   CommandActionDelegate::ManagedDelegate ^ redo_delegate,
                                   CommandValidateDelegate::ManagedDelegate ^ can_redo_delegate,
                                   CommandActionDelegate::ManagedDelegate ^ copy_delegate,
                                   CommandValidateDelegate::ManagedDelegate ^ can_copy_delegate,
                                   CommandActionDelegate::ManagedDelegate ^ cut_delegate,
                                   CommandValidateDelegate::ManagedDelegate ^ can_cut_delegate,
                                   CommandActionDelegate::ManagedDelegate ^ paste_delegate,
                                   CommandValidateDelegate::ManagedDelegate ^ can_paste_delegate,
                                   CommandActionDelegate::ManagedDelegate ^ select_all_delegate,
                                   CommandValidateDelegate::ManagedDelegate ^ can_select_all_delegate,
                                   CommandActionDelegate::ManagedDelegate ^ delete_delegate,
                                   CommandValidateDelegate::ManagedDelegate ^ can_delete_delegate,
                                   CommandActionDelegate::ManagedDelegate ^ find_delegate,
                                   CommandValidateDelegate::ManagedDelegate ^ can_find_delegate,
                                   CommandActionDelegate::ManagedDelegate ^ find_replace_delegate,
                                   CommandValidateDelegate::ManagedDelegate ^ can_find_replace_delegate) {
        this->undo_delegate = gcnew CommandActionDelegate(undo_delegate);
        this->can_undo_delegate = gcnew CommandValidateDelegate(can_undo_delegate);
        this->redo_delegate = gcnew CommandActionDelegate(redo_delegate);
        this->can_redo_delegate = gcnew CommandValidateDelegate(can_redo_delegate);
        this->copy_delegate = gcnew CommandActionDelegate(copy_delegate);
        this->can_copy_delegate = gcnew CommandValidateDelegate(can_copy_delegate);
        this->cut_delegate = gcnew CommandActionDelegate(cut_delegate);
        this->can_cut_delegate = gcnew CommandValidateDelegate(can_cut_delegate);
        this->paste_delegate = gcnew CommandActionDelegate(paste_delegate);
        this->can_paste_delegate = gcnew CommandValidateDelegate(can_paste_delegate);
        this->select_all_delegate = gcnew CommandActionDelegate(select_all_delegate);
        this->can_select_all_delegate = gcnew CommandValidateDelegate(can_select_all_delegate);
        this->delete_delegate = gcnew CommandActionDelegate(delete_delegate);
        this->can_delete_delegate = gcnew CommandValidateDelegate(can_delete_delegate);
        this->find_delegate = gcnew CommandActionDelegate(find_delegate);
        this->can_find_delegate = gcnew CommandValidateDelegate(can_find_delegate);
        this->find_replace_delegate = gcnew CommandActionDelegate(find_replace_delegate);
        this->can_find_replace_delegate = gcnew CommandValidateDelegate(can_find_replace_delegate);

        wb::WBContextUI::get()->get_command_ui()->add_builtin_command("undo", this->undo_delegate->get_slot(),
                                                                      this->can_undo_delegate->get_slot());
        wb::WBContextUI::get()->get_command_ui()->add_builtin_command("redo", this->redo_delegate->get_slot(),
                                                                      this->can_redo_delegate->get_slot());
        wb::WBContextUI::get()->get_command_ui()->add_builtin_command("copy", this->copy_delegate->get_slot(),
                                                                      this->can_copy_delegate->get_slot());
        wb::WBContextUI::get()->get_command_ui()->add_builtin_command("cut", this->cut_delegate->get_slot(),
                                                                      this->can_cut_delegate->get_slot());
        wb::WBContextUI::get()->get_command_ui()->add_builtin_command("paste", this->paste_delegate->get_slot(),
                                                                      this->can_paste_delegate->get_slot());
        wb::WBContextUI::get()->get_command_ui()->add_builtin_command(
          "selectAll", this->select_all_delegate->get_slot(), this->can_select_all_delegate->get_slot());
        wb::WBContextUI::get()->get_command_ui()->add_builtin_command("delete", this->delete_delegate->get_slot(),
                                                                      this->can_delete_delegate->get_slot());
        wb::WBContextUI::get()->get_command_ui()->add_builtin_command("find", this->find_delegate->get_slot(),
                                                                      this->can_find_delegate->get_slot());
        wb::WBContextUI::get()->get_command_ui()->add_builtin_command(
          "find_replace", this->find_replace_delegate->get_slot(), this->can_find_replace_delegate->get_slot());
      }

      // Plugins/command handling.
      void add_frontend_commands(List<String ^> ^ commands);
      void remove_frontend_commands(List<String ^> ^ commands);
      void activate_command(String ^ name);

      // Overview.
      auto get_physical_overview() -> Overview ^;
      auto get_history_tree() -> Aga::Controls::Tree::TreeViewAdv ^;
      auto get_usertypes_tree() -> Aga::Controls::Tree::TreeViewAdv ^;
      MySQL::Grt::GrtValueInspector ^
        get_inspector_for_selection(MySQL::Base::UIForm ^ form, [Out] List<String ^> ^ % items);
      bool are_lists_equal(GrtValue ^ v1, GrtValue ^ v2);
      String ^ get_description_for_selection(MySQL::Base::UIForm ^ form, [Out] GrtValue ^ % activeObjList,
                                             [Out] List<String ^> ^ % items);
      String ^ get_description_for_selection([Out] GrtValue ^ % activeObjList, [Out] List<String ^> ^ % items);
      void set_description_for_selection(GrtValue ^ activeObjList, String ^ val);
      ::MySQL::MySqlStudio::ModelDiagramFormWrapper ^ get_diagram_form_for_diagram(String ^ id);
      void set_active_form(MySQL::Base::UIForm ^ uiform);
      void set_active_form_from_appview(MySQL::Forms::AppViewDockContent ^ form);
      auto get_active_context() -> String ^;
      auto close_gui_plugin(IntPtr handle) -> void;
      void execute_plugin(String ^ name);
      void report_bug(String ^ errorInfo);

      // State support.
      String ^ read_state(String ^ name, String ^ domain, String ^ default_value);
      int read_state(String ^ name, String ^ domain, const int default_value);
      double read_state(String ^ name, String ^ domain, const double default_value);
      bool read_state(String ^ name, String ^ domain, const bool default_value);

      void save_state(String ^ name, String ^ domain, String ^ value);
      void save_state(String ^ name, String ^ domain, const int value);
      void save_state(String ^ name, String ^ domain, const double value);
      void save_state(String ^ name, String ^ domain, const bool value);

      // Preferences support.
      String ^ read_option_value(String ^ model, String ^ key, String ^ default_value);

      // Paper.
      auto get_paper_sizes() -> List<PaperSize ^> ^ {
          return MySQL::Grt::CppListToObjectList<::wb::WBPaperSize, PaperSize>(
            wb::WBContextUI::get()->get_paper_sizes(false));
        }

        auto get_page_settings() -> PageSettings ^ {
          app_PageSettingsRef settings(wb::WBContextUI::get()->get_page_settings());
          if (settings.is_valid())
            return gcnew PageSettings(settings);
          return nullptr;
        }

        void set_page_settings(PageSettings ^ settings) {
        settings->update_object(wb::WBContextUI::get()->get_wb()->get_root()->options()->paperTypes(),
                                wb::WBContextUI::get()->get_page_settings());
        wb::WBContextUI::get()->get_wb()->get_model_context()->update_page_settings();
      }

      /**
       * To be called by the front end once the main form is ready.
       */
      void finished_loading(WbOptions ^ options) {
        wb::WBContextUI::get()->init_finish(options->get_unmanaged_object());
      }

      auto close_document_finish() -> void {
        wb::WBContextUI::get()->get_wb()->close_document_finish();

        // Explicitly delete the overview object to avoid garbage collection to kick in after
        // our internal (non-managed) objects are gone already.
        delete physical_overview;
        physical_overview = nullptr;
      }

      auto new_model_finish() -> void {
        wb::WBContextUI::get()->get_wb()->new_model_finish();
      }

      String ^ get_filename() { return CppStringToNative(wb::WBContextUI::get()->get_wb()->get_filename()); }

        auto mainform_activated() -> void {
        mforms::Form::main_form()->activated();
      }
      auto mainform_deactivated() -> void {
        mforms::Form::main_form()->deactivated();
      }
      auto mainform_active() -> bool {
        return mforms::Form::main_form()->is_active();
      }
    };

  public
    ref class DiagramOptionsBE {
      wb::DiagramOptionsBE* inner;

      MySQL::Grt::DelegateSlot0<void, void> ^ _delegate;

    public:
      DiagramOptionsBE(WindowsGDICanvasView ^ view, WbContext ^ wbContext,
                       MySQL::Grt::DelegateSlot0<void, void>::ManagedDelegate ^ deleg) {
        _delegate = gcnew MySQL::Grt::DelegateSlot0<void, void>(deleg);

        inner = new wb::DiagramOptionsBE(
          view->get_unmanaged_object(),
          wb::WBContextUI::get()->get_wb()->get_model_context()->get_active_model_diagram(true),
          wb::WBContextUI::get()->get_wb());

        inner->scoped_connect(inner->signal_changed(), _delegate->get_slot());
      }

      ~DiagramOptionsBE() {
        delete _delegate;
        delete inner;
      }

      String ^ get_name() { return CppStringToNative(inner->get_name()); }

        void set_name(String ^ name) {
        inner->set_name(NativeToCppString(name));
      }

      auto get_xpages() -> int {
        return inner->get_xpages();
      }

      auto get_ypages() -> int {
        return inner->get_ypages();
      }

      auto set_xpages(int c) -> void {
        inner->set_xpages(c);
      }

      auto set_ypages(int c) -> void {
        inner->set_ypages(c);
      }

      auto commit() -> void {
        inner->commit();
      }

      auto update_size() -> void {
        inner->update_size();
      }
    };

  } // namespace MySqlStudio
} // namespace MySQL
