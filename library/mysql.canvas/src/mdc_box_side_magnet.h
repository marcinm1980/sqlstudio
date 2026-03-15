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

#ifndef __MDC_BOX_SIDE_MAGNET_H__
#define __MDC_BOX_SIDE_MAGNET_H__

#include "mdc_magnet.h"

namespace mdc {

  class Connector;
  class CanvasItem;

  class MYSQLCANVAS_PUBLIC_FUNC BoxSideMagnet : public Magnet {
  public:
    enum Side { Unknown, Top, Left, Right, Bottom };

    BoxSideMagnet(CanvasItem *owner);
    virtual ~BoxSideMagnet(){};

    auto set_compare_slot(const std::function<bool(Connector *, Connector *, Side)> &compare) -> void;

    virtual auto constrain_angle(double angle) const -> double;

    virtual auto get_position_for_connector(Connector *conn, const base::Point &srcpos) const -> base::Point;

    auto set_connector_side(Connector *conn, Side side) -> void;

    virtual auto remove_connector(mdc::Connector *conn) -> void;

    auto reorder_connector_closer_to(Connector *conn, const base::Point &pos) -> void;

  protected:
    friend struct CompareConnectors;
    class CompareConnectors {
      BoxSideMagnet *_magnet;

    public:
      CompareConnectors(BoxSideMagnet *magnet) : _magnet(magnet) {
      }

      auto operator()(Connector *a, Connector *b) -> bool {
        BoxSideMagnet::Side aside = _magnet->get_connector_side(a);
        BoxSideMagnet::Side bside = _magnet->get_connector_side(b);

        if ((int)aside < (int)bside)
          return true;
        if ((int)aside == (int)bside)
          return _magnet->_compare(a, b, aside);
        return false;
      }
    };

    std::map<Connector *, Side> _connector_info;
    std::function<bool(Connector *, Connector *, Side)> _compare;
    short _counts[5];

    auto get_connector_side(Connector *conn) const -> Side;
    auto connector_position(Side side, Connector *conn, double length) const -> double;

    auto notify_connectors(Side side) -> void;

    auto reorder_connectors() -> void;
  };

} // end of mdc namespace

#endif
