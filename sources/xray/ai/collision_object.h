////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_COLLISION_OBJECT_H_INCLUDED
#define XRAY_AI_COLLISION_OBJECT_H_INCLUDED

namespace xray {
namespace ai {

struct game_object;

struct XRAY_NOVTABLE collision_object
{
	virtual	game_object&	get_game_object	( ) const		= 0;
	virtual	bool			ray_test		(
								float3 const& origin,
								float3 const& direction,
								float max_distance,
								float& distance
							) const							= 0;
	virtual	float3			get_origin		( ) const		= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR	( collision_object );
}; // struct collision_object

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_COLLISION_OBJECT_H_INCLUDED