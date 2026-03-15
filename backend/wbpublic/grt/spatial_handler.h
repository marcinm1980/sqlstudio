/*
 * Copyright (c) 2014, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SPATIAL_HANDLER_H_
#define SPATIAL_HANDLER_H_

#include <ogrsf_frmts.h>
#include <ogr_api.h>
#include <gdal_pam.h>
#include <memdataset.h>
#include <gdal_alg.h>
#include <gdal.h>
#include <deque>
#include "base/geometry.h"
#include "wbpublic_public_interface.h"

#include "mdc.h"
/* Spatial Object Model

 Feature - corresponds to the value of a single geometry column of a row in a resultset
 Identified by the layer (resultset) and row_id and may contain one or more attributes, which are the rest
 of the columns of the resultset.

 Layer - corresponds to a single resutset or data source to be displayed.
 Can be toggled to be shown or not.
 Contains a list of features that are part of that layer.
 Should allow identifying the feature that is located at a specific coordinate.
 */

namespace spatial {

  auto stringFromErrorCode(const OGRErr &val) -> std::string WBPUBLICBACKEND_PUBLIC_FUNC;
  auto fetchAuthorityCode(const std::string &wkt) -> std::string WBPUBLICBACKEND_PUBLIC_FUNC;

  struct WBPUBLICBACKEND_PUBLIC_FUNC ProjectionView {
    int width;
    int height;
    double MaxLat;
    double MaxLon;
    double MinLat;
    double MinLon;
    friend bool operator==(const ProjectionView &v1, const ProjectionView &v2);
    friend bool operator!=(const ProjectionView &v1, const ProjectionView &v2);
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC Envelope {
  public:
    Envelope();
    Envelope(double left, double top, double right, double bottom);
    bool converted;
    base::Point top_left;
    base::Point bottom_right;
    friend bool operator==(const Envelope &env1, const Envelope &env2);
    friend bool operator!=(const Envelope &env1, const Envelope &env2);
    auto is_init() -> bool;
    auto within(const base::Point &p) const -> bool;
  };

  bool operator==(const ProjectionView &v1, const ProjectionView &v2);
  bool operator!=(const ProjectionView &v1, const ProjectionView &v2);
  bool operator==(const Envelope &env1, const Envelope &env2);
  bool operator!=(const Envelope &env1, const Envelope &env2);

  enum ProjectionType { ProjMercator = 1, ProjEquirectangular = 2, ProjRobinson = 3, ProjBonne = 4, ProjGeodetic = 5 };

  enum ShapeType {
    ShapeUnknown,
    ShapePoint,
    ShapeLineString,
    ShapeLinearRing,
    ShapePolygon,
    ShapeMultiPoint,
    ShapeMultiLineString,
    ShapeMultiPolygon,
    ShapeGeometryCollection
  };

  auto shape_description(ShapeType shp) -> std::string;
  auto ogrTypeToWb(const OGRwkbGeometryType type) -> ShapeType;

  enum AxisType { AxisLat = 1, AxisLon = 2 };

  class WBPUBLICBACKEND_PUBLIC_FUNC ShapeContainer {
  protected:
    auto distance_linearring(const base::Point &p) const -> double;
    auto distance_line(const std::vector<base::Point> &point_list, const base::Point &p) const -> double;
    auto distance_polygon(const base::Point &p) const -> double;
    auto distance_point(const base::Point &p) const -> double;

  public:
    ShapeContainer();
    ShapeType type;
    std::vector<base::Point> points;
    Envelope bounding_box;
    auto distance(const base::Point &p) const -> double;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC Projection {
  protected:
    OGRSpatialReference _mercator_srs;
    OGRSpatialReference _equirectangular_srs;
    OGRSpatialReference _robinson_srs;
    OGRSpatialReference _geodetic_srs;
    OGRSpatialReference _bonne_srs;

  public:
    static auto get_instance() -> Projection &;
    auto check_libproj_availability() -> bool;
    OGRSpatialReference *get_projection(ProjectionType);

  private:
    Projection();
    Projection(Projection const &);
    void operator=(Projection const &);
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC Importer {
    OGRGeometry *_geometry;
    bool _interrupt;
    auto extract_points(OGRGeometry *shape, std::deque<ShapeContainer> &shapes_container) -> void;
    int _srid;

  public:
    Importer();
    ~Importer();
    auto import_from_mysql(const std::string &data) -> int;
    auto import_from_wkt(std::string data) -> int;
    auto get_points(std::deque<ShapeContainer> &shapes_container) -> void;
    auto get_envelope(Envelope &env) -> void;
    auto interrupt() -> void;
    auto getName() const -> std::string;
    auto getType() const -> ShapeType;

    auto getSrid() const -> int;
    auto as_wkt() -> std::string;
    auto as_kml() -> std::string;
    auto as_json() -> std::string;
    auto as_gml() -> std::string;

    auto steal_data() -> OGRGeometry *;
  };

  class WBPUBLICBACKEND_PUBLIC_FUNC Converter {
    base::RecMutex _projection_protector;
    double _adf_projection[6];
    double _inv_projection[6];
    OGRCoordinateTransformation *_geo_to_proj;
    OGRCoordinateTransformation *_proj_to_geo;
    OGRSpatialReference *_source_srs;
    OGRSpatialReference *_target_srs;
    ProjectionView _view;
    bool _interrupt;

  public:
    Converter(ProjectionView view, OGRSpatialReference *src_srs, OGRSpatialReference *dst_srs);
    ~Converter();
    auto change_projection(OGRSpatialReference *src_srs = NULL, OGRSpatialReference *dst_srs = NULL) -> void;
    auto change_projection(ProjectionView view, OGRSpatialReference *src_srs = NULL,
                           OGRSpatialReference *dst_srs = NULL) -> void;
    auto from_projected(double lat, double lon, int &x, int &y) -> void;
    auto to_projected(int x, int y, double &lat, double &lon) -> void;

    auto to_latlon(int x, int y, double &lat, double &lon) -> bool;
    auto from_latlon(double lat, double lon, int &x, int &y) -> bool;

    auto from_latlon_to_proj(double &lat, double &lon) -> bool;
    auto from_proj_to_latlon(double &lat, double &lon) -> bool;
    static auto dec_to_dms(double angle, AxisType axis, int precision) -> std::string;
    auto transform_points(std::deque<ShapeContainer> &shapes_container) -> void;
    auto transform_envelope(spatial::Envelope &env) -> void;
    auto interrupt() -> void;
  };

  class Layer;

  class WBPUBLICBACKEND_PUBLIC_FUNC Feature {
    Layer *_owner;
    int _row_id;
    Importer _geometry;
    std::deque<ShapeContainer> _shapes;
    spatial::Envelope _env_screen;

  public:
    Feature(Layer *layer, int row_id, const std::string &data, bool wkt);
    ~Feature();

    auto interrupt() -> void;
    auto get_envelope(spatial::Envelope &env, const bool &screen_coords = false) -> void;
    auto render(spatial::Converter *converter) -> void;
    auto repaint(mdc::CairoCtx &cr, float scale, const base::Rect &clip_area,
                 base::Color fill_color = base::Color::invalid()) -> void;

    auto row_id() const -> int {
      return _row_id;
    }
    auto distance(const base::Point &p, const double &allowed_distance = 4.0) -> double;
  };

  using LayerId = int;
  WBPUBLICBACKEND_PUBLIC_FUNC auto new_layer_id() -> LayerId;

  class WBPUBLICBACKEND_PUBLIC_FUNC Layer {
    friend class Feature;

  protected:
    std::deque<Feature *> _features;

    LayerId _layer_id;
    base::Color _color;
    float _render_progress;
    bool _show;
    bool _interrupt;
    spatial::Envelope _spatial_envelope;
    bool _fill_polygons;

  public:
    Layer(LayerId layer_id, base::Color color);
    virtual ~Layer();

    virtual auto load_data() -> void {
    }

    auto interrupt() -> void;

    auto hidden() -> bool;
    auto layer_id() -> LayerId;

    auto set_show(bool flag) -> void;

    auto size() -> size_t {
      return _features.size();
    }

    auto color() -> base::Color {
      return _color;
    }
    auto fill() -> bool {
      return _fill_polygons;
    }

    auto add_feature(int row_id, const std::string &geom_data, bool wkt) -> void;
    virtual auto render(spatial::Converter *converter) -> void;
    auto feature_closest(const base::Point &p, const double &allowed_distance = 4.0) -> spatial::Feature *;
    auto set_fill_polygons(bool fill) -> void;
    auto get_fill_polygons() -> bool;
    virtual auto repaint(mdc::CairoCtx &cr, float scale, const base::Rect &clip_area) -> void;
    auto query_render_progress() -> float;
    auto get_envelope() -> spatial::Envelope;
  };
}; // namespace spatial
#endif /* SPATIAL_HANDLER_H_ */
