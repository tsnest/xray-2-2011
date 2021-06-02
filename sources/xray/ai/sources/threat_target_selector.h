////////////////////////////////////////////////////////////////////////////
//	Created		: 24.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef THREAT_TARGET_SELECTOR_H_INCLUDED
#define THREAT_TARGET_SELECTOR_H_INCLUDED

#include "target_selector_base.h"

namespace xray {
namespace ai {
namespace selectors {

class threat_target_selector : public target_selector_base
{
public:
								threat_target_selector	(
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
	virtual u32						get_targets_count	( ) const { return 0; }
	virtual void					dump_state			( npc_statistics& stats ) const;
}; // class threat_target_selector

} // namespace selectors
} // namespace ai
} // namespace xray

#endif // #ifndef THREAT_TARGET_SELECTOR_H_INCLUDED
