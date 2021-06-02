////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Media3D;

namespace xray.editor.wpf_controls.color_matrix_editor
{
	/// <summary>
	/// Interaction logic for gradient_rect.xaml
	/// </summary>
	public partial class gradient_rect
	{
		public		gradient_rect	( )				
		{
			InitializeComponent();
		}

		private		Point		m_top_left;
		private		Point		m_bottom_right;

		public		Double		left_x				
		{
			get
			{
				return m_top_left.X;
			}
			set
			{
				m_top_left.X		= value;
				compute_position	( );
				compute_size		( );
			}
		}
		public		Double		left_y				
		{
			get
			{
				return m_top_left.Y;
			}
			set
			{
				m_top_left.Y		= value;
				compute_position	( );
				compute_size		( );
			}
		}
		public		Double		right_x				
		{
			get
			{
				return m_bottom_right.X;
			}
			set
			{
				m_bottom_right.X	= value;
				compute_size		( );
			}
		}
		public		Double		right_y				
		{
			get
			{
				return m_bottom_right.Y;
			}
			set
			{
				m_bottom_right.Y	= value;
				compute_size		( );
			}
		}
		public		Double		width				
		{
			get
			{
				return m_bottom_right.X - m_top_left.X;
			}
		}
		public		Double		height				
		{
			get
			{
				return m_bottom_right.Y - m_top_left.Y;
			}
		}

		public		Point		top_left			
		{
			get
			{
				return m_top_left;
			}
			set
			{
				m_top_left		= value;

				if( m_top_left.X > m_bottom_right.X )
					m_bottom_right.X = m_top_left.X;

				if( m_top_left.Y > m_bottom_right.Y )
					m_bottom_right.Y = m_top_left.Y;

				compute_position	( );
				compute_size		( );
			}
		}
		public		Point		top_right			
		{
			get
			{
				return new Point( m_bottom_right.X, m_top_left.Y );
			}
			set
			{
				m_bottom_right.X	= value.X;
				m_top_left.Y		= value.Y;

				if( m_top_left.X > m_bottom_right.X )
					m_top_left.X = m_bottom_right.X;

				if( m_top_left.Y > m_bottom_right.Y )
					m_bottom_right.Y = m_top_left.Y;

				compute_position	( );
				compute_size		( );
			}
		}
		public		Point		bottom_left			
		{
			get
			{
				return new Point( m_top_left.X, m_bottom_right.Y );
			}
			set
			{
				m_top_left.X		= value.X;
				m_bottom_right.Y	= value.Y;

				if( m_top_left.X > m_bottom_right.X )
					m_bottom_right.X = m_top_left.X;

				if( m_top_left.Y > m_bottom_right.Y )
					m_top_left.Y = m_bottom_right.Y;

				compute_position	( );
				compute_size		( );
			}
		}
		public		Point		bottom_right		
		{
			get
			{
				return m_bottom_right;
			}
			set
			{
				m_bottom_right		= value;

				if( m_top_left.X > m_bottom_right.X )
					m_top_left.X = m_bottom_right.X;

				if( m_top_left.Y > m_bottom_right.Y )
					m_top_left.Y = m_bottom_right.Y;

				compute_size		( );
			}
		}

		public		color_rgb	color_top_left		
		{
			get
			{
				return (color_rgb)m_shader.color1;
			}
			set
			{
				m_shader.color1 = (Point4D)value;
			}
		}
		public		color_rgb	color_top_right		
		{
			get
			{
				return (color_rgb)m_shader.color3;
			}
			set
			{
				m_shader.color3 = (Point4D)value;
			}
		}
		public		color_rgb	color_bottom_left	
		{
			get
			{
				return (color_rgb)m_shader.color2;
			}
			set
			{
				m_shader.color2 = (Point4D)value;
			}
		}
		public		color_rgb	color_bottom_right	
		{
			get
			{
				return (color_rgb)m_shader.color4;
			}
			set
			{
				m_shader.color4 = (Point4D)value;
			}
		}
	
		private		void		compute_position	( )		
		{
			SetValue( Canvas.LeftProperty, left_x );
			SetValue( Canvas.TopProperty, left_y );
		}
		private		void		compute_size		( )		
		{
			SetValue( WidthProperty, width );
			SetValue( HeightProperty, height );
		}

	}
}
