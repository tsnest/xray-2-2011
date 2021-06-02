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

using System.Runtime.Serialization;
using System.Reflection;
using xray.editor.wpf_controls.control_containers;
using xray.editor.wpf_controls.hypergraph;
using xray.editor.wpf_controls.property_editors.attributes;
using property = xray.editor.wpf_controls.property_editors.property;

namespace xray.editor.wpf_controls
{
	public class property_descriptor: PropertyDescriptor
	{

		#region | Initialize |


		public		property_descriptor			( String property_name ):base( property_name, new Attribute[0] )
		{
			m_type				= typeof( Object );
			m_property_value	= null;
		}
		public		property_descriptor			( property_descriptor other_descriptor ):base( other_descriptor.Name, new Attribute[0] )
		{
			m_type				= other_descriptor.m_type;
			m_property_value	= other_descriptor.m_property_value;
			m_attributes		= new attribute_collection( other_descriptor.m_attributes );
			m_inner_properties	= other_descriptor.inner_properties;
		}
		public		property_descriptor			( String property_name, Type property_type, i_property_value value ):base( property_name, new Attribute[0] )
		{
			m_type				= property_type;
			m_property_value	= value;
		}
		public		property_descriptor			( String property_name, i_property_value value ):base( property_name, new Attribute[0] )
		{
			m_type				= value.value_type;
			m_property_value	= value;
		}
		public		property_descriptor			( i_property_info_value value ):base( value.property_info.Name, new Attribute[0] )
		{
			m_type			= value.value_type;
			m_property_value	= value;
		}
		public		property_descriptor			( String display_name, String category, String description, Object default_value, i_property_value value ):base( display_name, new Attribute[0] )
		{	
			m_type				= value.value_type;
			m_property_value	= value;

			this.category		= category;
			this.display_name	= display_name;
			this.description	= description;
			this.default_value	= default_value;
		}
		public		property_descriptor			( String display_name, String category, String description,  i_property_container container ):base( display_name, new Attribute[0] )
		{	
			var value			= new object_property_value<i_property_container>(container);
			m_type				= value.value_type;
			m_property_value	= value;

			this.category		= category;
			this.display_name	= display_name;
			this.description	= description;
		}
		public		property_descriptor			( String display_name, String category, String description,  property_container container ):base( display_name, new Attribute[0] )
		{	
			var value			= new object_property_value<property_container>(container);
			m_type				= value.value_type;
			m_property_value	= value;

			this.category		= category;
			this.display_name	= display_name;
			this.description	= description;
		}
		public		property_descriptor			( Object obj, String property_name ):base( property_name, new Attribute[0] )				
		{
			var prop_info = obj.GetType().GetProperty(property_name, BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Public);
			var value = new property_property_value(obj, prop_info);
			
			var dattrs	= (DisplayNameAttribute[])prop_info.GetCustomAttributes(typeof(DisplayNameAttribute), true);
			if ( dattrs.Length > 0 )
				display_name	= dattrs[0].DisplayName;

			m_type				= value.value_type;
			m_property_value	= value;
			default_value		= value.get_value( );

			var attrs	= (IEnumerable)prop_info.GetCustomAttributes(true);
			dynamic_attributes.add_range(attrs);
		}
		public		property_descriptor			( Object obj, String display_name, String property_name ):base( property_name, new Attribute[0] )				
		{
			var prop_info = obj.GetType().GetProperty(property_name, BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Public);
			var value = new property_property_value(obj, prop_info);
			
			this.display_name	= display_name;

			m_type				= value.value_type;
			m_property_value	= value;
			default_value		= value.get_value( );

			var attrs	= (IEnumerable)prop_info.GetCustomAttributes(true);
			dynamic_attributes.add_range(attrs);
		}
		public		property_descriptor			( String display_name, Object obj, String property_name ):base( display_name, new Attribute[0] )				
		{
			var prop_info = obj.GetType().GetProperty(property_name, BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Public);
			var value = new property_property_value(obj, prop_info);
			
			this.display_name	= display_name;
			m_type				= value.value_type;
			m_property_value	= value;
			default_value		= value.get_value( );

			var attrs	= (IEnumerable)prop_info.GetCustomAttributes( true );
			dynamic_attributes.add_range( attrs );
		}
		public		property_descriptor			( String display_name, String category, String description, Object default_value,  i_property_container container ):base( display_name, new Attribute[0] )
		{	
			var value			= new object_property_value<i_property_container>(container);
			m_type				= value.value_type;
			m_property_value	= value;

			this.category		= category;
			this.display_name	= display_name;
			this.description	= description;
			this.default_value	= default_value;
		}


		#endregion

		#region |   Fields   |


		protected			attribute_collection	m_attributes = new attribute_collection();
        protected			Type					m_type;
		private				i_property_value		m_property_value;
		protected			Dictionary<String, property_descriptor>		m_inner_properties = new Dictionary<String, property_descriptor>();


		#endregion

		#region | Properties |


		public				property_container_base	owner				
		{
			get;set;
		}
		public				attribute_collection	dynamic_attributes	
		{
			get{return m_attributes;}
		}
		public override		AttributeCollection		Attributes			
		{
			get
			{
				if( m_attributes.need_recreate )
					AttributeArray = m_attributes.to_array();

				return base.Attributes;
			}
		}
		public				Object					tag					
		{
			get;set;
		}
		public				Dictionary<String, property_descriptor>		inner_properties	
		{
			get
			{
				return m_inner_properties;
			}
		}
		public				Boolean					is_expanded			
		{
			get;set;
		}
		public				property				owner_property		
		{
			get;set;
		}

		public override		Type					PropertyType		
		{
			get { return m_type; }
		}
		public override		Type					ComponentType		
		{
			get { return null; }
		}

		public override		Boolean					IsReadOnly			
		{
			get
			{
				var attr = (ReadOnlyAttribute)dynamic_attributes[ typeof( ReadOnlyAttribute ) ];
				return attr != null && attr.IsReadOnly;
			}
		}
		public override		String					DisplayName			
		{
			get
			{
				var attr = (DisplayNameAttribute)dynamic_attributes[ typeof( DisplayNameAttribute ) ];
				return attr != null ? attr.DisplayName : Name;
			}
		}
		public override		String					Category			
		{
			get
			{
				var attr = (CategoryAttribute)dynamic_attributes[ typeof( CategoryAttribute ) ];
				return attr != null ? attr.Category : "";
			}
		}
		public override		String					Description			
		{
			get
			{
				var attr = (DescriptionAttribute)dynamic_attributes[ typeof( DescriptionAttribute ) ];
				return attr != null ? attr.Description : "";
			}
		}
		public override		Boolean					IsBrowsable			
		{
			get
			{
				var attr = (BrowsableAttribute)dynamic_attributes[ typeof( BrowsableAttribute ) ];
				return attr == null || attr.Browsable;
			}
		}

		public				Boolean					is_read_only		
		{
			get
			{
				return IsReadOnly;
			}
			set
			{
				var attr = (ReadOnlyAttribute)dynamic_attributes[ typeof( ReadOnlyAttribute ) ];
				if( attr == null )
				{
					dynamic_attributes.add( new ReadOnlyAttribute( value ) );
					return;
				}
				attr.GetType().GetField("isReadOnly", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic ).SetValue( attr, value );
			}
		}
		public				String					display_name		
		{
			get
			{
				return DisplayName;
			}
			set
			{
				var owner_ref = owner;
				if( owner != null && owner is property_container )
				{
					if( ((property_container)owner).properties.contains( category + "/" + value ) )
						throw new ArgumentException(
							"Can't change display name. Descriptor with that category and diplay name already exists in parent container.\n " + category + "/" + value
						);
					owner.properties.remove( this );
				}

				var attr = (DisplayNameAttribute)dynamic_attributes[ typeof( DisplayNameAttribute ) ];
				if( attr == null )
				{
					dynamic_attributes.add( new DisplayNameAttribute( value ) );
					if( owner_ref != null )
						owner_ref.properties.add( this );
					return;
				}
				attr.GetType().GetField("_displayName", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic ).SetValue( attr, value );

				if( owner_ref != null )
					owner_ref.properties.add( this );
			}
		}
		public				String					category			
		{
			get
			{
				return Category;
			}
			set
			{
				var owner_ref = owner;
				if( owner != null && owner is property_container)
				{
					if( ((property_container)owner).properties.contains( value + "/" + display_name ) )
						throw new ArgumentException(
							"Can't change category. Descriptor with that category and diplay name already exists in parent container.\n " + value + "/" + display_name
						);
					owner.properties.remove( this );
				}

				var attr = (CategoryAttribute)dynamic_attributes[ typeof( CategoryAttribute ) ];
				if( attr == null )
				{
					dynamic_attributes.add( new CategoryAttribute( value ) );
					if( owner_ref != null )
						owner_ref.properties.add( this );
					return;
				}
				attr.GetType().GetField("categoryValue", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic ).SetValue( attr, value );

				if( owner_ref != null )
					owner_ref.properties.add( this );
			}
		}
		public				String					description			
		{
			get
			{
				return Description;
			}
			set
			{
				var attr = (DescriptionAttribute)dynamic_attributes[ typeof( DescriptionAttribute ) ];
				if( attr == null )
				{
					dynamic_attributes.add( new DescriptionAttribute( value ) );
					return;
				}
				attr.GetType().GetField("description", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic ).SetValue( attr, value );
			}
		}
		public				String					full_name			
		{
			get
			{
				return category + "/" + display_name;
			}
		}
		public				Boolean					is_browsable		
		{
			get
			{
				return IsBrowsable;
			}
			set
			{
				var attr = (BrowsableAttribute)dynamic_attributes[ typeof( BrowsableAttribute ) ];
				if( attr == null )
				{
					dynamic_attributes.add( new BrowsableAttribute( value ) );
					return;
				}
				attr.GetType().GetField("browsable", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic ).SetValue( attr, value );
			}
		}
		public				Object					default_value		
		{
			get
			{
				var attr = (DefaultValueAttribute)dynamic_attributes[ typeof( DefaultValueAttribute ) ];
				return attr != null ? attr.Value : null;
			}
			set
			{
				var attr = (DefaultValueAttribute)dynamic_attributes[ typeof( DefaultValueAttribute ) ];
				if( attr == null )
				{
					dynamic_attributes.add( new DefaultValueAttribute( value ) );
					return;
				}
				attr.GetType().GetField("value", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic ).SetValue( attr, value );
			}
		}

		public				Object					value				
		{
			get
			{
				if(m_property_value == null)
					return null;
				return m_property_value.get_value(); 
			}
			set
			{
				m_property_value.set_value( value );
			}
		}
		public				i_property_value		property_value		
		{
			get
			{
				return m_property_value; 
			}
		}
		public				select_behavior			select_behavior		
		{
			get;set;
		}
		public				Boolean					is_selected			
		{
			get
			{
				return owner_property.is_selected;
			}
			set
			{
				owner_property.is_selected = value;
			}
		}


		#endregion

		#region |   Events   |


		public event	Action<Boolean>		selection_changed;


		#endregion

		#region |   Methods  |


		internal			void						raise_selection_changed	( Boolean is_selected )					
		{
			if( selection_changed != null )
				selection_changed( is_selected );
		}

		public static		List<property_descriptor>	from_obj			( Object obj )											
			{
				var descriptors = TypeDescriptor.GetProperties(obj);
				var list = new List<property_descriptor>(descriptors.Count);

				foreach (PropertyDescriptor descriptor in descriptors)
				{
					var desc = new property_descriptor(descriptor.DisplayName, new property_property_value(obj, descriptor.Name));
					desc.dynamic_attributes.add_range(descriptor.Attributes);
					list.Add(desc);
				}

				return list;
			}
		public static		property_descriptor			from_obj			( Object obj, String property_name )					
		{
			var prop_info = obj.GetType().GetProperty(property_name, BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Public);
			var value = new property_property_value(obj, prop_info);
			return from_value( prop_info, value );
		}
		public static		property_descriptor			from_obj			( Object obj, PropertyInfo property_info )				
		{
			return from_value( property_info, new property_property_value(obj, property_info) );
		}
		public static		property_descriptor			from_value			( i_property_info_value value )							
		{
			return new property_descriptor( value );
		}
		public static		property_descriptor			from_value			( PropertyInfo prop_info, i_property_value value )		
		{
			var display_name = prop_info.Name;
			var dattrs	= (DisplayNameAttribute[])prop_info.GetCustomAttributes(typeof(DisplayNameAttribute), true);
			if ( dattrs.Length > 0 )
				display_name = dattrs[0].DisplayName;

			var desc	= new property_descriptor(display_name, value.value_type, value) { default_value = value.get_value( ) };
			var attrs	= (IEnumerable)prop_info.GetCustomAttributes(true);

			desc.dynamic_attributes.add_range(attrs);

			return desc;
		}
		public static		property_descriptor			from_value			( String display_name, i_property_value value )			
		{
			return new property_descriptor(display_name, value.value_type, value);
		}
		public static		property_descriptor			from_value			( String display_name, String category, i_property_value value )											
		{
			return new property_descriptor(display_name, value.value_type, value) { category = category };
		}
		public static		property_descriptor			from_value			( String display_name, String category, String description, Object default_value, i_property_value value )	
		{
			return new property_descriptor( display_name, value.value_type, value )
			{
				category		= category,
				display_name	= display_name,
				description		= description,
				default_value	= default_value
			};
		}
		public static		property_descriptor			from_container		( String display_name, String category, String description, i_property_container container )				
		{
			if( container is property_container )
			{
				return new property_descriptor( display_name, new object_property_value<property_container>( (property_container)container ) )
				{
					category		= category,
					display_name	= display_name,
					description		= description
				};
			}
			return new property_descriptor( display_name, new object_property_value<i_property_container>( container ) )
			{
				category		= category,
				display_name	= display_name,
				description		= description
			};
		}
		public static		List<property_descriptor>	from_container		( i_property_container container )						
		{
			return container.ToList( );
		}

		public override		Boolean					CanResetValue			( Object component )					
		{
			return false;
		}
		public override		Object					GetValue				( Object component )					
		{
			return m_property_value != null ? m_property_value.get_value( ) : null;
		}
		public override		void					SetValue				( Object component, Object set_value )	
		{	
			m_property_value.set_value( set_value );
		}
		public override		void					ResetValue				( Object component )					
		{
			if( default_value != null )
				m_property_value.set_value( default_value );
		}
		public override		Boolean					ShouldSerializeValue	( Object component )					
		{
			return false;
		}

		public				void					set_source				( i_property_value new_value )			
		{
			m_property_value = new_value;
		}
		public				button					add_button				( String caption, String description, Action<button> callback )
		{
			var desc			= new property_descriptor( caption ) { display_name = caption, description = description };

			var button			= new button( caption ) { content = caption };
			button.click		+= callback;
			desc.dynamic_attributes.add( new control_attribute( button ) );

			inner_properties.Add( caption, desc );

			return button;
		}

		public				property_descriptor		set_compo_box_style			( String[ ] combo_box_items )											
		{
			dynamic_attributes.add( new combo_box_items_attribute( combo_box_items ) );
			return this;
		}
		public				property_descriptor		set_compo_box_style			( Object[ ] combo_box_associatons )										
		{
			dynamic_attributes.add( new combo_box_items_attribute( combo_box_associatons ) );
			return this;
		}
		public				property_descriptor		set_compo_box_style			( String argument, Func<String, IEnumerable<String>> get_items )		
		{
			dynamic_attributes.add( new combo_box_items_attribute( argument, get_items ) );
			return this;
		}
		public				property_descriptor		set_compo_box_style			( Func<Int32> items_count_func, Func<Int32, String> get_item_func )		
		{
			dynamic_attributes.add( new combo_box_items_attribute( items_count_func, get_item_func ) );
			return this;
		}
		public				property_descriptor		set_external_editor_style	( Type editor_type )													
		{
			dynamic_attributes.add( new external_editor_attribute( editor_type ) );
			return this;
		}
		public				property_descriptor		set_external_editor_style	( Type editor_type, Boolean can_directly_set )							
		{
			dynamic_attributes.add( new external_editor_attribute( editor_type, can_directly_set ) );
			return this;
		}
		public				property_descriptor		set_external_editor_style	( external_editor_event_handler run_editor_callback )					
		{
			dynamic_attributes.add( new external_editor_attribute( run_editor_callback ) );
			return this;
		}
		public				property_descriptor		set_external_editor_style	( external_editor_event_handler run_editor_callback, Boolean is_clear_visible )					
		{
			dynamic_attributes.add( new external_editor_attribute( run_editor_callback, is_clear_visible ) );
			return this;
		}
		public				property_descriptor		set_link_points				( Boolean input_enabled, Boolean output_enabled  )
		{
			dynamic_attributes.add( new node_property_attribute( input_enabled, output_enabled ) );
			return this;
		}

		public				void					update						( )										
		{
			if( owner_property != null )
				owner_property.owner_editor.item_editor.parent_container.update_hierarchy( );
		}


		#endregion

		#region | InnerTypes |
		

		public class attribute_collection: IEnumerable<Attribute>
		{

			#region | Initialize |


			internal	attribute_collection	()											
			{
			}
			internal	attribute_collection	( attribute_collection other_collection )	
			{
				m_attributes = new List<Attribute>( other_collection.m_attributes.Count );
				foreach( var attr in other_collection.m_attributes )
				{
					Attribute new_attr = (Attribute)FormatterServices.GetSafeUninitializedObject( attr.GetType() );
					var fields_info = attr.GetType().GetFields( BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Public );
					foreach( var field_info in fields_info )
						field_info.SetValue( new_attr, field_info.GetValue(attr) );
					m_attributes.Add(new_attr);
				}
				m_need_recreate = true;
			}

			
			#endregion

			#region |   Fields   |


			private readonly	List<Attribute>		m_attributes = new List<Attribute>();
			private				Boolean				m_need_recreate;


			#endregion

			#region | Properties |


			public		Attribute			this[ Type attribute_type ]	
			{
				get
				{
					var list = m_attributes.Where(attr => attr.GetType() == attribute_type).ToList();
					return (list.Count > 0) ? list[0] : null;
				}
			}
			public		Attribute			this[ Int32 index ]			
			{
				get
				{
					return m_attributes[index];
				}
				set
				{
					m_attributes[index] = value;
					m_need_recreate		= true;
				}
			}

			public		Boolean				need_recreate				
			{
				get{ return m_need_recreate; }
			}


			#endregion

			#region |   Methods  |


			public		void			add				( Attribute attribute )					
			{
				m_attributes.Add( attribute );
				m_need_recreate = true;
			}
			public		void			add_range		( IEnumerable<Attribute> attributes )	
			{
				m_attributes.AddRange( attributes );
				m_need_recreate = true;
			}
			public		void			add_range		( IEnumerable attributes )				
			{
				foreach (Attribute attribute in attributes)
				{
					m_attributes.Add( attribute );
				}
				m_need_recreate = true;
			}
			public		void			add_range		( AttributeCollection attributes )		
			{
				foreach( Attribute attribute in attributes )
					m_attributes.Add( attribute );
				m_need_recreate = true;
			}
			public		void			set_range		( IEnumerable<Attribute> attributes )	
			{
				m_attributes.Clear();
				add_range( attributes );
			}
			public		void			remove_at		( Int32 index )							
			{
				m_attributes.RemoveAt( index );
				m_need_recreate = true;
			}
			public		void			remove			( Attribute attribute )					
			{
				m_attributes.Remove( attribute );
				m_need_recreate = true;
			}
			public		void			remove<TType>	( Attribute attribute ) where TType: Attribute 
			{
				var count = m_attributes.Count;
				for( var i = count; i >= 0; --i )
				{
					if( m_attributes[i] is TType )
						m_attributes.RemoveAt( i );
				}
				m_need_recreate = true;
			}
			public		void			clear			( )										
			{
				m_attributes.Clear();
				m_need_recreate = true;
			}
			public		Attribute[]		to_array		( )										
			{
				return m_attributes.ToArray();
			}
			public		List<Attribute>	get_attributes	( Type attributes_type )				
			{
				var list = m_attributes.Where(attr => attr.GetType() == attributes_type).ToList();
				return (list.Count > 0) ? list : null;
			}

			public		IEnumerator<Attribute> GetEnumerator()	
			{
				return m_attributes.GetEnumerator();
			}
			IEnumerator IEnumerable.GetEnumerator()				
			{
				return GetEnumerator();
			}

			#endregion

		}


		#endregion

	}
}
