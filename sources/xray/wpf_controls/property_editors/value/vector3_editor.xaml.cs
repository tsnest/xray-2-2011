////////////////////////////////////////////////////////////////////////////
//	Created		: 04.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media.Media3D;
using xray.editor.wpf_controls.converters;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for vector3_editor.xaml
	/// </summary>
	public partial class vector3_editor
	{
		#region | Initialize |


		public vector3_editor( )
		{
			InitializeComponent( );

			var binding = new Binding( "value" )
          	{
          		Converter			= new Vector3D_to_string_converter( vector => fill_sub_items( ) ),
          		Mode				= BindingMode.TwoWay,
          		UpdateSourceTrigger = UpdateSourceTrigger.Explicit
          	};
			m_text_box.SetBinding( TextBox.TextProperty, binding );

			DataContextChanged += delegate
          	{
          		if( DataContext == null )
          			return;

          		m_property					= (property)DataContext;
          		m_property.owner_editor		= this;

          		var values				= m_property.values;
          		var count				= values.Length;
          		m_vector_helpers		= new property_container[count];

          		for( var i = 0; i < count; ++i )
          		{
          			var obj = new vector3D_helper( (Vector3D)values[i], m_property, i );
          			var container = new property_container( );
          			var x_desc = container.properties.add_from_obj( obj, "x" );
          			var y_desc = container.properties.add_from_obj( obj, "y" );
          			var z_desc = container.properties.add_from_obj( obj, "z" );

          			var vec_attr = (vector3_attribute)m_property.descriptor.Attributes[ typeof( vector3_attribute ) ];
          			if( vec_attr != null )
          			{
          				x_desc.display_name = vec_attr.x_name;
          				y_desc.display_name = vec_attr.y_name;
          				z_desc.display_name = vec_attr.z_name;

          				x_desc.dynamic_attributes.add( new value_range_and_format_attribute( vec_attr.x_min, vec_attr.x_max, vec_attr.x_step_size, vec_attr.x_precision ) );
          				y_desc.dynamic_attributes.add( new value_range_and_format_attribute( vec_attr.y_min, vec_attr.y_max, vec_attr.y_step_size, vec_attr.y_precision ) );
          				z_desc.dynamic_attributes.add( new value_range_and_format_attribute( vec_attr.z_min, vec_attr.z_max, vec_attr.z_step_size, vec_attr.z_precision ) );
          			}

          			m_vector_helpers[i] = container;
          		}

          		if( ( (property)DataContext ).is_read_only )
          		{
          			m_text_box.IsEnabled		= false;
          		}

          		var desc = (DescriptionAttribute)m_property.descriptors[0].Attributes[ typeof( DescriptionAttribute ) ];
          		m_text_box.ToolTip = desc == null ? null : desc.Description;

          		if( item_editor.parent_container != null )
				{
          			item_editor.parent_container.expand_visibility		= Visibility.Visible;
					item_editor.parent_container.fill_sub_properties	= fill_sub_items;
				}
          	};
		}


		#endregion

		#region |   Fields   |


		private		property_container[]	m_vector_helpers;


		#endregion

		#region |  Methods   |


		private				void	fill_sub_items				( )											
		{
			m_property.sub_properties		= property_extractor.extract( m_vector_helpers, m_property, m_property.extract_settings );
		}
		private				void	handle_input				( Object sender, InputCommandEventArgs e )	
		{
			e.Handled = true;
		}
		private				void	text_box_preview_key_down	( Object sender, KeyEventArgs e )			
		{
			if( e.Key == Key.Escape )
			{
				m_text_box.GetBindingExpression( TextBox.TextProperty ).UpdateTarget( );
				m_text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Previous ) );
			}

			if( e.Key != Key.Return )
				return;

			m_text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
			item_editor.parent_container.update_hierarchy( );
			m_text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Previous ) );
		}
		private				void	text_box_lost_focus			( Object sender, RoutedEventArgs e )		
		{
			m_text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
			item_editor.parent_container.update_hierarchy( );
		}

		public override		void	update						( )											
		{
			m_text_box.GetBindingExpression( TextBox.TextProperty ).UpdateTarget( );
		}


		#endregion

		#region | InnerTypes |


		private class vector3D_helper
		{
			public	vector3D_helper		( Vector3D vector, property prop, Int32 index )		
			{
				this.prop	= prop;
				this.index	= index;
			}
			[BrowsableAttribute(false)]
			public	Vector3D	vector
			{
				get
				{
					return (Vector3D)prop.get_descriptor_value( prop.property_owners[index], prop.descriptors[index] );
				}
			}
			public	property	prop;
			public  Int32		index;

			public	Double	x		
			{
				get
				{
					return vector.X;
				}
				set
				{
					var vec		= vector;
					vec.X		= value;
					set_parent_vector	( vec );
				}
			}
			public	Double	y		
			{
				get
				{
					return vector.Y;
				}
				set
				{
					var vec		= vector;
					vec.Y		= value;
					set_parent_vector	( vec );
				}
			}
			public	Double	z		
			{
				get
				{
					return vector.Z;
				}
				set
				{
					var vec		= vector;
					vec.Z		= value;
					set_parent_vector	( vec );
				}
			}

			private void set_parent_vector( Vector3D vec )
			{
				prop.set_descriptor_value( prop.property_owners[index], prop.descriptors[index], vec );
				prop.invalidate_value( );
			}
		}


		#endregion

	}
}
