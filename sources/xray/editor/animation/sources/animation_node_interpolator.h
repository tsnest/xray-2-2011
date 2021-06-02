////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_NODE_INTERPOLATOR_H_INCLUDED
#define ANIMATION_NODE_INTERPOLATOR_H_INCLUDED

#include "enum_defines.h"

using namespace System;
using namespace System::ComponentModel;
using xray::editor::wpf_controls::float_curve;

namespace xray {
namespace animation { 
	struct base_interpolator; 
}
namespace animation_editor {
	ref class time_instance;

	public ref class animation_node_interpolator 
	{
		typedef xray::editor::wpf_controls::property_container wpf_property_container;
	public:
									animation_node_interpolator	(System::Object^ parent);
									~animation_node_interpolator();
		wpf_property_container^		container					() {return m_container;};
		xray::animation::base_interpolator* interpolator		() {return m_interpolator;};
		System::Object^				parent						() {return m_parent;}
		void						save						(xray::configs::lua_config_value& cfg);
		void						load						(xray::configs::lua_config_value const& cfg);

		[CategoryAttribute("Node properties"), DefaultValueAttribute(interpolator_type(2)), DisplayNameAttribute("type")]
		property interpolator_type	type
		{
				interpolator_type	get							();
				void				set							(interpolator_type val);
		};

		[CategoryAttribute("Node properties"), DisplayNameAttribute("time")]
		property wpf_property_container^ time
		{
			wpf_property_container^	get							();
			void					set							(wpf_property_container^) {};
		};

		[CategoryAttribute("Node properties"), DefaultValueAttribute(0.005f), DisplayNameAttribute("epsilon")]
		[xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute(0, Single::MaxValue, 0.00001f, 5, update_on_edit_complete = true)]
		property float				epsilon
		{
				float				get							();
				void				set							(float new_val);
		};

		[CategoryAttribute("Node properties"), DisplayNameAttribute("visual"), ReadOnlyAttribute(true)]
		property float_curve^		visual
		{
				float_curve^		get							() {return m_curve;};
				void				set							(float_curve^ new_val) {XRAY_UNREFERENCED_PARAMETER(new_val);};
		};

		event	Action<String^>^	property_changed;

	private:
		void						reset_interpolator			(String^ param);
		void						set_interpolator_impl		(interpolator_type new_val);

	private:
		System::Object^						m_parent;
		wpf_property_container^				m_container;
		xray::animation::base_interpolator*	m_interpolator;
		time_instance^						m_time;
		float_curve^						m_curve;
		float								m_epsilon;
	}; // class animation_node_interpolator
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_NODE_INTERPOLATOR_H_INCLUDED