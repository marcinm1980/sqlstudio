/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef __ICON_MANAGER_H__
#define __ICON_MANAGER_H__

using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;

namespace MySQL {
  namespace Grt {
    typedef int IconId;

  public
    enum class IconSize { Icon11 = 11, Icon12 = 12, Icon16 = 16, Icon24 = 24, Icon32 = 32, Icon48 = 48, Icon64 = 64 };

  public
    ref class IconManagerWrapper {
    private:
      static IconManagerWrapper ^ _grtIconManager = nullptr;

      ImageList ^ imageList16;
      ImageList ^ imageList24;
      ImageList ^ imageList32;
      ImageList ^ imageList48;

      Dictionary<IconId, Bitmap ^> ^ icon_cache;

      // Hide constructor
      IconManagerWrapper() {
        icon_cache = gcnew Dictionary<IconId, Bitmap ^>();
        inner = bec::IconManager::get_instance();

        imageList16 = gcnew ImageList();
        imageList16->ColorDepth = ColorDepth::Depth32Bit;
        imageList16->ImageSize = System::Drawing::Size(16, 16);
        imageList24 = gcnew ImageList();
        imageList24->ColorDepth = ColorDepth::Depth32Bit;
        imageList24->ImageSize = System::Drawing::Size(24, 24);
        imageList32 = gcnew ImageList();
        imageList32->ColorDepth = ColorDepth::Depth32Bit;
        imageList32->ImageSize = System::Drawing::Size(32, 32);
        imageList48 = gcnew ImageList();
        imageList48->ColorDepth = ColorDepth::Depth32Bit;
        imageList48->ImageSize = System::Drawing::Size(48, 48);
      }

    protected:
      bec::IconManager *inner;

    public:
      inline auto get_unmanaged_object() -> bec::IconManager * {
        return inner;
      }

      // Singleton class.
      static auto get_instance() -> IconManagerWrapper ^ {
          if (_grtIconManager == nullptr)
            _grtIconManager = gcnew IconManagerWrapper();

          return _grtIconManager;
        }

        IconId get_icon_id(String ^ icon_file) {
        return (int)inner->get_icon_id(NativeToCppString(icon_file));
      }

      String ^ get_icon_file(IconId icon) { return CppStringToNative(inner->get_icon_file(icon)); }

        auto get_icon_path(IconId icon) -> String ^ { return CppStringToNative(inner->get_icon_path(icon)); }

        void add_search_path(String ^ path) {
        inner->add_search_path(NativeToCppString(path));
      }

      auto get_icon(IconId icon) -> Bitmap ^ {
          if (icon == 0)
            return nullptr;
          else if (icon_cache->ContainsKey(icon))
            return icon_cache[icon];
          else {
            String ^ path = get_icon_path(icon);
            Bitmap ^ img = nullptr;

            if (path->CompareTo("") != 0) {
              img = gcnew System::Drawing::Bitmap(path);

              icon_cache->Add(icon, img);
            }

            return img;
          }
        }

        auto add_icon_to_imagelist(IconId iconId, IconSize iconSize) -> int {
        ImageList ^ imageList;

        // Choose the correct ImageList
        switch (iconSize) {
          case IconSize::Icon16:
            imageList = imageList16;
            break;
          case IconSize::Icon24:
            imageList = imageList24;
            break;
          case IconSize::Icon32:
            imageList = imageList32;
            break;
          default:
            imageList = imageList48;
            break;
        }

        // Check if the icon is in the ImageList already
        int index = imageList->Images->IndexOfKey(System::Convert::ToString(iconId));
        if (index > -1)
          return index;
        else {
          // If not, load the image and add it to the ImageList
          Bitmap ^ img = get_icon(iconId);

          if (img != nullptr && imageList->ImageSize.Width == img->Width) {
            MySQL::Utilities::ImageListHelper::Add(img, imageList);
            MySQL::Utilities::ImageListHelper::RestoreImageInfo(imageList);

            index = imageList->Images->Count - 1;
            imageList->Images->SetKeyName(index, System::Convert::ToString(iconId));

            return index;
          } else
            return -1;
        }
      }

      auto add_icon_to_imagelist(IconId iconId) -> int {
        Bitmap ^ img = get_icon(iconId);
        IconSize iconSize = IconSize::Icon48;

        if (img != nullptr) {
          if (img->Width == 16)
            iconSize = IconSize::Icon16;
          else if (img->Width == 24)
            iconSize = IconSize::Icon24;
          else if (img->Width == 32)
            iconSize = IconSize::Icon32;

          return add_icon_to_imagelist(iconId, iconSize);
        } else
          return -1;
      }

      static auto get() -> property ImageList ^ ImageList16 { ImageList ^ { return get_instance()->imageList16; } }

        static auto get() -> property ImageList ^ ImageList24 { ImageList ^ { return get_instance()->imageList24; } }

        static auto get() -> property ImageList ^ ImageList32 { ImageList ^ { return get_instance()->imageList32; } }

        static auto get() -> property ImageList ^ ImageList48 { ImageList ^ { return get_instance()->imageList48; } }
    };
  } // namespace Grt
} // namespace MySQL

#endif // __ICON_MANAGER_H__