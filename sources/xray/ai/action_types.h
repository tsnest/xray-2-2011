////////////////////////////////////////////////////////////////////////////
//	Created		: 16.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_ACTION_TYPES_H_INCLUDED
#define XRAY_AI_ACTION_TYPES_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

enum action_types_enum
{
	action_type_attack,
	action_type_attack_from_cover,
	action_type_attack_melee,
	action_type_survey_area,
	action_type_take_cover,
	action_type_uncover,
	action_type_cloak,
	action_type_reload_weapon,
	action_type_play_animation,
	action_type_play_sound,
	action_type_play_animation_with_sound,
	action_type_move_to_point
}; // enum action_types_enum

static pcstr actions_captions[] =
{
	"attack",
	"attack_from_cover",
	"attack_melee",
	"survey_area",
	"take_cover",
	"uncover",
	"cloak",
	"reload_weapon",
	"play_animation",
	"play_sound",
	"play_animation_with_sound",
	"move_to_point"
};

#ifndef MASTER_GOLD
static configs::enum_tuple actions_tuples[] =
{
	std::make_pair( actions_captions[action_type_attack],						action_type_attack						),
	std::make_pair( actions_captions[action_type_attack_from_cover],			action_type_attack_from_cover			),
	std::make_pair( actions_captions[action_type_attack_melee],					action_type_attack_melee				),
	std::make_pair( actions_captions[action_type_survey_area],					action_type_survey_area					),
	std::make_pair( actions_captions[action_type_take_cover],					action_type_take_cover					),
	std::make_pair( actions_captions[action_type_uncover],						action_type_uncover						),
	std::make_pair( actions_captions[action_type_cloak],						action_type_cloak						),
	std::make_pair( actions_captions[action_type_reload_weapon],				action_type_reload_weapon				),
	std::make_pair( actions_captions[action_type_play_animation],				action_type_play_animation				),
	std::make_pair( actions_captions[action_type_play_sound],					action_type_play_sound					),
	std::make_pair( actions_captions[action_type_play_animation_with_sound],	action_type_play_animation_with_sound	),
	std::make_pair( actions_captions[action_type_move_to_point],				action_type_move_to_point				)
};
#endif // #ifndef MASTER_GOLD

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_ACTION_TYPES_H_INCLUDED