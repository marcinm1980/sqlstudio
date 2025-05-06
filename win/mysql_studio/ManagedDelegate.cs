/**
 * Copyright (c) 2008, 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

using MySQL.Forms;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MySqlStudio.X
{
  class ManagedDelegate : ManagedDockDelegate
  {
    public ManagedDelegate()
      : base(null)
    {
    }

    public override void dock_view(object represented_object, AppViewDockContent view, string arg1, int arg2)
    {
      throw new NotImplementedException();
    }

    public override System.Drawing.Size get_size(object represented_object)
    {
      throw new NotImplementedException();
    }

    public override string get_type(object represented_object)
    {
      throw new NotImplementedException();
    }

    public override bool select_view(object represented_object, AppViewDockContent view)
    {
      throw new NotImplementedException();
    }

    public override AppViewDockContent selected_view()
    {
      throw new NotImplementedException();
    }

    public override void set_view_title(object represented_object, AppViewDockContent view, string title)
    {
      throw new NotImplementedException();
    }

    public override void undock_view(object represented_object, AppViewDockContent view)
    {
      throw new NotImplementedException();
    }

    public override AppViewDockContent view_at_index(int i)
    {
      throw new NotImplementedException();
    }

    public override int view_count()
    {
      throw new NotImplementedException();
    }
  }
}
