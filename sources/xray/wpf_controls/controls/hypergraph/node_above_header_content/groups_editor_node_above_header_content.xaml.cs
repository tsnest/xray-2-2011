////////////////////////////////////////////////////////////////////////////
//	Created		: 10.05.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
using System;
using Microsoft.Windows.Controls;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.hypergraph
{
	public partial class groups_editor_node_above_header_content : UserControl
	{
		public groups_editor_node_above_header_content()
		{
			InitializeComponent();
		}

		public DoubleUpDown up_down
		{
			get
			{
				return m_up_down;
			}
		}

	}
}
