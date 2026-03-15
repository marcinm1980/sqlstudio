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

#ifndef _FIGURE_COMMON_H_
#define _FIGURE_COMMON_H_

#include "mdc.h"

#include <algorithm>

#include "wbpublic_public_interface.h"
#include <grts/structs.model.h>

namespace wbfig {

  /** Abstract class to be overridden by diagram class which will receive events from various objects in canvas.
   */
  class FigureEventHub {
  public:
    virtual ~FigureEventHub() {};
    virtual auto figure_click(const model_ObjectRef &owner, mdc::CanvasItem *target, const base::Point &point,
                              mdc::MouseButton button, mdc::EventState state) -> bool = 0;
    virtual auto figure_double_click(const model_ObjectRef &owner, mdc::CanvasItem *target, const base::Point &point,
                                     mdc::MouseButton button, mdc::EventState state) -> bool = 0;
    virtual auto figure_button_press(const model_ObjectRef &owner, mdc::CanvasItem *target, const base::Point &point,
                                     mdc::MouseButton button, mdc::EventState state) -> bool = 0;
    virtual auto figure_button_release(const model_ObjectRef &owner, mdc::CanvasItem *target, const base::Point &point,
                                       mdc::MouseButton button, mdc::EventState state) -> bool = 0;
    virtual auto figure_enter(const model_ObjectRef &owner, mdc::CanvasItem *target, const base::Point &point) -> bool = 0;
    virtual auto figure_leave(const model_ObjectRef &owner, mdc::CanvasItem *target, const base::Point &point) -> bool = 0;
  };

  class BaseFigure;

  class WBPUBLICBACKEND_PUBLIC_FUNC Titlebar : public mdc::Box {
    using super = mdc::Box;

  public:
    Titlebar(mdc::Layer *layer, FigureEventHub *hub, BaseFigure *owner, bool expander);
    virtual ~Titlebar();

    auto set_icon(cairo_surface_t *icon) -> void;
    auto set_title(const std::string &text) -> void;

    inline auto get_title() const -> const std::string & {
      return _icon_text.get_text();
    }

    auto set_color(const base::Color &color) -> void;
    auto set_text_color(const base::Color &color) -> void;
    auto set_font(const mdc::FontSpec &font) -> void;
    auto get_font() -> const mdc::FontSpec & {
      return _icon_text.get_font();
    }
    auto set_rounded(mdc::CornerMask corners) -> void;
    auto set_border_color(const base::Color &color) -> void;

    auto set_expanded(bool flag) -> void;
    auto get_expanded() -> bool;

    virtual auto set_auto_sizing(bool flag) -> void;

    auto auto_size() -> void {
      _icon_text.auto_size();
    }

    boost::signals2::signal<void(bool)> *signal_expand_toggle() {
      return &_signal_expand_toggle;
    }

  protected:
    FigureEventHub *_hub;
    BaseFigure *_owner;

    mdc::IconTextFigure _icon_text;
    mdc::Button *_expander;

    base::Color _back_color;
    mdc::CornerMask _corners;
    base::Color _border_color;

    boost::signals2::signal<void(bool)> _signal_expand_toggle;

    auto expand_toggled() -> void;

    virtual auto render(mdc::CairoCtx *cr) -> void;

    virtual auto on_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                          mdc::EventState state) -> bool;
    virtual auto on_double_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_button_press(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_button_release(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                   mdc::EventState state) -> bool;
    virtual auto on_enter(mdc::CanvasItem *target, const base::Point &point) -> bool;
    virtual auto on_leave(mdc::CanvasItem *target, const base::Point &point) -> bool;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC CaptionFigure : public mdc::TextFigure {
    using super = mdc::TextFigure;

    FigureEventHub *_hub;
    model_Object *_owner_object;

    virtual auto on_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                          mdc::EventState state) -> bool;
    virtual auto on_double_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_button_press(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_button_release(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                   mdc::EventState state) -> bool;
    virtual auto on_enter(mdc::CanvasItem *target, const base::Point &point) -> bool;
    virtual auto on_leave(mdc::CanvasItem *target, const base::Point &point) -> bool;

  public:
    CaptionFigure(mdc::Layer *layer, FigureEventHub *hub, model_Object *owner);
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC FigureItem : public mdc::IconTextFigure {
    using super = mdc::IconTextFigure;

    FigureEventHub *_hub;
    BaseFigure *_owner;

    std::string _object_id;
    bool _dirty;

    virtual auto on_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                          mdc::EventState state) -> bool;
    virtual auto on_double_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_button_press(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_button_release(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                   mdc::EventState state) -> bool;
    virtual auto on_enter(mdc::CanvasItem *target, const base::Point &point) -> bool;
    virtual auto on_leave(mdc::CanvasItem *target, const base::Point &point) -> bool;

    virtual auto draw_state(mdc::CairoCtx *cr) -> void;
    virtual auto get_intersection_with_line_to(const base::Point &p) -> base::Point;
    virtual auto get_root_bounds() const -> base::Rect;

  public:
    FigureItem(mdc::Layer *layer, FigureEventHub *hub, BaseFigure *owner);

    auto get_id() -> std::string {
      return _object_id;
    }
    auto set_id(const std::string &id) -> void {
      _object_id = id;
    }
    auto set_dirty(bool flag = true) -> void {
      _dirty = flag;
    }
    auto get_dirty() -> bool {
      return _dirty;
    }
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC BaseFigure : public mdc::Box {
    using super = mdc::Box;

  public:
    using ItemList = std::list<FigureItem *>;

    // default implementation just sets background color
    virtual auto unset_color() -> void;
    virtual auto set_color(const base::Color &color) -> void;

    virtual auto set_title_font(const mdc::FontSpec &font) -> void {
    }
    virtual auto set_content_font(const mdc::FontSpec &font) -> void;

    virtual auto highlight(const base::Color *color = 0) -> void;
    virtual auto unhighlight() -> void;

    virtual auto set_allow_manual_resizing(bool flag) -> void;

    boost::signals2::signal<void(base::Rect)> *signal_interactive_resize() {
      return &_signal_interactive_resize;
    }

    boost::signals2::signal<void(FigureItem *)> *signal_item_added() {
      return &_signal_item_added;
    }

    virtual auto toggle(bool flag) -> void {
    }
    virtual auto set_state_drawing(bool flag) -> void;

    auto represented_object() -> model_ObjectRef {
      return model_ObjectRef(_represented_object);
    }

    auto in_user_resize() const -> bool {
      return _resizing;
    }

  protected:
    BaseFigure(mdc::Layer *layer, FigureEventHub *hub, const model_ObjectRef &object);

    FigureEventHub *_hub;
    model_Object *_represented_object;

    boost::signals2::signal<void(base::Rect)> _signal_interactive_resize;
    boost::signals2::signal<void(FigureItem *)> _signal_item_added;

    base::Rect _initial_bounds;
    mdc::FontSpec _content_font;
    bool _manual_resizing;
    bool _resizing;

    auto invalidate_min_sizes() -> void;
    static auto invalidate_min_sizes(mdc::CanvasItem *item) -> void;

    using CreateItemSlot = std::function<FigureItem *(mdc::Layer *, FigureEventHub *)>;
    using UpdateItemSlot = std::function<void(FigureItem *)>;

    virtual auto begin_sync(mdc::Box &box, ItemList &list) -> ItemList::iterator;
    virtual auto sync_next(mdc::Box &box, ItemList &list, ItemList::iterator iter, const std::string &id,
                                         cairo_surface_t *icon, const std::string &text,
                                         const CreateItemSlot &create_item = CreateItemSlot(),
                                         const UpdateItemSlot &update_item = UpdateItemSlot()) -> ItemList::iterator;

    virtual auto end_sync(mdc::Box &box, ItemList &list, ItemList::iterator iter) -> void;

    virtual auto on_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                          mdc::EventState state) -> bool;
    virtual auto on_double_click(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_button_press(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                 mdc::EventState state) -> bool;
    virtual auto on_button_release(mdc::CanvasItem *target, const base::Point &point, mdc::MouseButton button,
                                   mdc::EventState state) -> bool;
    virtual auto on_enter(mdc::CanvasItem *target, const base::Point &point) -> bool;
    virtual auto on_leave(mdc::CanvasItem *target, const base::Point &point) -> bool;

    virtual auto on_drag_handle(mdc::ItemHandle *handle, const base::Point &pos, bool dragging) -> bool;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC ShrinkableBox : public mdc::Box {
    using super = mdc::Box;

    int _limit_item_count;
    int _hidden_item_count;
    float _visible_part_size;
    bool _manual_resizing;

    virtual auto render(mdc::CairoCtx *cr) -> void;
    virtual auto resize_to(const base::Size &size) -> void;

  public:
    ShrinkableBox(mdc::Layer *layer, mdc::Box::Orientation orientation);
    virtual auto calc_min_size() -> base::Size;

    auto set_item_count_limit(int limit) -> void;

    auto set_allow_manual_resizing(bool flag) -> void;
  };
}; // namespace wbfig

#endif /* _FIGURE_COMMON_H_ */
