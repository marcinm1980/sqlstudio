/*
 * Copyright (c) 2007, 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "base/file_utilities.h"
#include "base/threading.h"

#ifndef _MSC_VER
#include <cairo/cairo-pdf.h>
#include <cairo/cairo-ps.h>
#include <cairo/cairo-svg.h>
#include <cairo/cairo.h>
#include <math.h>
#define OutputDebugStringA printf
#endif

#include "mdc_canvas_view.h"
#include "mdc_algorithms.h"
#include "mdc_layouter.h"
#include "mdc_selection.h"
#include "mdc_back_layer.h"
#include "mdc_interaction_layer.h"
#include "mdc_area_group.h"

#include "mdc_line.h"

using namespace mdc;
using namespace base;

// XXX: use the values defined by the platform!
#define DOUBLE_CLICK_TIME 0.5

#include <stdio.h>

//----------------------------------------------------------------------------------------------------------------------

struct CanvasAutoLock {
  CanvasView *canvas;

  CanvasAutoLock(CanvasView *view) : canvas(view) {
    canvas->lock();
  }
  ~CanvasAutoLock() {
    canvas->unlock();
  }
};

//----------------------------------------------------------------------------------------------------------------------

CanvasView::CanvasView(int width, int height) : _fps(0), _total_item_cache_mem(0), _last_click_info(3) {

  _page_size = Size(2000, 1500);
  _x_page_num = 1;
  _y_page_num = 1;

  _zoom = 1.0;
  _offset = Point(0, 0);
  _view_width = width;
  _view_height = height;

  _grid_size = 10.0;
  _grid_snapping = false;

  _user_data = 0;

  _line_hop_rendering = true;

  _crsurface = 0;
  _cairo = 0;

  _default_font = FontSpec("Helvetica");

  cairo_matrix_init_identity(&_trmatrix);

  _event_state = SNone;
  _last_click_item = 0;
  _last_over_item = 0;
  _focused_item = 0;

  _repaint_lock = 0;
  _repaints_missed = 0;
  _ui_lock = 0;

  _printout_mode = false;

  _destroying = false;
  _debug = false;

  _blayer = new BackLayer(this);
  _ilayer = new InteractionLayer(this);

  _current_layer = new_layer("Default Layer");

  _selection = new Selection(this);
}

//----------------------------------------------------------------------------------------------------------------------

CanvasView::~CanvasView() {
  delete _blayer;
  delete _ilayer;

  LayerList::const_iterator next, iter = _layers.begin();
  while (iter != _layers.end()) {
    next = iter;
    ++next;
    delete *iter;
    iter = next;
  }

  // selection must be deleted after layers
  delete _selection;
  _selection = 0;

  delete _cairo;

  if (_crsurface) {
    cairo_surface_destroy(_crsurface);
    _crsurface = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_tag(const std::string &tag) -> void {
  _tag = tag;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_printout_mode(bool flag) -> void {
  _printout_mode = flag;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::initialize() -> bool {
  update_view_size(_view_width, _view_height);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::pre_destroy() -> void {
  _destroying = true;

  LayerList::const_iterator next, iter = _layers.begin();
  while (iter != _layers.end()) {
    next = iter;
    ++next;
    delete *iter;
    iter = next;
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::lock_ui() -> void {
  _ui_lock++;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::unlock_ui() -> void {
  _ui_lock--;
}

//----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Locks the canvas for multi-threaded access.
 *
 * Currently the canvas is locked when:
 * - event handler functions are called
 * - repaint is called
 *
 * You must lock when:
 * - adding new item to canvas
 *
 * @param
 *
 * @return
 */
auto CanvasView::lock() -> void {
  _lock.lock();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::unlock() -> void {
  _lock.unlock();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::lock_redraw() -> void {
  _repaint_lock++;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::unlock_redraw() -> void {
  if (_repaint_lock == 0)
    throw std::logic_error("unlock_redraw() called without matching lock_redraw()");
  _repaint_lock--;

  if (_repaint_lock == 0 && _repaints_missed > 0) {
    queue_repaint();
  }
}

//----------------------------------------------------------------------------------------------------------------------

// Geometry Handling

auto CanvasView::get_total_view_size() const -> Size {
  return Size(_x_page_num * _page_size.width, _y_page_num * _page_size.height);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_viewport_range() const -> Rect {
  return Rect(0, 0, _x_page_num * _page_size.width, _y_page_num * _page_size.height);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_viewport() const -> Rect {
  Rect rect = window_to_canvas(0, 0, _view_width, _view_height);
  Size size = get_total_view_size();

  rect.size.width = std::min(size.width, rect.size.width);
  rect.size.height = std::min(size.height, rect.size.height);

  if (rect.pos.x < 0)
    rect.pos.x = 0;
  if (rect.pos.y < 0)
    rect.pos.y = 0;

  return rect;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_viewable_size() const -> Size {
  return window_to_canvas(0, 0, _view_width, _view_height).size;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_offset(const Point &offs) -> void {
  Size viewable_size(get_viewable_size());
  Size total_size(get_total_view_size());
  Point new_offset;

  new_offset = offs.round();

  new_offset.x = std::max(0.0, std::min(new_offset.x, total_size.width - viewable_size.width));
  new_offset.y = std::max(0.0, std::min(new_offset.y, total_size.height - viewable_size.height));

  if (new_offset != _offset) {
    _offset = new_offset;
    update_offsets();
    queue_repaint();

    _viewport_changed_signal();
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::scroll_to(const Point &pos) -> void {
  set_offset(pos);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_zoom(float zoom) -> void {
  if (_zoom != zoom) {
    _zoom = zoom;
    update_offsets();
    queue_repaint();

    // Zoom notification is potentially slow, so do the viewport update first
    // to get the display (e.g. scrollbars, paper etc.) looking ok asap.
    _viewport_changed_signal();
    _zoom_changed_signal();
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_page_size(const Size &size) -> void {
  if (_page_size != size) {
    _page_size = size;
    update_offsets();
    queue_repaint();

    // update root area sizes
    for (LayerList::iterator iter = _layers.begin(); iter != _layers.end(); ++iter)
      (*iter)->get_root_area_group()->resize_to(get_total_view_size());

    _resized_signal();

    _viewport_changed_signal();
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_page_layout(Count xpages, Count ypages) -> void {
  _x_page_num = xpages;
  _y_page_num = ypages;
  update_offsets();
  queue_repaint();

  _resized_signal();

  _viewport_changed_signal();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::update_offsets() -> void {
  Size total_size(get_total_view_size());
  Size view_size(get_viewable_size());

  // check if visible area is smaller than available area
  if (total_size.width < view_size.width)
    _extra_offset.x = ceil((view_size.width - total_size.width) / 2);
  else
    _extra_offset.x = 0;

  if (total_size.height < view_size.height)
    _extra_offset.y = ceil((view_size.height - total_size.height) / 2);
  else
    _extra_offset.y = 0;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::apply_transformations_for_conversion(cairo_matrix_t *matrix) const -> void {
  Point offs;

  cairo_matrix_init_scale(matrix, _zoom, _zoom);

  cairo_matrix_translate(matrix, -_offset.x + _extra_offset.x, -_offset.y + _extra_offset.y);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::apply_transformations() -> void {
  Point offs;

  cairo_matrix_init_scale(&_trmatrix, _zoom, _zoom);

  cairo_matrix_translate(&_trmatrix, -_offset.x + _extra_offset.x, -_offset.y + _extra_offset.y);
  cairo_set_matrix(_cairo->get_cr(), &_trmatrix);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::apply_transformations_gl() -> void {
#ifndef __APPLE__
  glViewport(0, 0, _view_width, _view_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, _view_width, _view_height, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glScalef(_zoom, _zoom, 1.0);

  glTranslated(-_offset.x + _extra_offset.x, -_offset.y + _extra_offset.y, 0);
#endif
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::reset_transformations_gl() -> void {
#ifndef __APPLE__
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
#endif
}

//----------------------------------------------------------------------------------------------------------------------

// Grid

auto CanvasView::set_grid_snapping(bool flag) -> void {
  _grid_snapping = flag;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_grid_snapping() -> bool {
  return _grid_snapping;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::snap_to_grid(const Point &pos) -> Point {
  if (_grid_snapping) {
    return Point((int)((pos.x + _grid_size / 2) / _grid_size) * _grid_size,
                 (int)((pos.y + _grid_size / 2) / _grid_size) * _grid_size);
  }
  return pos;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::snap_to_grid(const Size &size) -> Size {
  if (_grid_snapping) {
    return Size(std::max((int)(size.width / _grid_size) * _grid_size, _grid_size),
                std::max((int)(size.height / _grid_size) * _grid_size, _grid_size));
  }
  return size;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::show_grid() -> void {
  _blayer->set_grid_visible(true);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::hide_grid() -> void {
  _blayer->set_grid_visible(false);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_grid_shown() -> bool {
  return _blayer->visible();
}

//----------------------------------------------------------------------------------------------------------------------

// Layer Handling

static auto layer_destroyed_cb(void *data) -> void * {
  std::pair<Layer *, CanvasView *> *pair = reinterpret_cast<std::pair<Layer *, CanvasView *> *>(data);

  pair->second->remove_layer(pair->first);

  delete pair;

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::new_layer(const std::string &name) -> Layer * {
  Layer *layer = new Layer(this);

  layer->add_destroy_notify_callback(new std::pair<Layer *, CanvasView *>(layer, this), layer_destroyed_cb);

  layer->set_name(name);

  add_layer(layer);

  return layer;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_current_layer(Layer *layer) -> void {
  _current_layer = layer;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_layer(const std::string &name) -> Layer * {
  for (std::list<mdc::Layer *>::const_iterator iter = _layers.begin(); iter != _layers.end(); ++iter) {
    if ((*iter)->get_name() == name)
      return (*iter);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::add_layer(Layer *layer) -> void {
  CanvasAutoLock lock(this);

  _layers.push_front(layer);

  queue_repaint();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::remove_layer(Layer *layer) -> void {
  CanvasAutoLock lock(this);

  _layers.erase(std::find(_layers.begin(), _layers.end(), layer));

  if (_current_layer == layer) {
    if (_layers.empty())
      _current_layer = 0;
    else
      _current_layer = _layers.front();
  }
  queue_repaint();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_needs_repaint_all_items() -> void {
  for (std::list<mdc::Layer *>::const_iterator iter = _layers.begin(); iter != _layers.end(); ++iter)
    (*iter)->set_needs_repaint_all_items();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_layers() -> CanvasView::LayerList & {
  return _layers;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::raise_layer(Layer *layer, Layer *above) -> void {
  CanvasAutoLock lock(this);

  restack_up(_layers, layer, above);

  queue_repaint();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::lower_layer(Layer *layer) -> void {
  CanvasAutoLock lock(this);

  restack_down(_layers, layer);

  queue_repaint();
}

//----------------------------------------------------------------------------------------------------------------------

static auto is_line(CanvasItem *item) -> bool {
  if (item->get_visible()) {
    Line *line = dynamic_cast<Line *>(item);
    if (line && line->get_hops_crossings())
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_draws_line_hops(bool flag) -> void {
  _line_hop_rendering = flag;
  queue_repaint();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::update_line_crossings(Line *line) -> void {
  if (!_line_hop_rendering)
    return;

  // get the lines in the same bounding box

  std::list<CanvasItem *> items = get_items_bounded_by(line->get_root_bounds(),  std::bind(&is_line, std::placeholders::_1));

  std::list<CanvasItem *>::iterator iter = items.begin();

  // check if the line crosses with anything under it
  for (; iter != items.end() && *iter != line; ++iter) {
    line->mark_crossings(static_cast<Line *>(*iter));
  }

  if (iter != items.end())
    ++iter; // skip the line itself

  // then check if anything over it crosses the line
  for (; iter != items.end(); ++iter) {
    static_cast<Line *>(*iter)->mark_crossings(line);
  }
}

auto CanvasView::remove_item(mdc::CanvasItem *item) -> void {
  if (item->get_layer())
    item->get_layer()->remove_item(item);

  if (_last_click_item && _last_click_item->get_common_ancestor(item) == item)
    _last_click_item = 0;

  if (_last_over_item && _last_over_item->get_common_ancestor(item) == item)
    _last_over_item = 0;
}

//----------------------------------------------------------------------------------------------------------------------

// Coordinate Transformation

auto CanvasView::window_to_canvas(int x, int y) const -> Point {
  cairo_matrix_t mtx;
  Point pt;
  double xx = x;
  double yy = y;

  apply_transformations_for_conversion(&mtx);
  cairo_matrix_invert(&mtx);

  cairo_matrix_transform_point(&mtx, &xx, &yy);

  pt.x = xx;
  pt.y = yy;

  return pt;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::window_to_canvas(int x, int y, int w, int h) const -> Rect {
  cairo_matrix_t mtx;
  Rect rect;
  double xx = x;
  double yy = y;

  apply_transformations_for_conversion(&mtx);
  cairo_matrix_invert(&mtx);

  cairo_matrix_transform_point(&mtx, &xx, &yy);
  rect.pos.x = xx;
  rect.pos.y = yy;

  xx = w;
  yy = h;
  cairo_matrix_transform_distance(&mtx, &xx, &yy);
  rect.size.width = xx;
  rect.size.height = yy;

  return rect;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::canvas_to_window(const Point &pt, int &x, int &y) const -> void {
  cairo_matrix_t mtx;
  double xx = pt.x;
  double yy = pt.y;

  apply_transformations_for_conversion(&mtx);

  cairo_matrix_transform_point(&mtx, &xx, &yy);

  x = (int)(xx + .5);
  y = (int)(yy + .5);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::canvas_to_window(const Rect &rect, int &x, int &y, int &w, int &h) const -> void {
  cairo_matrix_t mtx;
  double xx = rect.left();
  double yy = rect.top();
  double ww = rect.width();
  double hh = rect.height();

  apply_transformations_for_conversion(&mtx);

  cairo_matrix_transform_point(&mtx, &xx, &yy);
  cairo_matrix_transform_distance(&mtx, &ww, &hh);

  x = (int)(xx + .5);
  y = (int)(yy + .5);
  w = (int)(ww + .5);
  h = (int)(hh + .5);
}

//----------------------------------------------------------------------------------------------------------------------

// Font Management

auto CanvasView::get_default_font() -> const FontSpec & {
  return _default_font;
}

//----------------------------------------------------------------------------------------------------------------------

// Dragging Rectangle

auto CanvasView::start_dragging_rectangle(const Point &pos) -> void {
  _ilayer->start_dragging_rectangle(pos);
}

auto CanvasView::finish_dragging_rectangle() -> Rect {
  return _ilayer->finish_dragging_rectangle();
}

//----------------------------------------------------------------------------------------------------------------------

// Rendering

auto CanvasView::paint_item_cache(CairoCtx *cr, double x, double y, cairo_surface_t *cached_item, double alpha) -> void {
  cairo_matrix_t mtx;

  cairo_user_to_device(cr->get_cr(), &x, &y);

  cr->save();

  cairo_matrix_init_scale(&mtx, 1, 1);
  cairo_set_matrix(cr->get_cr(), &mtx);

  cairo_device_to_user(cr->get_cr(), &x, &y);

  cr->translate(floor(x), floor(y));
  cr->set_source_surface(cached_item, 0.0, 0.0);

  if (alpha < 1.0)
    cr->paint_with_alpha(alpha);
  else
    cr->paint();

#ifdef DEBUG
  if (getenv("DEBUG_CANVAS")) {
    cr->rectangle(0, 0, cairo_image_surface_get_width(cached_item) - 1,
                  cairo_image_surface_get_height(cached_item) - 1);
    cr->set_color(Color((rand() % 100) / 100.0, (rand() % 100) / 100.0, (rand() % 100) / 100.0));
    cr->stroke();
  }
#endif

  cr->restore();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::repaint() -> void {
  if (_ui_lock > 0)
    return;

  repaint(0, 0, _view_width, _view_height);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::repaint(int x, int y, int width, int height) -> void {
  if (_ui_lock > 0)
    return;

  CanvasAutoLock lock(this);
  repaint_area(window_to_canvas(x, y, width, height), x, y, width, height);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::repaint_area(const Rect &aBounds, int wx, int wy, int ww, int wh) -> void {
  if (_destroying || _ui_lock > 0)
    return;

  Rect bounds;

  if (has_gl())
    bounds = window_to_canvas(0, 0, _view_width, _view_height);
  else
    bounds = aBounds;

  CanvasAutoLock lock(this);
  Rect clip;

  begin_repaint(wx, wy, ww, wh);
  if (has_gl())
    glGetError(); // Resets error flag.

  _cairo->save();

  apply_transformations();

  if (has_gl())
    apply_transformations_gl();

  Rect vrect = get_viewport();

  if (_blayer->visible())
    _blayer->repaint(bounds);

  clip.set_xmin(std::max(vrect.left(), bounds.left()));
  clip.set_ymin(std::max(vrect.top(), bounds.top()));

  clip.set_xmax(std::min(vrect.right(), bounds.right()));
  clip.set_ymax(std::min(vrect.bottom(), bounds.bottom()));

  clip = bounds;

  _cairo->save();

  // Clip so that only the affected area is redrawn.
  _cairo->rectangle(clip);
  _cairo->clip();

  // Repaint layers from back to front.
  for (LayerList::reverse_iterator iter = _layers.rbegin(); iter != _layers.rend(); ++iter) {
    if ((*iter)->visible())
      (*iter)->repaint(bounds);
  }

  _cairo->restore();

  if (_ilayer->visible())
    _ilayer->repaint(bounds);

  _cairo->restore();

  end_repaint();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::queue_repaint() -> void {
  if (_repaint_lock > 0 || _destroying) {
    _repaints_missed++;
    return;
  }

  _repaints_missed = 0;
  lock();
  _need_repaint_signal(0, 0, _view_width, _view_height);
  unlock();
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::queue_repaint(const Rect &bounds) -> void {
  if (_repaint_lock > 0 || _destroying) {
    _repaints_missed++;
    return;
  }

  _repaints_missed = 0;

  {
    int x, y;
    int width, height;

    canvas_to_window(bounds, x, y, width, height);
    _need_repaint_signal(std::max(0, x - 1), std::max(0, y - 1), width + 2, height + 2);
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_content_bounds() const -> Rect {
  Size vs = get_total_view_size();
  double minx = vs.width, miny = vs.height, maxx = 0.0, maxy = 0.0;

  for (LayerList::const_iterator iter = _layers.begin(); iter != _layers.end(); ++iter) {
    if ((*iter)->visible()) {
      Rect rect = (*iter)->get_bounds_of_item_list((*iter)->get_root_area_group()->get_contents());
      if (rect.width() > 0 && rect.height() > 0) {
        minx = std::min(minx, rect.left());
        miny = std::min(miny, rect.top());
        maxx = std::max(maxx, rect.right());
        maxy = std::max(maxy, rect.bottom());
      }
    }
  }

  if (minx < maxx && miny < maxy)
    return Rect(minx, miny, maxx - minx, maxy - miny);
  return Rect(0, 0, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::export_png(const std::string &filename, bool crop) -> void {
  CanvasAutoLock lock(this);

  base::FileHandle fh(filename.c_str(), "wb");
  Size vsize = get_total_view_size();

  Rect bounds = get_content_bounds();
  if (!crop) {
    bounds.pos.x = 0;
    bounds.pos.y = 0;
    bounds.size = vsize;
  } else {
    bounds.pos.x = std::max(bounds.pos.x - 10, 0.0);
    bounds.pos.y = std::max(bounds.pos.y - 10, 0.0);
    bounds.size.width += 20;
    bounds.size.height += 20;
  }

  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, (int)bounds.width(), (int)bounds.height());
  try {
    CairoCtx ctx(surface);

    ctx.rectangle(0, 0, bounds.width(), bounds.height());
    ctx.set_color(Color::white());
    ctx.fill();
    render_for_export(bounds, &ctx);

    cairo_status_t status;

    if ((status = cairo_surface_write_to_png_stream(surface, &write_to_surface, fh.file())) != CAIRO_STATUS_SUCCESS)
      throw canvas_error(cairo_status_to_string(status));
  } catch (std::exception &) {
    cairo_surface_destroy(surface);
    throw;
  }
  cairo_surface_destroy(surface);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::export_pdf(const std::string &filename, const Size &size_in_pt) -> void {
  CanvasAutoLock lock(this);

  FileHandle fh(filename.c_str(), "wb");
  Size vsize = get_total_view_size();
  cairo_surface_t *surface =
    cairo_pdf_surface_create_for_stream(&write_to_surface, fh.file(), size_in_pt.width, size_in_pt.height);
  try {
    CairoCtx ctx(surface);
    ctx.check_state();

    ctx.scale(Point(size_in_pt.width / vsize.width, size_in_pt.width / vsize.width));

    render_for_export(Rect(Point(0, 0), vsize), &ctx);

    ctx.show_page();

    ctx.check_state();
  } catch (std::exception &) {
    cairo_surface_destroy(surface);
    throw;
  }
  cairo_surface_destroy(surface);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::export_ps(const std::string &filename, const Size &size_in_pt) -> void {
  CanvasAutoLock lock(this);

  FileHandle fh(filename.c_str(), "wb");
  Size vsize = get_total_view_size();
  cairo_surface_t *surface =
    cairo_ps_surface_create_for_stream(&write_to_surface, fh.file(), size_in_pt.width, size_in_pt.height);
  try {
    CairoCtx ctx(surface);
    ctx.check_state();

    ctx.scale(Point(size_in_pt.width / vsize.width, size_in_pt.width / vsize.width));

    render_for_export(Rect(Point(0, 0), vsize), &ctx);

    ctx.show_page();

    ctx.check_state();
  } catch (std::exception &) {
    cairo_surface_destroy(surface);
    throw;
  }
  cairo_surface_destroy(surface);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::export_svg(const std::string &filename, const Size &size_in_pt) -> void {
  CanvasAutoLock lock(this);

  FileHandle fh(filename.c_str(), "wb");
  Size vsize = get_total_view_size();
  cairo_surface_t *surface =
    cairo_svg_surface_create_for_stream(&write_to_surface, fh.file(), size_in_pt.width, size_in_pt.height);
  try {
    CairoCtx ctx(surface);
    ctx.check_state();

    ctx.scale(Point(size_in_pt.width / vsize.width, size_in_pt.width / vsize.width));

    render_for_export(Rect(Point(0, 0), vsize), &ctx);

    ctx.show_page();

    ctx.check_state();
  } catch (std::exception &) {
    cairo_surface_destroy(surface);
    throw;
  }
  cairo_surface_destroy(surface);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::render_for_export(const Rect &bounds, CairoCtx *ctx) -> void {
  CairoCtx *oldcr = _cairo;

  if (ctx)
    _cairo = ctx;

  set_printout_mode(true);

  _cairo->save();

  _cairo->translate(-bounds.left(), -bounds.top());

  _cairo->rectangle(bounds);
  _cairo->clip();

  // repaint layers from bottom to top
  for (LayerList::reverse_iterator iter = _layers.rbegin(); iter != _layers.rend(); ++iter) {
    if ((*iter)->visible())
      (*iter)->repaint_for_export(bounds);
  }

  set_printout_mode(false);

  _cairo->restore();

  _cairo = oldcr;
}

//----------------------------------------------------------------------------------------------------------------------

// Selection/Focusing

auto CanvasView::focus_item(CanvasItem *item) -> bool {
  if (get_focused_item() != item) {
    CanvasItem *old_item = _focused_item;

    if (_focused_item)
      _focused_item->destroy_handles();

    if (item && item->accepts_focus()) {
      item->set_focused(true);
      item->create_handles(_ilayer);
      _focused_item = item;
    } else
      _focused_item = 0;

    // remove the focus after the new item gets the focus
    // so that we can know who is the new focused item when the item
    // is unfocused
    if (old_item)
      old_item->set_focused(false);

    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_focused_item() -> CanvasItem * {
  return _focused_item;
}

auto CanvasView::select_items_inside(const Rect &rect, SelectType type, Group *group) -> void {
  if (type == SelectAdd) {
    for (std::list<Layer *>::iterator it = _layers.begin(); it != _layers.end(); ++it) {
      std::list<CanvasItem *> selection((*it)->get_items_bounded_by(rect, mdc::Layer::ItemCheckFunc(), group));

      _selection->add(selection);
    }
  } else if (type == SelectToggle) {
    for (std::list<Layer *>::iterator it = _layers.begin(); it != _layers.end(); ++it) {
      std::list<CanvasItem *> selection((*it)->get_items_bounded_by(rect, mdc::Layer::ItemCheckFunc(), group));

      _selection->toggle(selection);
    }
  } else {
    _selection->remove_items_outside(rect);

    if (rect.width() > 0 && rect.height() > 0) {
      // remake selection list
      for (std::list<Layer *>::iterator it = _layers.begin(); it != _layers.end(); ++it) {
        std::list<CanvasItem *> selection((*it)->get_items_bounded_by(rect, mdc::Layer::ItemCheckFunc(), group));
        if (selection.size() > 0)
          _selection->add(selection);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_selected_items() -> Selection::ContentType {
  if (_selection)
    return _selection->get_contents();
  return Selection::ContentType();
}

//----------------------------------------------------------------------------------------------------------------------

// Item Finding

auto CanvasView::get_item_at(int x, int y) -> CanvasItem * {
  return get_item_at(window_to_canvas(x, y));
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_item_at(const Point &point) -> CanvasItem * {
  for (LayerList::iterator iter = _layers.begin(); iter != _layers.end(); ++iter) {
    CanvasItem *item;

    if ((*iter)->visible()) {
      if ((item = (*iter)->get_top_item_at(point)))
        return item;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_leaf_item_at(int x, int y) -> CanvasItem * {
  return get_leaf_item_at(window_to_canvas(x, y));
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_leaf_item_at(const Point &point) -> CanvasItem * {
  CanvasItem *item = get_item_at(point);
  Layouter *layouter = dynamic_cast<Layouter *>(item);

  if (layouter) {
    CanvasItem *tmp = layouter->get_item_at(layouter->get_parent()->convert_point_from(point, 0));
    if (tmp)
      return tmp;
  }
  return item;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::get_items_bounded_by(const Rect &rect, const ItemCheckFunc &pred) -> std::list<CanvasItem *> {
  std::list<CanvasItem *> result;

  for (LayerList::iterator iter = _layers.begin(); iter != _layers.end(); ++iter) {
    if ((*iter)->visible()) {
      std::list<CanvasItem *> tmp = (*iter)->get_items_bounded_by(rect, pred);

      result.insert(result.end(), tmp.begin(), tmp.end());
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------

// Base Event Handling

auto CanvasView::set_event_callbacks(
  const std::function<bool(CanvasView *, MouseButton, bool, Point, EventState)> &button_handler,
  const std::function<bool(CanvasView *, Point, EventState)> &motion_handler,
  const std::function<bool(CanvasView *, KeyInfo, EventState, bool)> &key_handler) -> void {
  _button_event_relay = button_handler;
  _motion_event_relay = motion_handler;
  _key_event_relay = key_handler;
}

//----------------------------------------------------------------------------------------------------------------------

static auto propagate_event(mdc::CanvasItem *item,
  const std::function<bool(mdc::CanvasItem *, mdc::CanvasItem *, const Point &)> &functor, const Point &pos) -> bool {
  mdc::CanvasItem *target = item;

  while (item) {
    Point p;

    // convert pos to the item's local coordinate system
    p = item->convert_point_from(pos, 0);

    // offer it to the item
    if (functor(item, target, p))
      return true;

    if (item->is_toplevel())
      break;

    // if not handled, then propagate to its parent
    item = item->get_parent();
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

static auto propagate_event(
  mdc::CanvasItem *item,
  const std::function<bool(mdc::CanvasItem *, mdc::CanvasItem *, const Point &, mdc::EventState)> &functor,
  const Point &pos, mdc::EventState arg1) -> bool {
  mdc::CanvasItem *target = item;

  while (item) {
    Point p;

    // convert pos to the item's local coordinate system
    p = item->convert_point_from(pos, 0);

    // offer it to the item
    if (functor(item, target, p, arg1))
      return true;

    if (item->is_toplevel())
      break;

    // if not handled, then propagate to its parent
    item = item->get_parent();
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

static auto propagate_event(mdc::CanvasItem *item,
  const std::function<bool(mdc::CanvasItem *, mdc::CanvasItem *, const Point &,
  mdc::MouseButton, mdc::EventState)> &functor, const Point &pos, mdc::MouseButton arg1, mdc::EventState arg2) -> bool {
  mdc::CanvasItem *target = item;

  while (item) {
    Point p;

    // convert pos to the item's local coordinate system
    p = item->convert_point_from(pos, 0);

    // offer it to the item
    if (functor(item, target, p, arg1, arg2))
      return true;

    if (item->is_toplevel())
      break;

    // if not handled, then propagate to its parent
    item = item->get_parent();
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::perform_auto_scroll(const Point &mouse_pos) -> bool {
  double dx = 0.0;
  double dy = 0.0;

  Rect vp = get_viewport();

  if (mouse_pos.x < vp.left()) {
    dx = mouse_pos.x - vp.left();
    if (dx < -100)
      dx = -100;
  } else if (mouse_pos.x > vp.right()) {
    dx = mouse_pos.x - vp.right();
    if (dx > 100)
      dx = 100;
  }
  dx = ceil(dx / 10);

  if (mouse_pos.y < vp.top()) {
    dy = mouse_pos.y - vp.top();
    if (dy < -100)
      dy = -100;
  } else if (mouse_pos.y > vp.bottom()) {
    dy = mouse_pos.y - vp.bottom();
    if (dy > 100)
      dy = 100;
  }
  dy = ceil(dy / 10);

  set_offset(Point(_offset.x + dx, _offset.y + dy));

  return fabs(dx) > 0 || fabs(dy) > 0;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::handle_mouse_move(int x, int y, EventState state) -> void {
  if (_destroying || _ui_lock > 0)
    return;

  if ((state & (mdc::SEnterMask | mdc::SLeaveMask)) != 0)
    return;

  Point point = window_to_canvas(x, y);
  //  Group *group= 0;
  CanvasItem *item;
  Point offset;

  // drags are only valid for leftbutton
  bool is_dragging = (_event_state & SLeftButtonMask) != 0;

  if (_motion_event_relay && _motion_event_relay(this, point, state))
    return;

  if (is_dragging)
    perform_auto_scroll(point);

  if (_ilayer->handle_mouse_move(point, state)) {
    return;
  }

  // must lock after the external callback otherwise we may get a deadlock
  CanvasAutoLock lock(this);

  if (_destroying)
    return;

  if (is_dragging) {
    // if a button is pressed, the we send a drag event to the item
    // that was under the cursor when the press happened (not the current item)

    propagate_event(_last_click_item, std::bind(&CanvasItem::on_drag, std::placeholders::_1, std::placeholders::_2,
                                                std::placeholders::_3, std::placeholders::_4),
                    point, state);

    return; // if dragging, then don't process boundary crossings (?)
  }

  item = get_leaf_item_at(point);

  // check if we have crossed some item boundary
  if (_last_over_item != item) {
    // we need to send enter and leave events to all items
    // that have been crossed

    /*          ___
     *    _____|   |
     *   |  ___|   |
     * A |B|   | D |
     *   |_|_C_|___|
     *
     * A -> C  =>  enter B, enter C
     * C -> A  =>  leave C, leave B
     * A -> B  =>  enter B
     * B -> A  =>  leave B
     * C -> B  =>  leave C
     * B -> C  =>  enter C
     * C -> D  =>  leave C, leave B, enter D
     * D -> C  =>  leave D, enter B, enter C
     *
     * If difference between old hovered item and new hovered item is
     * positive (climb up hierarchy), then it means we entered items.
     * If negative, then we left items.
     *
     */

    CanvasItem *ancestor, *it;

    if (_last_over_item && item)
      ancestor = _last_over_item->get_common_ancestor(item);
    else
      ancestor = 0;

    // go from _last_over_item to ancestor (exclusive) sending leave events
    for (it = _last_over_item; it != ancestor; it = it->get_parent()) {
      propagate_event(
        it, std::bind(&CanvasItem::on_leave, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        point);
    }

    // go from ancestor (exclusive) to item and send enter events to all

    // first make an inverted list of the items
    std::list<CanvasItem *> list;
    for (it = item; it != ancestor; it = it->get_parent())
      list.push_front(it);

    // then traverse it sending the events
    for (std::list<CanvasItem *>::const_iterator i = list.begin(); i != list.end(); ++i) {
      it = *i;
      propagate_event(
        it, std::bind(&CanvasItem::on_enter, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        point);
    }

    set_last_over_item(item);
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::handle_mouse_button(MouseButton button, bool press, int x, int y, EventState state) -> void {
  if (_destroying || _ui_lock > 0)
    return;

  Point point = window_to_canvas(x, y);
  CanvasItem *item;
  Point offset;
  bool handled = false;

  if (_button_event_relay && _button_event_relay(this, button, press, point, state))
    return;

  // must lock after the external callback otherwise we may get a deadlock
  CanvasAutoLock lock(this);

  if (press)
    _event_state = _event_state + button;
  else
    _event_state = _event_state - button;

  if (_ilayer->handle_mouse_button_top(button, press, point, state)) {
    /* this is handled by the selection code
    // if clicked on nothing, unset focus
    if (!press && !_last_click_item)
      focus_item(0);
     */
    return;
  }

  item = get_leaf_item_at(point);

  if (item && press) {
    // if it's a button press, send the press event to the item at the point
    handled =
      propagate_event(item, std::bind(&CanvasItem::on_button_press, std::placeholders::_1, std::placeholders::_2,
                                      std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                      point, button, state);
  } else if (_last_click_item && !press) {
    // if it's a button release, send the release event to the item that
    // was originally pressed (not the item we're over now)

    handled = propagate_event(_last_click_item,
                              std::bind(&CanvasItem::on_button_release, std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                              point, button, state);
  }

  if (_last_click_item == item && !press) {
    // send click if the button was released over the same item it was pressed
    handled = propagate_event(item, std::bind(&CanvasItem::on_click, std::placeholders::_1, std::placeholders::_2,
                                              std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                              point, button, state);
  }

  set_last_click_item(item);
  _last_mouse_pos = point;
  _last_click_info[button].pos = point;

  if (!handled && !item)
    _ilayer->handle_mouse_button_bottom(button, press, point, state);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::handle_mouse_double_click(MouseButton button, int x, int y, EventState state) -> void {
  if (_destroying || _ui_lock > 0)
    return;

  Point point = window_to_canvas(x, y);

  // Must lock after the external callback otherwise we may get a deadlock.
  CanvasAutoLock lock(this);

  CanvasItem *item = get_leaf_item_at(point);

  // send click if the button was released over the same item it was pressed
  propagate_event(item, std::bind(&CanvasItem::on_double_click, std::placeholders::_1, std::placeholders::_2,
                                  std::placeholders::_3, std::placeholders::_4, std::placeholders::_5),
                  point, button, state);

  set_last_click_item(item);
  _last_mouse_pos = point;
  _last_click_info[button].pos = point;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::handle_mouse_enter(int x, int y, EventState state) -> void {
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::handle_mouse_leave(int x, int y, EventState state) -> void {
  if (_destroying || _ui_lock > 0)
    return;
  Point point = window_to_canvas(x, y);

  bool is_dragging = (_event_state & SLeftButtonMask) != 0;

  if (is_dragging)
    perform_auto_scroll(point);

  // must lock after the external callback otherwise we may get a deadlock
  CanvasAutoLock lock(this);

  if (is_dragging) {
    // if a button is pressed, the we send a drag event to the item
    // that was under the cursor when the press happened (not the current item)

    propagate_event(_last_click_item, std::bind(&CanvasItem::on_drag, std::placeholders::_1, std::placeholders::_2,
                                                std::placeholders::_3, std::placeholders::_4),
                    point, state);

    return; // if dragging, then don't process boundary crossings (?)
  }

  if (_last_over_item) {
    CanvasItem *it;

    // go from _last_over_item to ancestor (exclusive) sending leave events
    for (it = _last_over_item; it != 0; it = it->get_parent()) {
      propagate_event(
        it, std::bind(&CanvasItem::on_leave, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        point);
    }

    set_last_over_item(NULL);
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::handle_key(const KeyInfo &key, bool press, EventState state) -> bool {
#ifdef ___TRACE
  extern auto ___enable_tracing(bool flag) -> void;

  if (press && key.keycode == KF11) {
    ___enable_tracing(true);
  } else if (press && key.keycode == KF12) {
    ___enable_tracing(false);
  }
#endif

  if (_ui_lock > 0)
    return false;

  if (_key_event_relay)
    return _key_event_relay(this, key, state, press);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::canvas_item_destroyed(void *data) -> void * {
  CanvasView *view = (CanvasView *)data;

  view->_last_click_item = NULL;
  view->_last_over_item = NULL;

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_last_click_item(CanvasItem *item) -> void {
  if (_last_click_item != item) {
    // Remove the notification callback for this item if it isn't also stored as hot item
    // (otherwise we still need the notification).
    if (_last_click_item && _last_click_item != _last_over_item)
      _last_click_item->remove_destroy_notify_callback(this);

    _last_click_item = item;

    // Register the notification callback if not yet done (i.e. for the hot item tracking).
    if (_last_click_item && _last_click_item != _last_over_item)
      _last_click_item->add_destroy_notify_callback(this, canvas_item_destroyed);
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::set_last_over_item(CanvasItem *item) -> void {
  if (_last_over_item != item) {
    if (_last_over_item && _last_over_item != _last_click_item)
      _last_over_item->remove_destroy_notify_callback(this);

    _last_over_item = item;
    if (_last_over_item && _last_over_item != _last_click_item)
      _last_over_item->add_destroy_notify_callback(this, canvas_item_destroyed);
  }
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::find_item_with_tag(const std::string &tag) -> CanvasItem * {
  for (LayerList::reverse_iterator iter = _layers.rbegin(); iter != _layers.rend(); ++iter) {
    CanvasItem *item;
    item = (*iter)->get_root_area_group()->find_item_with_tag(tag);
    if (item)
      return item;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::create_temp_surface(const Size &size) const -> Surface* {
  return new ImageSurface(size.width, size.height, CAIRO_FORMAT_ARGB32);
}

//----------------------------------------------------------------------------------------------------------------------

auto CanvasView::setBackgroundColor(base::Color const& color) -> void {
  _blayer->set_color(color);
}

//----------------------------------------------------------------------------------------------------------------------
