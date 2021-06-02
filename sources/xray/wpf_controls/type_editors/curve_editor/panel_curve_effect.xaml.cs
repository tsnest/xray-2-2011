////////////////////////////////////////////////////////////////////////////
//	Created		: 22.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Input;
using xray.editor.wpf_controls.curve_editor.effects;

namespace xray.editor.wpf_controls.curve_editor
{
	/// <summary>
	/// Interaction logic for panel_curve_item_effect.xaml
	/// </summary>
	public partial class panel_curve_effect
	{
		public	panel_curve_effect	( panel_curve_item panel_item, float_curve_effect effect )				
		{
			InitializeComponent		( );
			m_panel_item			= panel_item;
			m_visual_effects				= new List<visual_effect_base>( 1 );
		}
		public	panel_curve_effect	( panel_curve_group curve_item, float_curve_effect effect )			
		{
			InitializeComponent		( );
			m_panel_group			= curve_item;
			m_visual_effects				= new List<visual_effect_base>( );
		}

		private readonly	panel_curve_item			m_panel_item;
		private readonly	panel_curve_group			m_panel_group;
		private				List<visual_effect_base>	m_visual_effects;
		private				float_curve_effect			m_effect;

		internal			float_curve_effect			effect						
		{
			get 
			{
				return m_effect;
			}
			set 
			{
				m_effect = value;
			}
		}
		internal			String					name						
		{
			get
			{
				return m_text_block.Text;
			}
			set
			{
				m_text_block.Text = value;
			}
		}

		private				void					mouse_down					( Object sender, MouseButtonEventArgs e )		
		{
			if( e.ChangedButton == MouseButton.Left && e.ClickCount > 1 )
			{
				open_effect_settings( );
				e.Handled = true;
			}
		}
		private				void					remove_click				( Object sender, RoutedEventArgs e )			
		{
			foreach( var current_effect in m_visual_effects )
			{
				current_effect.dispose							( );
				if( m_panel_group != null )
					m_panel_group.parent_panel.items.Remove		( current_effect.effect_curve );
				else
					m_panel_item.curves_panel.items.Remove		( current_effect.effect_curve );
			}

			if( m_panel_group != null )
				m_panel_group.remove_effect	( this );
			else
				m_panel_item.remove_effect	( this );
		}
		internal			void					add_effect					( visual_effect_base new_visual_effect )						
		{
			m_visual_effects.Add							( new_visual_effect );
			if( m_panel_group != null )
				m_panel_group.parent_panel.items.Add		( new_visual_effect.effect_curve );
			else
				m_panel_item.curves_panel.items.Add		( new_visual_effect.effect_curve );
		}
		public				void					open_effect_settings		( )
		{
			m_visual_effects[0].open_effect_settings( Window.GetWindow( this ), m_visual_effects );
		}
	}
}
