////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ENEMY_TARGET_SELECTOR_H_INCLUDED
#define ENEMY_TARGET_SELECTOR_H_INCLUDED

#include "target_selector_base.h"
#include <xray/ai/game_object.h>

namespace xray {
namespace ai {
namespace selectors {

class enemy_target_selector : public target_selector_base
{
public:
							enemy_target_selector		(
								ai_world& world,
								working_memory const& memory,
								blackboard& board,
								pcstr name
							);

	virtual	void					tick				( );
	virtual	void					clear_targets		( );
	virtual	void					fill_targets_list	( );
	virtual	planning::object_type	get_target			( u32 const target_index );
	virtual	pcstr					get_target_caption	( u32 const target_index ) const;
	virtual u32						get_targets_count	( ) const { return m_selected_enemies.size(); }
	virtual void					dump_state			( npc_statistics& stats ) const;

public:
	typedef std::pair< npc const*, float >				enemy_type;
	typedef fixed_vector< enemy_type, 16 >				selected_enemies_type;

private:
	selected_enemies_type			m_selected_enemies;
}; // class enemy_target_selector

} // namespace selectors
} // namespace ai
} // namespace xray

#endif // #ifndef ENEMY_TARGET_SELECTOR_H_INCLUDED
