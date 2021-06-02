////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_TRACK_H_INCLUDED
#define OBJECT_TRACK_H_INCLUDED

#include "object.h"

namespace xray{
	namespace animation{	struct anm_track; }
}

namespace stalker2 {

class object_track : public game_object_static
{
	typedef game_object_static	super;
public:
					object_track		( game_world& w );
	virtual			~object_track		( );
	virtual void	load				( configs::binary_config_value const& t );
	virtual void	load_contents		( );
	virtual void	unload_contents		( );
			float	length				( ) const;
			bool	evaluate			( float const time, math::float4x4& m );
	
private:
	xray::animation::anm_track*		m_track;

}; // class object_track

} // namespace stalker2

#endif // #ifndef OBJECT_TRACK_H_INCLUDED