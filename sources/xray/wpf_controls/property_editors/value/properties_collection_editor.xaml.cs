////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Linq;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for properties_collection_editor.xaml
	/// </summary>
	public partial class properties_collection_editor
	{
		public	properties_collection_editor	( )		
		{
			InitializeComponent( );

			DataContextChanged		+= delegate
			{
				if( DataContext == null )
					return;

				m_property = ((property)DataContext);
				m_property.owner_editor				= this;
				m_property.is_expandable_item		= true;
				m_property.is_collection			= true;

				var attribute			= (collection_attribute)m_property.descriptors[0].Attributes[typeof(collection_attribute)];
				m_add_property_func		= attribute.add_func;
				m_remove_property_func	= attribute.remove_func;
				m_move_property_func	= attribute.move_func;

				if( item_editor.parent_container != null )
				{
					item_editor.parent_container.expand_visibility		= Visibility.Visible;
					item_editor.parent_container.fill_sub_properties	= fill_sub_properties;
					item_editor.parent_container.decorate_container		= decorate_container;

					if( m_property.value is property_container_collection )
					{
						var properties_collection				= ( property_container_collection )m_property.value;
						properties_collection.property_added	+= indexed_property_added;
						properties_collection.property_removed	+= indexed_property_removed;
						properties_collection.updated			+= collection_updated;
						properties_collection.refreshed			+= collection_refreshed;
					}
					else if( m_property.value is property_container )
					{
						var properties_collection				= ( property_container )m_property.value;
						properties_collection.property_added	+= property_added;
						properties_collection.property_removed	+= property_removed;
						properties_collection.updated			+= collection_updated;
						properties_collection.refreshed			+= collection_refreshed;
					}
				}

				item_editor.parent_container.is_expanded = true;
			};
		}

		private Func<Boolean>					m_add_property_func;
		private Func<Object, Boolean>			m_remove_property_func;
		private Func<Int32, String, Boolean>	m_move_property_func;

		private				void	collection_refreshed	( )										
		{
			item_editor.parent_container.reset_sub_properties( );
		}
		private				void	collection_updated		( )										
		{
			item_editor.parent_container.update_hierarchy( );
		}
		private				void	indexed_property_removed( Int32 index )							
		{
			if( item_editor.parent_container != null )
			{
				item_editor.parent_container.remove_sub_container	( index );
				m_property.sub_properties.RemoveAt					( index );
			}
			else
				item_editor.parent_container.reset_sub_properties( );
		}
		private				void	indexed_property_added	( Int32 index )							
		{
			if( item_editor.parent_container != null )
			{
				var property = property_extractor.extract_from_collection	( index, m_property.values, m_property, m_property.extract_settings );
				item_editor.parent_container.insert_container_for_property	( index, property );
				m_property.sub_properties.Insert							( index, property );
			}
			else
				item_editor.parent_container.reset_sub_properties( );
		}
		private				void	property_removed		( String name )							
		{
			for( var i = 0; i < item_editor.parent_container.Items.Count; i++ )
			{
				if( ( ( property_items_control )item_editor.parent_container.Items[i] ).m_property.name == name )
				{
					item_editor.parent_container.remove_sub_container	( i );
					m_property.sub_properties.RemoveAt					( i );
					return;
				}
			}
		}
		private				void	property_added			( String name )							
		{
			var collection		= property_extractor.extract<List<property>>	( m_property.values, m_property, m_property.extract_settings );
			var property		= collection.First( prop => prop.name == name );
			var insert_index	= m_property.sub_properties.Count;
			item_editor.parent_container.insert_container_for_property		( insert_index, property );
			m_property.sub_properties.Insert								( insert_index, property );
		}

		private				void	add_click				( Object sender, RoutedEventArgs e )	
		{
			m_add_property_func( );
		}
		private				void	fill_sub_properties		( )										
		{
			if( m_property.sub_properties == null || m_property.sub_properties.Count == 0 )
			{
				m_property.sub_properties		= property_extractor.extract( m_property.values, m_property, m_property.extract_settings );
			}
		}
		private				void	decorate_container		( expandable_items_control container )	
		{
			var button		= new Button {
  		  		Width		= 11,
  		  		Height		= 11,
  		  		HorizontalContentAlignment	= HorizontalAlignment.Center,
  		  		VerticalContentAlignment	= VerticalAlignment.Center,
				Margin		= new Thickness( 0, 0, 0, 2 ),
  		  		Content		= new Path {
					Data				= (Geometry)FindResource( "remove_button_path_geometry" ),
					Stroke				= new SolidColorBrush( Colors.Black ),
					SnapsToDevicePixels	= false
				}
  		  	};

			button.Click	+= remove_item_click;
			container.add_right_pocket_inner( button );
		}
		private				void	remove_item_click		( Object sender, RoutedEventArgs e )	
		{
			remove_sub_property( (property)( (Button)sender ).DataContext );
		}

		internal override	void	remove_sub_property		( property sub_property )				
		{
			if( sub_property.descriptors[0] is item_property_descriptor )
			{
				var properties = sub_property.property_parent.sub_properties;
				var index = properties.IndexOf(sub_property);
				m_remove_property_func( index );

				var count = m_property.sub_properties.Count;
				for( var i = index; i < count; ++i )
				{
					foreach( var descriptor in m_property.sub_properties[i].descriptors )
						( (item_property_descriptor)descriptor ).decrease_item_index( );	
				}
// TODO fix this case !!!
				if (item_editor.parent_container == null)
					item_editor.parent_container.reset_sub_properties( );

				return;
			}
			if ( m_remove_property_func( sub_property.name ) )
				item_editor.parent_container.reset_sub_properties( );
		}
		internal override	void	move_up_sub_property	( property sub_property )				
		{
			if( sub_property.descriptors[0] is item_property_descriptor )
			{
				var properties = sub_property.property_parent.sub_properties;
				Int32 index = properties.IndexOf(sub_property);
				if (m_move_property_func(index, "up"))
					item_editor.parent_container.reset_sub_properties( );
			}
		}
		internal override	void	move_down_sub_property	( property sub_property )				
		{
			if( sub_property.descriptors[0] is item_property_descriptor )
			{
				var properties		= sub_property.property_parent.sub_properties;
				var index			= properties.IndexOf( sub_property );

				if ( m_move_property_func( index, "down" ) )
					item_editor.parent_container.reset_sub_properties( );
			}
		}
	}
}
