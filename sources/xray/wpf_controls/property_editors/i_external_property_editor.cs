////////////////////////////////////////////////////////////////////////////
//	Created		: 05.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray.editor.wpf_controls.property_editors
{
	public interface i_external_property_editor
	{
		void run_editor( property prop );
	}
	public interface i_additional_data
	{
		string additional_data
		{
			get;set;
		}
	}
}
