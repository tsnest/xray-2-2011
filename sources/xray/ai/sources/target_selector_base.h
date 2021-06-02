////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TARGET_SELECTOR_BASE_H_INCLUDED
#define TARGET_SELECTOR_BASE_H_INCLUDED

#include <xray/ai/parameter_types.h>

namespace xray {
namespace ai {

class ai_world;
class working_memory;
class blackboard;
struct npc_statistics; 

namespace selectors {

class XRAY_NOVTABLE target_selector_base : private boost::noncopyable
{
public:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR				( target_selector_base );

	virtual	void					tick				( )											= 0;
	virtual	void					clear_targets		( )											= 0;
	virtual	void					fill_targets_list	( )											= 0;
	virtual	planning::object_type	get_target			( u32 const target_index )					= 0;
	virtual	pcstr					get_target_caption	( u32 const target_index ) const			= 0;
	virtual u32						get_targets_count	( ) const									= 0;
	virtual	void					dump_state			( npc_statistics& stats ) const				= 0;
	
	inline	pcstr					get_name			( ) const	{ return m_name.c_str(); }

			target_selector_base*	m_next;

protected:
	inline	target_selector_base	( ai_world& world, working_memory const& memory, blackboard& board, pcstr name ) :
		m_next						( 0 ),
		m_world						( world ),
		m_working_memory			( memory ),
		m_blackboard				( board ),
		m_name						( name )
	{
	}

protected:
	ai_world&						m_world;
	working_memory const&			m_working_memory;
	blackboard&						m_blackboard;
	fixed_string< 32 >				m_name;
}; // class target_selector_base

} // namespace selectors
} // namespace ai
} // namespace xray

#endif // #ifndef TARGET_SELECTOR_BASE_H_INCLUDED
