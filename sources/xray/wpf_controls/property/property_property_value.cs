////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Reflection;

namespace xray.editor.wpf_controls
{
	public class property_property_value : i_property_value
	{

		public property_property_value		( Object obj, String property_name )	
		{
			m_obj		= obj;
			m_property	= obj.GetType().GetProperty( property_name );
		}
		public property_property_value		( Object obj, PropertyInfo property )	
		{
			m_obj		= obj;
			m_property	= property;
		}

		private readonly	Object			m_obj;
		private readonly	PropertyInfo	m_property;

		public		Type				value_type		
		{
			get { return m_property.PropertyType; }
		}
		public		PropertyInfo		property_info		
		{
			get { return m_property; }
		}

		public		Object		get_value	( )					
		{
			return m_property.GetValue( m_obj, null );
		}
		public		void		set_value	( Object value )	
		{
			m_property.SetValue( m_obj, value, null );
		}

	}
}
