////////////////////////////////////////////////////////////////////////////
//	Created		: 05.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using xray.editor.wpf_controls.property_editors.attributes;
using xray.editor.wpf_controls.property_editors.value;

namespace xray.editor.wpf_controls.property_editors.item
{
	/// <summary>
	/// Interaction logic for custom_property_item_editor.xaml
	/// </summary>
	public class custom_item_editor: item_editor_base
	{
		static					custom_item_editor				( )		
		{
			DefaultStyleKeyProperty.OverrideMetadata( typeof( custom_item_editor ), new FrameworkPropertyMetadata( typeof(custom_item_editor) ) );
		}
		public					custom_item_editor				( )		
		{
			m_type_box						= new ComboBox( );
			m_type_box.SelectionChanged		+= type_box_selection_changed;
			m_type_box.DropDownOpened		+= combo_box_drop_down_opened;
			type_box						= m_type_box;

			DataContextChanged += delegate
			{
				if( DataContext == null || !( DataContext is property ) )
					return;

				m_property					= (property)DataContext;
				m_property.owner_editor		= this;

				m_custom_property_attribute = (custom_property_attribute)m_property.descriptor.Attributes[typeof(custom_property_attribute)];
				Debug.Assert( m_custom_property_attribute != null );

				m_initializing = true;
				foreach( var box_value in m_custom_property_attribute.box_values_getter( ) )
					m_type_box.Items.Add( box_value );
				
				m_type_box.SelectedValue = m_custom_property_attribute.box_value_getter( );
				m_initializing = false;

				select_value_editor	( );
			};
		}

		private					Boolean							m_initializing;
		private					custom_property_attribute		m_custom_property_attribute;
		private					ComboBox						m_type_box;

		public static readonly	DependencyProperty				value_editorProperty		= DependencyProperty.Register( "value_editor", typeof( value_editor_base ), typeof( custom_item_editor ), new PropertyMetadata( null ) );
		public					value_editor_base				value_editor				
		{
			get	
			{
				return (value_editor_base)GetValue( value_editorProperty );
			}
			set	
			{
				SetValue( value_editorProperty, value );
			}
		}

		public static readonly	DependencyProperty				type_boxProperty			= DependencyProperty.Register( "type_box", typeof( ComboBox ), typeof( custom_item_editor ), new PropertyMetadata( null ) );
		public					ComboBox						type_box					
		{
			get	
			{
				return (ComboBox)GetValue( type_boxProperty );
			}
			set	
			{
				SetValue( type_boxProperty, value );
			}
		}

		private					void	type_box_selection_changed	( Object sender, SelectionChangedEventArgs e )	
		{
			if( m_initializing )
				return;

			if( e.AddedItems.Count != e.RemovedItems.Count || e.AddedItems[0] != e.RemovedItems[0] )
			{
				m_custom_property_attribute.box_value_setter	( (String) m_type_box.SelectedItem );
				select_value_editor								( );
			}
		}
		private					void	combo_box_drop_down_opened	( Object sender, EventArgs e)					
		{
			var last_selected_item = m_type_box.SelectedItem;
			
			m_type_box.Items.Clear();
			
			foreach( var box_value in m_custom_property_attribute.box_values_getter( ) )
			{
				m_type_box.Items.Add( box_value );
			}

			m_type_box.SelectedItem = last_selected_item;

		}
		private					void	select_value_editor			( )												
		{
			if( value_editor != null )
			{
				value_editor.m_property		= null;
				value_editor				= null;
			}

			if( m_property.inner_properties != null )
			{
				foreach( var inner_prop in m_property.inner_properties )
				{
					var editor				= value_editor_selector.select_editor( inner_prop );
					editor.item_editor		= this;
					parent_container.add_right_pocket_inner( editor );
					editor.DataContext		= inner_prop;
				}
			}
			
			{
				var editor			= value_editor_selector.select_editor( m_property );
				editor.DataContext	= null;
				editor.item_editor	= this;
				editor.DataContext	= null;
				value_editor		= editor;
				editor.DataContext	= m_property;
			}
		}

		public override			void	update						( )												
		{
			m_initializing				= true;
			m_type_box.Items.Clear		( );

			foreach( var box_value in m_custom_property_attribute.box_values_getter( ) )
				m_type_box.Items.Add	( box_value );
			
			m_type_box.SelectedValue	= m_custom_property_attribute.box_value_getter( );
			m_initializing				= false;

			value_editor.update			( );
		}
	}
}
