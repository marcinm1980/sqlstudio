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

#ifndef _MDC_CANVAS_VIEW_H_
#define _MDC_CANVAS_VIEW_H_

#include "mdc_canvas_public.h"

#include "mdc_layer.h"
#include "mdc_events.h"
#include "mdc_canvas_item.h"
#include "mdc_selection.h"
#include "base/threading.h"

#ifndef _MSC_VER
#include <glib.h>
#endif

namespace mdc {

  class Line;

  class BackLayer;

  enum SelectType { SelectSet, SelectAdd, SelectToggle };

  class MYSQLCANVAS_PUBLIC_FUNC CanvasView {
    friend class BackLayer;
    friend class CanvasViewExtras;

  public:
    using LayerList = std::list<Layer *>;

    virtual ~CanvasView();

    auto lock_ui() -> void;
    auto unlock_ui() -> void;

    auto lock() -> void;
    auto unlock() -> void;

    auto lock_redraw() -> void;
    auto unlock_redraw() -> void;

    auto pre_destroy() -> void;

    inline auto set_user_data(void *data) -> void {
      _user_data = data;
    }
    auto get_user_data() -> void * {
      return _user_data;
    }

    auto set_tag(const std::string &tag) -> void;
    auto get_tag() const -> std::string {
      return _tag;
    }

    auto find_item_with_tag(const std::string &tag) -> mdc::CanvasItem *;

    auto set_printout_mode(bool flag) -> void;
    auto is_printout() -> bool {
      return _printout_mode;
    }

    virtual auto update_view_size(int width, int height) -> void = 0;

    auto set_offset(const base::Point &offs) -> void;
    virtual auto scroll_to(const base::Point &offs) -> void;

    auto set_zoom(float zoom) -> void;
    auto get_zoom() const -> float {
      return _zoom;
    };

    auto set_page_size(const base::Size &size) -> void;
    auto get_page_size() const -> base::Size {
      return _page_size;
    };
    auto set_page_layout(Count xpages, Count ypages) -> void;
    auto get_page_layout(Count &xpages, Count &ypages) -> void {
      xpages = _x_page_num;
      ypages = _y_page_num;
    }

    // logical view size
    auto get_total_view_size() const -> base::Size;

    // physical view size
    inline auto get_view_size(int &w, int &h) const -> void {
      w = _view_width;
      h = _view_height;
    }

    auto get_viewable_size() const -> base::Size;

    auto get_viewport() const -> base::Rect;

    auto get_viewport_range() const -> base::Rect;

    auto get_content_bounds() const -> base::Rect;

    virtual auto window_to_canvas(int x, int y) const -> base::Point;
    virtual auto window_to_canvas(int x, int y, int w, int h) const -> base::Rect;
    virtual auto canvas_to_window(const base::Point &pt, int &x, int &y) const -> void;
    virtual auto canvas_to_window(const base::Rect &rect, int &x, int &y, int &w, int &h) const -> void;

    auto show_grid() -> void;
    auto hide_grid() -> void;
    auto get_grid_shown() -> bool;

    auto set_grid_snapping(bool flag) -> void;
    auto get_grid_snapping() -> bool;

    auto snap_to_grid(const base::Point &pos) -> base::Point;
    auto snap_to_grid(const base::Size &size) -> base::Size;

    auto set_draws_line_hops(bool flag) -> void;

    auto new_layer(const std::string &name) -> Layer *;
    auto set_current_layer(Layer *layer) -> void;
    auto get_current_layer() const -> Layer * {
      return _current_layer;
    }
    auto get_layer(const std::string &name) -> Layer *;
    auto get_background_layer() const -> BackLayer * {
      return _blayer;
    }
    auto get_interaction_layer() const -> InteractionLayer * {
      return _ilayer;
    }

    auto add_layer(Layer *layer) -> void;
    auto remove_layer(Layer *layer) -> void;
    virtual auto get_layers() -> LayerList &;

    auto remove_item(mdc::CanvasItem *item) -> void;

    virtual auto raise_layer(Layer *layer, Layer *above = 0) -> void;
    virtual auto lower_layer(Layer *layer) -> void;

    auto get_item_at(int x, int y) -> CanvasItem *;
    auto get_item_at(const base::Point &point) -> CanvasItem *;

    auto get_leaf_item_at(int x, int y) -> CanvasItem *;
    auto get_leaf_item_at(const base::Point &point) -> CanvasItem *;

    using ItemCheckFunc = std::function<bool(CanvasItem *)>;
    auto get_items_bounded_by(const base::Rect &rect, const ItemCheckFunc &pred = ItemCheckFunc())
      -> std::list<CanvasItem *>;

    auto repaint() -> void;
    auto repaint(int x, int y, int width, int height) -> void;

    auto set_needs_repaint_all_items() -> void;

    auto queue_repaint() -> void;
    auto queue_repaint(const base::Rect &bounds) -> void;

    virtual auto handle_mouse_move(int x, int y, EventState state) -> void;
    virtual auto handle_mouse_button(MouseButton button, bool press, int x, int y, EventState state) -> void;
    virtual auto handle_mouse_double_click(MouseButton button, int x, int y, EventState state) -> void;
    virtual auto handle_mouse_enter(int x, int y, EventState state) -> void;
    virtual auto handle_mouse_leave(int x, int y, EventState state) -> void;

    auto handle_key(const KeyInfo &key, bool press, EventState state) -> bool;

    auto start_dragging_rectangle(const base::Point &pos) -> void;
    auto finish_dragging_rectangle() -> base::Rect;

    auto focus_item(CanvasItem *item) -> bool;
    auto get_focused_item() -> CanvasItem *;

    auto select_items_inside(const base::Rect &rect, SelectType type, Group *group = 0) -> void;

    auto get_selection() const -> Selection * {
      return _selection;
    };
    auto get_selected_items() -> Selection::ContentType;

    auto update_line_crossings(Line *line) -> void;

    virtual auto initialize() -> bool;

    auto get_default_font() -> const FontSpec &;
    auto get_selection_color() const -> base::Color {
      return base::Color(0.6, 0.85, 0.95, 1.0);
    }
    auto get_highlight_color() const -> base::Color {
      return base::Color(1, 0.6, 0.0, 0.8);
    }
    auto get_hover_color() const -> base::Color {
      return base::Color(0.85, 0.5, 0.5, 0.8);
    }

    auto setBackgroundColor(base::Color const &color) -> void;

    inline auto cairoctx() const -> CairoCtx * {
      return _cairo;
    }
    virtual auto has_gl() const -> bool = 0;

    virtual auto create_temp_surface(const base::Size &size) const -> Surface *;

    auto export_png(const std::string &filename, bool crop = false) -> void;
    auto export_pdf(const std::string &filename, const base::Size &size_in_pt) -> void;
    auto export_ps(const std::string &filename, const base::Size &size_in_pt) -> void;
    auto export_svg(const std::string &filename, const base::Size &size_in_pt) -> void;

    auto set_event_callbacks(
      const std::function<bool(CanvasView *, MouseButton, bool, base::Point, EventState)> &button_handler,
      const std::function<bool(CanvasView *, base::Point, EventState)> &motion_handler,
      const std::function<bool(CanvasView *, KeyInfo, EventState, bool)> &key_handler) -> void;

    auto signal_resized() -> boost::signals2::signal<void()> * {
      return &_resized_signal;
    }
    auto signal_repaint() -> boost::signals2::signal<void(int, int, int, int)> * {
      return &_need_repaint_signal;
    }
    auto signal_viewport_changed() -> boost::signals2::signal<void()> * {
      return &_viewport_changed_signal;
    }
    auto signal_zoom_changed() -> boost::signals2::signal<void()> * {
      return &_zoom_changed_signal;
    }

    auto enable_debug(bool flag) -> void {
      _debug = flag;
    }
    inline auto debug_enabled() -> bool {
      return _debug;
    }

    auto get_fps() -> double {
      return _fps;
    }
    inline auto bookkeep_cache_mem(int amount) -> void {
      _total_item_cache_mem += amount;
    }

    auto paint_item_cache(CairoCtx *cr, double x, double y, cairo_surface_t *cached_item, double alpha = 1.0) -> void;

  protected:
    void *_user_data;
    std::string _tag;

    cairo_surface_t *_crsurface;
    CairoCtx *_cairo;
    cairo_matrix_t _trmatrix;

    int _ui_lock;
    int _repaint_lock;
    int _repaints_missed;

    FontSpec _default_font;

    LayerList _layers;
    BackLayer *_blayer;
    InteractionLayer *_ilayer;
    Layer *_current_layer;

    CanvasItem *_focused_item;

    Selection *_selection;

    base::Size _page_size;
    Count _x_page_num;
    Count _y_page_num;

    float _zoom;
    base::Point _offset;
    base::Point _extra_offset;
    int _view_width;
    int _view_height;

    float _grid_size;
    bool _grid_snapping;
    bool _printout_mode;
    bool _line_hop_rendering;

    bool _destroying;
    bool _debug;

    double _fps;

    size_t _total_item_cache_mem;

    boost::signals2::signal<void()> _resized_signal;
    boost::signals2::signal<void(int, int, int, int)> _need_repaint_signal;
    boost::signals2::signal<void()> _viewport_changed_signal;
    boost::signals2::signal<void()> _zoom_changed_signal;

    std::function<bool(CanvasView *, MouseButton, bool, base::Point, EventState)> _button_event_relay;
    std::function<bool(CanvasView *, base::Point, EventState)> _motion_event_relay;
    std::function<bool(CanvasView *, KeyInfo, EventState, bool)> _key_event_relay;

    CanvasView(int width, int height);

    virtual auto begin_repaint(int wx, int wy, int ww, int wh) -> void = 0;
    virtual auto end_repaint() -> void = 0;

    auto repaint_area(const base::Rect &rect, int wx, int wy, int ww, int wh) -> void;

    auto update_offsets() -> void;
    auto apply_transformations() -> void;
    auto apply_transformations_gl() -> void;
    auto reset_transformations_gl() -> void;
    auto apply_transformations_for_conversion(cairo_matrix_t *matrix) const -> void;

    auto perform_auto_scroll(const base::Point &mouse_pos) -> bool;

    auto render_for_export(const base::Rect &bounds, CairoCtx *cr) -> void;

  private:
    struct ClickInfo {
      base::Point pos;
    };

    EventState _event_state;
    CanvasItem *_last_click_item;
    CanvasItem *_last_over_item;
    std::vector<ClickInfo> _last_click_info;
    base::Point _last_mouse_pos;

    base::RecMutex _lock;

    static auto canvas_item_destroyed(void *data) -> void *;
    auto set_last_click_item(CanvasItem *item) -> void;
    auto set_last_over_item(CanvasItem *item) -> void;
  };

} // namespace mdc

#endif /* _MDC_CANVAS_VIEW_H_ */
