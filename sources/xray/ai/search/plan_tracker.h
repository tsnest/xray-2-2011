////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PLAN_TRACKER_H_INCLUDED
#define XRAY_AI_SEARCH_PLAN_TRACKER_H_INCLUDED

#include <xray/ai/search/base_types.h>
//#include <xray/ai/search/search.h>
#include <xray/ai/parameter_types.h>

namespace xray {
namespace ai {
namespace planning {

class oracle;
class pddl_planner;

class plan_tracker : private boost::noncopyable
{

public:
	inline							plan_tracker			( pddl_planner const& planner );
			
			void					track					( );
			void					on_plan_changed			( plan_type const& new_plan );
			void					finalize				( ) const;
	inline	void					verbose					( bool const value );
	inline	bool					verbose					( ) const;
//	inline	operator_id_type const&	current_operator_id		( ) const;

public:
// 			void					dump_oracles			( pcstr offset ) const;
// 			void					dump_state				( world_state const& state, pcstr offset ) const;
// 			void					dump_operators			( pcstr offset ) const;
// 			void					dump					( pcstr offset ) const;
// 	inline	void					dump_current_state		( ) const;
// 	inline	void					dump_target_state		( ) const;
// 			void					dump_fail				( ) const;
// 			void					dump_search				( search& search_service ) const;
// 			void					dump_plan				( plan_type const& plan ) const;
// 			void					dump_plan_changed		( search& search_service ) const;

private:
	inline	operator_type&			current_operator		( ) const;
			void					start_new_operator		( plan_type const& new_plan );

private:
	//typedef property_id_type		property_id_type;
	//typedef u32						operator_id_type;
	//typedef oracle					oracle_type;
	typedef plan_item				operator_type;

private:
//	inline	operator_type&			operator_object			( operator_id_type const& id ) const;
//	inline	pcstr					operator_descriptive_id	( operator_id_type const& id ) const;
//	inline	oracle_type&			oracle					( property_id_type const& id ) const;
//	inline	pcstr					oracle_descriptive_id	( property_id_type const& id ) const;

private:
	pddl_planner const&				m_planner;
//	operator_id_type				m_current_operator_id;
	operator_type					m_current_operator;
	bool							m_first_time;
	bool							m_verbose;
}; // class plan_tracker

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/plan_tracker_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_PLAN_TRACKER_H_INCLUDED