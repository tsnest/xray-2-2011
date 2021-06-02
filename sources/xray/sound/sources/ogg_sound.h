////////////////////////////////////////////////////////////////////////////
//	Created		: 21.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OGG_SOUND_H_INCLUDED
#define OGG_SOUND_H_INCLUDED

#include "ogg_file_contents.h"
#include <xray/sound/sound_rms.h>

namespace xray {
namespace sound {

class ogg_sound :	public resources::unmanaged_resource,
					private boost::noncopyable
{
public:
							ogg_sound			( ogg_file_contents_ptr const& ogg_file_contents,
								sound_rms_ptr const& rms 
							);
	virtual					~ogg_sound			( );
	
	ogg_file_contents_ptr	get_file_contents	( ) const { return m_ogg_file_contents; }
	float					get_rms				( u32 msec ) const { return sound_rms_pinned(m_sound_rms)->get_rms_by_time(msec); }

private:

	ogg_file_contents_ptr	m_ogg_file_contents;
	sound_rms_ptr			m_sound_rms;
};

typedef	resources::resource_ptr < ogg_sound, resources::unmanaged_intrusive_base > ogg_sound_ptr;

} // namespace sound
} // namespace xray

#endif // #ifndef OGG_SOUND_H_INCLUDED