////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
    public class special_attribute: Attribute
    {
        public special_attribute(String value)
        {
            this.value = value;
        }

        public String value;
    }
}
