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

#ifndef __MDC_CONNECTOR_H__
#define __MDC_CONNECTOR_H__

#include "mdc_common.h"
#include "base/trackable.h"

namespace mdc {

  class Magnet;
  class CanvasItem;

  class MYSQLCANVAS_PUBLIC_FUNC Connector : public base::trackable {
  public:
    Connector(CanvasItem *owner);
    virtual ~Connector();

    void set_update_handler(const std::function<void(Connector *)> &update_handler);

    virtual auto try_connect(Magnet *magnet) -> bool;
    virtual auto try_disconnect() -> bool;

    virtual void connect(Magnet *magnet);
    virtual void disconnect();

    void set_draggable(bool flag);
    auto is_draggable() -> bool {
      return _draggable;
    }

    void set_tag(int tag) {
      _tag = tag;
    }
    auto get_tag() -> int {
      return _tag;
    }

    auto get_connected_magnet() -> Magnet * {
      return _magnet;
    }
    auto get_connected_item() -> CanvasItem *;
    auto get_owner() -> CanvasItem * {
      return _owner;
    }

    auto get_position(const base::Point &srcpos) -> base::Point;
    auto get_position() -> base::Point;

    // callback for Magnet
    virtual void magnet_moved(Magnet *magnet);

  protected:
    CanvasItem *_owner;
    Magnet *_magnet;
    int _tag;

    bool _draggable;

    std::function<void(Connector *)> _update_handler;
  };

} // end of mdc namespace

#endif
