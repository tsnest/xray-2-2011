////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_WEAPON_H_INCLUDED
#define XRAY_AI_WEAPON_H_INCLUDED

namespace xray {
namespace ai {

struct game_object;

enum weapon_types_enum
{
	weapon_type_melee,
	weapon_type_sniper,
	weapon_type_heavy,
	weapon_type_energy,
	weapon_type_light,

	// all the other types must be inserted before this one
	weapon_types_count
}; // enum weapon_types_enum

struct XRAY_NOVTABLE weapon
{
	virtual game_object const*	cast_game_object	( ) const	= 0;
	virtual	weapon_types_enum	get_type			( ) const	= 0;
	virtual	bool				is_loaded			( ) const	= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR			( weapon );
}; // struct weapon

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_WEAPON_H_INCLUDED