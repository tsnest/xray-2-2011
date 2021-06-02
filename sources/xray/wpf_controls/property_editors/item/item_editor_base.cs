////////////////////////////////////////////////////////////////////////////
//	Created		: 01.11.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.item
{
	public class item_editor_base : property_editor_base
	{
		protected item_editor_base( )
		{
			item_editor = this;
		}

		internal			property_items_control		parent_container;

		internal			item_editor_selector		item_editor_selector;
		internal			value_editor_selector		value_editor_selector;

		public				Double						item_indent				
		{
			get
			{
				return parent_container.level * settings.hierarchical_item_indent;
			}
		}

		public virtual		void		reset_value_editor		( )	{}
		public override		void		update					( ) {}
	}
}
