////////////////////////////////////////////////////////////////////////////
//	Created		: 13.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls
{
	public class object_property_value<TType>: i_property_value
	{
		public object_property_value( TType obj )
		{
			m_obj = obj;
		}

		private TType m_obj;

		public object get_value()
		{
			return  m_obj;
		}

		public void set_value(object value)
		{
			m_obj = (TType)value;
		}

		public Type value_type
		{
			get { return typeof( TType ); }
		}
	}
}
