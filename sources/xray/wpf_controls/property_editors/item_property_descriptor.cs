////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections;

namespace xray.editor.wpf_controls.property_editors
{
	public class item_property_descriptor: property_descriptor
	{
		public item_property_descriptor( String item_name, Int32 item_index, IList parent_collection )
			:base( item_name )
		{
			m_list = parent_collection;
			if( parent_collection[item_index] is property_descriptor )
			{
				is_property_descriptor	= true;
				var desc				= (property_descriptor)parent_collection[item_index];
				m_type					= desc.PropertyType;
				is_expanded				= desc.is_expanded;
				m_attributes			= desc.dynamic_attributes;
				m_inner_properties		= desc.inner_properties;
				tag						= desc.tag;
				owner_property			= desc.owner_property;
			}
			else
				m_type = parent_collection[item_index].GetType( );

			m_item_index = item_index;
		}

		private IList	m_list;
		private Boolean is_property_descriptor;
		private	Int32	m_item_index;
		public	Int32	item_index					{ get { return m_item_index;} }
		
		internal		void	decrease_item_index	( )								
		{
			--m_item_index;
		}
		internal		void	increase_item_index	( )
		{
			++m_item_index;
		}
	
		public Boolean	expanded
		{
			get
			{
				return is_expanded;
			}
			set
			{
				is_expanded = value;
				if ( is_property_descriptor )
				{
					var desc = (property_descriptor)m_list[item_index];
					desc.is_expanded = value;
				}
			}
		}

		public override object	GetValue			( Object component )				
		{
			return ( is_property_descriptor )
				? ((property_descriptor)((IList)component)[m_item_index]).value
				: ((IList)component)[m_item_index];
		}
		public override void	SetValue			( Object component, Object set_value )
		{
			if( is_property_descriptor )
				((property_descriptor)((IList)component)[m_item_index]).value = set_value;
			else
				((IList)component)[m_item_index] = set_value;
		}
	}
}
