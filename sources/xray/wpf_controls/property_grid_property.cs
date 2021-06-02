using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Controls;
using System.Collections.ObjectModel;

namespace xray.editor.wpf_controls
{
	public class property_grid_property: INotifyPropertyChanged
	{

		#region | Initialize |


		internal property_grid_property()
		{
			default_values			= new List<object>();
			property_owners			= new List<object>();
			descriptors				= new List<PropertyDescriptor>();
		}

		internal property_grid_property(property_grid owner):this()
		{ 
			owner_property_grid = owner;
		}

		internal property_grid_property(
			String				name,
			PropertyDescriptor	descriptor,
			Object				value,
			Object				default_value,
			Object				propertyParent	)
		{
			this.name				= name;
			this.descriptors			= new List<PropertyDescriptor>{descriptor};
			this.value				= new List<Object>{value};
			this.default_values		= new List<Object>{default_value};
			this.property_owners	= new List<Object>{propertyParent};
		}


		#endregion

		#region |   Fields   |


		private Boolean						m_is_collection_item;
		private Boolean						m_is_expandable_item;
		private Boolean						m_is_expanded;


		#endregion

		#region | Properties |


		public property_grid				owner_property_grid	{ get; private	set; }

		public String						name				{ get; internal	set; }
		public Type							type				{ get { return descriptors[0].PropertyType;} }
		public List<PropertyDescriptor>		descriptors			{ get; private	set; }
		public List<Object>					default_values		{ get; private	set; }
		public property_grid_property		property_parent		{ get; internal	set; }
		public List<Object>					property_owners		{ get; private	set; }
		public Boolean						is_multiple_values	{ get; internal	set; }

		public ObservableCollection<property_grid_property> sub_properties  { get; internal set; }

		public Boolean						is_expandable_item	{
			get { return m_is_expandable_item; }
			internal set { m_is_expandable_item = value; on_property_changed("is_expandable_item"); } 
		}
		public Boolean						is_expanded			{
			get { return m_is_expanded; }
			internal set { m_is_expanded = value; on_property_changed("is_expanded"); } 
		}
		public Boolean						is_collection_item	{
			get { return m_is_collection_item; }
			internal set { m_is_collection_item = value; on_property_changed("is_collection_item"); } 
		}
		public Object						value				
		{
			get
			{
				return ( is_multiple_values )? null : get_property_value(descriptors[0], property_owners[0]);
			}
			set
			{
				Object old_value = value;
				for (int i = 0; i < property_owners.Count; ++i)
				{
					descriptors[i].SetValue(property_owners[i], Convert.ChangeType(value, descriptors[i].PropertyType));
				}
				on_property_changed("value");
				owner_property_grid.on_property_changed(this, old_value);
			}
		}
		public Object[]						values				
		{
			get
			{
				Object[] ret_values = new Object[property_owners.Count];
				for (int i = 0; i < property_owners.Count; ++i)
					ret_values[i] = descriptors[i].GetValue(property_owners[i]);
				return ret_values;
			}
		}


		#endregion

		#region |  Methods   |


		private			void	on_property_changed		(String property_name)						
		{ 
			if(PropertyChanged != null)
				PropertyChanged(this, new PropertyChangedEventArgs(property_name));
		}
		internal		Object	get_property_value		(PropertyDescriptor descriptor, Object obj)	
		{
			try
			{
				return descriptor.GetValue(obj);
			}
			catch (System.Reflection.TargetInvocationException)
			{
				if (descriptor.PropertyType.IsValueType)
					return Activator.CreateInstance(descriptor.PropertyType);
				else
					return null;
			}
		}
		public override string	ToString				()											
		{
			return (value != null)?value.ToString():null;
		}
		internal		void	update					()
		{
			on_property_changed("value");
		}


		#endregion

		#region | INotifyPropertyChanged Members |

		public event PropertyChangedEventHandler PropertyChanged;

		#endregion
	}
}
