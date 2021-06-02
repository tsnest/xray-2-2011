////////////////////////////////////////////////////////////////////////////
//	Created		: 22.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Windows;

namespace xray.editor.wpf_controls.curve_editor.effects
{
	/// <summary>
	/// Interaction logic for noise_effect_settings_window.xaml
	/// </summary>
	public partial class noise_effect_settings_window
	{
		public				noise_effect_settings_window		( )		
		{
			InitializeComponent( );
		}

		private				List<visual_noise_effect>	m_effects;

		internal			List<visual_noise_effect>	effects
		{
			get
			{
				return m_effects; 
			}
			set 
			{
				m_effects = value; 

				m_ceed_number_editor.value		= m_effects[0].seed;
				m_frequency_number_editor.value	= m_effects[0].frequency;
				m_strength_number_editor.value	= m_effects[0].strength;
				m_fade_in_number_editor.value	= m_effects[0].fade_in;
				m_fade_out_number_editor.value	= m_effects[0].fade_out;
			}
		}

		private				void			done_click					( Object sender, RoutedEventArgs e )			
		{
			utils.get_parent_form( this ).Close( );
		}
		private				void			ceed_changed				( )												
		{
			foreach( var effect in m_effects )
				effect.seed			= (Int32)m_ceed_number_editor.value;
		}
		private				void			frequency_changed			( )		
		{
			foreach( var effect in m_effects )
				effect.frequency	= m_frequency_number_editor.value;
		}
		private				void			strength_changed			( )		
		{
			foreach( var effect in m_effects )
				effect.strength		= m_strength_number_editor.value;
		}
		private				void			fade_in_changed				( )		
		{
			foreach( var effect in m_effects )
				effect.fade_in		= m_fade_in_number_editor.value;
		}
		private				void			fade_out_changed			( )		
		{
			foreach( var effect in m_effects )
				effect.fade_out		= m_fade_out_number_editor.value;
		}
	}
}