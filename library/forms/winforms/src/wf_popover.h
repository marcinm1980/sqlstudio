/*
 * Copyright (c) 2011, 2018, Oracle and/or its affiliates. All rights reserved.
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

namespace MySQL {
  namespace Forms {

    ref class PopoverControl;

  public
    class PopoverWrapper : public ObjectWrapper {
    private:
      boost::signals2::connection _track_connection;
      auto mouse_left_tracked_object() -> bool;

    protected:
      PopoverWrapper(mforms::Popover *backend);

      static auto create(mforms::Popover *backend, mforms::View *relative, mforms::PopoverStyle style) -> bool;
      static auto destroy(mforms::Popover *backend) -> void;
      static auto set_content(mforms::Popover *backend, mforms::View *content) -> void;
      static auto set_size(mforms::Popover *backend, int width, int height) -> void;
      static auto show(mforms::Popover *backend, int spot_x, int spot_y, mforms::StartPosition position) -> void;
      static auto show_and_track(mforms::Popover *backend, mforms::View *owner, int spot_x, int spot_y,
                                 mforms::StartPosition position) -> void;
      static auto get_content_rect(mforms::Popover *backend) -> base::Rect;
      static auto setName(mforms::Popover *backend, const std::string &name) -> void;
      static auto close(mforms::Popover *backend) -> void;

    public:
      static auto init() -> void;
    };
  };
};
