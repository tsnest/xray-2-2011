////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
    public class collection_attribute: Attribute
    {
        public collection_attribute( Func<Boolean> add_func, Func<Object, Boolean> remove_func )
        {
			this.add_func		= add_func;
			this.remove_func	= remove_func;
        }
		public collection_attribute(Func<Boolean> add_func, Func<Object, Boolean> remove_func, Func<Int32, String, Boolean> move_func)
		{
			this.add_func = add_func;
			this.remove_func = remove_func;
			this.move_func = move_func;
		}

		public readonly Func<Boolean>					add_func;
		public readonly Func<Object, Boolean>			remove_func;
		public readonly Func<Int32, String, Boolean>	move_func;
	}
}
