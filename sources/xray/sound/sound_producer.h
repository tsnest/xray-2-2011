////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_SOUND_PRODUCER_H_INCLUDED
#define XRAY_SOUND_SOUND_PRODUCER_H_INCLUDED

#include <xray/sound/api.h>

namespace xray {
namespace sound {

class sound_scene;
class world_user;

class XRAY_SOUND_API sound_producer
{
public:
					sound_producer		( );
	virtual pcstr	get_description		( ) const							= 0;	
	virtual	float3	get_source_position	( float3 const& requester ) const	= 0;

			u32		m_sound_type;
			u32		m_sound_power;

			void	on_play				( world_user& user ) const;
protected:
	virtual			~sound_producer		( );
private:
	mutable world_user*			m_world_user;
};

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_SOUND_PRODUCER_H_INCLUDED