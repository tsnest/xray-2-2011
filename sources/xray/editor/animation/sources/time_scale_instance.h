////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TIME_SCALE_INSTANCE_H_INCLUDED
#define TIME_SCALE_INSTANCE_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;

namespace xray {
namespace animation_editor {
	ref class animation_node_interpolator;
	ref class animation_node_playing_instance;

	public ref class time_scale_instance 
	{
		typedef xray::editor::wpf_controls::property_container wpf_property_container;
	public:
									time_scale_instance		(animation_node_playing_instance^ parent);
									~time_scale_instance	();
		wpf_property_container^		container				() {return m_container;};
		animation_node_playing_instance^ parent				() {return m_parent;};
		void						save					(xray::configs::lua_config_value& cfg);
		void						load					(xray::configs::lua_config_value const& cfg);
		float						time_scale_value		() {return m_value;};

		[CategoryAttribute("Node properties"), DefaultValueAttribute(1.0f), DisplayNameAttribute("value")]
		[xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute(Single::MinValue, Single::MaxValue, 0.01f, 3, update_on_edit_complete = true)]
		property float				value
		{
				float				get						();
				void				set						(float new_val);
		};

		[CategoryAttribute("Node properties"), DisplayNameAttribute("interpolator")]
		property wpf_property_container^ interpolator
		{
			wpf_property_container^	get						();
			void					set						(wpf_property_container^) {};
		};

	private:
		animation_node_playing_instance^	m_parent;
		wpf_property_container^				m_container;
		animation_node_interpolator^		m_interpolator;
		float								m_value;
	}; // class time_scale_instance
} // namespace animation_editor
} // namespace xray
#endif // #ifndef TIME_SCALE_INSTANCE_H_INCLUDED