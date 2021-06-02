////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_WEAPON_H_INCLUDED
#define OBJECT_WEAPON_H_INCLUDED

#include <xray/ai/weapon.h>
#include <xray/ai/game_object.h>

namespace xray {

namespace ai {
	struct npc;
} // namespace ai

} // namespace xray

namespace stalker2 {

class object_weapon :
	public ai::weapon,
	public ai::game_object
{
public:
								object_weapon			( ai::weapon_types_enum type, pcstr name, u32 id, u32 ammo_count = 32 );
								~object_weapon			( );

	virtual game_object const*	cast_game_object		( ) const	{ return this; }
	virtual	ai::weapon_types_enum get_type				( ) const	{ return m_type; }
	virtual	bool				is_loaded				( ) const	{ return m_ammo_count > 0; }	

	virtual	pcstr				get_name				( ) const	{ return m_name; } 
	virtual	u32					get_id					( ) const	{ return m_id; }
	
	virtual	ai::npc*			cast_npc				( )			{ return 0; }
	virtual	ai::npc const*		cast_npc				( )	const	{ return 0; }
	virtual	weapon*				cast_weapon				( )			{ return this; }
	virtual	weapon const*		cast_weapon				( )	const	{ return this; }
	virtual float				get_velocity			( ) const	{ return 0.f; }
	virtual float				get_luminosity			( ) const	{ return 0.003f; }
	virtual	ai::collision_object* get_collision_object	( )	const	{ return 0; }
	virtual	float3				get_random_surface_point( u32 const current_time ) const;
	virtual	float4x4			local_to_cell			( float3 const& requester ) const;

public:
	object_weapon*				m_next;

private:
	ai::weapon_types_enum		m_type;
	pcstr						m_name;
	u32							m_id;
	u32							m_ammo_count;
}; // class object_weapon

} // namespace stalker2

#endif // #ifndef OBJECT_WEAPON_H_INCLUDED