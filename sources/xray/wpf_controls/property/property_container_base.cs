////////////////////////////////////////////////////////////////////////////
//	Created		: 11.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;

namespace xray.editor.wpf_controls
{
	public abstract class property_container_base : ICustomTypeDescriptor, i_property_container
	{

		#region |   Fields   |


		private			String		m_string_view;


		#endregion

		#region | Properties |


		public	property_collection_base	properties					
		{
			get;
			protected set;
		}
		public	Object						owner						
		{
			get;
			set;
		}
		public	String						string_view					
		{
			get
			{
				return m_string_view;
			}
			set
			{
				m_string_view = value;
			}
		}
		public	property_descriptor			inner_value					
		{
			get;
			set;
		}
		public	Boolean						is_return_inner_property	
		{
			get;
			set;
		}


		#endregion

		#region |   Methods  |


		AttributeCollection				ICustomTypeDescriptor.GetAttributes		( )							
		{
			return TypeDescriptor.GetAttributes(this, true);
		}
		String							ICustomTypeDescriptor.GetClassName		( )							
		{
			return TypeDescriptor.GetClassName(this, true);
		}
		String							ICustomTypeDescriptor.GetComponentName	( )							
		{
			return TypeDescriptor.GetComponentName(this, true);
		}
		TypeConverter					ICustomTypeDescriptor.GetConverter		( )							
		{
			return TypeDescriptor.GetConverter(this, true);
		}
		EventDescriptor					ICustomTypeDescriptor.GetDefaultEvent	( )							
		{
			return TypeDescriptor.GetDefaultEvent(this, true);
		}
		PropertyDescriptor				ICustomTypeDescriptor.GetDefaultProperty( )							
		{
			return null;
		}
		Object							ICustomTypeDescriptor.GetEditor			( Type editor_base_type )	
		{
			return TypeDescriptor.GetEditor(this, editor_base_type, true);
		}
		EventDescriptorCollection		ICustomTypeDescriptor.GetEvents			( )							
		{
			return TypeDescriptor.GetEvents(this, true);
		}
		EventDescriptorCollection		ICustomTypeDescriptor.GetEvents			( Attribute[] attributes )	
		{
			return TypeDescriptor.GetEvents(this, attributes, true);
		}
		PropertyDescriptorCollection	ICustomTypeDescriptor.GetProperties		( )							
		{
			return ((ICustomTypeDescriptor)this).GetProperties(new Attribute[0]);
		}
		PropertyDescriptorCollection	ICustomTypeDescriptor.GetProperties		( Attribute[] attributes )	
		{
			var descriptors = new PropertyDescriptor[properties.count];

			var i = 0;
			foreach (var descriptor in properties)
			{
				descriptors[i] = descriptor;
				++i;
			}

			return new PropertyDescriptorCollection(descriptors);
		}
		Object							ICustomTypeDescriptor.GetPropertyOwner	( PropertyDescriptor pd )	
		{
			return this;
		}

		public override		String		ToString								( )							
		{
			return m_string_view;
		}
		public				void		update_properties						( )							
		{
			if( updated != null )
				updated( );
		}
		public				void		refresh_properties						( )							
		{
			if( refreshed != null )
				refreshed( );
		}

		IEnumerable<property_descriptor> i_property_container.get_properties	( )							
		{
			if (is_return_inner_property)
				return (inner_value == null) ? new property_descriptor[0] : new[] { inner_value };
			return properties;
		}

		public				IEnumerator<property_descriptor> GetEnumerator		( )							
		{
			return properties.GetEnumerator( );
		}
		IEnumerator			IEnumerable.GetEnumerator							( )							
		{
			return GetEnumerator();
		}



		#endregion

		#region |   Events   |


		public event			Action				updated;
		public event			Action				refreshed;


		#endregion

		#region | InnerTypes |


		public abstract class property_collection_base : IEnumerable<property_descriptor>
		{

			#region | Initialize |


			internal	property_collection_base	( property_container_base owner )	
			{
				m_owner = owner;
			}


			#endregion

			#region |   Fields   |


			protected readonly		property_container_base		m_owner;


			#endregion

			#region | Properties |


			public abstract			Int32		count		
			{
				get;
			}


			#endregion

			#region |  Methods   |


			public				List<property_descriptor>	add_from_obj		( Object obj )											
			{
				var list = property_descriptor.from_obj( obj );

				foreach (var desc in list)
					add( desc );	

				return list;
			}
			public				property_descriptor			add_from_obj		( Object obj, String property_name )					
			{
				return add( property_descriptor.from_obj( obj,property_name ) );
			}
			public				property_descriptor			add_from_obj		( Object obj, PropertyInfo property_info )				
			{
				return add( property_descriptor.from_obj( obj, property_info ) );
			}
			public				property_descriptor			add					( i_property_info_value value )							
			{
				return add( new property_descriptor( value ) );
			}
			public				property_descriptor			add					( PropertyInfo prop_info, i_property_value value )		
			{
				return add( property_descriptor.from_value( prop_info, value ) );
			}
			public				property_descriptor			add					( String display_name, i_property_value value )			
			{
				return add( new property_descriptor(display_name, value.value_type, value) );
			}
			public				property_descriptor			add					( String display_name, String category, i_property_value value )											
			{
				return add( new property_descriptor(display_name, value.value_type, value) { category = category } );
			}
			public				property_descriptor			add					( String display_name, String category, String description, Object default_value, i_property_value value )	
			{
				return add( 
					new property_descriptor( display_name, value.value_type, value )
						{
							category		= category,
							display_name	= display_name,
							description		= description,
							default_value	= default_value
						}
					);
			}
			public				property_descriptor			add_container		( String display_name, String category, String description, i_property_container container )				
			{
				return add(
					( container is property_container )
					? new property_descriptor( display_name, new object_property_value<property_container>( (property_container)container ) )
						{
							category		= category,
							display_name	= display_name,
							description		= description
						}
					: new property_descriptor( display_name, new object_property_value<i_property_container>( container ) )
						{
							category		= category,
							display_name	= display_name,
							description		= description
						}
					);
			}
			public				List<property_descriptor>	add_from_container	( IEnumerable<property_descriptor> container )			
			{
				return container.Select( desc => add( desc ) ).ToList( );
			}

			public abstract		property_descriptor			add					( property_descriptor desc );
			public abstract		void						remove				( property_descriptor desc );
			public abstract		void						clear				( );
			public abstract		Boolean						contains			( property_descriptor desc );

			public abstract IEnumerator<property_descriptor> GetEnumerator		( );
			IEnumerator			IEnumerable.GetEnumerator						( )														
			{
				return GetEnumerator();
			}


			#endregion

		}


		#endregion

	}
}
