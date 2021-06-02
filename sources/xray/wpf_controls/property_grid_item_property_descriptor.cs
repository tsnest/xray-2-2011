using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Collections;

namespace xray.editor.wpf_controls
{
	public class property_grid_item_property_descriptor: PropertyDescriptor
	{
		public property_grid_item_property_descriptor(String item_name, Int32 item_index, IList parent_collection):
			base(item_name, null)
		{
			m_item_index = item_index;
			m_component_type = parent_collection.GetType();
			m_property_type = parent_collection[item_index].GetType();
		}

		private	Type	m_component_type;
		private	Type	m_property_type;
		private	Int32	m_item_index;

		public	Type	component_type				{ get { return m_component_type;} }
		public	Type	property_type				{ get { return m_property_type;} }
		public	Int32	item_index					{ get { return m_item_index;} }

		public override bool	IsReadOnly			
		{
			get { throw new NotImplementedException(); }
		}
		public override Type	PropertyType		
		{
			get { return m_property_type; }
		}
		public override Type	ComponentType		
		{
			get { return m_component_type; }
		}

		internal		void	decrease_item_index	()								
		{
			m_item_index--;
		}
		public override bool	CanResetValue		(object component)				
		{
			return true;
		}
		public override object	GetValue			(object component)				
		{
			return ((IList)component)[m_item_index];
		}
		public override void	ResetValue			(object component)				
		{
			throw new NotImplementedException();
		}
		public override void	SetValue			(object component, object value)
		{
			((IList)component)[m_item_index] = value;
		}
		public override bool	ShouldSerializeValue(object component)				
		{
			return false;
		}
	}
}
