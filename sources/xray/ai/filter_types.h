////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_IGNORANCE_TYPES_H_INCLUDED
#define XRAY_AI_IGNORANCE_TYPES_H_INCLUDED

#include <xray/ai/game_object.h>

namespace xray {
namespace ai {

enum ignorance_types_enum
{
	ignorance_type_always,
	ignorance_type_until_hit
}; // enum ignorance_types_enum

typedef std::pair< game_object_ptr, ignorance_types_enum > ignorable_game_object;

enum filter_types_enum
{
	filter_type_enemy,
	filter_type_weapon,
	filter_type_cover,
	filter_type_animation,
	filter_type_sound,
	filter_type_position
}; // enum filter_types_enum

#ifndef MASTER_GOLD
static configs::enum_tuple filter_tuples[] =
{
	std::make_pair( "enemy",		filter_type_enemy		),
	std::make_pair( "weapon",		filter_type_weapon		),
	std::make_pair( "cover",		filter_type_cover		),
	std::make_pair( "animation",	filter_type_animation	),
	std::make_pair( "sound",		filter_type_sound		),
	std::make_pair( "position",		filter_type_position	)
};
#endif // #ifndef MASTER_GOLD

enum enemy_filter_types_enum
{
	enemy_filter_type_group,
	enemy_filter_type_character,
	enemy_filter_type_class,
	enemy_filter_type_outfit
}; // enum enemy_filter_types_enum

#ifndef MASTER_GOLD
static configs::enum_tuple enemy_filter_tuples[] =
{
	std::make_pair( "group",		enemy_filter_type_group		),
	std::make_pair( "character",	enemy_filter_type_character	),
	std::make_pair( "class",		enemy_filter_type_class		),
	std::make_pair( "outfit",		enemy_filter_type_outfit	)
};
#endif // #ifndef MASTER_GOLD

enum weapon_filter_types_enum
{
	weapon_filter_type_melee,
	weapon_filter_type_sniper,
	weapon_filter_type_heavy,
	weapon_filter_type_energy,
	weapon_filter_type_light
}; // enum weapon_filter_types_enum

#ifndef MASTER_GOLD
static configs::enum_tuple weapon_filter_tuples[] =
{
	std::make_pair( "melee",	weapon_filter_type_melee	),
	std::make_pair( "sniper",	weapon_filter_type_sniper	),
	std::make_pair( "heavy",	weapon_filter_type_heavy	),
	std::make_pair( "energy",	weapon_filter_type_energy	),
	std::make_pair( "light",	weapon_filter_type_light	)
};
#endif // #ifndef MASTER_GOLD

enum cover_filter_types_enum
{
	cover_filter_type_node
}; // enum cover_filter_types_enum

#ifndef MASTER_GOLD
static configs::enum_tuple cover_filter_tuples[] =
{
	std::make_pair( "node",	cover_filter_type_node )
};
#endif // #ifndef MASTER_GOLD

enum animation_filter_types_enum
{
	animation_filter_type_filename
}; // enum animation_filter_types_enum

#ifndef MASTER_GOLD
static configs::enum_tuple animation_filter_tuples[] =
{
	std::make_pair( "filename",	animation_filter_type_filename )
};
#endif // #ifndef MASTER_GOLD

enum sound_filter_types_enum
{
	sound_filter_type_filename
}; // enum sound_filter_types_enum

#ifndef MASTER_GOLD
static configs::enum_tuple sound_filter_tuples[] =
{
	std::make_pair( "filename",	sound_filter_type_filename )
};
#endif // #ifndef MASTER_GOLD

enum position_filter_types_enum
{
	position_filter_type_full
}; // enum position_filter_types_enum

#ifndef MASTER_GOLD
static configs::enum_tuple position_filter_tuples[] =
{
	std::make_pair( "full",	position_filter_type_full )
};
#endif // #ifndef MASTER_GOLD

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_IGNORANCE_TYPES_H_INCLUDED