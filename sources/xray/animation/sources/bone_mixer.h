////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BONE_MIXER_H_INCLUDED
#define BONE_MIXER_H_INCLUDED

#include "animation_mix.h"

namespace xray {

namespace debug {
	class renderer;
} // namespace debug

namespace animation {

struct bone_mixer_data;
class skeleton;

namespace mixing {
	struct animation_state;
	class animation_clip;
} // namespace mixing

class bone_mixer {
public:
			bone_mixer			( const skeleton &skel );

	void	on_new_animation	( mixing::animation_clip const& animation, bone_mixer_data& data, u32 const current_time_in_ms );
	void	process				( mixing::animation_state const* const data_begin, mixing::animation_state const* const data_end );
	void	render				( render::debug::renderer& renderer, const float4x4 &matrix, float time );
	void	reset_reference_pose( float4x4 &m );

private:
	animation_mix	m_mix;
	frame			m_current_mix_reference_pose;
	u32				m_change_reference_time;
}; // class bone_mixer

} // namespace animation
} // namespace xray

#endif // #ifndef BONE_MIXER_H_INCLUDED