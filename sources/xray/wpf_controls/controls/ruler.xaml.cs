using System;
using System.Globalization;
using System.Windows;
using System.Windows.Media;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Interaction logic for time_layout_ruler_control.xaml
	/// </summary>
	public partial class ruler
	{
		public		ruler	( )				
		{
			layout_scale		= delegate{ return 1; };
			layout_offset		= delegate{ return 0; };
			layout_width		= delegate{ return 200; };
			value_offset		= delegate{ return 0; };

			top_to_down			= true;

			m_guide_line_set	= new GuidelineSet( );
			m_guide_line_set.GuidelinesY.Add( 0.5 );

			divide_factor_min	= Double.MinValue;

			InitializeComponent( );
		}

		private					Double			m_current_divide_factor;
		private					String			m_format_string;

		private	static			Typeface		m_normal_typeface = new Typeface( "Arial" );
		private	static			Typeface		m_bold_typeface = new Typeface( new FontFamily( "Arial" ), FontStyles.Normal, FontWeights.Bold, FontStretches.Normal );
		private static			Pen				m_pen = new Pen( Brushes.Black, 1 );
		private static			Pen				m_bold_pen = new Pen( Brushes.Black, 2 );

		private readonly		GuidelineSet	m_guide_line_set;

		public					Func<Single>	layout_scale;
		public					Func<Single>	layout_offset;
		public					Func<Single>	layout_width;
		public					Func<Single>	value_offset;

		public					String			name					
		{
			get;set;
		}
		public					Boolean			top_to_down				
		{
			get;set;
		}
		public					Double			divide_factor_min		
		{
			get;set;
		}

		protected override		void			OnRender				( DrawingContext drawing_context )				
		{
			render			( drawing_context );
			base.OnRender	( drawing_context );
		}
		private					void			render					( DrawingContext drawing_context )				
		{
			Double lines_per_segment;

			m_current_divide_factor		= Math.Max( divide_factor_min, grid_helper.scale_to_divide_factor( out lines_per_segment, layout_scale( ) / 2 ) );
			var step					= m_current_divide_factor * layout_scale( );

			if( lines_per_segment == 1 )
				lines_per_segment = 10;

			if( lines_per_segment == 2 )
				lines_per_segment = 5;

			var scroll_offset		= layout_offset	( );
			var cur_value_offset	= value_offset	( );
			var render_width		= layout_width	( );

			m_guide_line_set.GuidelinesX.Clear	( );
			drawing_context.DrawLine			( m_pen, new Point( 0, top_to_down ? 0 : ActualHeight ), new Point( ActualWidth, top_to_down ? 0 : ActualHeight ) );

			var line_count = (Int32)( ActualWidth / step ) + 2;

			for ( var i = 0; i < line_count; ++i )
				m_guide_line_set.GuidelinesX.Add( step * i + ( cur_value_offset % step ) + 0.5 );

			drawing_context.PushGuidelineSet( m_guide_line_set );

			grid_helper.compute_format_string( m_current_divide_factor, out m_format_string );

			for ( var i = 0; i < line_count; ++i )
			{
				Pen pen;
				
				if ( step * i < scroll_offset - 5 || step * i > scroll_offset + render_width + 5 )
				    continue;

				var		offsetted_i_step	= step * i + cur_value_offset % step;
				var		number_offset		= (Int32)( cur_value_offset / step );
				var		bold_line_offset	= number_offset % lines_per_segment;

				var				number				= m_current_divide_factor * ( i - number_offset );
				var				line_height			= 5;
				var				text_offset			= 1;
				FormattedText	text;

				if ( ( i - bold_line_offset ) % lines_per_segment == 0 )
				{
					pen				= m_bold_pen;
					line_height		= 7;
					text			= new FormattedText( grid_helper.format( m_format_string, number ), CultureInfo.CurrentCulture, FlowDirection.LeftToRight, m_bold_typeface, 12, Brushes.DarkRed );
				}
				else
				{
					pen				= m_pen;
					text_offset		= 5;
					text			= new FormattedText( grid_helper.format( m_format_string, number ), CultureInfo.CurrentCulture, FlowDirection.LeftToRight, m_normal_typeface, 8, Brushes.Black );
				}
				
				drawing_context.DrawText	( text, new Point( offsetted_i_step - text.Width / 2, top_to_down ? ActualHeight - text_offset - text.Height : text_offset ) );
				drawing_context.DrawLine	( pen,	new Point( offsetted_i_step, top_to_down ? 0 : ActualHeight ), new Point( offsetted_i_step, top_to_down ? line_height : ActualHeight - line_height ) );
			}

			drawing_context.Pop( );

			if( name != null )
			{
				var name_text					= new FormattedText( name, CultureInfo.CurrentCulture, FlowDirection.LeftToRight, m_bold_typeface, 12, Brushes.Black );
				var y							= top_to_down ? ActualHeight - name_text.Height : 0;
				drawing_context.DrawRectangle	( Brushes.Gray, null, new Rect( 0, y, name_text.Width, name_text.Height ) );
				drawing_context.DrawText		( name_text, new Point( 0, y ) );
				
			}
		}
	}
}
