////////////////////////////////////////////////////////////////////////////
//	Created		: 13.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xray.editor.wpf_controls
{
	public class dictionary_item_property_value<TKey, TValue>: i_property_value
	{
		public dictionary_item_property_value( Dictionary<TKey, TValue> dictionary, TKey key )
		{
			m_key = key;
			m_dictionary = dictionary;
		}
		
		private	TKey						m_key;
		private Dictionary<TKey, TValue>	m_dictionary;

		public object get_value()
		{
			return  m_dictionary[m_key];
		}

		public void set_value(object value)
		{
			m_dictionary[m_key] = (TValue)value;
		}

		public Type value_type
		{
			get { return typeof( TValue ); }
		}
	}
}
