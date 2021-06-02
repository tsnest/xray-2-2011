////////////////////////////////////////////////////////////////////////////
//	Created		: 21.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	/// <summary>
	/// Describes that this property will be editied external editor
	/// </summary>
	[AttributeUsage(AttributeTargets.Property)]
	public class string_select_file_editor_attribute : Attribute
	{
		public string_select_file_editor_attribute()
		{

		}

		/// <summary>
		/// Constructs an instance of ExternalEditorAttribute
		/// </summary>
		public string_select_file_editor_attribute(String default_extension, String file_mask, String default_folder, String caption)
		{
			this.caption = caption;
			this.default_extension = default_extension;
			this.file_mask = file_mask;
			this.default_folder = default_folder;
		}

		public String default_extension;
		public String file_mask;
		public String default_folder;
		public String caption;
	}
}
