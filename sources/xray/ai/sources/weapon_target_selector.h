////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WEAPON_TARGET_SELECTOR_H_INCLUDED
#define WEAPON_TARGET_SELECTOR_H_INCLUDED

#include "target_selector_base.h"
#include <xray/ai/game_object.h>

namespace xray {
namespace ai {

class brain_unit;

namespace selectors {

class weapon_target_selector : public target_selector_base
{
public:
								weapon_target_selector	(
									ai_world& world,
									working_memory const& memory,
									blackboard& board,
									brain_unit& brain,
									pcstr name
								);

	virtual	void					tick				( );
	virtual	void					clear_targets		( );
	virtual	void					fill_targets_list	( );
	virtual	planning::object_type	get_target			( u32 const target_index );
	virtual	pcstr					get_target_caption	( u32 const target_index ) const;
	virtual u32						get_targets_count	( ) const { return m_selected_weapons.size(); }
	virtual	void					dump_state			( npc_statistics& stats ) const;

public:
	typedef std::pair< weapon const*, u32 >				weapon_type;
	typedef fixed_vector< weapon_type, 8 >				selected_weapons_type;

private:
	selected_weapons_type			m_selected_weapons;
	brain_unit&						m_brain_unit;
}; // class weapon_target_selector

} // namespace selectors
} // namespace ai
} // namespace xray

#endif // #ifndef WEAPON_TARGET_SELECTOR_H_INCLUDED