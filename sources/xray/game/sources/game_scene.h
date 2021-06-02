////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_SCENE_H_INCLUDED
#define GAME_SCENE_H_INCLUDED

#include <xray/input/handler.h>

namespace stalker2 {

class game_scene : private boost::noncopyable {
public:
	explicit		game_scene					( );
	virtual			~game_scene					( ) { }
	virtual void	on_activate					( ) { m_is_active = true; }
	virtual void	on_deactivate				( ) { m_is_active = false; }
	virtual void	tick						( ) = 0;
			bool	is_active					( ) const { return m_is_active;}
protected:
	bool	m_is_active;
}; // class game_scene

} // namespace stalker2

#endif // #ifndef GAME_SCENE_H_INCLUDED