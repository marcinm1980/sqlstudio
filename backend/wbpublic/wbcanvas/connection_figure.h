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

#ifndef _CONNECTION_FIGURE_H_
#define _CONNECTION_FIGURE_H_

#include "mdc.h"
#include <grts/structs.model.h>

namespace wbfig {
  class FigureEventHub;

  class ConnectionLineLayouter : public mdc::OrthogonalLineLayouter {
    using super = mdc::OrthogonalLineLayouter;

    virtual auto create_handles(mdc::Line *line, mdc::InteractionLayer *ilayer) -> std::vector<mdc::ItemHandle *>;

    virtual auto update_start_point() -> bool;
    virtual auto update_end_point() -> bool;

    virtual auto handle_dragged(mdc::Line *line, mdc::ItemHandle *handle, const base::Point &pos, bool dragging) -> bool;

  public:
    enum Type { NormalLine, ZLine };

  private:
    Type _type;

    virtual auto get_points_for_subline(int subline) -> std::vector<base::Point>;

  public:
    ConnectionLineLayouter(mdc::Connector *sconn, mdc::Connector *econn);

    auto set_type(Type type) -> void;
    auto get_type() const -> Type {
      return _type;
    }
  };

  class Connection : public mdc::Line {
    using super = mdc::Line;

  public:
    enum DiamondType { None, Filled, LeftEmpty, RightEmpty, Empty };
    enum CaptionPos { Below, Above, Middle };

    Connection(mdc::Layer *layer, FigureEventHub *hub, model_Object *represented_object);

    auto set_splitted(bool flag) -> void;

    auto get_middle_caption_pos(const base::Size &size, CaptionPos pos) -> base::Point;
    auto get_start_caption_pos(const base::Size &size) -> base::Point;
    auto get_end_caption_pos(const base::Size &size) -> base::Point;

    auto set_start_dashed(bool flag) -> void;
    auto set_end_dashed(bool flag) -> void;

    virtual auto render(mdc::CairoCtx *cr) -> void;
    virtual auto render_gl(mdc::CairoCtx *cr) -> void;

    auto set_start_figure(mdc::CanvasItem *item) -> void;
    auto set_end_figure(mdc::CanvasItem *item) -> void;
    auto get_start_figure() -> mdc::CanvasItem * {
      return _start_figure;
    }
    auto get_end_figure() -> mdc::CanvasItem * {
      return _end_figure;
    }

    auto set_diamond_type(DiamondType type) -> void;

    auto get_segment_offset(int subline) -> double;
    auto set_segment_offset(int subline, double offset) -> void;

    virtual auto on_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                          mdc::EventState state) -> bool;
    virtual auto on_double_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_enter(mdc::CanvasItem *target, const base::Point &point) -> bool;
    virtual auto on_leave(mdc::CanvasItem *target, const base::Point &point) -> bool;
    virtual auto on_button_press(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_button_release(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                   mdc::EventState state) -> bool;

    auto set_center_captions(bool flag) -> void;
    auto get_center_captions() -> bool {
      return _center_captions;
    }
    virtual auto can_render_gl() -> bool {
      return true;
    }

  private:
    model_Object *_represented_object;
    FigureEventHub *_hub;

    mdc::CanvasItem *_start_figure;
    mdc::CanvasItem *_end_figure;

    DiamondType _diamond;
    bool _start_dashed;
    bool _end_dashed;
    bool _split;
    bool _center_captions;

    virtual auto contains_point(const base::Point &point) const -> bool;

    virtual auto stroke_outline(mdc::CairoCtx *cr, float offset = 0) const -> void;
    auto stroke_outline_gl(float offset = 0) const -> void;

    auto get_middle_segment_angle() -> double;

    auto update_layouter() -> void;

    virtual auto mark_crossings(mdc::Line *line) -> void;
  };
}; // namespace wbfig

#endif
