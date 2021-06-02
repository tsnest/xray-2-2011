////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ogg_sound.h"

namespace xray
{
namespace sound
{

ogg_sound::ogg_sound (ogg_file_contents_ptr const& ogg_file_contents, sound_rms_ptr const& rms) :
	m_ogg_file_contents	( ogg_file_contents ),
	m_sound_rms			( rms )
{
	// empty
}

ogg_sound::~ogg_sound				( )
{
	// empty
}

} // namespace sound
} // namespace xray