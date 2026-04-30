/*
 * Copyright (c) 2026 dev4fun. All rights reserved.
 */

using System;
using System.Collections.Generic;
using System.Text;

namespace Aga.Controls.Tree.NodeControls
{
	public class NodeEventArgs : EventArgs
	{
		private TreeNodeAdv _node;
		public TreeNodeAdv Node
		{
			get { return _node; }
		}

		public NodeEventArgs(TreeNodeAdv node)
		{
			_node = node;
		}
	}
}
