////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors
{
	public static class property_extractor
	{
		private	static		Boolean							is_match_for_attributes	( PropertyDescriptor descriptor, settings settings )										
		{
			switch( settings.browsable_attribute_state )
			{
				case browsable_state.and:
					return descriptor.Attributes.Matches( settings.browsable_attributes.ToArray( ) );

				case browsable_state.or:
					foreach ( var attribute in settings.browsable_attributes )
					{
						if( descriptor.Attributes.Matches( attribute ) )
							return true;
					}
					break;
			}
			return false;
		}

		public static		TCollection			extract<TCollection> 		( Object obj ) where TCollection: IList, new()														
		{
			var ret_coll = new TCollection();
			extract( ret_coll, new []{obj} , null, new settings( ) );
			return ret_coll;
		}
		public static		TCollection			extract<TCollection>		( Object obj, property parent_paroperty ) where TCollection: IList, new()							
		{
			var ret_coll = new TCollection();
			extract( ret_coll, new []{obj} , parent_paroperty, new settings( ) );
			return ret_coll;
		}
		public static		TCollection			extract<TCollection>		( Object obj, property parent_paroperty, settings settings ) where TCollection: IList, new()		
		{
			var ret_coll = new TCollection();
			extract( ret_coll, new []{obj} , parent_paroperty, settings );
			return ret_coll;
		}
		public static		TCollection			extract<TCollection>		( Object[] objects ) where TCollection: IList, new()												
		{
			var ret_coll = new TCollection();
			extract( ret_coll, objects, null, new settings( ) );
			return ret_coll;
		}
		public static		TCollection			extract<TCollection>		( Object[] objects, property parent_paroperty ) where TCollection: IList, new()						
		{
			var ret_coll = new TCollection();
			extract( ret_coll, objects, parent_paroperty, new settings( ) );
			return ret_coll;
		}
		public static		TCollection			extract<TCollection>		( Object[] objects, property parent_paroperty, settings settings ) where TCollection: IList, new()	
		{
			var ret_coll = new TCollection();
			extract( ret_coll, objects, parent_paroperty, settings );
			return ret_coll;
		}
		public static		List<property>		extract						( Object obj, property parent_paroperty )															
		{
			var ret_coll = new List<property>( );
			extract( ret_coll, new []{obj} , parent_paroperty, new settings( ) );
			return ret_coll;
		}
		public static		List<property>		extract						( Object obj, property parent_paroperty, settings settings )										
		{
			var ret_coll = new List<property>( );
			extract( ret_coll, new []{obj} , parent_paroperty, settings );
			return ret_coll;
		}
		public static		List<property>		extract						( Object[] objects, property parent_paroperty )														
		{
			var ret_coll = new List<property>( );
			extract( ret_coll, objects, parent_paroperty, new settings( ) );
			return ret_coll;
		}
		public static		List<property>		extract						( Object[] objects, property parent_paroperty, settings settings )									
		{
			var ret_coll = new List<property>( );
			extract( ret_coll, objects, parent_paroperty, settings );
			return ret_coll;
		}
		public static		void				extract						( IList fill_collection, Object[] objects, property parent_paroperty, settings settings )			
		{
			if ( objects[0] is IList || objects[0] is property_container_collection )
			{
				extract_from_collection( fill_collection, objects, parent_paroperty, settings );
				return;
			}
			extract_from_class( fill_collection, objects, parent_paroperty, settings );
		}
		public static		void				extract_from_class			( IList fill_collection, Object[] objects, property parent_paroperty, settings settings )			
		{
			var local_properties        = fill_collection;

			var is_first_object		    = true;
            var properties_dict			= new Dictionary<String, property>();
			var new_properties_set		= new HashSet<String>();
			var to_remove_props			= new List<property>();

			foreach (var obj in objects)
			{
				new_properties_set.Clear();
				to_remove_props.Clear();
				IEnumerable collection;
				if( obj is property_descriptor )
					collection = ( (property_descriptor)obj ).inner_properties.Values;
				else if( obj is i_property_container )
					collection = ( (i_property_container)obj ).get_properties();
				else
					collection = TypeDescriptor.GetProperties(obj);
				foreach ( PropertyDescriptor descriptor in collection )
				{
					var attribute = (BrowsableAttribute)descriptor.Attributes[typeof(BrowsableAttribute)];
					if ( attribute != null && !attribute.Browsable)
						continue;

					if ( ( parent_paroperty == null || settings.browsable_attribute_affects_children ) &&
						settings.browsable_attributes.Count > 0 && !is_match_for_attributes( descriptor, settings ))
						continue;

					if ( !is_first_object && !properties_dict.ContainsKey( descriptor.Category + "/" + descriptor.DisplayName ) )
						continue;

					property prop;
					if ( is_first_object )
					{
						if( settings.create_prop_delegate == null )
							prop = (property)Activator.CreateInstance( settings.property_type );
						else
							prop = settings.create_prop_delegate( );

						prop.extract_settings		= settings;
						local_properties.Add		( prop );
						properties_dict.Add			( descriptor.Category + "/" + descriptor.DisplayName, prop );

						if( parent_paroperty != null )
						{
							prop.property_parent		= parent_paroperty;

							if( parent_paroperty.is_read_only )
								prop.is_read_only			= true;
						}
					}
					else
						prop = properties_dict[descriptor.Category + "/" + descriptor.DisplayName];

					var value_type = descriptor.PropertyType;
					var converter_attribute	= (type_converter_attribute)descriptor.Attributes[typeof(type_converter_attribute)];
					if( converter_attribute != null )
					{
						value_type		= converter_attribute.m_destination_type;
						if( prop.converter == null )
							prop.converter = (type_converter_base)Activator.CreateInstance( converter_attribute.m_converter_type );
					}

					prop.type					= value_type;

					if ( !is_first_object && prop.type != value_type )
						continue;
					
					var read_only_attribute		= (ReadOnlyAttribute)descriptor.Attributes[typeof(ReadOnlyAttribute)];
					if( read_only_attribute != null )
						prop.is_read_only		= prop.is_read_only || read_only_attribute.IsReadOnly || settings.read_only_mode;

                    prop.name                   = descriptor.DisplayName;
					prop.descriptors.Add		( descriptor );
					prop.property_owners.Add	( obj );

					if( descriptor is property_descriptor )
						( (property_descriptor)descriptor ).owner_property = prop;
					
					var def_attr = (DefaultValueAttribute)descriptor.Attributes[typeof(DefaultValueAttribute)];
					if( def_attr != null )
						prop.default_values.Add( def_attr.Value );
					else
						prop.default_values.Add( property.get_property_value( descriptor, obj ) );

					if( !is_first_object && descriptor.Attributes[typeof(expandable_item_attribute)] == null )
					{
						var old_val = property.get_property_value( prop.descriptors[0], prop.property_owners[0] );
						var new_val = property.get_property_value( descriptor, obj );

						if ( ( old_val != null && new_val != null ) && !old_val.Equals( new_val ) )
							prop.is_multiple_values = true;
					}
					
					if( settings.initialize_prop_delegate != null )
						settings.initialize_prop_delegate( prop );

					new_properties_set.Add		( descriptor.Category + "/" + descriptor.DisplayName );
				}
				if ( !is_first_object )
				{
					foreach ( var entry in properties_dict )
					{
						var attr = ( MergablePropertyAttribute )entry.Value.descriptors[0].Attributes[typeof( MergablePropertyAttribute )];
						if ( attr!= null && attr.AllowMerge == false )
						{
							to_remove_props.Add( entry.Value );
							continue;
						}
						if ( !new_properties_set.Contains( entry.Key ) )
							to_remove_props.Add( entry.Value );
					}
					for ( var i = to_remove_props.Count-1; i >= 0; --i )
					{
						properties_dict.Remove( to_remove_props[i].name );
						local_properties.Remove( to_remove_props[i] );
					}
				}
				is_first_object = false;
			}
		}
		public static		void				extract_from_collection		( IList fill_collection, Object[] objects, property parent_property, settings settings )			
		{
			var local_properties = fill_collection;

			String str_templ = "" ;
			
			const string c_item_name = "item_{0}";
			
			var is_first_object		= true;

			foreach (var obj in objects)
			{
				IList collection;
				if( obj is i_property_container ){
					collection = (List<property_descriptor>)(property_container_collection.property_collection)((i_property_container)obj).get_properties();
					str_templ = ((property_container_collection)obj).item_name_template;
				}
				else
					collection = (IList)obj;

				for (var i = 0; i < collection.Count; ++i)
				{
					property prop;
					if (is_first_object)
					{
						if( settings.create_prop_delegate == null )
							prop = (property)Activator.CreateInstance( settings.property_type );
						else
							prop = settings.create_prop_delegate( );
						prop.extract_settings = settings;
						local_properties.Add( prop );
						if(parent_property != null)
						{
							prop.property_parent = parent_property;
							if( parent_property.is_read_only )
								prop.is_read_only = true;
						}
					}
					else
						prop = (property)local_properties[i];

					if ( !is_first_object && prop.descriptors[0].PropertyType != collection[i].GetType( ) )
						continue;

					if ( str_templ == "" )
						str_templ = c_item_name;

					if( collection[i] is property_descriptor )
						( (property_descriptor)collection[i] ).owner_property = prop;

					var descriptor = new item_property_descriptor( str_templ, i, collection );
					
					prop.name					= String.Format( str_templ, i );
					prop.is_expandable_item		= true;
					prop.descriptors.Add		( descriptor );
					prop.type					= descriptor.PropertyType;
					prop.property_owners.Add	( collection );
					prop.is_collection_item		= true;
					prop.is_read_only			= prop.is_read_only || settings.read_only_mode;
					prop.default_values.Add( property.get_property_value( descriptor, collection ) );

					var old_val = property.get_property_value( prop.descriptors[0], prop.property_owners[0] );
					var new_val = property.get_property_value( descriptor, collection );

					if ( ( old_val != null && new_val != null ) && !old_val.Equals( new_val ) )
						prop.is_multiple_values = true;

					if( settings.initialize_prop_delegate != null )
						settings.initialize_prop_delegate( prop );
				}
				is_first_object = false;
			}
		}
		public static		property			extract_from_collection		( Int32 index, Object[] objects, property parent_property, settings settings )						
		{
			property local_property = null;

			var str_templ = "" ;
			
			const string c_item_name = "item_{0}";
			
			var is_first_object		= true;

			foreach ( var obj in objects )
			{
				IList collection;
				if( obj is i_property_container ){
					collection = (List<property_descriptor>)(property_container_collection.property_collection)((i_property_container)obj).get_properties();
					str_templ = ((property_container_collection)obj).item_name_template;
				}
				else
					collection = (IList)obj;

				var i = index;
				{
					property prop;
					if ( is_first_object )
					{
						if( settings.create_prop_delegate == null )
							prop = (property)Activator.CreateInstance( settings.property_type );
						else
							prop = settings.create_prop_delegate( );
						prop.extract_settings = settings;
						local_property = prop;
						if(parent_property != null)
						{
							prop.property_parent = parent_property;
							if( parent_property.is_read_only )
								prop.is_read_only = true;
						}
					}
					else
						prop = local_property;

					if ( !is_first_object && prop.descriptors[0].PropertyType != collection[i].GetType( ) )
						continue;

					if ( str_templ == "" )
						str_templ = c_item_name;

					if( collection[i] is property_descriptor )
						( (property_descriptor)collection[i] ).owner_property = prop;

					var descriptor = new item_property_descriptor( str_templ, i, collection );
					
					prop.name					= String.Format( str_templ, i );
					prop.is_expandable_item		= true;
					prop.descriptors.Add		( descriptor );
					prop.type					= descriptor.PropertyType;
					prop.property_owners.Add	( collection );
					prop.is_collection_item		= true;
					prop.is_read_only			= prop.is_read_only || settings.read_only_mode;
					prop.default_values.Add( property.get_property_value( descriptor, collection ) );

					var old_val = property.get_property_value( prop.descriptors[0], prop.property_owners[0] );
					var new_val = property.get_property_value( descriptor, collection );

					if ( ( old_val != null && new_val != null ) && !old_val.Equals( new_val ) )
						prop.is_multiple_values = true;

					if( settings.initialize_prop_delegate != null )
						settings.initialize_prop_delegate( prop );
				}
				is_first_object = false;
			}

			return local_property;
		}

		public class settings
		{
			public	Func<property>				create_prop_delegate;
			public	Action<property>			initialize_prop_delegate;
			public	Type						property_type = typeof( property );
			public	browsable_state				browsable_attribute_state;
			public	List<Attribute>				browsable_attributes = new List<Attribute>( );
			public	Boolean						browsable_attribute_affects_children;
			public	Boolean						read_only_mode;
		}
	}
}
