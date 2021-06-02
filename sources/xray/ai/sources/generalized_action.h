////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GENERALIZED_ACTION_H_INCLUDED
#define GENERALIZED_ACTION_H_INCLUDED

#include <xray/ai/parameter_types.h>
#include "pddl_world_state_property_impl.h"
#include "pddl_predicate.h"

namespace xray {
namespace ai {
namespace planning {

typedef u32 action_type;
class base_lexeme;
class pddl_domain;

class generalized_action : private boost::noncopyable
{
public:
	inline			generalized_action	( pddl_domain const& domain, action_type const type, pcstr name, u32 const cost = 1 );
					~generalized_action	( );

	inline	void	add_precondition	( pddl_world_state_property_impl const& precondition_to_be_added );
	inline	void	add_effect			( pddl_world_state_property_impl const& effect_to_be_added );
	inline	u32	get_preconditions_count	( ) const	{ return m_preconditions.size(); }
	inline	u32		get_parameters_count( ) const	{ return m_parameter_types.size(); }
	inline	u32		get_clones_count	( ) const	{ return m_clones.size(); }
	inline	u32		get_effects_count	( ) const	{ return m_effects.size(); }
	inline	pcstr	get_caption			( ) const	{ return m_caption.c_str(); }
	inline pddl_domain const& get_domain( ) const	{ return m_domain; }
	inline	action_type get_type		( ) const	{ return m_type; }
	inline	void	add_parameter_type	( parameter_type const type );
	inline	parameter_type	get_parameter_type	( u32 const index ) const;
	inline	u32		get_cost			( ) const	{ return m_cost; }
	
	inline	pddl_world_state_property_impl const&	get_precondition( u32 const index ) const;
	inline	pddl_world_state_property_impl const&	get_effect		( u32 const index ) const;
	inline	generalized_action const*				get_clone		( u32 const index ) const;
			
			void						set_preconditions	( base_lexeme const& target_expression );
			void						set_effects			( base_lexeme const& target_expression );
			generalized_action*			clone				( );

public:
	generalized_action*					next;

private:
	typedef fixed_vector< parameter_type, 4 >				parameters_types_type;
	typedef fixed_vector< generalized_action*, 16 >			clones_type;

private:
	world_state_type					m_preconditions;
	world_state_type					m_effects;
	parameters_types_type				m_parameter_types;
	clones_type							m_clones;
	generalized_action*					m_parent;
	fixed_string< 32 >					m_caption;
	action_type const					m_type;
	u32									m_cost;
	pddl_domain const&					m_domain;
}; // class generalized_action

} // namespace planning
} // namespace ai
} // namespace xray

#include "generalized_action_inline.h"

#endif // #ifndef GENERALIZED_ACTION_H_INCLUDED