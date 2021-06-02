////////////////////////////////////////////////////////////////////////////
//	Created		: 07.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xray.editor.wpf_controls.panels
{
	public class centralized_scroll_panel: Panel
	{

		public		centralized_scroll_panel	( )		
		{
			m_origin					= new Vector( 0.5, 0.5 );
		}

		private		Vector					m_offset;
		private		Vector					m_origin;

		public		Vector					offset				
		{
			get 
			{
				return m_offset; 
			}
			set 
			{
				m_offset			= value; 
				InvalidateArrange	( );
				InvalidateVisual	( );
			}
		}
		public						Vector					origin														
		{
			get 
			{
				return m_origin; 
			}
			set 
			{
				m_origin			= value;
				InvalidateArrange	( );
				InvalidateVisual	( );
			}
		}

		protected override		Size		ArrangeOverride( Size final_size )		
		{
			foreach( FrameworkElement element in InternalChildren )
			{
				if ( element != null )
				{
					var center_point		= new Vector( final_size.Width * origin.X, final_size.Height * origin.Y );
					center_point			-= offset;

					var element_position	= new Point( );

					if( element is i_centralized_control )
						element_position	= ( (i_centralized_control)element ).control_position;
					
					element_position.Offset	( center_point.X, center_point.Y );

					element.Arrange			( new Rect( element_position, element.DesiredSize ) );
				}
			} 
			
			return final_size;
		}
	}
}
