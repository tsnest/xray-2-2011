////////////////////////////////////////////////////////////////////////////
//	Created		: 25.05.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_NODE_INTERVAL_INSTANCE_H_INCLUDED
#define ANIMATION_NODE_INTERVAL_INSTANCE_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;

namespace xray {
namespace animation_editor {

	ref class animation_node_interval;

	public ref class animation_node_interval_instance
	{
	public:
		animation_node_interval_instance(animation_node_interval^ interval, float max_weight, float min_weight);
		~animation_node_interval_instance();

		event Action<animation_node_interval_instance^, String^, Object^, Object^>^ property_changed;
		virtual String^ ToString() override;

		[CategoryAttribute("Interval properties"), DisplayNameAttribute("max_weight"), DefaultValueAttribute(1.0f)]
		[xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute(0.0f, 1.0f, 0.001f, 3, update_on_edit_complete = true)]
		property float max_weight
		{
			float get() {return m_max_weight;};
			void set(float new_val);
		}
		[CategoryAttribute("Interval properties"), DisplayNameAttribute("min_weight"), DefaultValueAttribute(0.0f)]
		[xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute(0.0f, 1.0f, 0.001f, 3, update_on_edit_complete = true)]
		property float min_weight
		{
			float get() {return m_min_weight;};
			void set(float new_val);
		}
		[CategoryAttribute("Interval properties"), DisplayNameAttribute("interval"), ReadOnlyAttribute(true)]
		property animation_node_interval^ interval
		{
			animation_node_interval^ get() {return m_interval;};
		}

	private:
		animation_node_interval^ m_interval;
		float m_max_weight;
		float m_min_weight;
	}; // class animation_node_interval_instance
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_NODE_INTERVAL_INSTANCE_H_INCLUDED