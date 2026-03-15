/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

/**
 * Definitions and implementations of geometry related types and classes. Defines all the usual base types
 * like point, rectangle and the like.
 */

#include <stdio.h>

#ifndef SWIG
#include "common.h"
#endif

namespace base {

#ifndef SWIG

  struct BASELIBRARY_PUBLIC_FUNC Point {
    double x;
    double y;

    Point();
    Point(double x, double y);

    inline auto operator+(const Point &p) const -> Point {
      return Point(p.x + x, p.y + y);
    };
    inline auto operator-(const Point &p) const -> Point {
      return Point(x - p.x, y - p.y);
    };
    inline auto operator-() const -> Point {
      return Point(-x, -y);
    };
    inline auto operator==(const Point &p) const -> bool {
      return p.x == x && p.y == y;
    };
    inline auto operator!=(const Point &p) const -> bool {
      return p.x != x || p.y != y;
    };
    inline auto round() const -> Point {
      Point p;
      p.x = ceil(x);
      p.y = ceil(y);
      return p;
    };
    inline auto str() const -> std::string {
      char buf[20];
      snprintf(buf, sizeof(buf), "{%.2f,%.2f}", x, y);
      return buf;
    };
  };

  struct BASELIBRARY_PUBLIC_FUNC Size {
    double width;
    double height;

    auto empty() -> bool;

    Size();
    Size(double w, double h);

    inline auto round() const -> Size {
      Size s;
      s.width = ceil(width);
      s.height = ceil(height);
      return s;
    };
    inline auto str() const -> std::string {
      char buf[20];
      snprintf(buf, sizeof(buf), "{%.2fx%.2f}", width, height);
      return buf;
    };

    inline auto operator==(const Size &s) const -> bool {
      return s.width == width && s.height == height;
    };
    inline auto operator!=(const Size &s) const -> bool {
      return s.width != width || s.height != height;
    };
  };

  struct BASELIBRARY_PUBLIC_FUNC Rect {
    Point pos;
    Size size;
    bool use_inter_pixel; // For some drawing operations we need coordinates that are between two pixels.

    Rect();
    Rect(double x, double y, double w, double h);
    Rect(const Point &tl, const Point &br);
    Rect(const Point &apos, const Size &asize);

    auto contains(double x, double y) const -> bool;
    auto contains_flipped(double x, double y) const -> bool;
    void inflate(double horizontal, double vertical);

    auto right() const -> double;
    auto bottom() const -> double;
    inline auto empty() const -> bool {
      return (size.width <= 0) || (size.height <= 0);
    }

    auto left() const -> double;
    auto top() const -> double;
    inline auto width() const -> double {
      return size.width;
    };
    inline auto height() const -> double {
      return size.height;
    };

    inline auto xcenter() const -> double {
      return pos.x + size.width / 2;
    }
    inline auto ycenter() const -> double {
      return pos.y + size.height / 2;
    }

    // Note: these 4 routines do not move the rectangle but only a given side
    //       adjusting the width to keep all other sides constant.
    inline void set_xmin(double x) {
      size.width -= x - pos.x;
      pos.x = x;
    };
    inline void set_ymin(double y) {
      size.height -= y - pos.y;
      pos.y = y;
    };
    inline void set_xmax(double x) {
      size.width = x - pos.x;
    };
    inline void set_ymax(double y) {
      size.height = y - pos.y;
    };

    inline auto center() const -> Point {
      return Point(xcenter(), ycenter());
    }

    inline auto top_left() const -> Point {
      return Point(left(), top());
    }
    inline auto top_right() const -> Point {
      return Point(right(), top());
    }
    inline auto bottom_left() const -> Point {
      return Point(left(), bottom());
    }
    inline auto bottom_right() const -> Point {
      return Point(right(), bottom());
    }

    inline auto operator==(const Rect &r) const -> bool {
      return r.pos == pos && r.size == size;
    };
    inline auto operator!=(const Rect &r) const -> bool {
      return r.pos != pos || r.size != size;
    };

    inline auto str() const -> std::string {
      char buf[40];
      snprintf(buf, sizeof(buf), "{%.2f,%.2f  %.2fx%.2f}", pos.x, pos.y, size.width, size.height);
      return buf;
    };
  };

  /**
   * Used to specify (integer) coordinates used to place/size controls.
   */
  struct BASELIBRARY_PUBLIC_FUNC ControlBounds {
    int left;
    int top;
    int width;
    int height;

    ControlBounds();
    ControlBounds(int x, int y, int w, int h);
  };

#endif // SWIG

/**
 * Four values describing the space on each side of an area.
 */
#ifndef SWIG
  using Padding = struct BASELIBRARY_PUBLIC_FUNC Padding
#else
  struct Padding
#endif
  {
    int left;
    int top;
    int right;
    int bottom;

    Padding();
    Padding(int padding);
    Padding(int left, int top, int right, int bottom);

    int horizontal();
    int vertical();
  };

  /** A struct describing a range in a container. */
  struct BASELIBRARY_PUBLIC_FUNC Range {
    size_t position;
    size_t size;

    Range();
    Range(size_t position, size_t size);

    auto end() -> size_t;
    auto contains_point(size_t point) -> bool;
  };

} // namespace base
