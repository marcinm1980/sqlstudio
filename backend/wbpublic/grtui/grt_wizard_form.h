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

#include <vector>
#include <set>

#include "grt.h"
#include "grt/common.h"

#include "wbpublic_public_interface.h"
#include "base/string_utilities.h"

#include "mforms/wizard.h"
#include "mforms/box.h"
#include "mforms/filechooser.h"

namespace mforms {
  class TextEntry;
};

namespace grtui {

  class WizardPage;

  class WBPUBLICBACKEND_PUBLIC_FUNC WizardForm : public mforms::Wizard {
  public:
    WizardForm();
    virtual ~WizardForm();
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif
    virtual auto run_modal() -> bool;
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

    auto add_page(WizardPage *page) -> void;

    auto update_buttons() -> void;
    auto update_heading() -> void;

    auto set_problem(const std::string &text) -> void;
    auto clear_problem() -> void;

    virtual auto reset() -> void;

    auto switch_to_page(WizardPage *page, bool advancing) -> void;
    auto get_active_page() -> WizardPage * {
      return _active_page;
    }
    auto get_active_page_number() -> int;

    auto get_page_with_id(const std::string &id) -> WizardPage *;

    auto values() -> grt::DictRef {
      return _values;
    }

    // util stuff for storing state
    auto set_wizard_option(const std::string &key, const std::string &value) -> void;
    std::string string_wizard_option(const std::string &key, const std::string &default_value = "");

    auto set_wizard_option(const std::string &key, int value) -> void;
    auto int_wizard_option(const std::string &key, int default_value = 0) -> int;

  private:
    grt::DictRef _values;

    std::string _problem;

    WizardPage *_active_page;
    std::vector<WizardPage *> _pages;
    std::list<WizardPage *> _turned_pages;

    bool _cancelled;

  protected:
    virtual auto get_next_page(WizardPage *current) -> WizardPage *;
    auto refresh_step_list() -> void;

    auto extra_clicked() -> void;

  public:
    auto go_to_next() -> void;
    auto go_to_back() -> void;
    auto finish() -> void;
    virtual auto cancel() -> bool;
  };

  /** A page of a wizard.
    */
  class WBPUBLICBACKEND_PUBLIC_FUNC WizardPage : public ::mforms::Box {
  public:
    WizardPage(WizardForm *form, const std::string &pageid);

    auto get_id() const -> std::string {
      return _id;
    }

    auto get_title() const -> std::string {
      return _title;
    }
    auto get_short_title() const -> std::string {
      return _short_title;
    }

    auto set_title(const std::string &title) -> void;
    auto set_short_title(const std::string &title) -> void;

    auto validate() -> void;

    boost::signals2::signal<void(bool)> *signal_enter() {
      return &_signal_enter;
    }
    boost::signals2::signal<void(bool)> *signal_leave() {
      return &_signal_leave;
    }

  public:
  protected:
    friend class WizardForm;

    auto wizard() -> WizardForm * {
      return _form;
    }

    auto values() -> grt::DictRef {
      return _form->values();
    }

    //! Subclasses must override this to implement validation.
    //! If there is a validation error, it must call _form->set_problem()
    virtual auto do_validate() -> void {
    }

    virtual auto load() -> int {
      return -1;
    } // delme XXX

    virtual auto pre_load() -> bool;
    virtual auto enter(bool advancing) -> void;
    virtual auto advance() -> bool;
    virtual auto leave(bool advancing) -> void;

    virtual auto allow_next() -> bool {
      return true;
    }
    virtual auto allow_back() -> bool {
      return true;
    }
    virtual auto allow_cancel() -> bool {
      return true;
    }
    virtual auto skip_page() -> bool {
      return false;
    } // Return true if the page should not be displayed (due to some condition).

    //! return true if this is the last page and pressing next should close wizard
    virtual auto next_closes_wizard() -> bool {
      return false;
    }

    //! overrider may return "" for default caption
    virtual auto next_button_caption() -> std::string {
      return "";
    }

    virtual auto extra_button_caption() -> std::string {
      return "";
    }

    auto finish_button_caption() const -> std::string {
#ifdef __APPLE__
      return _("Close");
#elif defined(_MSC_VER)
      return _("Finish");
#else
      return _("_Close");
#endif
    }

    virtual auto extra_clicked() -> void {
    }

  protected:
    WizardForm *_form;
    std::string _id;
    boost::signals2::signal<void(bool)> _signal_enter;
    boost::signals2::signal<void(bool)> _signal_leave;
    std::string _title;
    std::string _short_title;

    auto execute_caption() const -> std::string {
#ifdef __APPLE__
      return _("Execute");
#elif defined(_MSC_VER)
      return _("_Execute >");
#else
      return _("_Execute");
#endif
    }

    auto finish_caption() const -> std::string {
#ifdef __APPLE__
      return _("Finish");
#else
      return _("_Finish");
#endif
    }

    virtual auto close_caption() const -> std::string {
#ifdef __APPLE__
      return _("Close");
#else
      return _("_Close");
#endif
    }

  private:
    auto filename_changed(mforms::TextEntry *entry) -> void;
    auto browse_file_callback(mforms::TextEntry *entry, mforms::FileChooserType type, const std::string &extensions) -> void;
  };
};
