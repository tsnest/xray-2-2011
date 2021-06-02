////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_PREDICATE_INLINE_H_INCLUDED
#define PDDL_PREDICATE_INLINE_H_INCLUDED

#include <xray/ai/parameter_types.h>

namespace xray {
namespace ai {
namespace planning {

inline pddl_predicate::pddl_predicate	( predicate_type required_type, pcstr name ) :
	m_type								( required_type ),
	m_caption							( name )
{
}

inline parameter_type pddl_predicate::get_parameter	( u32 const index ) const
{
	return								m_parameters[index];
}

inline bool pddl_predicate::has_owner_parameter		( ) const
{
	for ( u32 i = 0; i < m_parameters.size(); ++i )
		if ( m_parameters[i] == parameter_type_owner )
			return							true;

	return									false;
}

inline void pddl_predicate::add_parameter	( parameter_type type )
{
	m_parameters.push_back					( type );
}

inline bool pddl_predicate::evaluate_value	( property_values_type const& values ) const
{
	return									( *m_predicate_binder )( *this, values );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef PDDL_PREDICATE_INLINE_H_INCLUDED