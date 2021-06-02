////////////////////////////////////////////////////////////////////////////
//	Created		: 08.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_ON_DEMAND_PLANNER_H_INCLUDED
#define XRAY_AI_SEARCH_ON_DEMAND_PLANNER_H_INCLUDED

#include <xray/ai/search/propositional_planner_base.h>

namespace xray {
namespace ai {
namespace planning {

class search;
class plan_tracker;

class on_demand_planner : public propositional_planner_base
{
private:
	typedef world_state							state_type;
	typedef state_type::properties_type			properties_type;
	typedef properties_type::const_iterator		properties_const_iterator;
	typedef property_id_type					property_id_type;

private:
	typedef operator_holder::objects_type		operators_type;
	typedef oracle_holder::objects_type			oracles_type;

public:
	typedef u32									distance_type;
	typedef state_type							vertex_id_type;
	typedef operator_holder::operator_type		edge_type;
	typedef operator_id_type					edge_id_type;
	typedef operators_type::const_iterator		const_edge_iterator;
	typedef plan_tracker						plan_tracker;

public:
						on_demand_planner		( pcstr id );
	virtual			~on_demand_planner			( );
	
			void				update			( search& search_service );
			void				evaluate		( properties_const_iterator& iter, properties_const_iterator& iter_end, property_id_type const& property_id );
	inline	void				target			( state_type const& target );
	inline	state_type const&	current			( ) const;
	inline	state_type const&	target			( ) const;
	inline	bool				failed			( ) const;
	inline	pcstr				id				( ) const;
	inline	plan_tracker&		tracker			( ) const;
	inline	void				forward_search	( bool const forward_search );
	inline	bool				forward_search	( ) const;

protected:
			bool				actual			( );
	virtual	void				on_plan_changed	( planning::search& search_service );

private:
	inline	on_demand_planner* return_this		( );

private:
	state_type					m_current_state;
	state_type					m_target_state;
	fixed_string< 32 >			m_id;
	plan_tracker*				m_plan_tracker;
	bool						m_failed;
	bool						m_forward_search;
}; // class on_demand_planner

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_ON_DEMAND_PLANNER_H_INCLUDED