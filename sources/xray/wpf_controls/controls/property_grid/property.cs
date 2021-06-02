////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;

namespace xray.editor.wpf_controls.property_grid
{
	public sealed class property: property_editors.property
	{

		#region | Initialize |


		public		property	( )
		{
		}
		internal	property	( property_grid_control owner):this()
		{ 
			owner_property_grid		= owner;
		}
		internal	property	(
			String				name,
			PropertyDescriptor	descriptor,
			Object				value,
			Object				default_value,
			Object				property_owners	)
		{
			is_valid				= true;
			this.name				= name;
			descriptors				= new List<PropertyDescriptor>{descriptor};
			this.value				= new List<Object>{value};
			default_values			= new List<Object>{default_value};
			this.property_owners	= new List<Object>{property_owners};
		}


		#endregion

		#region |   Fields   |


		private Boolean								m_is_selected;
		

		#endregion

		#region | Properties |


		public			property_grid_control			owner_property_grid	
		{
			get; private set;
		}
		public override	Boolean							is_selected			
		{
			get 
			{
				return m_is_selected; 
			}
			set 
			{
				if( !( descriptor is property_descriptor ) 
					|| ( ( (property_descriptor)descriptor ).select_behavior == select_behavior.select ) )
				{
					var old_selected = m_is_selected;
					m_is_selected = value; 
					if( old_selected != m_is_selected )
					{
						if( value )
						{
							owner_property_grid.selected_properties.Add( this );
							process_selection_changed	( );
						}
						else
						{
							owner_property_grid.selected_properties.Remove( this );
							process_selection_changed	( );
						}
						on_property_changed("is_selected");
					}
				}
				else if( ( (property_descriptor)descriptor ).select_behavior == select_behavior.select_parent )
				{
					property_parent.is_selected = value;
				}
			}
		}
		public override Object							value				
		{
			get	
			{
				return ( is_multiple_values ) ? null : (!is_valid )? null : get_descriptor_value( property_owners[0], descriptors[0] );
			}
			set	
			{
				if( owner_property_grid.selected_properties.Count > 1 && !owner_property_grid.setting_values )
				{
					owner_property_grid.set_properties_values( value );
					process_important_property( );
				}
				else
				{
					var old_value	= this.value;
					for ( var i = 0; i < property_owners.Count; ++i )
						set_descriptor_value( property_owners[i], descriptors[i], value );

					is_multiple_values = false;
					on_property_changed("value");
					on_property_changed("is_default_value");
					owner_property_grid.on_property_changed(this, old_value);
					if( !owner_property_grid.setting_values ) 
						process_important_property( );
				}
			}
		}
		

		#endregion

		#region |   Methods  |


		private void	process_selection_changed	( ) 
		{
			foreach (var desc in descriptors.OfType<property_descriptor>( ) )
				 desc.raise_selection_changed( m_is_selected );
		}
		private void	process_important_property	( )	
		{
			var attr = (refresh_grid_on_change_attribute)descriptor.Attributes[typeof(refresh_grid_on_change_attribute)];
            if (attr != null)
            {
                owner_property_grid.on_refresh_property_changed();
                owner_property_grid.update( );
            }
		}


		#endregion

	}
}
