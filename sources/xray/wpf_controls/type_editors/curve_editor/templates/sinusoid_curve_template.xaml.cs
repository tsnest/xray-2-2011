////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Forms;

namespace xray.editor.wpf_controls.curve_editor.templates
{
	/// <summary>
	/// Interaction logic for sinusoid_curve_template_settings_window.xaml
	/// </summary>
	public partial class sinusoid_curve_template
	{
		public					sinusoid_curve_template	( )		
		{
			InitializeComponent( );

			m_amplitude		= 25;
			m_period		= 200;
			m_phase			= 0;
			m_y_offset		= 0;
			m_peak_offset	= 0;
			m_fade_in		= 0;
			m_fade_out		= 0;

			m_amplitude_editor.value	= (Single)m_amplitude;
			m_period_editor.value		= (Single)m_period;
			m_phase_editor.value		= (Single)m_phase;
			m_y_offset_editor.value		= (Single)m_y_offset;
			m_peak_offset_editor.value	= (Single)m_peak_offset;
			m_fade_in_editor.value		= (Single)m_fade_in;
			m_fade_out_editor.value		= (Single)m_fade_out;
		}

		private					Double			m_amplitude;
		private					Double 			m_period;
		private 				Double 			m_phase;
		private 				Double 			m_y_offset;
		private 				Double 			m_peak_offset;
		private 				Double 			m_fade_in;
		private 				Double 			m_fade_out;

		public					Double			amplitude		
		{
			get 
			{
				return m_amplitude; 
			}
			set 
			{
				m_amplitude = value; 
				commit ( );
			}
		}
		public					Double			period			
		{
			get 
			{ 
				return m_period; 
			}
			set 
			{
				m_period = value; 
				commit ( );
			}
		}
		public					Double			phase			
		{
			get 
			{
				return m_phase; 
			}
			set 
			{
				m_phase = value; 
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
		public					Double			peak_offset		
		{
			get 
			{
				return m_peak_offset; 
			}
			set 
			{
				m_peak_offset = value; 
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

		private					void			amplitude_changed	( )		
		{
			amplitude	= m_amplitude_editor.value;
		}
		private					void			period_changed		( )		
		{
			period		= m_period_editor.value;
		}
		private					void			phase_changed		( )		
		{
			phase		= m_phase_editor.value;
		}
		private					void			y_offset_changed	( )		
		{
			y_offset	= m_y_offset_editor.value;
		}
		private					void			peak_offset_changed	( )		
		{
			peak_offset	= m_peak_offset_editor.value;
		}
		private					void			fade_in_changed		( )		
		{
			fade_in		= m_fade_in_editor.value;
		}
		private					void			fade_out_changed	( )		
		{
			fade_out	= m_fade_out_editor.value;
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

			var		direct		= ( phase + Math.PI * 0.5 ) % ( Math.PI * 2 ) / ( Math.PI * 2 ) < 0.5;
			var		local_phase	= phase % Math.PI;
			var		angle		= direct ? Math.PI * 0.5 : -Math.PI * 0.5;
			var		position_x	= m_left_limit + local_phase / Math.PI * ( period / 2 );

			var		fade_exists	= m_fade_in > 0;

			var i = 0;
			for( ; position_x <= m_right_limit; angle += Math.PI, position_x += period / 2, ++i )
			{
				var fade		= 1.0;

				if( position_x - m_left_limit < m_fade_in )
				{
					var t	= ( position_x - m_left_limit ) / m_fade_in;
					fade	= 3*t*t - 2*t*t*t;
				}
				else if( m_right_limit - position_x < m_fade_out )
				{
					var t	= ( m_right_limit - position_x ) / m_fade_out;
					fade	= 3*t*t - 2*t*t*t;
				}

				var position_y	= Math.Sin( angle ) * amplitude * fade + y_offset;

				if( fade_exists && fade == 1 )
				{
					fade_exists = false;
					
					if( position_x != m_left_limit + m_fade_in )
					{
						var fade_end_angle		= angle + Math.PI * ( ( position_x - ( m_left_limit + m_fade_in ) ) / ( period / 2 ) ); 
						var fade_end_angle2		= angle + Math.PI * ( ( position_x - ( m_left_limit + m_fade_in + math.epsilon_3 ) ) / ( period / 2 ) ); 
						var fade_end_y			= Math.Sin( fade_end_angle ) * amplitude + y_offset;
						var fade_end_y2			= Math.Sin( fade_end_angle2 ) * amplitude + y_offset;
						set_key					( i++, m_left_limit + m_fade_in, fade_end_y, key => 
						{
							var tangents_vector					= new Vector( m_left_limit + m_fade_in, fade_end_y ) - new Vector( m_left_limit + m_fade_in + math.epsilon_3, fade_end_y2 );
							if( !key.is_first_key )
							{
								key.left_tangent.compute_tangent	( tangents_vector );
								key.left_segment.update_right		( );
							}
							if( !key.is_last_key )
							{
								key.right_tangent.compute_tangent	( -tangents_vector );
								key.right_segment.update_left		( );
							}
						} );
					}
				}
				if( !fade_exists && fade < 1 )
				{
					fade_exists = true;

					if( position_x != m_right_limit - m_fade_out )
					{

						var fade_start_angle	= angle + Math.PI * ( ( position_x - ( m_right_limit - m_fade_out ) ) / ( period / 2 ) ); 
						var fade_start_angle2	= angle + Math.PI * ( ( position_x - ( m_right_limit - m_fade_out + math.epsilon_3 ) ) / ( period / 2 ) ); 
						var fade_start_y		= Math.Sin( fade_start_angle ) * amplitude + y_offset;
						var fade_start_y2		= Math.Sin( fade_start_angle2 ) * amplitude + y_offset;
						set_key					( i++, m_right_limit - m_fade_out, fade_start_y, key => 
						{
							var tangents_vector					= new Vector( m_right_limit - m_fade_out, fade_start_y ) - new Vector( m_right_limit - m_fade_out + math.epsilon_3, fade_start_y2 );
							if( !key.is_first_key )
							{
								key.left_tangent.compute_tangent	( tangents_vector );
								key.left_segment.update_right		( );
							}
							if( !key.is_last_key )
							{
								key.right_tangent.compute_tangent	( -tangents_vector );
								key.right_segment.update_left		( );
							}
						} );
					}
				}

				if( fade != 1 )
				{
					if( position_x - m_left_limit < m_fade_in && m_pre_template_key != null )
					{
						var pre_template_y	= m_pre_template_key.position_y;
						position_y			= pre_template_y + ( position_y - pre_template_y ) * fade;
					}
					else if( m_right_limit - position_x < m_fade_out && m_post_template_key != null )
					{
						var post_template_y	= m_post_template_key.position_y;
						position_y			= post_template_y + ( position_y - post_template_y ) * fade;
					}
				}

				if( angle % ( Math.PI * 2 ) < Math.PI )
					set_key			( i, position_x + peak_offset * ( period / 2 ), position_y );
				else
					set_key			( i, position_x, position_y );
			}
			trim_keys( i );
			curve.deselect_all_keys( );
		}
		public override			void			show_settings		( )		
		{
			var form				= utils.new_form_host( "Sinusoid Template Settings", this );
			form.ControlBox			= false;
			form.Width				= 264;
			form.Height				= 222;
			form.FormBorderStyle	= FormBorderStyle.FixedSingle;
			form.Show				( );
			base.show_settings		( );
		}
	}
}