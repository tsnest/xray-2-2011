////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.curve_editor
{
	public class curves_panel_presenter: ContentControl
	{
		public curves_panel_presenter( )
		{
			Loaded += delegate
			{
				var panel	= find_curve_editor_panel( );
				if( panel != null )
					Content		= panel.curves_panel;
			};
		}

		private		curve_editor_panel		find_curve_editor_panel( )
		{
			var panel = LogicalTreeHelper.GetParent( this );

			while( panel != null && !( panel is curve_editor_panel ) )
				panel = LogicalTreeHelper.GetParent( panel );

			return (curve_editor_panel)panel;
		}

		

		protected override void OnVisualParentChanged(DependencyObject oldParent)
		{
			var panel = find_curve_editor_panel( );
			Content = panel;
			base.OnVisualParentChanged(oldParent);
		}
	}
}