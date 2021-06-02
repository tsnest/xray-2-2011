////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GOAL_H_INCLUDED
#define GOAL_H_INCLUDED

#include <xray/ai/goal_types.h>
#include <xray/ai/parameter_types.h>
#include "pddl_world_state_property_impl.h"
#include "base_filter.h"

namespace xray {
namespace ai {
namespace planning {

class action_parameter;

class goal : private boost::noncopyable
{
public:
	typedef pddl_world_state_property_impl::indices_type	indices_type;

public:	
						goal				( goal_types_enum goal_type, u32 priority, pcstr caption );

			void		add_parameter		( action_parameter* parameter );
	action_parameter*	pop_parameter		( );
	action_parameter*	get_parameter		( u32 const index ) const;

			void		add_filters_list	( parameters_filters_type* parameters_filters );
			bool	are_parameters_suitable	( property_values_type const& objects )  const;
	parameters_filters_type* pop_filter_list( );
	
			void	add_target_property		(
						pddl_predicate const* required_predicate,
						bool value,
						indices_type const& object_ids
					);
	pddl_world_state_property_impl const& get_target_state_property	( u32 const index ) const;

	inline	u32				get_priority				( ) const		{ return m_priority; }
	inline	u32				get_parameters_count		( ) const		{ return m_parameters.size(); }
	inline	u32				get_target_world_state_size	( ) const		{ return m_target_state.size(); }
	inline	goal_types_enum	get_type					( ) const		{ return m_goal_type; }
	inline	pcstr			get_caption					( ) const		{ return m_caption.c_str(); }
	inline	bool			can_be_executed				( ) const		{ return m_can_be_executed; }
	inline	void			cannot_be_executed			( )				{ m_can_be_executed = false; }
	inline	void			was_executed				( )				{ cannot_be_executed(); }
			bool			has_owner_parameter			( ) const;
			
public:
	goal*					m_next;
	
public:
	typedef fixed_vector< action_parameter*, 4 >			parameters_type;
	typedef intrusive_list_item< parameters_filters_type* >	filter_set_type;
	typedef intrusive_list< filter_set_type,
							filter_set_type*,
							&filter_set_type::next >		filters_set_type;

private:
	parameters_type			m_parameters;
	filters_set_type		m_filters_set;
	world_state_type		m_target_state;
	fixed_string< 32 >		m_caption;

	goal_types_enum const	m_goal_type;
	u32	const				m_priority;
	bool					m_can_be_executed;
}; // class goal

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef GOAL_H_INCLUDED