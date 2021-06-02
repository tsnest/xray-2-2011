////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls
{
	public class callback_property_value<TType> : i_property_value
	{
		public callback_property_value( Func<TType>	get_value_callback, Action<TType>	set_value_callback )	
		{
			if( get_value_callback == null )
				throw new ArgumentNullException("get_value_callback");
			if( set_value_callback == null )
				throw new ArgumentNullException("set_value_callback");

			m_get_value_callback = get_value_callback;
			m_set_value_callback = set_value_callback;
		}

		private readonly	Func<TType>		m_get_value_callback;
		private readonly	Action<TType>	m_set_value_callback;

		public	Type		value_type	
		{
			get { return typeof( TType ); }
		}

		public	object		get_value		( )					
		{
			return m_get_value_callback();
		}
		public	void		set_value		( Object value )	
		{
			m_set_value_callback( (TType)value );
		}

	}
}
