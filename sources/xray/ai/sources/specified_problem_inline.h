////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SPECIFIED_PROBLEM_INLINE_H_INCLUDED
#define SPECIFIED_PROBLEM_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

template < typename T >
inline void specified_problem::add_object( T instance, pcstr caption )
{
	parameter_type const type		= m_domain.get_registered_type< T >();
	R_ASSERT						( type != parameter_type(-1), "invalid object type" );
	
	object_instance_type			new_instance;
	object_instance::set_instance	( new_instance, instance );

	if ( get_object_index( type, instance ) == u32(-1) )
		m_objects[type].push_back	( object_instance( type, instance, caption ) );
}

inline pddl_world_state_property_impl const& specified_problem::get_world_state_property	( u32 const index ) const
{
	return							m_target_world_state[index];
}

inline u32 specified_problem::get_target_offset	( u32 const index ) const
{
	return							m_target_offsets[index];
}

inline u32 specified_problem::get_objects_count	( ) const
{
	u32 result						= 0;
	for ( objects_type::const_iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter )
		result						+= iter->second.size();

	return							result;
}

inline void specified_problem::add_target_state_offset	( u32 const new_offset )
{
	m_target_offsets.push_back		( new_offset );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef SPECIFIED_PROBLEM_INLINE_H_INCLUDED