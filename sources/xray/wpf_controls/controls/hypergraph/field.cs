////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xray.editor.wpf_controls.hypergraph
{
	public class field: Canvas
	{
		public field( )
		{
			grid_step				= 20;
			bold_line_frequency		= 5;
		}

		private static readonly		Pen						m_grid_pen						= new Pen( new SolidColorBrush( new Color{ A = 255, R = 189, G = 189, B = 190 } ), 1 );
		private static readonly		Pen						m_grid_dark_pen					= new Pen( new SolidColorBrush( new Color{ A = 255, R = 120, G = 120, B = 120 } ), 1 );
		private static readonly		Pen						m_grid_zero_pen					= new Pen( new SolidColorBrush( new Color{ A = 255, R = 200, G = 120, B = 120 } ), 1 );
		private static readonly		GuidelineSet			m_guidelines					= new GuidelineSet( );

		private						Size					m_bounds_infalte_amount			= new Size( 150, 150 );
		private						Boolean					m_is_fix_min_size_by_parent;
		private						Boolean					m_is_grid_enabled;
		private						Rect					m_control_rect;

		internal					hypergraph_control		hypergraph													
		{
			get;set;
		}
		public						Boolean					is_fix_min_size_by_parent									
		{
			get 
			{
				return m_is_fix_min_size_by_parent; 
			}
			set 
			{
				m_is_fix_min_size_by_parent = value; 
				InvalidateMeasure( );
			}
		}
		public						Boolean					is_fixed_size												
		{
			get;set;
		}				
		public						Boolean					is_grid_enabled												
		{
			get
			{
				return m_is_grid_enabled; 
			}
			set 
			{
				m_is_grid_enabled = value; 
				InvalidateVisual( );
			}
		}
		public						Size					bounds_infalte_amount										
		{
			get 
			{
				return m_bounds_infalte_amount; 
			}
			set 
			{
				m_bounds_infalte_amount = value; 
				InvalidateMeasure( );
			}
		}
		public						Int32					grid_step													
		{
			get;set;
		}
		public						Int32					bold_line_frequency											
		{						
			get;set;
		}

		private static				Double					element_x				( UIElement element )				
		{
			var x = 0.0;
			var left = GetLeft( element );
			if ( !Double.IsNaN( left ) )
				x = left;

			return x;
		}
		private static				Double					element_y				( UIElement element )				
		{
			var y = 0.0;
			var top = GetTop( element );
			if ( !Double.IsNaN( top ) )
				y = top;
			return y;
		}

		protected override			Size					ArrangeOverride			( Size arrange_size )				
		{
			if( m_is_fix_min_size_by_parent )
			{
				var size = new Size( ((FrameworkElement)Parent).ActualWidth, ((FrameworkElement)Parent).ActualHeight );

				size.Width	= Math.Max( size.Width, m_control_rect.Size.Width );
				size.Height = Math.Max( size.Height, m_control_rect.Size.Height );

				m_control_rect.Size = size;
			}
			if( hypergraph.need_expand_to_field )
			{
				hypergraph.MinWidth		= m_control_rect.Size.Width;
				hypergraph.MinHeight	= m_control_rect.Size.Height;
			}

			foreach ( UIElement element in InternalChildren )
			{
				if ( element == null )
					continue;

				var x = element_x( element );
				var y = element_y( element );
				
				element.Arrange( new Rect( new Point( x, y ), element.DesiredSize ) );
			}
			
			return m_control_rect.Size;
		}
		protected override			Geometry				GetLayoutClip			( Size layout_slot_size )			
		{
			return ClipToBounds ? new RectangleGeometry( m_control_rect ) : null;
		}
		protected override			Size					MeasureOverride			( Size constraint )					
		{
			var available_size = new Size( Double.PositiveInfinity, Double.PositiveInfinity );
			
			var rect = new Rect( 0, 0, 0, 0 );
			
			foreach ( UIElement element in InternalChildren )
			{
				if ( element != null )
				{
					var x = element_x( element );
					var y = element_y( element );
					element.Measure( available_size );
					if( element is node )
						rect.Union( new Rect( new Point( x, y ), element.DesiredSize ) );
				}
			}
			rect.Inflate( m_bounds_infalte_amount );

			if( m_is_fix_min_size_by_parent )
			{
				rect.Width	= Math.Max( rect.Width, ( (FrameworkElement)Parent ).ActualWidth );
				rect.Height = Math.Max( rect.Height, ( (FrameworkElement)Parent ).ActualHeight );
			}

			m_control_rect = rect;

			if( is_fixed_size )
			{
				m_control_rect = new Rect( new Point(0, 0), new Size( Width, Height ) );
				return new Size( Width, Height );
			}

			return rect.Size;
		}
		protected override			void					OnRender				( DrawingContext drawing_context )	
		{
			base.OnRender( drawing_context );

			if( !is_grid_enabled )
				return;

			m_guidelines.GuidelinesX.Clear( );
			m_guidelines.GuidelinesY.Clear( );

			//Compute bounds
			var x_start				= m_control_rect.X - m_control_rect.X % grid_step;
			var y_start				= m_control_rect.Y - m_control_rect.Y % grid_step;
			var x_end				= m_control_rect.Right;
			var y_end				= m_control_rect.Bottom;

			//Set guidelines
			m_guidelines.GuidelinesX.Clear( );
			m_guidelines.GuidelinesY.Clear( );

			for( var line_position = x_start; line_position < x_end; line_position += grid_step )
				m_guidelines.GuidelinesX.Add( line_position + 0.5 );
			
			for( var line_position = y_start; line_position < y_end; line_position += grid_step )
			    m_guidelines.GuidelinesY.Add( line_position + 0.5 );
			    
			drawing_context.PushGuidelineSet( m_guidelines );

			//Draw vertical lines
			var line_logic_position	= (Int32)( x_start / grid_step );
		    for( var line_position = x_start; line_position < x_end; line_position += grid_step, ++line_logic_position )
		        drawing_context.DrawLine( ( line_logic_position % bold_line_frequency == 0 ) ? (line_logic_position == 0) ? m_grid_zero_pen : m_grid_dark_pen : m_grid_pen, new Point( line_position, m_control_rect.Top ), new Point( line_position, m_control_rect.Bottom ) );

			//Draw horizontal lines
			line_logic_position		= (Int32)( y_start / grid_step );
		    for( var line_position = y_start; line_position < y_end; line_position += grid_step, ++line_logic_position )
		        drawing_context.DrawLine( ( line_logic_position % bold_line_frequency == 0 ) ? (line_logic_position == 0) ? m_grid_zero_pen : m_grid_dark_pen : m_grid_pen, new Point( m_control_rect.Left, line_position ), new Point( m_control_rect.Right, line_position ) );
			   
			drawing_context.Pop( );

			drawing_context.DrawRectangle( null, new Pen( new SolidColorBrush( Colors.Black ), 3 ), m_control_rect );
			//drawing_context.DrawLine( new Pen( new SolidColorBrush( Colors.Black ), 3 ), new Point(-20, 0), new Point(20, 0) );
			//drawing_context.DrawLine( new Pen( new SolidColorBrush( Colors.Black ), 3 ), new Point(0, -20), new Point(0, 20) );
		}
	}
}
