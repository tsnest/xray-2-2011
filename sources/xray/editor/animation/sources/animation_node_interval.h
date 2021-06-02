////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_NODE_INTERVAL_H_INCLUDED
#define ANIMATION_NODE_INTERVAL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace xray::editor::wpf_controls::animation_setup;

namespace xray {
namespace animation_editor {

	ref class animation_node_clip;

	public ref class animation_node_interval
	{
	public:
		animation_node_interval			(animation_node_clip^ p, u32 id, float offset, float length, u32 type)
			:m_parent(p),
			m_id(id),
			m_type((animation_channel_partition_type)type),
			m_offset(offset),
			m_length(length)
		{};

		virtual String^ ToString() override;

		property u32 id
		{
			u32						get			() {return m_id;};
		}
		[CategoryAttribute("Interval properties"), DisplayNameAttribute("type"), ReadOnlyAttribute(true)]
		property animation_channel_partition_type type
		{
			animation_channel_partition_type get() {return m_type;};
		}
		[CategoryAttribute("Interval properties"), DisplayNameAttribute("offset"), ReadOnlyAttribute(true)]
		property float offset
		{
			float					get			() {return m_offset;};
		}
		[CategoryAttribute("Interval properties"), DisplayNameAttribute("length"), ReadOnlyAttribute(true)]
		property float length
		{
			float					get			() {return m_length;};
		}
		[BrowsableAttribute(false)]
		property animation_node_clip^ parent
		{
			animation_node_clip^	get			() {return m_parent;};
		}

	private:
		u32 m_id;
		animation_channel_partition_type m_type;
		float m_offset;
		float m_length;
		animation_node_clip^ m_parent;
	}; // class animation_node_interval
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_NODE_INTERVAL_H_INCLUDED