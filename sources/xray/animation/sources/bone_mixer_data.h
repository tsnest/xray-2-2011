////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BONE_MIXER_DATA_H_INCLUDED
#define BONE_MIXER_DATA_H_INCLUDED

#include <xray/animation/anim_track_common.h>

namespace xray {
namespace animation {

struct bone_mixer_data 
{	
	bone_mixer_data	( ) : m_frame( zero )
	{
	}

	frame 	m_frame;
}; // struct bone_mixer_data

} // namespace animation
} // namespace xray

#endif // #ifndef BONE_MIXER_DATA_H_INCLUDED