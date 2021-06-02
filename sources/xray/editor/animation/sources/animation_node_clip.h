////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_NODE_CLIP_H_INCLUDED
#define ANIMATION_NODE_CLIP_H_INCLUDED

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
	ref class animation_node_clip_instance;
	ref class animation_editor_form;

	public ref class animation_node_clip
	{
		typedef Collections::Generic::List<animation_node_interval^> intervals_list;
	public:
									animation_node_clip		(System::String^ name, xray::animation::skeleton_animation_ptr anim, xray::configs::lua_config_ptr cfg, animation_editor_form^ f);
									~animation_node_clip	();		
	xray::animation::skeleton_animation_ptr	animation		();
	xray::animation::i_editor_animation*	editor_animation();
		pcstr						name					() {return m_name;};
		void						save					();
		void						load_intervals			();
		float						length					();
	animation_node_clip_instance^	make_instance			();

		property intervals_list^	intervals
		{
			intervals_list^			get						() {return m_intervals;};
		};

		property long				id_queried
		{
				long				get						()  {return m_id_queried;};
				void				set						(long new_val)  {m_id_queried = new_val;};
		};

		property animation_editor_form^	form
		{
			animation_editor_form^	get						() {return m_form;};
		};

	private:
		pstr										m_name;
		long										m_id_queried;
		intervals_list^								m_intervals;
		xray::animation::i_editor_animation*		m_animation;
		xray::configs::lua_config_ptr*				m_config;
		animation_editor_form^						m_form;
	}; // class animation_node_clip
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_NODE_CLIP_H_INCLUDED