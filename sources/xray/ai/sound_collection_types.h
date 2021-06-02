////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SOUND_COLLECTION_H_INCLUDED
#define XRAY_AI_SOUND_COLLECTION_H_INCLUDED

namespace xray {
namespace ai {

enum sound_collection_types
{
	sound_collection_type_undefined			= -1,
	
	sound_collection_type_item_picking		= 0,
	sound_collection_type_item_dropping,
	sound_collection_type_item_hiding,
	sound_collection_type_item_taking,
	sound_collection_type_item_using,
	
	sound_collection_type_weapon_shooting,
	sound_collection_type_weapon_reloading,
	sound_collection_type_weapon_empty_clicking,

	sound_collection_type_npc_dying_in_combat,
	sound_collection_type_npc_dying_in_water,
	sound_collection_type_npc_pain,
	sound_collection_type_npc_walking,
	sound_collection_type_npc_talking,
	sound_collection_type_npc_eating,

	sound_collection_type_world_ambient,

	// new types must be inserted before this one
	sound_collection_type_count
}; // enum sound_collection_types

#ifndef MASTER_GOLD
static configs::enum_tuple sounds_tuples[] =
{
	std::make_pair( "item_pick",		sound_collection_type_item_picking			),
	std::make_pair( "item_drop",		sound_collection_type_item_dropping			),
	std::make_pair( "item_hide",		sound_collection_type_item_hiding			),
	std::make_pair( "item_take",		sound_collection_type_item_taking			),
	std::make_pair( "item_use",			sound_collection_type_item_using			),
	std::make_pair( "weapon_hit",		sound_collection_type_weapon_shooting		),
	std::make_pair( "weapon_reload",	sound_collection_type_weapon_reloading		),
	std::make_pair( "weapon_empty",		sound_collection_type_weapon_empty_clicking	),
	std::make_pair( "die_in_combat",	sound_collection_type_npc_dying_in_combat	),
	std::make_pair( "die_in_water",		sound_collection_type_npc_dying_in_water	),
	std::make_pair( "pain",				sound_collection_type_npc_pain				),
	std::make_pair( "walk",				sound_collection_type_npc_walking			),
	std::make_pair( "talk",				sound_collection_type_npc_talking			),
	std::make_pair( "eat",				sound_collection_type_npc_eating			),
	std::make_pair( "ambient",			sound_collection_type_world_ambient			)
};
#endif // #ifndef MASTER_GOLD

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SOUND_COLLECTION_H_INCLUDED