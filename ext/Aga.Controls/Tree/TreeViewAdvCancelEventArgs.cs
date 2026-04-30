/*
 * Copyright (c) 2026 dev4fun. All rights reserved.
 */

using System;
using System.Collections.Generic;
using System.Text;

namespace Aga.Controls.Tree
{
	public class TreeViewAdvCancelEventArgs : TreeViewAdvEventArgs
	{
		private bool _cancel;

		public bool Cancel
		{
			get { return _cancel; }
			set { _cancel = value; }
		}

		public TreeViewAdvCancelEventArgs(TreeNodeAdv node)
			: base(node)
		{
		}

	}
}
