////////////////////////////////////////////////////////////////////////////
//	Created		: 16.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_GOAL_TYPES_H_INCLUDED
#define XRAY_AI_GOAL_TYPES_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

enum goal_types_enum
{
	goal_type_kill_enemy,
	goal_type_patrol,
	goal_type_goto_location,
	goal_type_escape_from_danger,
	goal_type_blind_fire,
	goal_type_play_animation,
	goal_type_play_sound,
	goal_type_play_animation_with_sound,
	goal_type_move_to_point
}; // enum goal_types_enum

static pcstr goals_captions[] =
{
	"kill_enemy",
	"patrol",
	"goto_location",
	"escape_from_danger",
	"blind_fire",
	"play_animation",
	"play_sound",
	"play_animation_with_sound",
	"move_to_point"
};

#ifndef MASTER_GOLD
static configs::enum_tuple goals_tuples[] =
{
	std::make_pair( goals_captions[goal_type_kill_enemy],					goal_type_kill_enemy				),
	std::make_pair( goals_captions[goal_type_patrol],						goal_type_patrol					),
	std::make_pair( goals_captions[goal_type_goto_location],				goal_type_goto_location				),
	std::make_pair( goals_captions[goal_type_escape_from_danger],			goal_type_escape_from_danger		),
	std::make_pair( goals_captions[goal_type_blind_fire],					goal_type_blind_fire				),
	std::make_pair( goals_captions[goal_type_play_animation],				goal_type_play_animation			),
	std::make_pair( goals_captions[goal_type_play_sound],					goal_type_play_sound				),
	std::make_pair( goals_captions[goal_type_play_animation_with_sound],	goal_type_play_animation_with_sound	),
	std::make_pair( goals_captions[goal_type_move_to_point],				goal_type_move_to_point				)
};
#endif // #ifndef MASTER_GOLD

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_GOAL_TYPES_H_INCLUDED