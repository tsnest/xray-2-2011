////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xray.editor.wpf_controls.panels
{
	public class scale_to_fit_control: ContentControl
	{
		private Size m_last_measure_constraint;

		protected override		Size	ArrangeOverride		( Size arrange_size )						
		{
			var arrange_bounds = m_last_measure_constraint;
		    if ( VisualChildrenCount > 0 )
		    {
		        var visual_child = (FrameworkElement) GetVisualChild( 0 );
		        if ( visual_child != null )
		        {
					var child_size = visual_child.DesiredSize;
		            visual_child.Arrange(new Rect(arrange_size));
					if( 
						arrange_bounds.Width != child_size.Width ||
						arrange_bounds.Height != child_size.Height  
						)
					{
						Double x_scale = 1;
						Double y_scale = 1;

						if( visual_child.LayoutTransform is ScaleTransform )
						{
							var transform = (ScaleTransform)visual_child.LayoutTransform;
							x_scale = transform.ScaleX;
							y_scale = transform.ScaleY;
						}
						
						Double real_width	= child_size.Width / x_scale;
						Double real_height	= child_size.Height / y_scale;

						visual_child.LayoutTransform = new ScaleTransform(
							arrange_bounds.Width / real_width,
							arrange_bounds.Height / real_height
						);
					}
		        }
		    }
		    return arrange_bounds;
		}
		protected override		Size	MeasureOverride		( Size constraint )							
		{
			m_last_measure_constraint = constraint;

			var child_size = base.MeasureOverride( new Size( Double.PositiveInfinity, Double.PositiveInfinity ) );

			if( Double.IsInfinity( constraint.Width ) || Double.IsInfinity( constraint.Height ) )
				m_last_measure_constraint = child_size;

			return child_size;
		}
		protected override		void	OnContentChanged	( Object old_content, Object new_content )	
		{
			base.OnContentChanged(old_content, new_content);
			InvalidateMeasure( );
		}
	}
}
