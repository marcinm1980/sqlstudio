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

#include "mdc_layer.h"
#include "mdc_canvas_view.h"
#include "mdc_algorithms.h"
#include "mdc_draw_util.h"
#include "mdc_item_handle.h"
#include "mdc_area_group.h"
#include "mdc_selection.h"

using namespace mdc;
using namespace base;

Layer::Layer(CanvasView *view) : _owner(view) {
  _visible = true;
  _needs_repaint = true;

  _root_area = new AreaGroup(this);
  _root_area->resize_to(_owner->get_total_view_size());
  _root_area->set_accepts_focus(false);
  _root_area->set_accepts_selection(false);
  _root_area->set_draw_background(false);

  scoped_connect(view->signal_resized(), std::bind(&Layer::view_resized, this));
}

auto Layer::set_root_area(AreaGroup *group) -> void {
  delete _root_area;
  _root_area = group;
  _root_area->set_cache_toplevel_contents(false);
  _root_area->resize_to(_owner->get_total_view_size());
}

Layer::~Layer() {
  delete _root_area;
}

auto Layer::set_name(const std::string &name) -> void {
  _name = name;
}

auto Layer::view_resized() -> void {
  _root_area->resize_to(_owner->get_total_view_size());
}

auto Layer::set_visible(bool flag) -> void {
  if (_visible != flag) {
    _visible = flag;
    if (flag)
      queue_repaint();
    _owner->queue_repaint();
  }
}

auto Layer::add_item(CanvasItem *item, AreaGroup *location) -> void {
  get_view()->lock();

  if (!location)
    _root_area->add(item);
  else
    location->add(item);

  item->set_needs_relayout();

  get_view()->unlock();

  queue_repaint();
}

auto Layer::remove_item(CanvasItem *item) -> void {
  get_view()->get_selection()->remove(item);

  if (item->get_parent())
    dynamic_cast<Layouter *>(item->get_parent())->remove(item);

  std::list<CanvasItem *>::iterator iter = std::find(_relayout_queue.begin(), _relayout_queue.end(), item);
  if (iter != _relayout_queue.end())
    _relayout_queue.erase(iter);

  queue_repaint();
}

static auto invalidate(CanvasItem *item) -> void {
  item->invalidate_cache();
  Layouter *l = dynamic_cast<Layouter *>(item);
  if (l)
    l->foreach(std::bind(&invalidate, std::placeholders::_1));
}

auto Layer::invalidate_caches() -> void {
  _root_area->foreach(std::bind(&invalidate, std::placeholders::_1));
}

auto Layer::set_needs_repaint_all_items() -> void {
  _root_area->foreach (std::bind(&CanvasItem::set_needs_repaint, std::placeholders::_1));
}

auto Layer::repaint_pending() -> void {
  if (_needs_repaint) {
    // XXX record pending areas and repaint only what's needed
    repaint(Rect(Point(0, 0), _owner->get_total_view_size()));
    _needs_repaint = false;
  }
}

auto Layer::repaint(const Rect &bounds) -> void {
  for (std::list<CanvasItem *>::iterator iter = _relayout_queue.begin(); iter != _relayout_queue.end(); ++iter) {
    (*iter)->relayout();
  }
  _relayout_queue.clear();

  if (_visible)
    _root_area->repaint(bounds, false);
}

auto Layer::repaint_for_export(const Rect &aBounds) -> void {
  for (std::list<CanvasItem *>::iterator iter = _relayout_queue.begin(); iter != _relayout_queue.end(); ++iter) {
    (*iter)->relayout();
  }
  _relayout_queue.clear();

  if (_visible)
    _root_area->repaint(aBounds, true);
}

//--------------------------------------------------------------------------------------------------

auto Layer::queue_repaint() -> void {
  _needs_repaint = true;
  _owner->queue_repaint();
}

//--------------------------------------------------------------------------------------------------

auto Layer::queue_repaint(const Rect &bounds) -> void {
  _needs_repaint = true;
  _owner->queue_repaint(bounds);
}

//--------------------------------------------------------------------------------------------------

auto Layer::queue_relayout(CanvasItem *item) -> void {
  if (!item->is_toplevel())
    throw std::logic_error("trying to queue non-toplevel item for relayout");

  if (std::find(_relayout_queue.begin(), _relayout_queue.end(), item) == _relayout_queue.end()) {
    queue_repaint();
    _relayout_queue.push_back(item);
  }
}

auto Layer::get_other_item_at(const Point &point, CanvasItem *item) -> CanvasItem * {
  return _root_area->get_other_item_at(point, item);
}

auto Layer::get_item_at(const Point &point) -> CanvasItem * {
  return _root_area->get_item_at(point);
}

auto Layer::get_top_item_at(const Point &point) -> CanvasItem * {
  return _root_area->get_direct_subitem_at(point);
}

static auto get_items_bounded_by(const Rect &rect, const Layer::ItemCheckFunc &pred, Group *group) -> std::list<CanvasItem *> {
  std::list<CanvasItem *> &items = group->get_contents();
  std::list<CanvasItem *> result;

  for (std::list<CanvasItem *>::iterator iter = items.begin(); iter != items.end(); ++iter) {
    Group *g;

    if (bounds_intersect((*iter)->get_root_bounds(), rect) && (!pred || pred(*iter)))
      result.push_back(*iter);

    g = dynamic_cast<Group *>(*iter);
    if (g && bounds_intersect(g->get_root_bounds(), rect)) {
      std::list<CanvasItem *> tmp = get_items_bounded_by(rect, pred, g);

      result.insert(result.end(), tmp.begin(), tmp.end());
    }
  }
  return result;
}

auto Layer::get_items_bounded_by(const Rect &rect, const ItemCheckFunc &pred,
                                                    mdc::Group *inside_group) -> std::list<CanvasItem *> {
  if (!inside_group)
    inside_group = _root_area;
  return ::get_items_bounded_by(rect, pred, inside_group);
}

auto Layer::get_bounds_of_item_list(const std::list<CanvasItem *> &items) -> Rect {
  std::list<CanvasItem *>::const_iterator it = items.begin();
  Rect rect;

  if (it != items.end()) {
    rect = (*it)->get_bounds();
    ++it;
  }
  while (it != items.end()) {
    Rect bounds = (*it)->get_bounds();
    Rect obounds = rect;

    rect.set_xmin(std::min(obounds.left(), bounds.left()));
    rect.set_ymin(std::min(obounds.top(), bounds.top()));
    rect.set_xmax(std::max(obounds.right(), bounds.right()));
    rect.set_ymax(std::max(obounds.bottom(), bounds.bottom()));
    ++it;
  }

  return rect;
}

auto Layer::create_group_with(const std::list<CanvasItem *> &contents) -> Group * {
  if (contents.size() <= 1)
    return 0;

  Rect bounds = get_bounds_of_item_list(contents);

  Group *group = new Group(this);
  group->set_position(bounds.pos);

  group->freeze();

  for (std::list<CanvasItem *>::const_reverse_iterator iter = contents.rbegin(); iter != contents.rend(); ++iter) {
    group->add(*iter);
    (*iter)->set_position((*iter)->get_position() - bounds.pos);
  }

  group->thaw();

  add_item(group);

  queue_repaint(group->get_bounds());

  return group;
}

auto Layer::create_area_group_with(const std::list<CanvasItem *> &contents) -> AreaGroup * {
  if (contents.size() <= 1)
    return 0;

  Rect bounds = get_bounds_of_item_list(contents);

  bounds = expand_bound(bounds, 20, 20);

  AreaGroup *group = new AreaGroup(this);
  group->set_position(bounds.pos);
  group->resize_to(bounds.size);

  //  group->freeze();

  for (std::list<CanvasItem *>::const_reverse_iterator iter = contents.rbegin(); iter != contents.rend(); ++iter) {
    _root_area->remove(*iter);
    group->add(*iter);
    (*iter)->set_position((*iter)->get_position() - bounds.pos);
  }

  //  group->thaw();

  _root_area->add(group);

  group->set_needs_render();
  queue_repaint();

  //  set_needs_repaint(group->get_bounds());

  return group;
}

auto Layer::dissolve_group(Group *group) -> void {
  group->dissolve();
  remove_item(group);
  delete group;
}
