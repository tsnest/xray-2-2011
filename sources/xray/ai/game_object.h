////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_GAME_OBJECT_H_INCLUDED
#define XRAY_AI_GAME_OBJECT_H_INCLUDED

#include <xray/loose_ptr_base.h>
#include <xray/loose_ptr.h>

namespace xray {
namespace ai {

struct npc;
struct collision_object;
struct weapon;

struct XRAY_NOVTABLE game_object : public loose_ptr_base
{
	virtual	pcstr				get_name				( ) const							= 0; 
	virtual	u32					get_id					( ) const							= 0;
	
	virtual	npc*				cast_npc				( )									= 0;
	virtual	npc const*			cast_npc				( )	const							= 0;
	virtual	weapon*				cast_weapon				( )									= 0;
	virtual	weapon const*		cast_weapon				( )	const							= 0;
	virtual float				get_velocity			( ) const							= 0;
	virtual float				get_luminosity			( ) const							= 0;
	virtual	collision_object*	get_collision_object	( )	const							= 0;
	virtual	float3				get_random_surface_point( u32 const current_time ) const	= 0;
	virtual	float4x4			local_to_cell			( float3 const& requester ) const	= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR				( game_object );
}; // struct game_object

typedef loose< game_object const >::ptr					game_object_ptr;

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_GAME_OBJECT_H_INCLUDED
