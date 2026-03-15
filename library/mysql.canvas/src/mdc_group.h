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

#ifndef _MDC_GROUP_H_
#define _MDC_GROUP_H_

#include "mdc_layouter.h"

namespace mdc {

  class Layer;

  class MYSQLCANVAS_PUBLIC_FUNC Group : public Layouter {
  public:
    Group(Layer *layer);
    virtual ~Group();

    virtual auto dissolve() -> void;

    virtual auto add(CanvasItem *item) -> void;
    virtual auto remove(CanvasItem *item) -> void;

    auto has_item(CanvasItem *item) -> bool;
    auto get_contents() -> std::list<CanvasItem *> & {
      return _contents;
    };
    auto empty() const -> bool {
      return _contents.empty();
    };

    virtual auto foreach (const std::function<void(CanvasItem *)> &slot) -> void;

    auto freeze() -> void;
    auto thaw() -> void;

    auto get_direct_subitem_at(const base::Point &point) -> CanvasItem *;
    virtual auto get_other_item_at(const base::Point &point, CanvasItem *item) -> CanvasItem *;
    virtual auto get_item_at(const base::Point &point) -> CanvasItem *;

    virtual auto move_item(CanvasItem *child_item, const base::Point &pos) -> void;

    virtual auto raise_item(CanvasItem *item, CanvasItem *above = 0) -> void;
    virtual auto lower_item(CanvasItem *item) -> void;

    virtual auto move_to(const base::Point &point) -> void;

    virtual auto set_selected(bool flag) -> void;

    virtual auto repaint(const base::Rect &clipArea, bool direct) -> void;

  protected:
    struct ItemInfo {
      boost::signals2::connection connection;
    };

    // front of list is top stack
    std::list<CanvasItem *> _contents;

    std::map<CanvasItem *, ItemInfo> _content_info;
    int _freeze_bounds_updates;
#ifdef no_group_activate
    bool _activated;
#endif

    virtual auto update_bounds() -> void;

    auto focus_changed(bool f, CanvasItem *item) -> void;
#ifdef no_group_activate
    auto activate_group(bool flag) -> void;
#endif
  };

} // end of mdc namespace

#endif /* _MDC_GROUP_H_ */
