////////////////////////////////////////////////////////////////////////////
//	Created		: 02.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SPECIFIED_PROBLEM_H_INCLUDED
#define SPECIFIED_PROBLEM_H_INCLUDED

#include "object_instance.h"
#include "pddl_world_state_property_impl.h"

namespace xray {
namespace ai {

class brain_unit;

namespace selectors {
	class target_selector_base;
} // namespace selectors

namespace planning {

class pddl_domain;
class pddl_problem;
class pddl_predicate;
class pddl_world_state_property_impl;
class action_parameter;
class base_lexeme;
class goal;

typedef u32 pddl_predicate_id;

class specified_problem : private boost::noncopyable
{
public:
	typedef pddl_world_state_property_impl::indices_type indices_type;

public:
							specified_problem			(
								pddl_domain const& domain,
								pddl_problem const& problem,
								brain_unit* const owner
							);
							~specified_problem			( );

	void					reset						( );
	pddl_predicate const*	get_lower_bound_predicate	( u32 const offset ) const;
	u32						get_predicate_offset		( pddl_predicate_id const type ) const;
	u32						get_world_state_size		( ) const;
	void					calculate_predicates_offsets( );

	template < typename T >
	inline	void			add_object					( T instance, pcstr caption );

			void			add_object_instance			( object_type instance, pcstr caption );
//			void			add_owner_as_object			( );

			void			add_target_property			( pddl_world_state_property_impl const& property_to_be_added );
	inline	u32				get_target_world_state_size	( ) const	{ return m_target_world_state.size(); }
	inline	u32				get_targets_offsets_count	( ) const	{ return m_target_offsets.size(); }
	inline	u32				get_objects_count			( ) const;
	inline	void			add_target_state_offset		( u32 const new_offset );
	inline	u32				get_target_offset			( u32 const index ) const;
	inline	pddl_domain const&	get_domain				( ) const { return m_domain; }
	inline	pddl_problem const&	get_problem				( ) const { return m_problem; }
	object_instance const*	get_object_by_type_and_index( parameter_type const type, u32 const index ) const;
			object_type		get_owner_as_object			( ) const;
			pcstr			get_owner_caption			( ) const;

			bool			are_parameters_suitable		( u32 const action_type, indices_type const& objects_indices ) const;
			bool			parameter_iterate_first_only( u32 const action_type, u32 const parameter_index ) const;
	selectors::target_selector_base* get_parameter_selector ( u32 const action_type, u32 const parameter_index ) const;
	selectors::target_selector_base* get_parameter_selector ( action_parameter const* const parameter ) const;
			void			fill_parameter_targets		( u32 const action_type );
			void			fill_parameter_targets		( action_parameter const* const parameter );
			bool			has_no_targets				( action_parameter const* const parameter ) const;
			
			u32				get_count_of_objects_by_type( parameter_type const type ) const;
			void			dump_target_world_state		( ) const;
			void			set_target_state			( base_lexeme const& target_expression );
			u32				get_object_index			( parameter_type const type, object_instance_type const& instance ) const;
	inline	pddl_world_state_property_impl const&	get_world_state_property	( u32 const index ) const;

private:
	void					restore_predicates			( );

private:
	typedef	std::pair< pddl_predicate const*, u32 >		predicate_type;
	typedef map< pddl_predicate_id, predicate_type >	predicates_type;
	typedef fixed_vector< pddl_predicate const*, 32 >	excluded_predicates_type;
	typedef fixed_vector< object_instance, 16 >			instances_type;
	typedef map< parameter_type, instances_type >		objects_type;

	friend bool operator <	( specified_problem::predicates_type::value_type const& predicate1, specified_problem::predicates_type::value_type const& predicate2 );

private:
	predicates_type										m_predicates;
	excluded_predicates_type							m_excluded_predicates;
	world_state_type									m_target_world_state;
	world_state_offsets_type							m_target_offsets;
	objects_type										m_objects;
	pddl_domain const&									m_domain;
	pddl_problem const&									m_problem;
	brain_unit*											m_owner;
}; // class specified_problem

} // namespace planning
} // namespace ai
} // namespace xray

#include "specified_problem_inline.h"

#endif // #ifndef SPECIFIED_PROBLEM_H_INCLUDED