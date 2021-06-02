using System;
using System.Reflection;
using System.Windows.Markup;

namespace xray.editor.wpf_controls.markup_extensions
{
	public class static_propertyExtension: MarkupExtension
	{
		public static_propertyExtension( )
		{
		}

		public static_propertyExtension( Type type )
		{
			m_type = type;
		}

		Type			m_type;
		String			m_property_name;
		StaticPropertyValue m_value = new StaticPropertyValue( );

		public Type		property_type
		{
			get{ return m_type; }
			set{ m_type = value; }
		}
		public String	property_name
		{
			get{ return m_property_name; }
			set
			{
				m_property_name = value; 
				if( m_type != null )
					m_value.m_prop_info = m_type.GetProperty( m_property_name );
			}
		}

		public override object ProvideValue( IServiceProvider serviceProvider )
		{
			return m_value;
		}

		class StaticPropertyValue
		{
			public PropertyInfo	m_prop_info;

			public Object value
			{
				get
				{
					return m_prop_info.GetValue( null, null );
				}
				set
				{
					m_prop_info.SetValue( null, value, null );
				}
			}
		}
	}
}
