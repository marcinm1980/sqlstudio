/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef _LF_SCROLL_PANEL_H_
#define _LF_SCROLL_PANEL_H_

#include "mforms/scrollpanel.h"

#include "lfi_bin.h"

namespace mforms {
  namespace gtk {

    class ScrollPanelImpl : public ViewImpl, public BinImpl {
      Gtk::ScrolledWindow *_swin;
      bool _vertical, _horizontal;
      bool _autohide;
      bool _noAutoScroll;

      virtual auto get_outer() const -> Gtk::Widget * {
        return _swin;
      }
      virtual auto get_inner() const -> Gtk::Widget * {
        return _swin;
      }

    protected:
      ScrollPanelImpl(::mforms::ScrollPanel *self, mforms::ScrollPanelFlags flags);
      ~ScrollPanelImpl();

      static auto create(::mforms::ScrollPanel *self, mforms::ScrollPanelFlags flags) -> bool;
      static auto add(::mforms::ScrollPanel *self, ::mforms::View *child) -> void;
      static auto remove(::mforms::ScrollPanel *self) -> void;
      static auto set_visible_scrollers(::mforms::ScrollPanel *self, bool vertical, bool horizontal) -> void;
      static auto set_autohide_scrollers(::mforms::ScrollPanel *self, bool flag) -> void;
      static auto scroll_to_view(mforms::ScrollPanel *, mforms::View *) -> void;
      static auto get_content_rect(mforms::ScrollPanel *) -> base::Rect;
      static auto scroll_to(mforms::ScrollPanel *self, int x, int y) -> void;
      virtual auto set_padding_impl(int left, int top, int right, int bottom) -> void;
      auto disableAutomaticScrollToChildren() -> void;

    public:
      static auto init() -> void;
    };
  };
};

#endif
