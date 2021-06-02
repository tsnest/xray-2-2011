////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef POSITION_TARGET_SELECTOR_H_INCLUDED
#define POSITION_TARGET_SELECTOR_H_INCLUDED

#include "target_selector_base.h"
#include <xray/ai/movement_target.h>

namespace xray {
namespace ai {

class brain_unit;

namespace selectors {

class position_target_selector : public target_selector_base
{
public:
								position_target_selector(
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
	virtual u32						get_targets_count	( ) const { return m_selected_positions.size(); }
	virtual void					dump_state			( npc_statistics& stats ) const;

private:
	movement_targets_type			m_selected_positions;
	brain_unit&						m_brain_unit;
}; // class position_target_selector

} // namespace selectors
} // namespace ai
} // namespace xray

#endif // #ifndef POSITION_TARGET_SELECTOR_H_INCLUDED