////////////////////////////////////////////////////////////////////////////
//	Created		: 28.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_TARGET_SELECTOR_H_INCLUDED
#define ANIMATION_TARGET_SELECTOR_H_INCLUDED

#include "target_selector_base.h"
#include <xray/ai/animation_item.h>

namespace xray {
namespace ai {

struct animation_item;
class brain_unit;

namespace selectors {

class animation_target_selector : public target_selector_base
{
public:
							animation_target_selector	(
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
	virtual u32						get_targets_count	( ) const { return m_selected_animations.size(); }
	virtual void					dump_state			( npc_statistics& stats ) const;

private:
	animation_items_type			m_selected_animations;
	brain_unit&						m_brain_unit;
}; // class animation_target_selector

} // namespace selectors
} // namespace ai
} // namespace xray

#endif // #ifndef ANIMATION_TARGET_SELECTOR_H_INCLUDED