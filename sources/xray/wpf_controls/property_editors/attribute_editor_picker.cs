////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Data;

namespace xray.editor.wpf_controls.property_editors
{
	/// <summary>
	/// Describes an Editor selector by Attribute that can be in property
	/// </summary>
	public class attribute_editor_picker : editor_picker
	{
		public attribute_editor_picker(Type edited_attribute_type, Type editor_type ): base( editor_type )
		{
			this.edited_attribute_type = edited_attribute_type;
		}
		public attribute_editor_picker(Type edited_attribute_type, IValueConverter converter, Type editor_type ):base( editor_type )
		{
			this.edited_attribute_type = edited_attribute_type;
			this.converter = converter;
		}
		public attribute_editor_picker(Type edited_attribute_type, Predicate<Object> compare_callback, Type editor_type ):base( editor_type )
		{
			this.edited_attribute_type = edited_attribute_type;
			this.compare_callback = compare_callback;
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
		/// Call back that -> is attribute right
		/// </summary>
		public Predicate<Object> compare_callback { get; set; }

		/// <summary>
		/// Describes whether the property be modified by this editor
		/// </summary>
		/// <param name="node"> Property that need to check </param>
		/// <returns> Returns true if node can be modified by this editor, otherwise false </returns>
		protected override Boolean can_edit_internal(property property)
		{
			var attribute = property.descriptors[0].Attributes[edited_attribute_type];
			if(attribute != null)
			{
				if (converter != null)
					return (Boolean)converter.Convert(attribute, typeof(Boolean), null, null);
				
				if(compare_callback != null)
					return compare_callback(attribute);

				return true;
			}
			return false;
		}
	}
}
