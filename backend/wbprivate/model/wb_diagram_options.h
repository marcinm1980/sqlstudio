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

#ifndef _WB_DIAGRAM_OPTIONS_H_
#define _WB_DIAGRAM_OPTIONS_H_

#include "mdc.h"

#include "studio/wb_backend_public_interface.h"

#include "grts/structs.model.h"

#include "base/trackable.h"

namespace wb {
  class WBContext;

  class MYSQLWBBACKEND_PUBLIC_FUNC DiagramOptionsBE : public base::trackable {
    friend class SizerFigure;

    mdc::CanvasView *_view;
    model_DiagramRef _target_view;
    class SizerFigure *_sizer;
    std::string _name;

    boost::signals2::signal<void()> _changed_signal;

    auto get_min_size_in_pages(int &xc, int &yc) -> void;

  public:
    DiagramOptionsBE(mdc::CanvasView *view, model_DiagramRef target_view, WBContext *wb);
    ~DiagramOptionsBE();

    auto update_size() -> void;

    auto get_name() -> std::string;
    auto set_name(const std::string &name) -> void;

    auto get_xpages() -> int;
    auto get_ypages() -> int;
    auto set_xpages(int c) -> void;
    auto set_ypages(int c) -> void;

    auto get_max_page_counts(int &max_xpages, int &max_ypages) -> void;

    auto commit() -> void;

    boost::signals2::signal<void()> *signal_changed() {
      return &_changed_signal;
    }
  };
};

#endif //  _WB_DIAGRAM_OPTIONS_H_
