////////////////////////////////////////////////////////////////////////////
//	Created		: 17.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROPOSITIONAL_SYMBOL_INLINE_H_INCLUDED
#define PROPOSITIONAL_SYMBOL_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline propositional_symbol::propositional_symbol	( pddl_predicate const* required_predicate ) :
	pddl_predicate_lexeme							( required_predicate )
{
}

inline propositional_symbol::propositional_symbol	(
		pddl_predicate const* required_predicate,
		parameters_order_enum param1
	) :
	pddl_predicate_lexeme							( required_predicate )
{
	m_parameters_placeholders.push_back				( param1 );
}

inline propositional_symbol::propositional_symbol	(
		pddl_predicate const* required_predicate,
		parameters_order_enum param1,
		parameters_order_enum param2
	) :
	pddl_predicate_lexeme							( required_predicate )
{
	m_parameters_placeholders.push_back				( param1 );
	m_parameters_placeholders.push_back				( param2 );
}

inline propositional_symbol::propositional_symbol	(
		pddl_predicate const* required_predicate,
		parameters_order_enum param1,
		parameters_order_enum param2,
		parameters_order_enum param3
	) :
	pddl_predicate_lexeme							( required_predicate )
{
	m_parameters_placeholders.push_back				( param1 );
	m_parameters_placeholders.push_back				( param2 );
	m_parameters_placeholders.push_back				( param3 );
}

inline propositional_symbol::propositional_symbol	(
		pddl_predicate const* required_predicate,
		parameters_order_enum param1,
		parameters_order_enum param2,
		parameters_order_enum param3,
		parameters_order_enum param4
	) :
	pddl_predicate_lexeme							( required_predicate )
{
	m_parameters_placeholders.push_back				( param1 );
	m_parameters_placeholders.push_back				( param2 );
	m_parameters_placeholders.push_back				( param3 );
	m_parameters_placeholders.push_back				( param4 );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef PROPOSITIONAL_SYMBOL_INLINE_H_INCLUDED