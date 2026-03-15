/*
 * Copyright (c) 2011, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include "gtk_helpers.h"
#include "../lf_popover.h"
#include "base/log.h"

DEFAULT_LOG_DOMAIN("Popover")

namespace mforms {
  PopoverNormal::PopoverNormal(mforms::View *owner) : PopoverWidget(PopoverStyleNormal), _owner(nullptr) {

    _popover = new Gtk::Popover();
    if (owner != nullptr) {
      _owner = widget_for_view(owner);
      if (_owner != nullptr) {
        _popover->set_relative_to(*_owner);
      } else {
        logError("Owner not set, some functionality may not work properly.\n");
      }

    }

#if GTK_VERSION_LT(3, 22)
    _popover->set_transitions_enabled(true);
#endif
    _popover->set_border_width(2);
  }

  //------------------------------------------------------------------------------

  PopoverNormal::~PopoverNormal() {
    delete _popover;
  };

  //------------------------------------------------------------------------------

  auto PopoverNormal::showPopover(const int x, const int y, const StartPosition pos) -> void {
    switch (pos) {
      case mforms::StartLeft: // The popover is initially left to the ref point, having its arrow pointing to the right.
        _popover->set_position(Gtk::POS_LEFT);
        break;
      case mforms::StartRight: // Similar for the other positions.
        _popover->set_position(Gtk::POS_RIGHT);
        break;
      case mforms::StartAbove:
        _popover->set_position(Gtk::POS_TOP);
        break;
      case mforms::StartBelow:
        _popover->set_position(Gtk::POS_BOTTOM);
        break;
    }

    if (_owner == nullptr) {
      logError("Unable to show popover, relative element is not set.\n");
      return;
    }

    auto window = _owner->get_window();
    int wx, wy = 0;
    window->get_root_coords(_owner->get_allocation().get_x(), _owner->get_allocation().get_y(), wx, wy);

    Gdk::Rectangle r;
    r.set_x(x - wx);
    r.set_y(y - wy);
    r.set_width(1);
    r.set_height(1);
    _popover->set_pointing_to(r);
#if GTK_VERSION_GT(3, 22)
    _popover->popup();
#else
    _popover->show_all();
#endif
  }

  //------------------------------------------------------------------------------

  auto PopoverNormal::close() -> void {
#if GTK_VERSION_GT(3, 22)
    _popover->popdown();
#else
    _popover->hide();
#endif
  }

  //------------------------------------------------------------------------------

  auto PopoverNormal::setSize(const int w, const int h) -> void {
    _popover->set_size_request(w, h);
  }

  //------------------------------------------------------------------------------

  auto PopoverNormal::setContent(Gtk::Widget* w) -> void {
    _popover->add(*w);
  }

  //------------------------------------------------------------------------------

  auto PopoverNormal::setName(const std::string& name) -> void {
    auto acc = _popover->get_accessible();
    if (acc)
      acc->set_name(name);
  }

  //------------------------------------------------------------------------------

  PopoverTooltip::PopoverTooltip(mforms::View *owner)
    : PopoverWidget(PopoverStyleTooltip),
      Gtk::Window(Gtk::WINDOW_POPUP),
      _contentPos(mforms::StartLeft),
      _handleX(0),
      _handleY(0) {
    if (owner != nullptr) {
      auto widget = widget_for_view(owner);
      if (widget != nullptr) {
        auto top = widget->get_toplevel();
        if (top->get_is_toplevel()) {
          _parent = dynamic_cast<Gtk::Window*>(top);
        }
      }
      if (_parent == nullptr) {
        logError("Owner not set, some functionality may not work properly.\n");
      }
    } else {
      _parent = get_mainwindow();
    }
    set_type_hint(Gdk::WINDOW_TYPE_HINT_TOOLTIP);
    set_app_paintable(true);
    set_resizable(false);
    set_name("Tooltip");
    set_border_width(2);

    _hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    add(*_hbox);
    signal_event().connect(sigc::mem_fun(this, &PopoverTooltip::tooltipSignalEvent));
    _parent->add_events(Gdk::KEY_RELEASE_MASK);
    _parent->signal_key_release_event().connect_notify(sigc::mem_fun(this, &PopoverTooltip::parentKeyRelease));
    show();
  }

  //------------------------------------------------------------------------------

  auto PopoverTooltip::showPopover(const int rx, const int ry, const StartPosition pos) -> void {
    auto wnd = get_window();
    if (wnd) {
      int xx, yy;
      Gdk::ModifierType mask;
      wnd->get_pointer(xx, yy, mask);
      if ((mask & Gdk::BUTTON1_MASK) || (mask & Gdk::BUTTON2_MASK) || (mask & Gdk::BUTTON3_MASK))
        return;
    }

    int x = rx, y = ry;
    if (x < 0 && y < 0) {
      Glib::RefPtr<Gdk::Display> dsp = Gdk::Display::get_default();
      if (dsp) {
        Glib::RefPtr<Gdk::DeviceManager> dvm = dsp->get_device_manager();
        if (dvm) {
          Glib::RefPtr<Gdk::Device> dev = dvm->get_client_pointer();
          if (dev)
            dev->get_position(x, y);
        }
      }
    }

    _contentPos = pos;
    _handleX = x;
    _handleY = y;

    adjustPosition();

    show_all();
  }

  //------------------------------------------------------------------------------

  auto PopoverTooltip::adjustPosition() -> void {
    int w = get_width();
    int h = get_height();

    int x = _handleX;
    int y = _handleY;

    if (_style == mforms::PopoverStyleTooltip) {
      Gdk::Rectangle rect;
      get_screen()->get_monitor_geometry(get_screen()->get_monitor_at_point(x, y), rect);

      if ((x + w) > rect.get_width()) {
        _contentPos = mforms::StartLeft;
      }

      if ((y + h) > rect.get_height()) {
        _contentPos = mforms::StartAbove;
      }
    }

    switch (_contentPos) {
      case mforms::StartAbove: {
        x -= 3 * w / 4;
        y -= (h + 5);
        break;
      }
      case mforms::StartBelow: {
        x -= w / 4;
        break;
      }
      case mforms::StartRight: {
        y = _handleY + 10;
        x += 10;
        break;
      }
      case mforms::StartLeft: {
        x -= w;
        y = _handleY + 10;
        break;
      }
    }
    move(x, y);
  }

  //------------------------------------------------------------------------------

  auto PopoverTooltip::close() -> void {
    hide();
  }

  //------------------------------------------------------------------------------

  auto PopoverTooltip::setSize(const int w, const int h) -> void {
    if (w > 1 && h > 1) {
      resize(w, h);
    }
  }

  //------------------------------------------------------------------------------

  auto PopoverTooltip::setContent(Gtk::Widget* w) -> void {
    _hbox->pack_start(*w, false, false, 0);
  }

  //------------------------------------------------------------------------------

  auto PopoverTooltip::setName(const std::string& name) -> void {
    auto acc = get_accessible();
    if (acc)
      acc->set_name(name);
  }

  //------------------------------------------------------------------------------

  auto PopoverTooltip::tooltipSignalEvent(GdkEvent* ev) -> bool {
    if (ev->type == GDK_LEAVE_NOTIFY)
      hide();

    return false;
  }

  //------------------------------------------------------------------------------

  auto PopoverTooltip::parentKeyRelease(GdkEventKey* ev) -> void {
    hide();
  }
}

//------------------------------------------------------------------------------

static auto delete_PopoverWidget(void* o) -> void {
  delete (mforms::PopoverWidget*)o;
}

//------------------------------------------------------------------------------

static auto createPopover(mforms::PopoverStyle style, mforms::View *owner) -> mforms::PopoverWidget* {
  switch (style) {
    case mforms::PopoverStyleNormal:
      return new mforms::PopoverNormal(owner);
    case mforms::PopoverStyleTooltip:
      return new mforms::PopoverTooltip(owner);
    default:
      throw std::runtime_error("Unknown tooltip style");
  }
}

//------------------------------------------------------------------------------

static auto create(mforms::Popover* self, mforms::View *owner, mforms::PopoverStyle style) -> bool {
  mforms::PopoverWidget* w = createPopover(style, owner);
  self->set_data(w, delete_PopoverWidget);
  return w;
}

//------------------------------------------------------------------------------

static auto set_content(mforms::Popover* self, mforms::View* content) -> void {
  mforms::PopoverWidget* w = self->get_data<mforms::PopoverWidget>();
  mforms::gtk::ViewImpl* view = content->get_data<mforms::gtk::ViewImpl>();
  w->setContent(view->get_outer());
}

//------------------------------------------------------------------------------

static auto set_size(mforms::Popover* self, int w, int h) -> void {
  mforms::PopoverWidget* widget = self->get_data<mforms::PopoverWidget>();
  widget->setSize(w, h);
}

//------------------------------------------------------------------------------

static auto show(mforms::Popover* self, int x, int y, mforms::StartPosition pos) -> void {
  mforms::PopoverWidget* widget = self->get_data<mforms::PopoverWidget>();
  widget->showPopover(x, y, pos);
}

//------------------------------------------------------------------------------

static auto close(mforms::Popover* self) -> void {
  mforms::PopoverWidget* w = self->get_data<mforms::PopoverWidget>();
  w->close();
}

//------------------------------------------------------------------------------

static auto setName(mforms::Popover* self, const std::string& name) -> void {
  mforms::PopoverWidget* w = self->get_data<mforms::PopoverWidget>();
  if (w) {
    w->setName(name);
  }
}

//------------------------------------------------------------------------------

static auto show_and_track(mforms::Popover* self, mforms::View* owner, int x, int y, mforms::StartPosition pos) -> void {
  show(self, x, y, pos);
}

namespace mforms {
  namespace gtk {
    auto Popover_init() -> void {
      ::mforms::ControlFactory* f = ::mforms::ControlFactory::get_instance();

      f->_popover_impl.create = create;
      f->_popover_impl.set_content = set_content;
      f->_popover_impl.set_size = set_size;
      f->_popover_impl.show = show;
      f->_popover_impl.close = close;
      f->_popover_impl.setName = setName;
      f->_popover_impl.show_and_track = show_and_track;
    }
  }
}
