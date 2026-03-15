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

#ifndef _MDC_LINE_H_
#define _MDC_LINE_H_

#include "mdc_figure.h"
#include "base/trackable.h"

namespace mdc {

  class Connector;

  enum LineEndType {
    NormalEnd,
    DashedNormalEnd,
    FilledTriangleEnd,
    HollowTriangleEnd,
    ChickenFootEnd,
    ChickenFoot0End,
    ChickenFoot1End,
    Cross0End,
    Cross1End,
    DashedChickenFootEnd,
    HollowDiamondEnd,
    FilledDiamondEnd,
    HollowCircleEnd,
    FilledCircleEnd,
    BoldStickEnd
  };

  enum LinePatternType {
    SolidPattern = 0,
    Dotted1Pattern,
    Dotted2Pattern,
    Dashed1Pattern,
    Dashed2Pattern,
    Dashed3Pattern,
    Dashed4Pattern,
    DashDot1Pattern,
    DashDot2Pattern,
    LastPattern
  };

  class Line;

  class MYSQLCANVAS_PUBLIC_FUNC LineLayouter : public base::trackable {
  public:
    LineLayouter();
    virtual ~LineLayouter();

    auto signal_changed() -> boost::signals2::signal<void()> * {
      return &_changed;
    }

    virtual auto get_start_connector() const -> Connector * = 0;
    virtual auto get_end_connector() const -> Connector * = 0;

    virtual auto get_points() -> std::vector<base::Point> = 0;
    virtual auto get_start_point() -> base::Point = 0;
    virtual auto get_end_point() -> base::Point = 0;

    virtual auto create_handles(Line *line, InteractionLayer *ilayer) -> std::vector<ItemHandle *>;
    virtual void update_handles(Line *line, std::vector<ItemHandle *> &handles);

    virtual auto handle_dragged(Line *line, ItemHandle *handle, const base::Point &pos, bool dragging) -> bool;

    virtual void update() = 0;

  protected:
    struct Segment {
      base::Point p1;
      base::Point p2;
    };

    boost::signals2::signal<void()> _changed;
  };

  class MYSQLCANVAS_PUBLIC_FUNC Line : public Figure {
  public:
    Line(Layer *layer, LineLayouter *layouter = 0);
    virtual ~Line();

    void set_layouter(LineLayouter *layouter);
    auto get_layouter() -> LineLayouter * {
      return _layouter;
    }

    virtual void resize_to(const base::Size &size);
    virtual void move_to(const base::Point &pos);

    virtual auto contains_point(const base::Point &point) const -> bool;

    virtual void draw_contents(CairoCtx *cr);
    virtual void stroke_outline(CairoCtx *cr, float offset = 0) const;
    virtual void stroke_outline_gl(float offset = 0) const;

    void set_vertices(const std::vector<base::Point> &points);
    void add_vertex(const base::Point &pos);
    void set_vertex(size_t vertex, const base::Point &pos);
    inline auto get_vertex(size_t vertex) -> base::Point {
      return _vertices[vertex];
    }
    auto count_vertices() -> size_t {
      return _vertices.size();
    }

    void set_end_type(LineEndType start, LineEndType end);
    void set_line_pattern(LinePatternType pattern);

    void set_hops_crossings(bool flag);
    auto get_hops_crossings() const -> bool {
      return _hop_crossings;
    }

    virtual void mark_crossings(Line *line);

    virtual void create_handles(InteractionLayer *ilayer);
    virtual void update_handles();

    auto signal_layout_changed() -> boost::signals2::signal<void()> * {
      return &_layout_changed;
    }

  protected:
    LineLayouter *_layouter;

    boost::signals2::signal<void()> _layout_changed;

    struct SegmentPoint {
      base::Point pos;
      Line *hop;
      inline auto operator==(const SegmentPoint &sp) const -> bool {
        return sp.pos == pos && sp.hop == hop;
      };
      inline auto operator!=(const SegmentPoint &sp) const -> bool {
        return sp.pos != pos || sp.hop != hop;
      };
      SegmentPoint(const base::Point &p, Line *l) : pos(p), hop(l) {
      }
    };

    // the points that define the line
    std::vector<base::Point> _vertices;

    // the points that define the final appearance of the line (including intersections)
    std::vector<SegmentPoint> _segments;

    LineEndType _start_type;
    LineEndType _end_type;
    LinePatternType _line_pattern;

    bool _hop_crossings;

    void update_bounds();
    void update_layout();

    void set_line_pattern(CairoCtx *cr, LinePatternType pattern);
    auto get_gl_pattern(LinePatternType pattern) -> GLushort;

    auto get_line_start_angle() -> double;
    auto get_line_end_angle() -> double;

    void draw_line_ends(CairoCtx *cr);
    void draw_line_ends_gl();
    virtual void draw_outline_ring(CairoCtx *cr, const base::Color &color);
    virtual void draw_outline_ring_gl(const base::Color &color);

  private:
    virtual auto on_drag_handle(ItemHandle *handle, const base::Point &pos, bool dragging) -> bool;
  };

} // end of mdc namespace

#endif /* _MDC_LINE_H_ */
