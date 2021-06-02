////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_BASE_H_INCLUDED
#define XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_BASE_H_INCLUDED

#include <xray/ai/search/operator_holder.h>
#include <xray/ai/search/oracle_holder.h>
#include <xray/ai/search/world_state.h>
#include <xray/ai/search/operator.h>
#include <xray/ai/search/operator_holder.h>

namespace xray {
namespace ai {
namespace planning {

class propositional_planner;

class propositional_planner_base : private boost::noncopyable
{
public:
	inline 				propositional_planner_base	( propositional_planner* actual_planner );
	
	inline	bool					actual			( ) const;
	inline	void					make_inactual	( );
	inline	void					clear			( );

public:
	inline	operator_holder&		operators		( );
	inline	operator_holder const&	operators		( ) const;
	inline	oracle_holder&			oracles			( );
	inline	oracle_holder const&	oracles			( ) const;

private:
	typedef world_state				state_type;
	typedef state_type::properties_type	properties_type;

protected:
	operator_holder					m_operators;
	oracle_holder					m_oracles;
	bool							m_actual;

#ifdef DEBUG
	public:
		void						validate		( state_type const& state ) const;
#endif // #ifdef DEBUG
};

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/propositional_planner_base_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_BASE_H_INCLUDED