////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System.Windows;

namespace xray.editor.wpf_controls.property_editors.value
{
	partial class default_editor
	{
		public				default_editor			( )		
		{
			InitializeComponent( );
			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property						= (property)DataContext;
				m_property.owner_editor			= this;

				if( !m_property.is_multiple_values && m_property.value == null )
					return;

				m_property.sub_properties		= property_extractor.extract( m_property.values, m_property, m_property.extract_settings );
				if ( m_property.sub_properties.Count > 0 )
				{
					m_property.is_expandable_item						= true;

					item_editor.parent_container.expand_visibility		= Visibility.Visible;
					item_editor.parent_container.fill_sub_properties	= fill_sub_properties;

					if( m_property.descriptor is property_descriptor )
						item_editor.parent_container.is_expanded			= ((property_descriptor)m_property.descriptor).is_expanded;
				}
			};
		}

		private				void	fill_sub_properties		( )				
		{
			if( m_property.sub_properties == null || m_property.sub_properties.Count == 0 )
				m_property.sub_properties		= property_extractor.extract( m_property.values, m_property, m_property.extract_settings );
		}
	}
}
