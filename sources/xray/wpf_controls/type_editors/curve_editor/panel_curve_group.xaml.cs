////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Linq;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using xray.editor.wpf_controls.curve_editor.effects;

namespace xray.editor.wpf_controls.curve_editor
{
	/// <summary>
	/// Interaction logic for animation_channel_group.xaml
	/// </summary>
	public partial class panel_curve_group
	{
		public				panel_curve_group			( curve_editor_panel panel )	
		{
			InitializeComponent( );
			m_parent_panel = panel;
		}
		public				panel_curve_group			( curve_editor_panel panel, float_curve_group group )	
		{
			InitializeComponent	( );
			header				= group.name;
			m_parent_panel		= panel;
			m_curve_group		= group;

			foreach( var sub_group in group.groups )
				add_group( sub_group );

			foreach( var sub_curve in group.curves )
				add_curve( sub_curve.name, sub_curve.name, sub_curve );
		}

		private				Boolean						m_is_selected;
		private				float_curve_group			m_curve_group;

		private static		DependencyProperty			headerProperty = DependencyProperty.Register( "header", typeof(String), typeof(panel_curve_group), new PropertyMetadata("unnamed") );
		private readonly	curve_editor_panel			m_parent_panel;

		public				String						header				
		{
			get
			{
				return (String)GetValue( headerProperty );
			}
			set
			{
				SetValue( headerProperty, value );
			}
		}
		public				Boolean						is_selected			
		{
			get 
			{
				return m_is_selected;
			}
			set 
			{
				m_is_selected				= value;
				m_header.Background			= ( m_is_selected ) ? new SolidColorBrush( Colors.Gray ) : new SolidColorBrush( Colors.Transparent );

				foreach( var channel in m_sub_channels.Items )
				{
					if( channel is panel_curve_item )
						( (panel_curve_item)channel ).is_selected = value;
					else
						( (panel_curve_group)channel ).is_selected = value;
				}
			}
		}
		public				curve_editor_panel			parent_panel		
		{
			get
			{
				return m_parent_panel;
			}
		}

		private				void						add_effect_click	( Object sender, RoutedEventArgs e )						
		{
			var noise				= new float_curve_noise_effect	( );
			var panel_curve_effect	= new panel_curve_effect		( this, noise ){ name = "noise" };

			m_curve_group.effects.Add					( noise );
			add_noise_to_hierarchy						( this, noise, panel_curve_effect );
			m_effects.Items.Add							( panel_curve_effect );
			panel_curve_effect.open_effect_settings		( );

			e.Handled									= true;
		}
		private				void						add_noise_to_hierarchy( panel_curve_group group, float_curve_noise_effect noise, panel_curve_effect effect_item )
		{
			foreach( var item in group.m_sub_channels.Items.OfType<panel_curve_item>( ) )
			{
				var effect								= new visual_noise_effect( item.visual_curve, noise );
				effect_item.add_effect					( effect );
			}
			foreach( var sub_group in group.m_sub_channels.Items.OfType<panel_curve_group>( ) )
				add_noise_to_hierarchy( sub_group, noise, effect_item );
		}
		private				void						mouse_down			( Object sender, MouseButtonEventArgs e )					
		{
			if( e.ChangedButton == MouseButton.Left )
			{
				if( Keyboard.PrimaryDevice.IsKeyDown( Key.LeftCtrl ) )
				{
					is_selected = !is_selected;
				}
				else
				{
					m_parent_panel.deselect_all_groups( );
					is_selected = true;
				}
				e.Handled = true;
			}
		}
		private static		void						get_group_curves_r	( panel_curve_group group, List<visual_curve> list )		
		{
			foreach( var item in group.m_sub_channels.Items )
			{
				if( item is panel_curve_item )
					list.Add( ( (panel_curve_item)item ).visual_curve );
				else
					get_group_curves_r( (panel_curve_group)item, list );
			}
		}

		internal			void						remove_effect		( panel_curve_effect effect )								
		{
			m_effects.Items.Remove			( effect );

			foreach( var item in m_sub_channels.Items.OfType<panel_curve_item>( ) )
			{
				if( item.visual_curve.effects.Count == 0 )
					item.visual_curve.path.StrokeDashArray	= null;
			}
		}

		public				visual_curve				add_curve			( String curve_key, String name, float_curve curve )		
		{
			var item					= new panel_curve_item( this ){ header = name, curve_key = curve_key, curve = curve, is_selected = true };
			m_sub_channels.Items.Add	( item );
			return item.visual_curve;
		}
		public				panel_curve_group			add_group			( String name )												
		{
			var group					= new panel_curve_group( m_parent_panel ) { header = name };
			m_sub_channels.Items.Add	( group );

			return group;
		}
		public				panel_curve_group			add_group			( float_curve_group group )									
		{
			var panel_group					= new panel_curve_group( m_parent_panel, group );

			m_sub_channels.Items.Add	( panel_group );

			return panel_group;
		}
		public				curves_link					link_group_curves	( )															
		{
			var list			= new List<visual_curve>( );
			get_group_curves_r	( this, list );

			return				m_parent_panel.link_curves( list );
		}
		public				void						get_selected_curves	( List<visual_curve> selected_curves )						
		{
			foreach( var channel in m_sub_channels.Items  )
			{
				if( channel is panel_curve_group )
					( (panel_curve_group)channel ).get_selected_curves( selected_curves );

				if( channel is panel_curve_item && ( (panel_curve_item)channel ).is_selected )
					selected_curves.Add( ( (panel_curve_item)channel ).visual_curve );
			}
		}
	}
}
