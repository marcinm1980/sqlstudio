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

#ifndef _GTK_HELPERS_H_
#define _GTK_HELPERS_H_

#include <gdk/gdk.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/eventbox.h>

namespace Gtk {
  class TreeView;
  class Widget;
  class Entry;
  class ListStore;
  class EntryCompletion;
  class Box;
  class ComboBox;
  class ComboBoxText;
  class Window;
  class Menu;
  class Label;
  class Paned;
}

#include <glibmm/refptr.h>
#include <gtkmm/treemodelcolumn.h>
#include <vector>
#include <string>
#include "base/ui_form.h"

#include <sigc++/sigc++.h>

//!
//! \addtogroup linuxutils Linux utils
//! @{
//!
class TextListColumnsModel;
class TreeModelWrapper;
auto expand_tree_nodes_as_in_be(const Glib::RefPtr<TreeModelWrapper> &model, Gtk::TreeView *tv) -> void;

auto create_icon_label(const std::string &icon, const std::string &label) -> Gtk::Box &;

// Gtk::Widget *create_closeable_tab(const Glib::ustring &title, const sigc::slot<void> &close_callback,
//                                  Gtk::Label **title_label);

auto swap_panned_children(Gtk::Paned *paned, bool fixed_size1) -> void;

auto model_from_string_list(const std::vector<std::string> &list,
                                                    TextListColumnsModel *columns) -> Glib::RefPtr<Gtk::ListStore>;
auto model_from_string_list(const std::vector<std::string> &list,
                                                    TextListColumnsModel **columns = 0) -> Glib::RefPtr<Gtk::ListStore>;
auto model_from_string_list(const std::list<std::string> &list,
                                                    TextListColumnsModel **columns = 0) -> Glib::RefPtr<Gtk::ListStore>;
auto recreate_model_from_string_list(Glib::RefPtr<Gtk::ListStore>, const std::vector<std::string> &list) -> void;
auto setup_combo_for_string_list(Gtk::ComboBox *combo) -> void;

auto get_selected_combo_item(Gtk::ComboBox *combo) -> std::string;
auto set_selected_combo_item(Gtk::ComboBox *combo, const std::string &value) -> bool;

//! Wrapper to set string values to a Glib::ValueBase
//! Used in ListModelWrapper
auto set_glib_string(Glib::ValueBase &value, const std::string &str, bool escape_nuls = false) -> void;
auto set_glib_int(Glib::ValueBase &value, const int i) -> void;
auto set_glib_bool(Glib::ValueBase &value, const bool b) -> void;
auto set_glib_double(Glib::ValueBase &value, const double d) -> void;

auto fill_combo_from_string_list(Gtk::ComboBox *combo, const std::vector<std::string> &list) -> void;
auto fill_combo_from_string_list(Gtk::ComboBoxText *combo, const std::vector<std::string> &list) -> void;

//! get_mainwindow is declared here as extern while it is implemented in frontend/linux/studio/Program.cpp
//! and frontend/linux/shell/shell.cpp. get_mainwindow is needed by some functions/methods to
//! set dialog transient. Returns ptr to Gtk::Window
extern auto get_mainwindow_impl() -> void *;
inline auto get_mainwindow() -> Gtk::Window * {
  return (Gtk::Window *)get_mainwindow_impl();
}

extern std::string open_file_chooser(const std::string &filter = "*");
extern std::string save_file_chooser(const std::string &filter = "*");

template <typename MutexType>
class Locker {
public:
  Locker(MutexType &m) : _m(m) {
    _m.lock();
  }
  ~Locker() {
    _m.unlock();
  }

private:
  MutexType &_m;
};

auto run_popup_menu(const bec::MenuItemList &items, const int time, const sigc::slot<void, std::string> &activate_slot,
                    Gtk::Menu *popup) -> void;

auto fix_broken_gtk_selection_handling(Gtk::TreeView *tree) -> void;

auto get_empty_model() -> Glib::RefPtr<Gtk::ListStore>;

auto gtk_paned_set_pos_ratio(Gtk::Paned *paned, const float ratio) -> void;
auto gtk_paned_get_pos_ratio(Gtk::Paned *paned) -> float;

auto gtk_reparent_realized(Gtk::Widget *widget, Gtk::Container *new_parent) -> void;

Gdk::RGBA color_to_rgba(Gdk::Color);

class PanedConstrainer {
public:
  enum PanedInfo {
    PANED_HIDDEN,       // Paned position is set to 0
    PANED_VISIBLE,      // Paned position is between 0 and it's max position.
    PANED_FULLY_VISIBLE // Paned position is set to it's max position.
  };
  typedef std::function<void(PanedInfo)> state_notifier;

  /** Add sticky behaviour or limit size for Gtk::Paned widget.
     *
     * @param top_or_left_limit should be set to prevent Gtk::Paned to be smaller than the specified size,
     * or to be automagically hidden when user make it smaller than that value, based on sticky behaviour.
     * from the left or top side, depends if it's horizontal or vertical. Set to 0 to disable this limit.
     * @param bottom_or_right_limit it's similar to the previous parameter except it limits the right or bottom size,
     * depends if it's horizontal or vertical. Set to 0 to disable this limit.

     * @return PanedContrainer* Pointer to PanedConstrainer. The pointer will be automagically freed when Gtk::Paned is
  desotryed.
     */
  static auto make_constrainer(Gtk::Paned *paned, int top_or_left_limit, int bottom_or_right_limit) -> PanedConstrainer *;

  auto disable_sticky(bool disable) -> void;
  auto set_state_cb(const state_notifier &cb) -> void;
  ~PanedConstrainer();
  static auto destroy(void *data) -> void *;

  auto set_limit(int top_or_left = 0, int bottom_or_right = 0) -> void;
  auto get() -> Gtk::Paned *;

private:
  Gtk::Paned *_pan;
  bool _reentrant;
  int _top_or_left_limit;
  int _bottom_or_right_limit;
  bool _vertical;
  bool _allow_sticky;
  bool _was_hidden;
  sigc::connection _size_alloc_sig;

  state_notifier _state_notifier_cb;

  auto size_alloc(Gtk::Allocation &_alloc) -> void;
  PanedConstrainer(Gtk::Paned *pan);
};

//!
//! }@
//!

#endif /* _GTK_HELPERS_H_ */
