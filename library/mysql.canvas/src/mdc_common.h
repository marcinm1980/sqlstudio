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

#ifndef _MSC_VER

#include <glib.h>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stdexcept>
#include <assert.h>
#include <algorithm>
#include <typeinfo>
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "cairo/cairo.h"

#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "base/geometry.h"
#include "base/drawing.h"
#include "base/string_utilities.h"

#include "mdc_canvas_public.h"

#ifdef _MSC_VER
#define DEFAULT_FONT_FACE "Arial"
#elif defined(__APPLE__)
#define DEFAULT_FONT_FACE "Lucida Grande"
#else
#define DEFAULT_FONT_FACE "Helvetica"
#endif

#define MM_TO_PT(m) ((m) / (25.4 / 72.0))

#ifndef GL_BGRA
#define GL_BGRA GL_BGRA_EXT
#endif

namespace mdc {

  using Count = unsigned int;
  using Timestamp = double;

  enum FontSlant {
    SNormal = CAIRO_FONT_SLANT_NORMAL,
    SOblique = CAIRO_FONT_SLANT_OBLIQUE,
    SItalic = CAIRO_FONT_SLANT_ITALIC
  };

  enum FontWeight { WNormal = CAIRO_FONT_WEIGHT_NORMAL, WBold = CAIRO_FONT_WEIGHT_BOLD };

  struct MYSQLCANVAS_PUBLIC_FUNC FontSpec {
    std::string family;
    FontSlant slant;
    FontWeight weight;
    float size;

    inline auto operator=(const FontSpec &font) -> FontSpec & {
      family = font.family;
      slant = font.slant;
      weight = font.weight;
      size = font.size;

      return *this;
    }

    inline auto operator!=(const FontSpec &font) const -> bool {
      return (family != font.family || slant != font.slant || weight != font.weight) || size != font.size;
    }

    inline auto operator==(const FontSpec &font) const -> bool {
      return (family == font.family && slant == font.slant && weight == font.weight && size == font.size);
    }

    FontSpec(const FontSpec &other) : family(other.family), slant(other.slant), weight(other.weight), size(other.size) {
    }

    FontSpec() : family(DEFAULT_FONT_FACE), slant(SNormal), weight(WNormal), size(12) {
    }

    FontSpec(const std::string &afamily, FontSlant aslant = SNormal, FontWeight aweight = WNormal, float asize = 12.0)
      : family(afamily), slant(aslant), weight(aweight), size(asize) {
    }

    auto toggle_bold(bool flag) -> void {
      weight = flag ? WBold : WNormal;
    }
    auto toggle_italic(bool flag) -> void {
      slant = flag ? SItalic : SNormal;
    }

    static auto from_string(const std::string &spec) -> FontSpec {
      std::string font;
      float size;
      bool bold;
      bool italic;
      if (base::parse_font_description(spec, font, size, bold, italic))
        return FontSpec(font, italic ? SItalic : SNormal, bold ? WBold : WNormal, size);
      else
        return FontSpec();
    }
  };

  class canvas_error : public std::runtime_error {
  public:
    canvas_error(const std::string &msg) : std::runtime_error(msg) {};
  };

  class MYSQLCANVAS_PUBLIC_FUNC Surface {
  protected:
    cairo_surface_t *surface;

    Surface() : surface(0) {
    }

  public:
    Surface(const Surface &other);
    Surface(cairo_surface_t *surface);

    virtual ~Surface();

    auto operator=(const Surface &s) -> Surface & {
      if (this != &s) {
        if (surface != NULL)
          cairo_surface_destroy(surface);
        surface = cairo_surface_reference(s.surface);
      }

      return *this;
    }

    auto get_surface() const -> cairo_surface_t * {
      return surface;
    }
  };

  class MYSQLCANVAS_PUBLIC_FUNC PDFSurface : public Surface {
  public:
    PDFSurface(cairo_surface_t *surface) : Surface(surface) {
    }
    PDFSurface(const std::string &path, double width, double height);
  };

  class MYSQLCANVAS_PUBLIC_FUNC PSSurface : public Surface {
  public:
    PSSurface(cairo_surface_t *surface) : Surface(surface) {
    }
    PSSurface(const std::string &path, double width, double height);
  };

  class MYSQLCANVAS_PUBLIC_FUNC ImageSurface : public Surface {
  public:
    ImageSurface(double width, double height, cairo_format_t format);
    auto save_to_png(const std::string &destination) const -> void;
  };

#ifdef _MSC_VER
  class MYSQLCANVAS_PUBLIC_FUNC Win32Surface : public Surface {
  public:
    Win32Surface(HDC hdc, bool printing = false);
  };
#endif

  class FontManager;

  struct MYSQLCANVAS_PUBLIC_FUNC CairoCtx {
  private:
    cairo_t *cr;

    FontManager *fm;

    bool _free_cr;

  public:
    CairoCtx();
    CairoCtx(cairo_t *cr);
    CairoCtx(cairo_surface_t *surf);
    CairoCtx(const Surface &surf);
    ~CairoCtx();

    auto check_state() const -> void;

    auto update_cairo_backend(cairo_surface_t *surface) -> void;
    inline auto get_cr() -> cairo_t * {
      return cr;
    }

    inline auto save() const -> void {
      cairo_save(cr);
      check_state();
    }
    inline auto restore() const -> void {
      cairo_restore(cr);
      check_state();
    }
    inline auto show_page() -> void {
      cairo_show_page(cr);
    }

    inline auto translate(const base::Point &p) -> void {
      cairo_translate(cr, p.x, p.y);
    }
    inline auto translate(double x, double y) -> void {
      cairo_translate(cr, x, y);
    }
    inline auto scale(const base::Point &p) -> void {
      cairo_scale(cr, p.x, p.y);
    }
    inline auto scale(double x, double y) -> void {
      cairo_scale(cr, x, y);
    }
    inline auto rotate(double rad) -> void {
      cairo_rotate(cr, rad);
    }

    inline auto set_line_width(double width) -> void {
      cairo_set_line_width(cr, width);
    }
    inline auto set_line_cap(cairo_line_cap_t t) -> void {
      cairo_set_line_cap(cr, t);
    }
    inline auto set_line_join(cairo_line_join_t t) -> void {
      cairo_set_line_join(cr, t);
    }
    inline auto set_miter_limit(double l) -> void {
      cairo_set_miter_limit(cr, l);
    }

    inline auto user_to_device(double *x, double *y) -> void {
      cairo_user_to_device(cr, x, y);
    };
    inline auto device_to_user(double *x, double *y) -> void {
      cairo_device_to_user(cr, x, y);
    };
    inline auto set_dash(double dashes[], int ndashes, double offset) -> void {
      cairo_set_dash(cr, dashes, ndashes, offset);
    }
    inline void set_operator(cairo_operator_t oper) {
      cairo_set_operator(cr, oper);
    }

    inline auto set_color(const base::Color &color) const -> void {
      if (color.alpha == 1.0)
        cairo_set_source_rgb(cr, color.red, color.green, color.blue);
      else
        cairo_set_source_rgba(cr, color.red, color.green, color.blue, color.alpha);
    }

    inline auto set_color(const base::Color &color, double alpha) const -> void {
      cairo_set_source_rgba(cr, color.red, color.green, color.blue, alpha);
    }

    auto set_font(const FontSpec &font) const -> void;
    auto get_text_extents(const FontSpec &font, const std::string &text, cairo_text_extents_t &extents) -> void;
    auto get_text_extents(const FontSpec &font, const char *text, cairo_text_extents_t &extents) -> void;
    auto get_font_extents(const FontSpec &font, cairo_font_extents_t &extents) -> bool;

    inline auto set_source_surface(cairo_surface_t *srf, double x, double y) -> void {
      cairo_set_source_surface(cr, srf, x, y);
    }

    inline auto set_mask(cairo_pattern_t *pat) -> void {
      cairo_mask(cr, pat);
    }

    inline auto set_mask_surface(cairo_surface_t *surf, double x, double y) -> void {
      cairo_mask_surface(cr, surf, x, y);
    }

    inline auto set_pattern(cairo_pattern_t *pat) -> void {
      cairo_set_source(cr, pat);
    }

    inline auto paint() -> void {
      cairo_paint(cr);
    }
    inline auto paint_with_alpha(double a) -> void {
      cairo_paint_with_alpha(cr, a);
    }

    inline auto clip() -> void {
      cairo_clip(cr);
    }

    inline auto stroke() -> void {
      cairo_stroke(cr);
    }
    inline auto fill() -> void {
      cairo_fill(cr);
    }
    inline auto stroke_preserve() -> void {
      cairo_stroke_preserve(cr);
    }
    inline auto fill_preserve() -> void {
      cairo_fill_preserve(cr);
    }

    inline auto move_to(const base::Point &pt) -> void {
      cairo_move_to(cr, pt.x, pt.y);
    }
    inline auto move_to(double x, double y) -> void {
      cairo_move_to(cr, x, y);
    }
    inline auto rel_move_to(double x, double y) -> void {
      cairo_rel_move_to(cr, x, y);
    }

    inline auto line_to(const base::Point &pt) -> void {
      cairo_line_to(cr, pt.x, pt.y);
    }
    inline auto line_to(double x, double y) -> void {
      cairo_line_to(cr, x, y);
    }

    inline auto arc(double cx, double cy, double r, double start, double end) -> void {
      cairo_arc(cr, cx, cy, r, start, end);
    }

    inline auto show_text(const std::string &text) -> void {
      cairo_show_text(cr, text.c_str());
    }

    inline auto new_path() -> void {
      cairo_new_path(cr);
    }
    inline auto close_path() -> void {
      cairo_close_path(cr);
    }

    inline auto rectangle(const base::Rect &rect) -> void {
      cairo_rectangle(cr, rect.left(), rect.top(), rect.width(), rect.height());
    }
    inline auto rectangle(double x, double y, double w, double h) -> void {
      cairo_rectangle(cr, x, y, w, h);
    }
  };

#define DOUBLE_CLICK_DELAY 0.400

  MYSQLCANVAS_PUBLIC_FUNC auto get_time() -> Timestamp;

  auto write_to_surface(void *closure, const unsigned char *data, unsigned int length) -> cairo_status_t;

} // namespace mdc
