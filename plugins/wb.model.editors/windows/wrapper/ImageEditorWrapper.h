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

#include "wb_editor_image.h"
#include "GrtTemplates.h"

#pragma make_public(ImageEditorBE)

namespace MySQL {
  namespace Grt {

  public
    ref class ImageEditorWrapper : public BaseEditorWrapper {
    protected:
      ImageEditorWrapper(::ImageEditorBE *inn);

    public:
      ImageEditorWrapper::ImageEditorWrapper(MySQL::Grt::GrtValue ^ arglist);
      ~ImageEditorWrapper();

      auto get_unmanaged_object() -> ImageEditorBE *;
      void set_filename(String ^ text);
      auto get_filename() -> String ^;
      auto get_attached_image_path() -> String ^;
      void get_size([Out] int % w, [Out] int % h);
      auto set_size(int w, int h) -> void;
      auto set_width(int w) -> void;
      auto set_height(int h) -> void;
      auto get_keep_aspect_ratio() -> bool;
      auto set_keep_aspect_ratio(bool flag) -> void;
    };

  } // namespace Grt
} // namespace MySQL
