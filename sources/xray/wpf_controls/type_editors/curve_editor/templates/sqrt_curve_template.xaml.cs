////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Forms;
using xray.editor.wpf_controls.curve_editor;

namespace xray.editor.wpf_controls.type_editors.curve_editor.templates
{
	/// <summary>
	/// Interaction logic for hyperbola_curve_template.xaml
	/// </summary>
	public partial class sqrt_curve_template
	{
		public					sqrt_curve_template	( )		
		{
			InitializeComponent( );

			m_x_scale		= 10;
			m_y_scale		= 10;
			m_y_offset		= -100;
			m_fade_in		= 0;
			m_fade_out		= 0;

			m_x_scale_editor.value		= (Single)m_x_scale;
			m_y_scale_editor.value		= (Single)m_y_scale;
			m_y_offset_editor.value		= (Single)m_y_offset;
			//m_fade_in_editor.value		= (Single)m_fade_in;
			//m_fade_out_editor.value		= (Single)m_fade_out;
		}

		private					Double			m_x_scale;
		private					Double 			m_y_scale;
		private 				Double 			m_y_offset;
		private 				Double 			m_fade_in;
		private 				Double 			m_fade_out;

		public					Double			x_scale			
		{
			get 
			{
				return m_x_scale; 
			}
			set 
			{
				m_x_scale = value; 
				commit ( );
			}
		}
		public					Double			y_scale			
		{
			get 
			{ 
				return m_y_scale; 
			}
			set 
			{
				m_y_scale = value; 
				commit ( );
			}
		}
		public					Double			y_offset		
		{
			get 
			{
				return m_y_offset; 
			}
			set 
			{
				m_y_offset = value; 
				commit ( );
			}
		}
		public					Double			fade_in			
		{
			get 
			{
				return m_fade_in; 
			}
			set 
			{
				m_fade_in = value; 
				commit ( );
			}
		}
		public					Double			fade_out		
		{
			get 
			{
				return m_fade_out; 
			}
			set 
			{
				m_fade_out = value; 
				commit ( );
			}
		}

		private					void			x_scale_changed		( )		
		{
			x_scale	= m_x_scale_editor.value;
		}
		private					void			y_scale_changed		( )		
		{
			y_scale		= m_y_scale_editor.value;
		}
		private					void			y_offset_changed	( )		
		{
			y_offset	= m_y_offset_editor.value;
		}
		private					void			left_limit_changed	( )		
		{
			m_left_limit	= m_left_limit_editor.value;
			commit			( );
		}
		private					void			right_limit_changed	( )		
		{
			m_right_limit	= m_right_limit_editor.value;
			commit			( );
		}
		//private					void			fade_in_changed		( )		
		//{
		//    fade_in		= m_fade_in_editor.value;
		//}
		//private					void			fade_out_changed	( )		
		//{
		//    fade_out	= m_fade_out_editor.value;
		//}

		protected override		void			initialize			( )		
		{
			base.initialize( );

			if( !m_is_manual_left_limit )
				m_left_limit_panel.Visibility = Visibility.Collapsed;

			if( !m_is_manual_right_limit )
				m_right_limit_panel.Visibility = Visibility.Collapsed;

			m_left_limit_editor.value	= (Single)m_left_limit;
			m_right_limit_editor.value	= (Single)m_right_limit;
		}
		
		public override			void			commit				( )		
		{
			if( curve == null )
				return;

			var		function_position = 0;

			var i = 0;
			for( ; m_left_limit + function_position * m_x_scale <= m_right_limit; ++i )
			{
				var position_y		= Math.Sqrt( function_position ) * y_scale + y_offset;
				set_key				( i, m_left_limit + function_position * m_x_scale , position_y );
				function_position	= ( function_position == 0 ) ? 1 : function_position * 2;
			}
			if( m_left_limit + function_position * m_x_scale - m_right_limit > 0.001 )
			{
				var position_y		= Math.Sqrt( ( m_right_limit - m_left_limit ) / m_x_scale ) * y_scale + y_offset;
				set_key				( i, m_right_limit, position_y );
				++i;
			}

			trim_keys				( i );
			set_last_key_tangent	( );	

			curve.deselect_all_keys	( );
		}
		protected new 			void			set_key				( Int32 key_index, Double x, Double y )		
		{
			set_key( key_index, x, y, key => key.type_of_key = float_curve_key_type.locked_auto );
		}
		protected				void			set_last_key_tangent( )		
		{
			var key = template_keys[template_keys.Count - 1];
			
			key.type_of_key		= float_curve_key_type.breaked;
			key.left_tangent.compute_tangent( (Vector)key.prev_key.position - (Vector)key.position );
			key.left_tangent.update_visual	( );
			key.left_segment.update_right	( );
		}
		public override			void			show_settings		( )		
		{
			var form				= utils.new_form_host( "Sqrt Template Settings", this );
			form.ControlBox			= false;
			form.Width				= 263;
			form.Height				= 155;
			form.FormBorderStyle	= FormBorderStyle.FixedSingle;
			form.Show				( );
			base.show_settings		( );
		}
		
	}
}
