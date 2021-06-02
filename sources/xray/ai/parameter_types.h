////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_PARAMETER_TYPES_H_INCLUDED
#define XRAY_AI_PARAMETER_TYPES_H_INCLUDED

namespace xray {
namespace ai {

struct game_object;

namespace planning {

enum parameter_types_enum
{
 	parameter_type_owner	= 0,
	parameter_type_npc,
	parameter_type_weapon,
 	parameter_type_object,
	parameter_type_node,
	parameter_type_animation,
	parameter_type_sound,
	parameter_type_position
}; // enum parameter_types_enum

#ifndef MASTER_GOLD
static configs::enum_tuple parameters_tuples[] =
{
	std::make_pair( "npc",			parameter_type_npc			),
	std::make_pair( "weapon",		parameter_type_weapon		),
	std::make_pair( "object",		parameter_type_object		),
	std::make_pair( "node",			parameter_type_node			),
	std::make_pair( "animation",	parameter_type_animation	),
	std::make_pair( "sound",		parameter_type_sound		),
	std::make_pair( "position",		parameter_type_position		)
};
#endif // #ifndef MASTER_GOLD

enum parameters_order_enum
{
	_0,
	_1,
	_2,
	_3,
	_4
};

typedef u32 parameter_type;
typedef pcvoid object_instance_type;
typedef std::pair< object_instance_type, pcstr > object_type;
typedef fixed_vector< object_instance_type, 4 > property_values_type;

class action_instance;

struct plan_item
{
	action_instance const*	action;
	property_values_type	parameters;
};

typedef fixed_vector< plan_item, 32 >	plan_type;

union expression_parameter
{
	object_instance_type	instance;
	parameters_order_enum	index;
};

typedef fixed_vector< expression_parameter, 4 > expression_parameters_type;

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_PARAMETER_TYPES_H_INCLUDED