////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_EDITOR_MIXER_INLINE_H_INCLUDED
#define I_EDITOR_MIXER_INLINE_H_INCLUDED

#include "mixing_n_ary_tree_animation_node.h"

using xray::animation::i_editor_mixer;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::editor_animations_event;

inline void i_editor_mixer::call_user_callback(editor_animations_event::event_type t, u32 time, n_ary_tree_animation_node* n, u32 ad)
{
#ifndef MASTER_GOLD
	if(callback)
	{
		editor_animations_event e(t, time, n->user_data, ad);
		(*callback)(e);
		n_ary_tree_animation_node* next = n->m_next_animation;
		while(next && next->driving_animation()==n)
		{
			editor_animations_event e(t, time, next->user_data, ad);
			(*callback)(e);
			next = next->m_next_animation;
		}
	}
#else // #ifndef MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETERS(t, time, n, ad);
#endif // #ifndef MASTER_GOLD
};

#endif // #ifndef I_EDITOR_MIXER_INLINE_H_INCLUDED