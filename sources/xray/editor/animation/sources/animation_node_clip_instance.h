////////////////////////////////////////////////////////////////////////////
//	Created		: 10.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_NODE_CLIP_INSTANCE_H_INCLUDED
#define ANIMATION_NODE_CLIP_INSTANCE_H_INCLUDED

#include <xray/animation/api.h>

using namespace System;
using namespace System::ComponentModel;

namespace xray {
namespace animation { 
	namespace mixing { 
		class animation_clip;
	}
}
namespace animation_editor {

	ref class animation_node_interval;
	ref class animation_node_clip;
	ref class animation_node_interpolator;

	public ref class animation_node_clip_instance
	{
		typedef xray::editor::wpf_controls::property_container wpf_property_container;
		typedef Collections::Generic::List<animation_node_interval^> intervals_list;
	public:
									animation_node_clip_instance	(animation_node_clip^ base, xray::configs::lua_config_ptr cfg);
									~animation_node_clip_instance	();
		wpf_property_container^		container						() {return m_container;};
		float						animation_length				();
		xray::animation::skeleton_animation_ptr	animation			();
		xray::animation::i_editor_animation*	editor_animation	();
		void						save							();
		pcstr						name							();
		animation_node_interval^	get_interval_by_offset_and_length(float offset, float length);

		[CategoryAttribute("Node properties"), DefaultValueAttribute(30), DisplayNameAttribute("fps"), ReadOnlyAttribute(true)]
		property u32				fps
		{
				u32					get								();
		};

		[CategoryAttribute("Node properties"), DefaultValueAttribute(""), DisplayNameAttribute("length"), ReadOnlyAttribute(true)]
		property System::String^	length
		{
				System::String^		get								();
		};

		[CategoryAttribute("Node properties"), DefaultValueAttribute(false), DisplayNameAttribute("is_additive"), ReadOnlyAttribute(true)]
		property bool				additive
		{
				bool				get								();
		};

		[CategoryAttribute("Node properties"), DefaultValueAttribute(-1), DisplayNameAttribute("synchronization_group")]
		[xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute(-1, Int32::MaxValue, 1, 0, update_on_edit_complete = true)]
		property s32				synchronization_group
		{
				s32					get								();
				void				set								(s32 new_val);
		};

		[CategoryAttribute("Node properties"), DisplayNameAttribute("interpolator")]
		property wpf_property_container^ interpolator
		{
			wpf_property_container^	get								();
			void					set								(wpf_property_container^) {};
		};

		[BrowsableAttribute(false)]
		property intervals_list^	intervals
		{
			intervals_list^			get								();
		};

		[BrowsableAttribute(false)]
		property long				id_queried
		{
				long				get								();
				void				set								(long new_val);
		};

		[BrowsableAttribute(false)]
		property float				old_clip_length
		{
				float				get								()  {return m_old_clip_length;};
		};

		[BrowsableAttribute(false)]
		property animation_node_clip^ base
		{
			animation_node_clip^	get								() {return m_base;};
		};

	private:
				void				reset_animation_clip			(String^ param);

	private:
		animation_node_clip^						m_base;
		wpf_property_container^						m_container;
		u32											m_synchronization_group;
		float										m_old_clip_length;
		bool										m_time_changed;
		bool										m_epsilon_changed;
		bool										m_type_changed;
		bool										m_sg_changed;
		animation_node_interpolator^				m_interpolator;
		xray::animation::skeleton_animation_ptr*	m_animation_clip;
		xray::configs::lua_config_ptr*				m_config;	
	}; // class animation_node_clip_instance
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_NODE_CLIP_INSTANCE_H_INCLUDED