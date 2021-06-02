////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System.Windows.Controls;

namespace xray.editor.wpf_controls.panels
{
	public class scroll_viewer: ScrollViewer
	{
		protected override void OnMouseLeftButtonDown(System.Windows.Input.MouseButtonEventArgs e)
		{
			base.OnMouseLeftButtonDown(e);
			e.Handled = false;
		}
	}
}
