////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BEHAVIOUR_COOK_H_INCLUDED
#define BEHAVIOUR_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace ai {

class ai_world;
class behaviour;
class brain_unit;

enum behaviour_resource_type_enum
{
	resource_type_animation,
	resource_type_sound,
	resource_type_movement_target
};

typedef buffer_vector< resources::request >	requests_type;

class behaviour_cook : public resources::translate_query_cook
{
public:
					behaviour_cook			( ai_world* world );

	virtual	void	translate_request_path	( pcstr request, fs_new::virtual_path_string & new_request ) const;
	virtual	void	translate_query			( resources::query_result_for_cook&	parent );
	virtual void	delete_resource			( resources::resource_base* resource );

private:
			void	on_behaviour_options_received	( resources::queries_result& data );
			void	load_behaviour_data		( 
						resources::query_result_for_cook* const parent,
						configs::binary_config_value const& behaviour_value,
						behaviour* const new_behaviour
					);
			void	get_requests_for_subresources	(
						configs::binary_config_value const& behaviour_value,
						requests_type& requests,
						behaviour_resource_type_enum const resource_type
					);
			void	load_sounds				(
						resources::query_result_for_cook* const parent,
						configs::binary_config_value const& behaviour_value,
						behaviour* const new_behaviour
					);
			void	load_movement_targets	(
						resources::query_result_for_cook* const parent,
						configs::binary_config_value const& behaviour_value,
						behaviour* const new_behaviour
					);
			void	on_animations_loaded	(
						resources::queries_result& data,
						configs::binary_config_value const& behaviour_value,
						behaviour* const new_behaviour
					);
			void	on_sounds_loaded		(
						resources::queries_result& data,
						configs::binary_config_value const& behaviour_value,
						behaviour* const new_behaviour
					);
			void	fill_movement_targets	( configs::binary_config_value const& behaviour_value, behaviour* const new_behaviour );
			void	finish_creation			( resources::query_result_for_cook* const parent, behaviour* const new_behaviour );

private:
	ai_world*								m_ai_world;
	configs::binary_config_ptr				m_loaded_binary_config;
}; // class behaviour_cook

} // namespace ai
} // namespace xray

#endif // #ifndef BEHAVIOUR_COOK_H_INCLUDED