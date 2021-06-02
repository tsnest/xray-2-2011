using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Windows;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Describes an Editor selector by Attribute that can be in property
	/// </summary>
	public class property_grid_attribute_editor : property_grid_editor
	{
		public property_grid_attribute_editor()
		{

		}
		public property_grid_attribute_editor(Type edited_attributr_type, DataTemplate editor_template)
		{
			this.edited_attribute_type = edited_attributr_type;
			this.editor_template = editor_template;
		}
		public property_grid_attribute_editor(Type edited_attributr_type, IValueConverter converter, DataTemplate editor_template)
		{
			this.edited_attribute_type = edited_attributr_type;
			this.converter = converter;
			this.editor_template = editor_template;
		}

		/// <summary>
		/// Type of Attribute that will be editied by this EditorTemplate
		/// </summary>
		public Type edited_attribute_type { get; set; }

		/// <summary>
		/// Value, that with EditedAttributeType will be checked 
		/// </summary>
		public IValueConverter converter { get; set; }

		/// <summary>
		/// Describes whether the property be modified by this editor
		/// </summary>
		/// <param name="node"> Property that need to check </param>
		/// <returns> Returns true if node can be modified by this editor, otherwise false </returns>
		public override Boolean can_edit(property_grid_property property)
		{

			foreach(var attribute in property.descriptors[0].Attributes)
			{
				if(attribute.GetType() == edited_attribute_type)
				{
					if (converter == null)
						return true;
					
					return (Boolean)converter.Convert(attribute, typeof(Boolean), null, null);
				}
			}
			return false;
		}
	}
}
