////////////////////////////////////////////////////////////////////////////
//	Created		: 30.09.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;
using xray.editor.wpf_controls.property_editors.attributes;
using xray.editor.wpf_controls.property_editors.value;

namespace xray.editor.wpf_controls.property_editors.item
{
	/// <summary>
	/// Interaction logic for material_stage_effect_editor.xaml
	/// </summary>
	public class	material_stage_effect_editor: item_editor_base
	{
		static			material_stage_effect_editor					( )										
		{
			DefaultStyleKeyProperty.OverrideMetadata( typeof(material_stage_effect_editor), new FrameworkPropertyMetadata( typeof(material_stage_effect_editor) ) );
		}
		public			material_stage_effect_editor					( )										
		{
			effects_editor	= new external_editor{ DataContext = null }; 

			DataContextChanged					+=	delegate 
			{
				if( DataContext == null )
					return;

				m_property					= (property)DataContext;
				m_property.owner_editor		= this;
				is_can_edit_effect			= ( (material_stage_effect_holder_attribute)m_property.descriptors[0].Attributes[typeof( material_stage_effect_holder_attribute )] ).is_can_edit_effect;
				
			    parent_container.fill_sub_properties	= fill_sub_items_and_return_first;
				parent_container.expand_visibility		= Visibility.Visible;
				parent_container.is_expanded			= true;
			};
		}

		private						Button					m_delete_button;

		public static readonly		DependencyProperty		is_can_edit_effectProperty			= DependencyProperty.Register( "is_can_edit_effect", typeof(Boolean), typeof(material_stage_effect_editor), new FrameworkPropertyMetadata( true ) );
		public						Boolean					is_can_edit_effect					
		{
			get
			{
				return (Boolean)GetValue( is_can_edit_effectProperty );
			}
			set
			{
				SetValue( is_can_edit_effectProperty, value );
			}
		}

		public static readonly		DependencyProperty		effects_editorProperty				= DependencyProperty.Register( "effects_editor", typeof(external_editor), typeof(material_stage_effect_editor), new FrameworkPropertyMetadata( null ) );
		public						external_editor			effects_editor						
		{
			get
			{
				return (external_editor)GetValue( effects_editorProperty );
			}
			set
			{
				SetValue( effects_editorProperty, value );
			}
		}

		private						void					fill_sub_items_and_return_first		( )										
		{
			m_property.sub_properties			= property_extractor.extract( m_property.values, m_property, m_property.extract_settings );
			var first_property					= m_property.sub_properties[0];
			m_property.sub_properties.RemoveAt	( 0 );
			effects_editor.DataContext			= first_property;
		}
		private						void					delete_item_click					( Object sender, RoutedEventArgs e )	
		{
			( (value_collection_editor_base)m_property.property_parent.owner_editor ).remove_sub_property( m_property );
		}

		public override				void					OnApplyTemplate						( )										
		{
			m_delete_button				= (Button)GetTemplateChild( "delete_item" );
			m_delete_button.Click		+= delete_item_click;

			base.OnApplyTemplate		( );
		}
		public override				void					update								( )										
		{
			effects_editor.update( );
		}
	}
}
