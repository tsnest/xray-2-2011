////////////////////////////////////////////////////////////////////////////
//	Created		: 06.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Windows.Controls;
using xray.editor.wpf_controls.property_editors;

namespace xray.editor.wpf_controls.hypergraph
{
	public class properties_control: StackPanel
	{
		public		properties_control		( )		
		{
			m_extract_settings	= new property_extractor.settings
			{
				property_type				= typeof(property),
				initialize_prop_delegate	= initialize_property
			};
		}

		private				Object								m_source_object;
		private readonly	Dictionary<String, property_view>	m_property_views		= new Dictionary<String, property_view>( );
		private				List<property>						m_properties			= new List<property>( );
		private	readonly	property_extractor.settings			m_extract_settings;

		internal			Dictionary<String, property_view>	property_views		
		{
			get
			{
				return m_property_views;
			}
		}
		public				Object								source_object		
		{
			get
			{
				return m_source_object;
			}
			set
			{
				disconnect_container_events	( );
				m_source_object				= value;
				connect_container_events	( );

				fill_property_views_from_list( 
					property_extractor.extract<List<property>>(
						value,
						null,
						m_extract_settings
					) 
				);
			}
		}

		private		void				initialize_property				( property_editors.property p )				
		{
			var prop			= (property)p;
			var attr			= (node_property_attribute)prop.descriptor.Attributes[ typeof(node_property_attribute) ];

			prop.node			= node.get_parent_node( this );
			prop.type_id		= prop.type.Name;

			var parent_prop = prop.property_parent;
			while( parent_prop != null )
			{
				var parent_attr	= (node_sub_properties_attribute)parent_prop.descriptor.Attributes[ typeof(node_sub_properties_attribute) ];
				if( parent_attr != null )
				{
					if( !String.IsNullOrEmpty( parent_attr.type_id ) )
						prop.type_id =  parent_attr.type_id;

					prop.output_enabled = parent_attr.output_link_enabled;
					prop.input_enabled	= parent_attr.input_link_enabled;
					break;
				}

				parent_prop = parent_prop.property_parent;
			}

			if( attr != null )
			{
				if( !String.IsNullOrEmpty( attr.type_id ) )
					prop.type_id		=  attr.type_id;

				prop.output_enabled = attr.output_link_enabled;
				prop.input_enabled	= attr.input_link_enabled;
			}
		}

		private		void				fill_property_views_from_list	( List<property> list )						
		{
			Children.Clear( );
			m_property_views.Clear	( );
			m_properties.Clear		( );
			m_properties			= list;

			foreach( var prop in list )
				add_property( prop );
		}
		private		void				connect_container_events		( )											
		{
			if( source_object != null && source_object is property_container )
			{
				var container = (property_container)source_object;
				container.property_added	+= property_added;
				container.property_removed	+= property_removed;
				container.updated			+= collection_updated;
				container.refreshed			+= collection_refreshed;
			}
		}
		private		void				disconnect_container_events		( )											
		{
			if( source_object != null && source_object is property_container )
			{
				var container = (property_container)source_object;
				container.property_added	-= property_added;
				container.property_removed	-= property_removed;
				container.updated			-= collection_refreshed;
				container.refreshed			-= collection_refreshed;
			}
		}

		private		void				collection_refreshed			( )											
		{
			fill_property_views_from_list( 
				property_extractor.extract<List<property>>(
					m_source_object,
					null,
					m_extract_settings
				) 
			);
		}
		private		void				collection_updated				( )											
		{
			update_properties( );
		}
		private		void				property_removed				( String property_name )					
		{
			fill_property_views_from_list( 
				property_extractor.extract<List<property>>(
					m_source_object,
					null,
					m_extract_settings
				) 
			);
		}
		private		void				property_added					( String property_name )					
		{
			fill_property_views_from_list( 
				property_extractor.extract<List<property>>(
					m_source_object,
					null,
					m_extract_settings
				) 
			);
		}

		private		void				add_property					( property prop )							
		{
			var hypergraph = node.get_parent_node( this ).hypergraph;

			var property_view					= new property_view( prop, hypergraph.item_editor_selector, hypergraph.value_editor_selector );
			Children.Add						( property_view );
			m_property_views.Add				( prop.name, property_view );
		}
		public		void				update_properties				( )											
		{
			foreach( var property_view in m_property_views.Values )
				property_view.update_hierarchy( );
		}
	}
}
