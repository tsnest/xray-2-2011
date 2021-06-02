////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System.Windows;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.panels
{
	public class single_place_panel: Panel
	{
		protected override		Size		ArrangeOverride		( Size final_size )			
		{
			foreach ( UIElement element in InternalChildren )
			{
				if( element == null )
					continue;

				element.Arrange( new Rect( final_size ) );
			}

			return final_size;
		}
		protected override		Size		MeasureOverride		( Size available_size )		
		{
			var need_size = new Size( 1, 1 );

			foreach ( UIElement element in InternalChildren )
			{
				if( element == null )
					continue;

				element.Measure( available_size );
				if( element.DesiredSize.Width > need_size.Width )
					need_size.Width = element.DesiredSize.Width;
				if( element.DesiredSize.Height > need_size.Height )
					need_size.Width = element.DesiredSize.Height;
			}
			return need_size;
		}
	}
}
