////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System.Windows.Controls;
using xray.editor.wpf_controls.property_editors.item;

namespace xray.editor.wpf_controls.property_editors
{
	public abstract class property_editor_base: UserControl
	{
		public				property			m_property;
		public				item_editor_base	item_editor;

		public abstract		void				update		( );
	}
}
