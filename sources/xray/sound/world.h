////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_WORLD_H_INCLUDED
#define XRAY_SOUND_WORLD_H_INCLUDED

namespace xray {
namespace sound {

class world_user;

enum calculation_type
{
	xaudio,
	openal
}; // enum calculation_type

struct /*XRAY_NOVTABLE*/ world
{
public:
	virtual	void				tick					( )							= 0;
	virtual	void				clear_resources			( ) 						= 0;

	virtual	world_user&			get_logic_world_user	( )	const					= 0;
#ifndef MASTER_GOLD
	virtual	world_user&			get_editor_world_user	( )	const					= 0;
#endif // #ifndef MASTER_GOLD

	virtual	void				start_destruction		( )							= 0;

	virtual	float				get_speed_of_sound		( ) const;
	// debug stuff
	virtual void				set_calculation_type	( calculation_type type )	= 0;
	virtual calculation_type	get_calculation_type	( )	const					= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world )
protected:
	static float				m_speed_of_sound;
}; // class world


} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_WORLD_H_INCLUDED