////////////////////////////////////////////////////////////////////////////
//	Created		: 24.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using xray.editor.wpf_controls.property_editors.attributes;
using xray.editor.wpf_controls.property_editors.value;

namespace xray.editor.wpf_controls.property_editors.item
{
	/// <summary>
	/// Interaction logic for data_source_editor.xaml
	/// </summary>
	public class data_source_editor: item_editor_base
	{

		static					data_source_editor				( )		
		{
			DefaultStyleKeyProperty.OverrideMetadata( typeof(data_source_editor), new FrameworkPropertyMetadata( typeof(data_source_editor) ) );
		}
		public					data_source_editor				( )		
		{
			m_input_combo_box		= new ComboBox( );
			m_input_combo_box.SelectionChanged += input_combo_box_selection_changed;
			input_combo_box			= m_input_combo_box;
			
			m_data_source_combo_box = new combo_box_editor { DataContext = null };
			m_data_source_combo_box.selection_changed += combo_box_selection_changed;
			data_source_compo_box	= m_data_source_combo_box;

			DataContextChanged		+= delegate
			{
				if( DataContext == null )
					return;

				m_property					= (property)DataContext;
				m_property.owner_editor		= this;
				m_property.sub_properties	= property_extractor.extract( m_property.values, m_property, m_property.extract_settings );

				//create value editors
				property data_source_property	= null;

				foreach ( var sub_property in m_property.sub_properties )
				{
					switch( sub_property.name )
					{
						case "DataSource":		data_source_property = sub_property;		break;
						case "Input":			break;
					}
				}

				m_data_source_attrbiute = (data_source_attribute)m_property.descriptor.Attributes[typeof(data_source_attribute)];
				( (property_descriptor)data_source_property.descriptor ).dynamic_attributes.add( new combo_box_items_attribute( m_data_source_attrbiute.data_source_type, m_data_source_attrbiute.get_items ) );

				m_data_source_combo_box.DataContext		= data_source_property;

				foreach ( var val in m_data_source_attrbiute.input_values_list )
					m_input_combo_box.Items.Add( val );

				m_input_combo_box.SelectedItem = m_data_source_attrbiute.input_value_getter( );

				//var desc = (DescriptionAttribute)m_property.descriptors[0].Attributes[ typeof( DescriptionAttribute ) ];
				//item_text_panel.ToolTip = desc == null ? null : desc.Description;

				combo_box_selection_changed( );
			};
		}

		private					ComboBox				m_input_combo_box;
		private					combo_box_editor		m_data_source_combo_box;

		private					data_source_attribute	m_data_source_attrbiute;
		private					Boolean					m_is_updateing;

		public static readonly	DependencyProperty		value_editorProperty			= DependencyProperty.Register( "value_editor", typeof( value_editor_base ), typeof( data_source_editor ), new PropertyMetadata( null ) );
		public					value_editor_base		value_editor					
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

		public static readonly	DependencyProperty		input_combo_boxProperty			= DependencyProperty.Register( "input_combo_box", typeof( ComboBox ), typeof( data_source_editor ), new PropertyMetadata( null ) );
		public					ComboBox				input_combo_box					
		{
			get
			{
				return (ComboBox)GetValue( input_combo_boxProperty );
			}
			set
			{
				SetValue( input_combo_boxProperty, value );
			}
		}

		public static readonly	DependencyProperty		data_source_compo_boxProperty	= DependencyProperty.Register( "data_source_compo_box", typeof( combo_box_editor ), typeof( data_source_editor ), new PropertyMetadata( null ) );
		public					combo_box_editor		data_source_compo_box			
		{
			get
			{
				return (combo_box_editor)GetValue( data_source_compo_boxProperty );
			}
			set
			{
				SetValue( data_source_compo_boxProperty, value );
			}
		}

		private					void					input_combo_box_selection_changed		( Object sender, SelectionChangedEventArgs e )	
		{
			if( m_is_updateing )
				return;

			var data_source_attr = (data_source_attribute)( (property)DataContext ).descriptor.Attributes[typeof(data_source_attribute)];
			data_source_attr.input_value_setter( (String)(m_input_combo_box.SelectedItem) );
		}
		private					void					combo_box_selection_changed				( )												
		{
			if( m_is_updateing )
				return;

			var			property		= (property)DataContext;

			property	data_property	= null;

			foreach( var sub_property in property.sub_properties.Where( sub_property => sub_property.name == "Data" ) )
				data_property = sub_property;

			if( data_property == null )
				return;

			var data_editor				= value_editor_selector.select_editor( data_property );
			data_editor.DataContext		= null;
			data_editor.item_editor		= this;
			value_editor				= data_editor;
			data_editor.DataContext		= data_property;
		}

		public override			void					update									( )												
		{
			m_is_updateing = true;

			m_input_combo_box.Items.Clear( );
			foreach ( var val in m_data_source_attrbiute.input_values_list )
				m_input_combo_box.Items.Add( val );

			m_input_combo_box.SelectedItem = m_data_source_attrbiute.input_value_getter( );

			m_data_source_combo_box.update	( );
			value_editor.update				( );

			m_is_updateing = false;
		}
	}
}