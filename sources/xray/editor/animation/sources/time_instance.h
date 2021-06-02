////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef TIME_INSTANCE_H_INCLUDED
#define TIME_INSTANCE_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;

namespace xray {
namespace animation_editor {
	public ref class time_instance 
	{
		typedef xray::editor::wpf_controls::property_container wpf_property_container;
	public:
									time_instance	(System::Object^ parent);
									~time_instance	();
		wpf_property_container^		container		() {return m_container;};
		System::Object^				parent			() {return m_parent;}

		[CategoryAttribute("Node properties"), DefaultValueAttribute(0.3f), DisplayNameAttribute("in seconds")]
		[xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute(0, Single::MaxValue, 0.001f, 3, update_on_edit_complete = true)]
		property float				time_s
		{
				float				get				();
				void				set				(float new_val);
		};

		[CategoryAttribute("Node properties"), DefaultValueAttribute(0.3f), DisplayNameAttribute("in frames")]
		[xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute(0, Single::MaxValue, 1.0f, 3, update_on_edit_complete = true)]
		property float				time_f
		{
			float					get				();
			void					set				(float new_val);
		};

		event	Action<String^>^	property_changed;

	private:
		System::Object^			m_parent;
		wpf_property_container^	m_container;
		float					m_time;
	}; // class time_instance
} // namespace animation_editor
} // namespace xray
#endif // #ifndef TIME_INSTANCE_H_INCLUDED