////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXER_INLINE_H_INCLUDED
#define MIXER_INLINE_H_INCLUDED

namespace xray {
namespace animation {

inline mixer::mixer				( bone_mixer& bone_mixer, memory::base_allocator& allocator ) :
	m_allocator				( allocator ),
	m_last_actual_time_ms	( u32(-1) ),
	m_bone_mixer			( bone_mixer )
{
}

inline void mixer::make_inactual( )
{
	m_last_actual_time_ms	= u32( -1 );
}

} // namespace animation
} // namespace xray

#endif // #ifndef MIXER_INLINE_H_INCLUDED