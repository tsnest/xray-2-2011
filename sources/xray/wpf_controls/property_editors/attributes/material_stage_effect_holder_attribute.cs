////////////////////////////////////////////////////////////////////////////
//	Created		: 08.11.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class material_stage_effect_holder_attribute: Attribute
	{
		public material_stage_effect_holder_attribute	( )								
		{
			is_can_edit_effect = true;
		}
		public material_stage_effect_holder_attribute	( Boolean is_can_edit_effect )		
		{
			this.is_can_edit_effect = is_can_edit_effect;
		}

		public Boolean is_can_edit_effect	
		{
			get;set;
		}
	}
}
