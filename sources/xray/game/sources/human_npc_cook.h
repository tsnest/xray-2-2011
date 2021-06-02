////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef HUMAN_NPC_COOK_H_INCLUDED
#define HUMAN_NPC_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include <xray/sound/sound.h>

namespace xray {

namespace ai {
	struct world;
} // namespace ai

namespace sound {
	struct world;
} // namespace sound

namespace collision {
	struct space_partitioning_tree;
} // namespace collision

namespace render {
namespace game {
	class renderer;
} // namespace game
} // namespace render

} // namespace xray

namespace stalker2 {

class game;
class human_npc;
class game;

class human_npc_cook :
	public resources::translate_query_cook,
	private boost::noncopyable
{
public:
					human_npc_cook			( game& game );

	//virtual	void	translate_request_path	( pcstr request, fs_new::virtual_path_string& new_request ) const;
	virtual	void	translate_query			( resources::query_result_for_cook&	parent );
	virtual void	delete_resource			( resources::resource_base* resource );	

private:
			void	on_queried_data_received( resources::queries_result& data );
			void	on_npc_options_received	( configs::binary_config_value const& config , resources::query_result_for_cook& parent );
			void	on_subresources_loaded	( resources::queries_result& data, human_npc* const human );

private:
	game&									m_game;	
}; // class human_npc_cook

} // namespace stalker2

#endif // #ifndef HUMAN_NPC_COOK_H_INCLUDED
