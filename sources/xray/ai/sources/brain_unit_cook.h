////////////////////////////////////////////////////////////////////////////
//	Created		: 02.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BRAIN_UNIT_COOK_H_INCLUDED
#define BRAIN_UNIT_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include <xray/ai/sound_player.h>

namespace xray {
namespace ai {

class ai_world;
class brain_unit;

class brain_unit_cook : public resources::translate_query_cook
{
public:
					brain_unit_cook			( ai_world* world );

	virtual	void	translate_request_path	( pcstr request, fs_new::virtual_path_string& new_request ) const;
	virtual	void	translate_query			( resources::query_result_for_cook&	parent );
	virtual void	delete_resource			( resources::resource_base* resource );

private:
			void	on_brain_unit_options_received	( resources::queries_result& data );
			void	on_sound_player_loaded			( resources::queries_result& data, configs::binary_config_ptr config );
			void	on_default_behaviour_loaded		( resources::queries_result& data, brain_unit* const new_brain );

private:
	ai_world*								m_ai_world;
}; // class brain_unit_cook

} // namespace ai
} // namespace xray

#endif // #ifndef BRAIN_UNIT_COOK_H_INCLUDED