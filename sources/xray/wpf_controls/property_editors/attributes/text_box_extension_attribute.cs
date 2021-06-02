using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class text_box_extension_attribute: extension_attribute
	{
		public text_box_extension_attribute( Func<Single> getter, Action<Single> setter )
		{
			m_getter = getter;
			m_setter = setter;
		}

		public		Func<Single>		m_getter;
		public		Action<Single>		m_setter;
	}
}
