////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_H_INCLUDED
#define XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_H_INCLUDED

#include <xray/ai/search/propositional_planner_base.h>
#include <xray/ai/search/dnf_world_state.h>

namespace xray {
namespace ai {
namespace planning {

class search;

class propositional_planner : public propositional_planner_base
{
private:
	typedef world_state									state_type;
	typedef dnf_world_state								target_state_type;
	typedef state_type::properties_type					properties_type;
	typedef properties_type::const_iterator				properties_const_iterator;
	typedef property_id_type							property_id_type;
	typedef u32											offset_type;

private:
	typedef operator_holder::objects_type				operators_type;
	typedef oracle_holder::objects_type					oracles_type;
	typedef vector< offset_type >						target_offsets_type;

public:
	typedef u32											distance_type;
	typedef state_type									vertex_id_type;
	typedef target_state_type							target_id_type;
	typedef operator_holder::operator_type				edge_type;
	typedef operator_id_type							edge_id_type;
	typedef operators_type::const_iterator				const_edge_iterator;

public:
								propositional_planner	( );
	virtual					~propositional_planner		( );
	
			bool						update			( search& search_service, pcstr id );
			void						evaluate		( properties_const_iterator& iter, properties_const_iterator& iter_end, property_id_type const& property_id );
	inline	void						target			( target_state_type const& target );
	inline	void					add_target_offset	( offset_type const offset );
	inline	state_type const&			current			( ) const;
	inline	target_state_type const&	target			( ) const;
	inline	state_type					target			( u32 const offset ) const;
	inline	u32						get_offsets_count	( ) const;
	inline	bool						failed			( ) const;
	inline	pcstr						id				( ) const;
	inline	void						forward_search	( bool const forward_search );
	inline	bool						forward_search	( ) const;
			bool						actual			( );
//	virtual	void						on_plan_changed	( planning::search& search_service );

private:
	inline	propositional_planner*		return_this		( );

private:
	state_type							m_current_state;
	target_state_type					m_target_state;
	target_offsets_type					m_target_state_offsets;
	fixed_string< 32 >					m_current_id;
	bool								m_failed;
	bool								m_forward_search;
};

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/propositional_planner_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_H_INCLUDED