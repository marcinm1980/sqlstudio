/*
 * Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

#include "mdc.h"
#include "mdc_canvas_view_image.h"

#include "gtest/gtest.h"

namespace {

struct CairoSurface {
  std::string path;
  unsigned char *data;
  cairo_surface_t *surface;
  base::Size size;
  unsigned int stride;
  bool owner;

  CairoSurface() {
    data = nullptr;
    surface = nullptr;
    stride = 0;
    owner = false;
  }
  CairoSurface(const CairoSurface &other) {
    surface = other.surface;

    size.height = other.size.height;
    size.width = other.size.width;
    stride = other.stride;
    data = other.data;
    owner = false;
  }
  CairoSurface(const std::string &path) {
    surface = cairo_image_surface_create_from_png(path.c_str());

    size.height = cairo_image_surface_get_height(surface);
    size.width = cairo_image_surface_get_width(surface);
    stride = cairo_image_surface_get_stride(surface);
    data = cairo_image_surface_get_data(surface);
    owner = true;
  }

  ~CairoSurface() {
    if (surface && owner)
      cairo_surface_destroy(surface);
  }

  bool operator == (const CairoSurface &other) const {
    if (cairo_surface_status(surface) != cairo_surface_status(other.surface))
      return false;
    if (size != other.size)
      return false;
    if (stride != other.stride)
      return false;
    if (memcmp(data, other.data, size.height * stride) != 0)
      return false;
    return true;
  }

  std::string toString() const {
    return base::strfmt("{ width: %f, height: %f, stride: %d }", size.width, size.height, stride);
  }
};



class Thing : public mdc::Box {
  mdc::Box title_bar;
  mdc::IconTextFigure title;
  mdc::TextFigure title_expander;

  mdc::Box column_box;

  cairo_surface_t *column_icon;
  cairo_surface_t *key_icon;

  std::vector<mdc::IconTextFigure *> columns;

public:
  Thing(mdc::Layer *layer)
    : mdc::Box(layer, Box::Vertical),
      title_bar(layer, Box::Horizontal),
      title(layer),
      title_expander(layer),
      column_box(layer, Box::Vertical, true) {

    // TODO: Verify these urls
    column_icon =
      cairo_image_surface_create_from_png("/Users/kojima/Development/mysql-studio-pro/images/grt/column.png");
    key_icon =
      cairo_image_surface_create_from_png("/Users/kojima/Development/mysql-studio-pro/images/grt/column_pk.png");

    set_accepts_focus(true);
    set_accepts_selection(true);

    set_background_color(base::Color(1, 1, 1));
    set_border_color(base::Color(0.5, 0.5, 0.5));
    set_draw_background(true);

    add(&title_bar, false, false);
    title_bar.set_padding(4, 4);
    title_bar.add(&title, true, true);
    title_bar.set_background_color(base::Color(0.5, 0.7, 0.83));
    title_bar.set_border_color(base::Color(0.5, 0.5, 0.5));
    title_bar.set_draw_background(true);

    title.set_icon(cairo_image_surface_create_from_png(
      "/Users/kojima/Development/mysql-studio-pro/images/grt/db.Table.12x12.png"));
    title.set_font(mdc::FontSpec("Lucida Grande", mdc::SNormal, mdc::WBold, 10));
    title.set_text("Hello World");

    title_expander.set_fixed_size(base::Size(10, -1));
    title_expander.set_text(">");
    title_bar.add(&title_expander, false, true);

    add(&column_box, false, true);
    column_box.set_spacing(2);
    column_box.set_padding(3, 3);

    add_column("id int primary key", key_icon);
    add_column("name varchar(32)", column_icon);
    add_column("address varchar(200)", column_icon);
    add_column("city int", column_icon);
    add_column("country int", column_icon);
    add_column("phone varchar(40)", column_icon);
    add_column("email varchar(80)", column_icon);
  }

  void add_column(const std::string &text, cairo_surface_t *icon) {
    mdc::IconTextFigure *tf;

    tf = new mdc::IconTextFigure(_layer);
    tf->set_icon(icon);
    tf->set_spacing(1);
    tf->set_font(mdc::FontSpec("Lucida Grande", mdc::SNormal, mdc::WNormal, 10));
    tf->set_text(text);

    column_box.add(tf, false, true);
  }
};

class MDC_CanvasTest : public ::testing::Test {
protected:
    std::unique_ptr<mdc::CanvasView> view;
    std::unique_ptr<mdc::AreaGroup> group;
    std::unique_ptr<Thing> item0, item1, item2;
    std::string outputDir;
    std::string dataDir;

    void SetUp() override {
        outputDir = "./output";
        dataDir = "./data";
    }
};

TEST_F(MDC_CanvasTest, ViewCreationZoom) {
    mdc::ImageCanvasView view(500, 400);
    view.set_page_size(base::Size(500, 400));
    EXPECT_EQ(view.get_viewport().str(), base::Rect(0, 0, 500, 400).str());
    view.set_zoom(2);
    EXPECT_EQ(view.get_viewport().str(), base::Rect(0, 0, 250, 200).str());
    view.set_zoom(0.5);
    EXPECT_EQ(view.get_viewport().str(), base::Rect(0, 0, 500, 400).str());
}

TEST_F(MDC_CanvasTest, ViewHierarchyCreation) {
    view = std::make_unique<mdc::ImageCanvasView>(1000, 1000);
    view->initialize();
    mdc::Layer *layer = view->get_current_layer();
    ASSERT_NE(layer, nullptr);
    item0 = std::make_unique<Thing>(layer);
    layer->add_item(item0.get());
    item0->move_to(base::Point(100, 100));
    item1 = std::make_unique<Thing>(layer);
    layer->add_item(item1.get());
    item2 = std::make_unique<Thing>(layer);
    layer->add_item(item2.get());
    item1->move_to(base::Point(100, 100));
    item2->move_to(base::Point(200, 50));
    view->get_selection()->add(item1.get());
    view->get_selection()->add(item2.get());
    EXPECT_EQ(view->get_selected_items().size(), 2U);
    std::list<mdc::CanvasItem *> items;
    mdc::Selection::ContentType selection(view->get_selected_items());
    for (auto iter = selection.begin(); iter != selection.end(); ++iter)
        items.push_back(*iter);
    group.reset(layer->create_area_group_with(items));
    group->set_draw_background(true);
    group->set_background_color(base::Color(1.0, 1.0, 0.6));
    EXPECT_TRUE(group != nullptr);
}

TEST_F(MDC_CanvasTest, TestGetCommonAncestor) {
    mdc::CanvasItem *ancestor;
    ancestor = item1->get_common_ancestor(item2.get());
    EXPECT_EQ(ancestor, item1->get_parent());
    ancestor = item2->get_common_ancestor(item1.get());
    EXPECT_EQ(ancestor, item1->get_parent());
    ASSERT_NE(ancestor, nullptr);
}

TEST_F(MDC_CanvasTest, CoordinateConversion) {
    base::Point p;
    ASSERT_EQ(item0->get_position().x, 100);
    ASSERT_EQ(item0->get_position().y, 100);
    p = item0->convert_point_to(base::Point(4, 5), 0);
    EXPECT_EQ(p.x, 104);
    EXPECT_EQ(p.y, 105);
    p = item0->convert_point_from(base::Point(304, 305), 0);
    EXPECT_EQ(p.x, 204);
    EXPECT_EQ(p.y, 205);
}

TEST(MDC_CanvasStandaloneTest, NonHomogeneousBoxLayout) {
    mdc::ImageCanvasView view(1000, 1000);
    view.initialize();
    mdc::Layer *layer = view.get_current_layer();
    mdc::Box *hbox = new mdc::Box(layer, mdc::Box::Horizontal, false);
    mdc::RectangleFigure r1(layer);
    mdc::RectangleFigure r2(layer);
    mdc::RectangleFigure r3(layer);
    mdc::RectangleFigure r4(layer);
    r1.set_fixed_min_size(base::Size(20, -1));
    r2.set_fixed_min_size(base::Size(20, -1));
    r3.set_fixed_min_size(base::Size(20, -1));
    r4.set_fixed_min_size(base::Size(20, -1));
    hbox->set_fixed_size(base::Size(200, 20));
    hbox->add(&r1, true, true, false);
    hbox->add(&r2, true, false, false);
    hbox->add(&r3, false, true, false);
    hbox->add(&r4, false, false, false);
    hbox->relayout();
    EXPECT_EQ(hbox->get_size().width, 200);
    EXPECT_EQ(r1.get_size().width, 80);
    EXPECT_EQ(r1.get_size().height, 20);
    EXPECT_EQ(r2.get_size().width, 20);
    EXPECT_EQ(r2.get_size().height, 20);
    EXPECT_EQ(r3.get_size().width, 20);
    EXPECT_EQ(r3.get_size().height, 20);
    EXPECT_EQ(r4.get_size().width, 20);
    EXPECT_EQ(r4.get_size().height, 20);
}

TEST_F(MDC_CanvasTest, RectangleRendering) {
    mdc::ImageCanvasView imageView(500, 400);
    mdc::Layer *layer;
    imageView.initialize();
    layer = imageView.get_current_layer();
    mdc::RectangleFigure r[17] = {
      layer, layer, layer, layer, layer, layer, layer, layer, layer, layer, layer, layer, layer, layer, layer, layer, layer
    };
    for (int i = 0; i < 17; i++)
      layer->add_item(&r[i]);
    r[0].move_to(base::Point(10, 10));
    r[0].set_fixed_size(base::Size(50, 100));
    r[0].set_pen_color(base::Color(1, 0, 0));
    r[1].move_to(base::Point(25, 50));
    r[1].set_fixed_size(base::Size(50, 50));
    r[1].set_pen_color(base::Color(0, 1, 0));
    r[2].move_to(base::Point(40, 10));
    r[2].set_fixed_size(base::Size(50, 50));
    r[2].set_pen_color(base::Color(0, 0, 0));
    r[2].set_filled(true);
    r[2].set_fill_color(base::Color(0, 1, 0, 0.5));
    r[3].move_to(base::Point(100, 10));
    r[3].set_fixed_size(base::Size(80, 80));
    r[3].set_pen_color(base::Color::black());
    r[3].set_filled(true);
    r[3].set_fill_color(base::Color(1, 0.5, 0.8));
    r[3].set_rounded_corners(10, mdc::CTopLeft | mdc::CBottomRight);
    r[4].move_to(base::Point(200, 10));
    r[4].set_fixed_size(base::Size(80, 80));
    r[4].set_pen_color(base::Color::black());
    r[4].set_filled(true);
    r[4].set_fill_color(base::Color(1, 0.5, 0.8));
    r[4].set_rounded_corners(10, mdc::CTopRight | mdc::CBottomLeft);
    r[5].move_to(base::Point(300, 10));
    r[5].set_fixed_size(base::Size(80, 80));
    r[5].set_pen_color(base::Color::black());
    r[5].set_filled(true);
    r[5].set_fill_color(base::Color(1, 0.5, 0.8));
    r[5].set_rounded_corners(10, mdc::CTop);
    r[6].move_to(base::Point(400, 10));
    r[6].set_fixed_size(base::Size(80, 80));
    r[6].set_pen_color(base::Color::black());
    r[6].set_filled(true);
    r[6].set_fill_color(base::Color(1, 0.5, 0.8));
    r[6].set_rounded_corners(10, mdc::CBottom);
    r[7].move_to(base::Point(100, 300));
    r[7].set_fixed_size(base::Size(80, 80));
    r[7].set_pen_color(base::Color::black());
    r[7].set_filled(true);
    r[7].set_fill_color(base::Color(1, 1, 0.8));
    r[7].set_rounded_corners(10, mdc::CAll);
    r[8].move_to(base::Point(100, 100));
    r[8].set_fixed_size(base::Size(80, 80));
    r[8].set_pen_color(base::Color::black());
    r[8].set_filled(true);
    r[8].set_fill_color(base::Color(0.5, 0.5, 0.8));
    r[8].set_rounded_corners(10, mdc::CTopLeft);
    r[9].move_to(base::Point(200, 100));
    r[9].set_fixed_size(base::Size(80, 80));
    r[9].set_pen_color(base::Color::black());
    r[9].set_filled(true);
    r[9].set_fill_color(base::Color(0.5, 0.5, 0.8));
    r[9].set_rounded_corners(10, mdc::CTopRight);
    r[10].move_to(base::Point(300, 100));
    r[10].set_fixed_size(base::Size(80, 80));
    r[10].set_pen_color(base::Color::black());
    r[10].set_filled(true);
    r[10].set_fill_color(base::Color(0.5, 0.5, 0.8));
    r[10].set_rounded_corners(10, mdc::CBottomLeft);
    r[11].move_to(base::Point(400, 100));
    r[11].set_fixed_size(base::Size(80, 80));
    r[11].set_pen_color(base::Color::black());
    r[11].set_filled(true);
    r[11].set_fill_color(base::Color(0.5, 0.5, 0.8));
    r[11].set_rounded_corners(10, mdc::CBottomRight);
    r[12].move_to(base::Point(100, 200));
    r[12].set_fixed_size(base::Size(80, 80));
    r[12].set_pen_color(base::Color::black());
    r[12].set_filled(true);
    r[12].set_fill_color(base::Color(0.5, 0.8, 0.8));
    r[12].set_rounded_corners(10, (mdc::CornerMask)~mdc::CTopLeft);
    r[13].move_to(base::Point(200, 200));
    r[13].set_fixed_size(base::Size(80, 80));
    r[13].set_pen_color(base::Color::black());
    r[13].set_filled(true);
    r[13].set_fill_color(base::Color(0.5, 0.8, 0.8));
    r[13].set_rounded_corners(10, (mdc::CornerMask)~mdc::CTopRight);
    r[14].move_to(base::Point(300, 200));
    r[14].set_fixed_size(base::Size(80, 80));
    r[14].set_pen_color(base::Color::black());
    r[14].set_filled(true);
    r[14].set_fill_color(base::Color(0.5, 0.8, 0.8));
    r[14].set_rounded_corners(10, (mdc::CornerMask)~mdc::CBottomLeft);
    r[15].move_to(base::Point(400, 200));
    r[15].set_fixed_size(base::Size(80, 80));
    r[15].set_pen_color(base::Color::black());
    r[15].set_filled(true);
    r[15].set_fill_color(base::Color(0.5, 0.8, 0.8));
    r[15].set_rounded_corners(10, (mdc::CornerMask)~mdc::CBottomRight);
    r[16].move_to(base::Point(200, 300));
    r[16].set_fixed_size(base::Size(80, 80));
    r[16].set_pen_color(base::Color::black());
    r[16].set_filled(true);
    r[16].set_fill_color(base::Color(1, 1, 0.8));
    r[16].set_rounded_corners(10, mdc::CNone);
    std::string target = outputDir + "/shapes_rects_test.png";
    imageView.save_to(target);
    CairoSurface targetSurface(target);
    ASSERT_EQ((int)cairo_surface_status(targetSurface.surface), CAIRO_STATUS_SUCCESS) << "Target surface invalid";
    CairoSurface sourceSurface(dataDir + "/images/shapes_rects.png");
    ASSERT_EQ((int)cairo_surface_status(sourceSurface.surface), CAIRO_STATUS_SUCCESS) << "Source surface invalid";
    EXPECT_EQ(targetSurface == sourceSurface, true);
}

}

