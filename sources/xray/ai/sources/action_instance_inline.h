////////////////////////////////////////////////////////////////////////////
//	Created		: 05.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_INSTANCE_INLINE_H_INCLUDED
#define ACTION_INSTANCE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline void action_instance::add_parameter	( action_parameter* parameter )
{
	m_parameters.push_back					( parameter );
}

inline parameter_type action_instance::get_parameter_type	( u32 const index ) const
{
	return						m_parameters[index]->get_type();
}

inline action_parameter* action_instance::get_parameter		( u32 const index ) const
{
	return						m_parameters[index];
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef ACTION_INSTANCE_INLINE_H_INCLUDED