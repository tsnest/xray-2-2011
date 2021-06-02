////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors
{
	public class property: INotifyPropertyChanged
	{

		#region |   Events   |


		public event PropertyChangedEventHandler PropertyChanged;


		#endregion

		#region | Initialize |


		public property( )
		{
			m_index				= s_count++;
			descriptors			= new List<PropertyDescriptor>( );
			property_owners		= new List<object>( );
			default_values		= new List<object>( );
			is_valid			= true;
			is_read_only		= false;
		}


		#endregion

		#region |   Fields   |


		private static Int32		s_count;

		private readonly Int32		m_index;
		private Boolean				m_is_collection_item;
		private Boolean				m_is_expandable_item;
		private Boolean				m_is_multiple_values;
		private Type				m_type;
		
		private property			m_property_parent;
		private	List<property>		m_inner_properties;
		private type_converter_base	m_converter;
	

		#endregion

		#region | Properties |


		public			String						name				
		{
			get; internal set; 
		}
		public			Type						type				
		{
			get 
			{ 
				return m_type; 
			} 
			set
			{
				m_type = value;
			}
		}

		public			List<PropertyDescriptor>	descriptors			
		{
			get; protected set; 
		}
		public			PropertyDescriptor			descriptor			
		{
			get
			{
				return descriptors[0];
			}
		}
		public			Boolean						is_read_only		
		{
			get; set; 
		}
		public			Boolean						is_valid			
		{
			get; internal	set; 
		}
		public			List<property>				sub_properties		
		{
			get; internal set; 
		}
		public			List<property>				inner_properties	
		{ 
			get
			{
				if( m_inner_properties == null && descriptors[0] is property_descriptor )
					m_inner_properties = property_extractor.extract( descriptors.ToArray(), this, extract_settings );
				return m_inner_properties;
			}
		}
		public			type_converter_base			converter			
		{
			get
			{
				return m_converter;
			}
			set
			{
				m_converter = value;
			}
		}


		private property_editor_base m_owner_editor;

		public			property_editor_base		owner_editor		
		{
			get { return m_owner_editor; }
			internal set { m_owner_editor = value; }
		}

		public virtual	Boolean						is_selected			
		{
			get;set;
		}
		public			Boolean						is_collection		
		{
			get; internal	set; 
		}

		public			List<Object>				default_values		
		{
			get; protected set; 
		}
		public			List<Object>				property_owners		
		{
			get; protected	set; 
		}

		public			property					property_parent		
		{
						get		
			{
				return m_property_parent;
			}
			internal	set		
			{
				m_property_parent	= value;
				if( value != null )
					is_collection_item	= value.is_collection; 
			}
 
		}
		public			Boolean						is_multiple_values	
		{
			get { return m_is_multiple_values; }
			internal set { m_is_multiple_values = value; on_property_changed("is_multiple_values"); }
		}
		public			Boolean						is_expandable_item	{
			get { return m_is_expandable_item; }
			internal set { m_is_expandable_item = value; on_property_changed("is_expandable_item"); } 
		}
		public			Boolean						is_expanded			{
			get {
					if (descriptor is property_descriptor)
						return ((property_descriptor)descriptor).is_expanded; 
					return false;
				}
			internal set {
					if (descriptor is property_descriptor)
							((property_descriptor)descriptor).is_expanded = value; 
					on_property_changed("is_expanded"); 
			} 
		}
		public			Boolean						is_collection_item	{
			get { return m_is_collection_item; }
			internal set { m_is_collection_item = value; on_property_changed("is_collection_item"); } 
		}
		public			Boolean						is_default_value	
		{
			get				
			{
				var vls		= values;
				var count	= vls.Length;
				for (var i = 0; i < count; i++)
				{
                    if (vls[i] == null)
                    {
                        if (default_values[i] != null)
                            return false;
                    }
					else if( !vls[i].Equals(default_values[i]) )
						return false;
				}
				return true;
			}
			internal	set	
			{
				on_property_changed("is_default_value");
			}
		}
		public virtual	Object						value				
		{
			get	
			{
				return ( is_multiple_values ) ? null : (!is_valid )? null : get_descriptor_value( property_owners[0], descriptors[0] );
			}
			set	
			{
				for ( var i = 0; i < property_owners.Count; ++i )
					set_descriptor_value( property_owners[i], descriptors[i], value );

				on_property_changed("value");
				on_property_changed("is_default_value");
			}
		}
		public			Object[]					values				
		{
			get
			{
				var ret_values = new Object[property_owners.Count];
				for (var i = 0; i < property_owners.Count; ++i)
					ret_values[i] = get_descriptor_value( property_owners[i], descriptors[i] );
				return ret_values;
			}
		}

		public			property_extractor.settings	extract_settings	
		{
			get;set;
		}


		#endregion

		#region |  Methods   |


		public override		String		ToString				( )												
		{
			return name + " = " + ( ( value != null ) ? value.ToString( ) : "null" );
		}
		public				void		destroy					( )												
		{
			destroy( true );
		}
		public				void		destroy					( Boolean is_hierarchical )						
		{
			if( is_hierarchical )
			{
				if( sub_properties != null )
					foreach( var property in sub_properties )
						property.destroy( );

				if( inner_properties != null )
					foreach( var property in inner_properties )
						property.destroy( );
			}

			descriptors			= null;
			owner_editor		= null;
			property_owners		= null;
			default_values		= null;
			m_inner_properties	= null;
			sub_properties		= null;
			property_parent		= null;
			is_valid			= false;

		}

		internal static		Object		get_property_value		( PropertyDescriptor descriptor, Object obj )	
		{
			try
			{
				return descriptor.GetValue(obj);
			}
			catch (System.Reflection.TargetInvocationException)
			{
				if (descriptor.PropertyType.IsValueType)
					return Activator.CreateInstance(descriptor.PropertyType);
				return null;
			}
		}
		internal static		Object		create_property_value	( Type type )									
		{
			try
			{
				if( type == typeof( String ) )
					return "";
				return Activator.CreateInstance( type );
			}
			catch ( System.Reflection.TargetInvocationException )
			{
				return null;
			}
			catch ( MissingMethodException )
			{
				return null;
			}
		}

		internal			void		invalidate_value		( )												
		{
			on_property_changed("value");
			on_property_changed("is_default_value");
		}
		internal			void		update_owner_editor		( )												
		{
			owner_editor.update( );
		}
		internal			void		set_descriptor_value	( Object owner, PropertyDescriptor descriptor, Object value )
		{
			if( m_converter != null && descriptor.PropertyType != m_type )
				descriptor.SetValue( owner, m_converter.convert_back( value ) );
			else
				descriptor.SetValue( owner, value.cast_to( descriptor.PropertyType ) );
		}
		internal			Object		get_descriptor_value	( Object owner, PropertyDescriptor descriptor )
		{
			if( m_converter != null && descriptor.PropertyType != m_type )
				return m_converter.convert( descriptor.GetValue( owner ) );
			
			return descriptor.GetValue( owner );
		}

		protected			void		on_property_changed		( String property_name )						
		{ 
			if(PropertyChanged != null)
				PropertyChanged(this, new PropertyChangedEventArgs(property_name));
		}	
			

		#endregion

	}
}
