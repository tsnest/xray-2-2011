////////////////////////////////////////////////////////////////////////////
//	Created		: 17.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROPOSITIONAL_SYMBOL_H_INCLUDED
#define PROPOSITIONAL_SYMBOL_H_INCLUDED

#include "pddl_predicate_lexeme.h"
#include <xray/ai/parameter_types.h>

namespace xray {
namespace ai {
namespace planning {

class propositional_symbol : public pddl_predicate_lexeme
{
public:
	inline	propositional_symbol	( pddl_predicate const* required_predicate );
	inline	propositional_symbol	( pddl_predicate const* required_predicate, parameters_order_enum param1 );
	inline	propositional_symbol	(
				pddl_predicate const* required_predicate,
				parameters_order_enum param1,
				parameters_order_enum param2
			);
	inline	propositional_symbol	(
				pddl_predicate const* required_predicate,
				parameters_order_enum param1,
				parameters_order_enum param2,
				parameters_order_enum param3
			);
	inline	propositional_symbol	(
				pddl_predicate const* required_predicate,
				parameters_order_enum param1,
				parameters_order_enum param2,
				parameters_order_enum param3,
				parameters_order_enum param4
			);

	virtual	u32		memory_size_for_brackets_expansion	( ) const;
	virtual	base_lexeme const&	expand_brackets			( memory::stack_allocator& allocator ) const;
	
	virtual	void	add_to_target_world_state			( pddl_problem& problem ) const;
	virtual	void	add_to_preconditions				( generalized_action& action ) const;
	virtual	void	add_to_effects						( generalized_action& action ) const;
	
	virtual	void	destroy								( ) const;

private:
	typedef fixed_vector< u32, 4 >	indices_type;

private:
	indices_type	m_parameters_placeholders;
}; // class propositional_symbol

} // namespace planning
} // namespace ai
} // namespace xray

#include "propositional_symbol_inline.h"

#endif // #ifndef PROPOSITIONAL_SYMBOL_H_INCLUDED