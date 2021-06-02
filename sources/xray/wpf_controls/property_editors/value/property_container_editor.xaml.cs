////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Windows;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for property_container_value_editor.xaml
	/// </summary>
	public partial class property_container_editor
	{
		public			property_container_editor				( )							
		{
			InitializeComponent( );

			DataContextChanged		+= delegate
			{
				if( DataContext == null )
					return;

				m_property = (property)DataContext;

				var desc = (DescriptionAttribute)m_property.descriptor.Attributes[ typeof( DescriptionAttribute ) ];
				m_value_editor_place.ToolTip = desc == null ? null : desc.Description;

				var values = m_property.values;

				foreach( var container in values.Cast<property_container_base>( ).Where( container => container != null ) )
					container.is_return_inner_property = true;

				var inner_prop = new List<property>( );
				property_extractor.extract_from_class( inner_prop, m_property.values, m_property, m_property.extract_settings );

				foreach( var container in values.Cast<property_container_base>( ).Where( container => container != null ) )
					container.is_return_inner_property = false;

				m_inner_property				= inner_prop.Count > 0 ? inner_prop[0] : null;

				if ( m_inner_property == null )
					m_value_editor_place.Content = "";
				else
				{
					m_inner_value_editor				= item_editor.value_editor_selector.select_editor( m_inner_property );
					m_inner_value_editor.item_editor	= item_editor;
					m_inner_value_editor.DataContext	= null;
					m_value_editor_place.Content		= m_inner_value_editor;
					m_inner_value_editor.DataContext	= m_inner_property;
				}

				if( !m_property.is_multiple_values && m_property.value == null )
				{
					if( m_inner_property == null )
						m_value_editor_place.Content = "<not set>";
				}
				else
				{
					item_editor.parent_container.fill_sub_properties	= fill_sub_properties;
					item_editor.parent_container.is_expanded			= m_property.is_expanded;
					item_editor.parent_container.expand_visibility		= Visibility.Visible;

					if( m_property.is_multiple_values )
					{
						if( m_inner_property == null )
							m_value_editor_place.Content = "<many>";
					}
					else
					{
                                               
						var properties_collection				= m_property.value as property_container;
						
						if( properties_collection == null )
							return;

						properties_collection.property_added	+= property_added;
						properties_collection.property_removed	+= property_removed;
						properties_collection.updated			+= collection_updated;
						properties_collection.refreshed			+= collection_refreshed;
					}
				}
			};
		}

		private			value_editor_base	m_inner_value_editor;
		private			property			m_inner_property;

		public			property	inner_property				
		{
			get
			{
				return m_inner_property;
			}
		}

		private			void		collection_refreshed		( )							
		{
			item_editor.parent_container.reset_sub_properties( );
		}
		private			void		collection_updated			( )							
		{
			item_editor.parent_container.update_hierarchy( );
		}
		private			void		property_removed			( String property_name )	
		{
			item_editor.parent_container.reset_sub_properties( );
		}
		private			void		property_added				( String property_name )	
		{
			item_editor.parent_container.reset_sub_properties( );
		}
		private			void		fill_sub_properties			( )							
		{
			if( m_property.sub_properties == null || m_property.sub_properties.Count == 0 )
			{
				m_property.sub_properties		= property_extractor.extract( m_property.values, m_property, m_property.extract_settings );
			}
		}

		public override void		update						( )							
		{
			if( m_inner_value_editor != null )
				m_inner_value_editor.update( );
		}
	}
}
