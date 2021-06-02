////////////////////////////////////////////////////////////////////////////
//	Created		: 24.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PREDICATE_TYPES_H_INCLUDED
#define PREDICATE_TYPES_H_INCLUDED

namespace xray {
namespace ai {

enum predicate_types_enum
{
	predicate_type_target_dead			= 0,
	predicate_type_weapon_loaded,
	predicate_type_target_in_melee_range,
	predicate_type_at_node,
	predicate_type_patrolling,
	predicate_type_at_cover,
	predicate_type_invisible,
	predicate_type_no_danger,
	predicate_type_animation_played,
	predicate_type_animation_playing,
	predicate_type_sound_played,
	predicate_type_sound_playing,
	predicate_type_animation_with_sound_played,
	predicate_type_is_at_position,
	predicate_type_is_moving
}; // enum predicate_types_enum

static pcstr predicates_captions[] = 
{
	"target_dead",			
	"weapon_loaded",		
	"target_in_melee_range",
	"at_node",				
	"patrolling",			
	"at_cover",				
	"invisible",			
	"no_danger",
	"animation_played",
	"animation_playing",
	"sound_played",
	"sound_playing",
	"animation_with_sound_played",
	"be_at_position",
	"is_moving"
};

#ifndef MASTER_GOLD
static configs::enum_tuple world_state_properties_tuples[] =
{
	std::make_pair( predicates_captions[predicate_type_target_dead],					predicate_type_target_dead					),
	std::make_pair( predicates_captions[predicate_type_weapon_loaded],					predicate_type_weapon_loaded				),
	std::make_pair( predicates_captions[predicate_type_target_in_melee_range],			predicate_type_target_in_melee_range		),
	std::make_pair( predicates_captions[predicate_type_at_node],						predicate_type_at_node						),
	std::make_pair( predicates_captions[predicate_type_patrolling],						predicate_type_patrolling					),
	std::make_pair( predicates_captions[predicate_type_at_cover],						predicate_type_at_cover						),
	std::make_pair( predicates_captions[predicate_type_invisible],						predicate_type_invisible					),
	std::make_pair( predicates_captions[predicate_type_no_danger],						predicate_type_no_danger					),
	std::make_pair( predicates_captions[predicate_type_animation_played],				predicate_type_animation_played				),
	std::make_pair( predicates_captions[predicate_type_animation_playing],				predicate_type_animation_playing			),
	std::make_pair( predicates_captions[predicate_type_sound_played],					predicate_type_sound_played					),
	std::make_pair( predicates_captions[predicate_type_sound_playing],					predicate_type_sound_playing				),
	std::make_pair( predicates_captions[predicate_type_animation_with_sound_played],	predicate_type_animation_with_sound_played	),
	std::make_pair( predicates_captions[predicate_type_is_at_position],					predicate_type_is_at_position				),
	std::make_pair( predicates_captions[predicate_type_is_moving],						predicate_type_is_moving					)
};
#endif // #ifndef MASTER_GOLD

} // namespace ai
} // namespace xray

#endif // #ifndef PREDICATE_TYPES_H_INCLUDED